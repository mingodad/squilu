#include <stdio.h>
#include "squirrel.h"
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
//#include <pthread.h>

SQ_OPT_STRING_STRLEN();

extern "C" {
#include "nn.h"

void *ann_malloc(size_t sz)
{
    return sq_malloc(sz);
}
void ann_free(void *p)
{
    sq_free(p, 0);
}
}

#define NR_FLAG_NONE 0
#define NR_FLAG_TRAINING (1<<0)         /* NN is training in a thread. */
#define NR_FLAG_REGRESSOR (1<<1)        /* NN will be used for regression. */
#define NR_FLAG_CLASSIFIER (1<<2)       /* NN will be used for classification.*/
#define NR_FLAG_NORMALIZE (1<<3)        /* Perform input/output normalization.*/
#define NR_FLAG_AUTO_STOP (1<<4)        /* Auto stop on training. */
#define NR_FLAG_OF_DETECTED (1<<5)      /* Auto stopped on overfitting. */
#define NR_FLAG_BACKTRACK (1<<6)        /* Auto stop with backtracking. */

/* Flags to persist when saving the NN. */
#define NR_FLAG_TO_PERSIST (NR_FLAG_REGRESSOR| \
                            NR_FLAG_CLASSIFIER| \
                            NR_FLAG_NORMALIZE| \
                            NR_FLAG_OF_DETECTED)

/* Flags to transfer after training. */
#define NR_FLAG_TO_TRANSFER (NR_FLAG_OF_DETECTED)

#define NR_MAX_LAYERS 32
#define NR_RDB_ENC_VER 2

typedef struct {
    uint32_t len, maxlen;
    float *inputs, *outputs;
} NRDataset;

typedef struct {
    uint64_t id;        /* Neural network unique ID. */
    uint64_t training_total_steps; /* How many steps of trainig the network
                                      received. A step is a single input/output
                                      pattern presented to the net (counting
                                      the same pattern multiple times) */
    uint64_t training_total_ms;   /* Total milliseconds time of training. */
    uint64_t training_max_cycles; /* Max cycles of a single training. */
    uint64_t training_max_ms; /* Max time of a single training. */
    uint32_t flags;     /* NR_FLAG_... */
    uint32_t epochs;    /* Number of training epochs so far. */
    AnnRprop *nn;     /* Neural network structure. */
    NRDataset dataset;  /* Training dataset. */
    NRDataset test;     /* Testing dataset. */
    float dataset_error;   /* Average error in the training dataset. */
    float test_error;      /* Average error in the test dataset. */
    float test_class_error;    /* Percentage of wrong classifications in test
                                   dataset. Only applicable to nets flagged with
                                   NR_FLAG_CLASSIFIER. */
    /* For normalized (NR_FLAG_NORMALIZE) networks. */
    float *inorm;          /* Inputs normalization factors. */
    float *onorm;          /* Outputs normalization factors. */
} NRTypeObject;
#if 0
typedef struct {
    //RedisModuleString *key; /* Key name of the NN we are training.
    //                           Set to NULL for unused slots. */
    int db_id;              /* DB ID where the key is. */
    pthread_t tid;          /* Thread ID of the trainer. */
    int in_progress;        /* 0 if training terminated. */
    NRTypeObject *nr;       /* A copy of the NN we are training. */
    float dataset_error;    /* Dataset error in the last cycle. */
    float test_error;       /* Test error in the last cycle. */
    float class_error;      /* Percentage of wrong classifications. */
    int curcycle;           /* Current cycle. */
} NRPendingTraining;
#endif
/* We take an array with NNs currently training in other threads.
 * Every time an NN command is called, we try to see if there are
 * finished trainings, in order to udpate weights of the original
 * NN stored into the key (we work on a copy on the other thread).*/
#define NR_PENDING_TRAINING_MAX_LEN 32

#if 0
#define REDISMODULE_ERR -1
#define REDISMODULE_OK 0

#define RedisModuleCtx void

#define RedisModule_Log(ctx, log_level, msg)

#define UNUSED(V) ((void) V)
typedef SQString RedisModuleString;

static uint64_t NRNextId = 1; /* Next neural network unique ID. */

#define RedisModule_Alloc(x) sq_malloc(x)

static void *RedisModule_Calloc(size_t nelm, size_t sz)
{
    size_t malloc_size = nelm * sz;
    void *ptr = sq_malloc(malloc_size);
    if(ptr) memset(ptr, 0, malloc_size);
    return ptr;
}

static void *RedisModule_Realloc(void *oldPtr, size_t sz)
{
    void *ptr = sq_realloc(oldPtr, 0, sz);
    return ptr;
}
#define RedisModule_Free(x) sq_free(x, 0)

static pthread_mutex_t NRPendingTrainingMutex = PTHREAD_MUTEX_INITIALIZER;
/* All the followings must be accessed after acquiring the mutex. */
static NRPendingTraining NRTrainings[NR_PENDING_TRAINING_MAX_LEN];
static int NRPendingTrainingCount = 0; /* Number of pending trainings. */

/* ========================== Low level object API ========================== */

long long NRMilliseconds(void) {
    struct timeval tv;
    long long ust;

    gettimeofday(&tv, NULL);
    ust = ((long long)tv.tv_sec)*1000000;
    ust += tv.tv_usec;
    return ust/1000;
}

/* Create a network with the specified parameters. Note that the layers
 * must be specified from the output layer[0] to the input
 * layer[N]. Each element in the integer array 'layer' specify how many
 * units there are in the corresponding layer. */
static NRTypeObject *createNRTypeObject(int flags, int *layers, int numlayers, int dset_len, int test_len) {
    NRTypeObject *o;
    o = (NRTypeObject*)RedisModule_Calloc(1,sizeof(*o));
    o->id = NRNextId++;
    o->flags = flags;
    o->nn = AnnCreateNet(numlayers,layers);
    o->dataset.maxlen = dset_len;
    o->test.maxlen = test_len;
    int ilen = ANN_INPUT_UNITS(o->nn);
    int olen = ANN_OUTPUT_UNITS(o->nn);
    o->inorm = (float*)RedisModule_Calloc(1,sizeof(float)*ilen);
    o->onorm = (float*)RedisModule_Calloc(1,sizeof(float)*olen);
    for (int j = 0; j < ilen; j++) o->inorm[j] = 1;
    for (int j = 0; j < olen; j++) o->onorm[j] = 1;
    return o;
}

/* Insert data (observations needed to train and test the NN) into the
 * NN object. While the learning and testing datasets are yet not full
 * the observed pattern is inserted evenly in one or the other side in
 * order to make sure the two datasets are populated evenly. When both
 * are already full, a random elmenet from one or the other (doing
 * a random weighted choice depending on the length) is substituted with
 * the new item. */
#define NR_INSERT_NO_TARGET 0   /* Auto select where to insert. */
#define NR_INSERT_TRAIN 1       /* Insert in training dataset. */
#define NR_INSERT_TEST 2        /* Insert in testing dataset. */
static void NRTypeInsertData(NRTypeObject *o, float *inputs, float *outputs,
                      int target_ds) {
    NRDataset *target = NULL;

    /* Check if there is no dataset at all. This may be a valid setup
     * with online learning, sample by sample. */
    if (o->dataset.maxlen == 0 && o->test.maxlen == 0) return;

    /* If the user specified a target, select it. */
    if (target_ds == NR_INSERT_TRAIN) target = &o->dataset;
    else if (target_ds == NR_INSERT_TEST) target = &o->test;

    /* If no target is specified, but there is only one possible
     * target, select it ASAP. */
    if (o->dataset.maxlen == 0) {
        target = &o->test;
    } else if (o->test.maxlen == 0) {
        target = &o->dataset;
    }

    /* Otherwise choose as the target to populate the one with less data
     * relatively to its size. */
    if (target == NULL) {
        /* If one of the two datasets are still not full, pick
         * based on fill percentage. Otherwise pick a random
         * target relatively to their size. */
        if (o->dataset.len != o->dataset.maxlen ||
            o->test.len != o->dataset.len)
        {
            float fill_a = (float)o->dataset.len / o->dataset.maxlen;
            float fill_b = (float)o->test.len / o->test.maxlen;
            target = (fill_a <= fill_b) ? &o->dataset : &o->test;
        } else {
            double r = rand()/RAND_MAX;
            double sumlen = o->dataset.maxlen + o->test.maxlen;
            if (r < (double)o->dataset.maxlen/sumlen) {
                target = &o->dataset;
            } else {
                target = &o->test;
            }
        }
    }

    /* Append if there is room or substitute with a random entry. */
    size_t idx;
    int j, numin = ANN_INPUT_UNITS(o->nn),
           numout = ANN_OUTPUT_UNITS(o->nn);

    if (target->maxlen == target->len) {
        idx = rand() % target->maxlen;
    } else {
        idx = target->len;
        target->len++;
        target->inputs = (float*)RedisModule_Realloc(target->inputs,
            sizeof(float)*numin*target->len);
        target->outputs = (float*)RedisModule_Realloc(target->outputs,
            sizeof(float)*numout*target->len);
    }

    /* Finally store the values at position. */
    for (j = 0; j < numin; j++)
        target->inputs[idx*numin+j] = inputs[j];
    for (j = 0; j < numout; j++)
        target->outputs[idx*numout+j] = outputs[j];
}

/* Free the specified dataset. */
void NRDatasetFree(NRDataset *dset) {
    RedisModule_Free(dset->inputs);
    RedisModule_Free(dset->outputs);
}

/* Free a whole NN object. */
void NRTypeReleaseObject(NRTypeObject *o) {
    AnnFree(o->nn);
    NRDatasetFree(&o->dataset);
    NRDatasetFree(&o->test);
    RedisModule_Free(o->inorm);
    RedisModule_Free(o->onorm);
    RedisModule_Free(o);
}

/* ================================ Training =============================== */

/* Clone a neural network object, including the training and test dataset.
 * We use cloning in order to train in a different thread, and later
 * copy the weights back into the original NN.
 *
 * Note when 'newid' is 0, the copied object NN unique ID is the same as the
 * original as normally this is what we want, in order to later match the
 * trained network with the object stored at the specified key
 * in the pending traning structure.
 *
 * However if the copy is performed with other goals, 'newid' should
 * be set to non-zero in order to create a net with a different ID. */
NRTypeObject *NRClone(NRTypeObject *o, int newid) {
    NRTypeObject *copy;
    copy = (NRTypeObject*)RedisModule_Calloc(1,sizeof(*o));
    *copy = *o;
    if (newid) copy->id = NRNextId++;
    copy->nn = AnnClone(o->nn);
    copy->dataset = o->dataset;
    copy->test = o->test;

    int ilen = ANN_INPUT_UNITS(o->nn);
    int olen = ANN_OUTPUT_UNITS(o->nn);
    copy->dataset.inputs = (float*)RedisModule_Alloc(sizeof(float)*ilen*o->dataset.len);
    copy->dataset.outputs = (float*)RedisModule_Alloc(sizeof(float)*olen*o->dataset.len);
    copy->test.inputs = (float*)RedisModule_Alloc(sizeof(float)*ilen*o->test.len);
    copy->test.outputs = (float*)RedisModule_Alloc(sizeof(float)*olen*o->test.len);
    memcpy(copy->dataset.inputs,o->dataset.inputs,sizeof(float)*ilen*o->dataset.len);
    memcpy(copy->dataset.outputs,o->dataset.outputs,sizeof(float)*olen*o->dataset.len);
    memcpy(copy->test.inputs,o->test.inputs,sizeof(float)*ilen*o->test.len);
    memcpy(copy->test.outputs,o->test.outputs,sizeof(float)*olen*o->test.len);

    copy->inorm = (float*)RedisModule_Alloc(sizeof(float)*ilen);
    copy->onorm = (float*)RedisModule_Alloc(sizeof(float)*olen);
    memcpy(copy->inorm,o->inorm,sizeof(float)*ilen);
    memcpy(copy->onorm,o->onorm,sizeof(float)*olen);
    return copy;
}

/* Transfer the weights from the source to the destination NN.
 * This is used after the learning process finished in a different
 * thread in order to transfer the learning back to the orignal
 * NN. */
static void NRTransferWeights(RedisModuleCtx *ctx, NRTypeObject *dst, NRTypeObject *src) {
    if (dst->id != src->id) {
        RedisModule_Log(ctx,"warning",
            "NSTransferWeight(): source and destination neural network IDs "
            "don't match. This is unexpected, probably a bug inside the "
            "module. Weights not transferred back to the origina NN.");
        return;
    }

    /* It would be faster to memcpy just the weight array for each layer,
     * however this way we access the NN in a more abstract way, and should
     * be fast enough in most cases. We can always optimized it later. */
    AnnFree(dst->nn);
    dst->nn = AnnClone(src->nn);
    dst->training_total_steps = src->training_total_steps;
    dst->training_total_ms = src->training_total_ms;
    dst->dataset_error = src->dataset_error;
    dst->test_error = src->test_error;
    dst->test_class_error = src->test_class_error;
    dst->flags |= src->flags & NR_FLAG_TO_TRANSFER;

    int ilen = ANN_INPUT_UNITS(src->nn);
    int olen = ANN_OUTPUT_UNITS(src->nn);
    memcpy(dst->inorm,src->inorm,sizeof(float)*ilen);
    memcpy(dst->onorm,src->onorm,sizeof(float)*olen);
}

/* Threaded training entry point.
 *
 * To get some clue about overfitting algorithm behavior:
 * #define NR_TRAINING_DEBUG 1
 */
void *NRTrainingThreadMain(void *arg) {
    NRPendingTraining *pt = (NRPendingTraining*)arg;
    NRTypeObject *nr = pt->nr;
    int training_iterations = 1;
    float train_error = 0;
    float test_error = 0;
    float class_error = 0;
    float past_train_error = 1.0/0.0;
    float past_test_error = 1.0/0.0;
    int auto_stop = nr->flags & NR_FLAG_AUTO_STOP;
    int backtrack = nr->flags & NR_FLAG_BACKTRACK;

    uint64_t cycles = 0;
    long long start = NRMilliseconds();
    long long cycle_time;
    int overfitting_count = 0;
    int overfitting_limit = 5;
    float best_test_error = 1.0/0.0;

    nr->flags &= ~NR_FLAG_TO_TRANSFER;

    /* If the network is auto normalized, we need to trasnform the inputs
     * in a way that's acceptable for the NN. We just find the maximum
     * absolute value, and divide for it, to get a -1,1 range. There
     * are more advanced transformations that are usually performed that
     * could be implemented in the future.
     *
     * Note that we compute the normalization vectors for all the inputs
     * and outputs, however if the network is a classifier, flagged with
     * (NR_FLAG_CLASSIFIER), no output normalization will be done since
     * the data is already in 0/1 format. */
    if ((nr->flags & NR_FLAG_NORMALIZE) && nr->dataset.len) {
        int ilen = ANN_INPUT_UNITS(nr->nn);
        int olen = ANN_OUTPUT_UNITS(nr->nn);
        float *imax = nr->inorm;
        float *omax = nr->onorm;
        float *inputs = nr->dataset.inputs;
        float *outputs = nr->dataset.outputs;
        for (int i = 0; i < ilen; i++) imax[i] = 1;
        for (int i = 0; i < olen; i++) omax[i] = 1;

        /* Compute the max values vectors. */
        for (uint32_t j = 0; j < nr->dataset.len; j++) {
            for (int i = 0; i < ilen; i++)
                if (fabs(inputs[i]) > imax[i]) imax[i] = fabs(inputs[i]);
            for (int i = 0; i < olen; i++)
                if (fabs(outputs[i]) > omax[i]) omax[i] = fabs(outputs[i]);
            inputs += ilen;
            outputs += olen;
        }

        /* Likely we are not seeing what will really be the true input/output
         * maximum value, so we multiply the maximum values found by a constant.
         * However if the max is exactly "1" we assume it's a classification
         * input and don't alter it. */
        for (int i = 0; i < ilen; i++) if (imax[i] != 1) imax[i] *= 1.2;
        for (int i = 0; i < olen; i++) if (omax[i] != 1) omax[i] *= 1.2;

        /* We can normalize the dataset directly: after the training it will
         * be discarded anyway. */
        inputs = nr->dataset.inputs;
        outputs = nr->dataset.outputs;
        for (uint32_t j = 0; j < nr->dataset.len; j++) {
            for (int i = 0; i < ilen; i++) inputs[i] /= nr->inorm[i];
            if (!(nr->flags & NR_FLAG_CLASSIFIER))
                for (int i = 0; i < olen; i++) outputs[i] /= nr->onorm[i];
            inputs += ilen;
            outputs += olen;
        }

        inputs = nr->test.inputs;
        outputs = nr->test.outputs;
        for (uint32_t j = 0; j < nr->test.len; j++) {
            for (int i = 0; i < ilen; i++) inputs[i] /= nr->inorm[i];
            if (!(nr->flags & NR_FLAG_CLASSIFIER))
                for (int i = 0; i < olen; i++) outputs[i] /= nr->onorm[i];
            inputs += ilen;
            outputs += olen;
        }
    }

    AnnRprop *saved = NULL;  /* Saved to recover on overfitting. */
    float saved_error;          /* The test error of the saved NN. */
    float saved_train_error;    /* The training dataset error of the saved NN */
    float saved_class_error;    /* The % of classification errors of saved NN */

    while(1) {
        long long cycle_start = NRMilliseconds();

        train_error = AnnTrain(nr->nn,
                               nr->dataset.inputs,
                               nr->dataset.outputs,
                               0,
                               training_iterations,
                               nr->dataset.len,
                               ANN_ALGO_BPROP);
        cycle_time = NRMilliseconds() - cycle_start;
        nr->training_total_steps += nr->dataset.len*training_iterations;

        /* Evaluate the error in the case of auto training, stop it
         * once we see that the error in the traning set is decreasing
         * while the one in the test set is not. */
        if (auto_stop) {
            AnnTestError(nr->nn,
                         nr->test.inputs,
                         nr->test.outputs,
                         nr->test.len, &test_error, &class_error);

            if (train_error < past_train_error &&
                test_error > past_test_error)
            {
                overfitting_count++;
                #ifdef NR_TRAINING_DEBUG
                printf("+YCLE %lld: [%d] %f VS %f\n", (long long)cycles,
                    overfitting_count, train_error, test_error);
                #endif
                if (overfitting_count == overfitting_limit) {
                    nr->flags |= NR_FLAG_OF_DETECTED;
                    break;
                }
            } else if (overfitting_count > 0) {
                #ifdef NR_TRAINING_DEBUG
                printf("-YCLE %lld: [%d] %f VS %f\n", (long long)cycles,
                    overfitting_count, train_error, test_error);
                #endif
                overfitting_count--;
            }

            /* Save all the networks with a score better than the currently
             * saved network. This can be a bit costly, but is safe: one
             * cycle of training more and overfitting can ruin it all. */
            if (backtrack && (saved == NULL || test_error < saved_error)) {
                #ifdef NR_TRAINING_DEBUG
                printf("SAVED! %f < %f\n", test_error, saved_error);
                #endif
                saved_error = test_error;
                saved_train_error = train_error;
                saved_class_error = class_error;
                if (saved) AnnFree(saved);
                saved = AnnClone(nr->nn);
            }

            /* Best network found? Reset the overfitting hints counter. */
            if (test_error < best_test_error) {
                overfitting_count = 0;
                best_test_error = test_error;
                #ifdef NR_TRAINING_DEBUG
                printf("BEST! %lld: <%d> %f VS %f\n", (long long)cycles,
                    overfitting_limit,train_error, test_error);
                #endif
            }

           /* Also stop if the loss is zero in both datasets. */
            if (train_error < 0.000000000000001 &&
                test_error  < 0.000000000000001) break;
        }

        cycles++;
        long long total_time = NRMilliseconds()-start;

        /* Cycles and milliseconds stop conditions. */
        if (nr->training_max_cycles && cycles == nr->training_max_cycles)
            break;
        if (nr->training_max_ms && total_time > (long long)nr->training_max_ms)
            break;

        /* If this is a long training, to do just a single training iteration
         * for each cycle is not optimal: tune the number of iterations to
         * at least take 100 milliseconds. */
        if (total_time > 10000 && cycle_time < 100) training_iterations++;

        past_train_error = train_error;
        past_test_error = test_error;

        /* Update stats for NR.THREADS to show progresses. */
        pthread_mutex_lock(&NRPendingTrainingMutex);
        pt->dataset_error = train_error;
        pt->test_error = test_error;
        if (nr->flags & NR_FLAG_CLASSIFIER) pt->class_error = class_error;
        pt->curcycle = cycles;
        pthread_mutex_unlock(&NRPendingTrainingMutex);
    }

    /* If auto stop is disabled, we still need to compute the test error
     * in order to return this information to the main thread. */
    if (!auto_stop) {
        AnnTestError(nr->nn,
                     nr->test.inputs,
                     nr->test.outputs,
                     nr->test.len, &test_error, &class_error);
    }

    /* If both autostop and backtracking are enabled, we may have
     * a better network saved! */
    if (auto_stop && backtrack) {
        if (saved && saved_error < test_error) {
            #ifdef NR_TRAINING_DEBUG
            printf("BACKTRACK: Saved network used!\n");
            #endif
            AnnFree(nr->nn);
            nr->nn = saved;
            test_error = saved_error;
            train_error = saved_train_error;
            class_error = saved_class_error;
        } else if (saved) {
            AnnFree(saved);
        }
    }

    if (nr->flags & NR_FLAG_CLASSIFIER) nr->test_class_error = class_error;
    nr->dataset_error = train_error;
    nr->test_error = test_error;
    nr->training_total_ms += NRMilliseconds()-start;

    /* Signal that the training process has finished, it's up to the main
     * thread to cleanup this training slot, copying the weights to the
     * original neural network and reclaiming memory for the copy we
     * used to work. */
    pthread_mutex_lock(&NRPendingTrainingMutex);
    pt->in_progress = 0;
    pthread_mutex_unlock(&NRPendingTrainingMutex);
    return NULL;
}

/* Start a background training in another thread. Return REDISMODULE_ERR if
 * there is no free slot for training, as we already reached the maximum of
 * networks we can train in parallel.
 *
 * The 'flags' argument specifies the additional NN flags to pass to the
 * training ruotine:
 *
 *  NR_FLAG_AUTO_STOP -- Automatically stop training on overtraining.
 *  NR_FLAG_BACKTRACK -- Save current NN state when overfitting is likely.
 */
int NRStartTraining(RedisModuleCtx *ctx, RedisModuleString *key, int dbid, NRTypeObject *nr) {
    pthread_mutex_lock(&NRPendingTrainingMutex);
    if (NRPendingTrainingCount == NR_PENDING_TRAINING_MAX_LEN) {
        pthread_mutex_unlock(&NRPendingTrainingMutex);
        return REDISMODULE_ERR;
    }

    /* Setup our trainig data. */
    NRPendingTraining *pt = &NRTrainings[NRPendingTrainingCount];
    //pt->key = RedisModule_CreateStringFromString(ctx,key);
    //RedisModule_RetainString(ctx,pt->key);
    pt->db_id = dbid;
    pt->in_progress = 1;
    pt->nr = NRClone(nr,0);
    pt->dataset_error = 0;
    pt->test_error = 0;
    pt->class_error = 0;
    pt->curcycle = 0;
    if (pthread_create(&pt->tid,NULL,NRTrainingThreadMain,pt) != 0) {
        RedisModule_Log(ctx,"warning","Unable to create a new pthread in NRStartTraining()");
        //RedisModule_FreeString(ctx,pt->key);
        pt->key = NULL;
        NRTypeReleaseObject(pt->nr);
        pthread_mutex_unlock(&NRPendingTrainingMutex);
        return REDISMODULE_ERR;
    }
    NRPendingTrainingCount++;
    nr->flags |= NR_FLAG_TRAINING;
    nr->flags &= ~NR_FLAG_TO_TRANSFER;
    pthread_mutex_unlock(&NRPendingTrainingMutex);
    return REDISMODULE_OK;
}

/* Check if there are threads that terminated the NN training, and
 * collect the info they computed (that is the new NN). */
int NRCollectThreads(RedisModuleCtx *ctx) {
    int collected = 0;
    pthread_mutex_lock(&NRPendingTrainingMutex);
    for (int j = 0; j < NRPendingTrainingCount; j++) {
        NRPendingTraining *pt = &NRTrainings[j];
        if (pt->in_progress == 0) {
            /* Training terminated. Let's see if the key
             * is still there and NN ID matches. */
            int orig_id = RedisModule_GetSelectedDb(ctx);
            if (orig_id != pt->db_id) RedisModule_SelectDb(ctx,pt->db_id);
            RedisModuleKey *key = RedisModule_OpenKey(ctx,pt->key,
                REDISMODULE_READ|REDISMODULE_WRITE);
            if (RedisModule_ModuleTypeGetType(key) == NRType) {
                NRTypeObject *nr = RedisModule_ModuleTypeGetValue(key);
                if (nr->id == pt->nr->id) {
                    NRTransferWeights(ctx,nr,pt->nr);
                    nr->flags &= ~NR_FLAG_TRAINING;
                }
                RedisModule_FreeString(ctx,pt->key);
                pt->key = NULL;
                NRTypeReleaseObject(pt->nr);
                NRPendingTrainingCount--;
                memcpy(&NRTrainings[j],&NRTrainings[j+1],
                    (NRPendingTrainingCount-j)*sizeof(NRTrainings[0]));
            }
            if (orig_id != pt->db_id) RedisModule_SelectDb(ctx,orig_id);
            collected++;
        }
    }
    pthread_mutex_unlock(&NRPendingTrainingMutex);
    return collected;
}
#endif // 0

#define RedisModule_Free(x) sq_free(x, 0)

static void *RedisModule_Calloc(size_t nelm, size_t sz)
{
    size_t malloc_size = nelm * sz;
    void *ptr = sq_malloc(malloc_size);
    if(ptr) memset(ptr, 0, malloc_size);
    return ptr;
}

static void *RedisModule_Realloc(void *oldPtr, size_t sz)
{
    void *ptr = sq_realloc(oldPtr, 0, sz);
    return ptr;
}

static uint64_t NRNextId = 1; /* Next neural network unique ID. */

long long NRMilliseconds(void) {
    struct timeval tv;
    long long ust;

    gettimeofday(&tv, NULL);
    ust = ((long long)tv.tv_sec)*1000000;
    ust += tv.tv_usec;
    return ust/1000;
}

/* Create a network with the specified parameters. Note that the layers
 * must be specified from the output layer[0] to the input
 * layer[N]. Each element in the integer array 'layer' specify how many
 * units there are in the corresponding layer. */
static NRTypeObject *createNRTypeObject(int flags, int *layers, int numlayers, int dset_len, int test_len) {
    NRTypeObject *o;
    o = (NRTypeObject*)RedisModule_Calloc(1,sizeof(*o));
    o->id = NRNextId++;
    o->flags = flags;
    o->nn = AnnCreateNet(numlayers,layers);
    o->dataset.maxlen = dset_len;
    o->test.maxlen = test_len;
    int ilen = ANN_INPUT_UNITS(o->nn);
    int olen = ANN_OUTPUT_UNITS(o->nn);
    o->inorm = (float*)RedisModule_Calloc(1,sizeof(float)*ilen);
    o->onorm = (float*)RedisModule_Calloc(1,sizeof(float)*olen);
    for (int j = 0; j < ilen; j++) o->inorm[j] = 1;
    for (int j = 0; j < olen; j++) o->onorm[j] = 1;
    return o;
}

/* Insert data (observations needed to train and test the NN) into the
 * NN object. While the learning and testing datasets are yet not full
 * the observed pattern is inserted evenly in one or the other side in
 * order to make sure the two datasets are populated evenly. When both
 * are already full, a random elmenet from one or the other (doing
 * a random weighted choice depending on the length) is substituted with
 * the new item. */
#define NR_INSERT_NO_TARGET 0   /* Auto select where to insert. */
#define NR_INSERT_TRAIN 1       /* Insert in training dataset. */
#define NR_INSERT_TEST 2        /* Insert in testing dataset. */
static void NRTypeInsertData(NRTypeObject *o, float *inputs, float *outputs,
                      int target_ds) {
    NRDataset *target = NULL;

    /* Check if there is no dataset at all. This may be a valid setup
     * with online learning, sample by sample. */
    if (o->dataset.maxlen == 0 && o->test.maxlen == 0) return;

    /* If the user specified a target, select it. */
    if (target_ds == NR_INSERT_TRAIN) target = &o->dataset;
    else if (target_ds == NR_INSERT_TEST) target = &o->test;

    /* If no target is specified, but there is only one possible
     * target, select it ASAP. */
    if (o->dataset.maxlen == 0) {
        target = &o->test;
    } else if (o->test.maxlen == 0) {
        target = &o->dataset;
    }

    /* Otherwise choose as the target to populate the one with less data
     * relatively to its size. */
    if (target == NULL) {
        /* If one of the two datasets are still not full, pick
         * based on fill percentage. Otherwise pick a random
         * target relatively to their size. */
        if (o->dataset.len != o->dataset.maxlen ||
            o->test.len != o->dataset.len)
        {
            float fill_a = (float)o->dataset.len / o->dataset.maxlen;
            float fill_b = (float)o->test.len / o->test.maxlen;
            target = (fill_a <= fill_b) ? &o->dataset : &o->test;
        } else {
            double r = rand()/RAND_MAX;
            double sumlen = o->dataset.maxlen + o->test.maxlen;
            if (r < (double)o->dataset.maxlen/sumlen) {
                target = &o->dataset;
            } else {
                target = &o->test;
            }
        }
    }

    /* Append if there is room or substitute with a random entry. */
    size_t idx;
    int j, numin = ANN_INPUT_UNITS(o->nn),
           numout = ANN_OUTPUT_UNITS(o->nn);

    if (target->maxlen == target->len) {
        idx = rand() % target->maxlen;
    } else {
        idx = target->len;
        target->len++;
        target->inputs = (float*)RedisModule_Realloc(target->inputs,
            sizeof(float)*numin*target->len);
        target->outputs = (float*)RedisModule_Realloc(target->outputs,
            sizeof(float)*numout*target->len);
    }

    /* Finally store the values at position. */
    for (j = 0; j < numin; j++)
        target->inputs[idx*numin+j] = inputs[j];
    for (j = 0; j < numout; j++)
        target->outputs[idx*numout+j] = outputs[j];
}

/* Free the specified dataset. */
void NRDatasetFree(NRDataset *dset) {
    RedisModule_Free(dset->inputs);
    RedisModule_Free(dset->outputs);
}

/* Free a whole NN object. */
void NRTypeReleaseObject(NRTypeObject *o) {
    AnnFree(o->nn);
    NRDatasetFree(&o->dataset);
    NRDatasetFree(&o->test);
    RedisModule_Free(o->inorm);
    RedisModule_Free(o->onorm);
    RedisModule_Free(o);
}

static const SQChar sq_nn_TAG[] = _SC("AnnRprop");

static SQRESULT sq_nn_release_hook(SQUserPointer p, SQInteger size, void */*ep*/) {
    NRTypeObject *self = (NRTypeObject *)p;
    if(self) NRTypeReleaseObject(self);
    return 0;
}

/*
** Creates a new AnnRprop.
*/
static SQRESULT sq_nn_constructor (HSQUIRRELVM v) {
    SQ_FUNC_VARS(v);
    SQ_GET_INTEGER(v, 2, flags);
    SQ_GET_INTEGER(v, 3, ninputs);
    const SQInteger nhidden_pos = 4;
    SQ_GET_INTEGER(v, 5, noutputs);
    SQ_OPT_INTEGER(v, 6, ndata, 0);
    SQ_OPT_INTEGER(v, 7, ntest, 0);

    if(!(
            ((flags & NR_FLAG_CLASSIFIER) && !(flags & NR_FLAG_REGRESSOR))
            || (!(flags & NR_FLAG_CLASSIFIER) && (flags & NR_FLAG_REGRESSOR))
         )
       )
        return sq_throwerror(v, _SC("invalid neural network type. Must be "
                                    "CLASSIFIER or REGRESSOR"));

    int layers[NR_MAX_LAYERS], num_layers=0;
    layers[num_layers++] = noutputs;

    /* Our NN library takes the definition of layers in the opposite
     * order, swap the layers array. */
    SQInteger asize = sq_getsize(v, nhidden_pos);
    for(int i=asize-1; i >= 0; --i)
    {
        sq_pushinteger(v, i);
        sq_get(v, nhidden_pos);
        SQInteger nhidden;
        SQRESULT rc = sq_getinteger(v, -1, &nhidden);
        if(rc != SQ_OK) return sq_throwerror(v, _SC("only integers expected on hidden layers array"));
        layers[num_layers++] = nhidden;
        sq_poptop(v);
    }

    layers[num_layers++] = ninputs;
    //for(int i=0; i < num_layers; ++i) printf("layers %d : %d\n", i, layers[i]);

    NRTypeObject *self = createNRTypeObject(flags, layers, num_layers, ndata, ntest);

    if(self){
        self->flags = flags;
        sq_setinstanceup(v, 1, self);
        sq_setreleasehook(v, 1, sq_nn_release_hook);
        return 1;
    }
    delete self;
    return sq_throwerror(v, _SC("failed to create AnnRprop"));
}

#define SQ_GET_NN_INSTANCE(v, at) SQ_GET_INSTANCE_VAR(v, at, NRTypeObject, self, sq_nn_TAG)

static SQRESULT sq_nn_observe(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_NN_INSTANCE(v, 1);
    SQ_OPT_INTEGER(v, 4, target, NR_INSERT_NO_TARGET);

    SQInteger ilen = ANN_INPUT_UNITS(self->nn);
    SQInteger olen = ANN_OUTPUT_UNITS(self->nn);
    SQInteger oargs = (self->flags & NR_FLAG_CLASSIFIER) ? 1 : olen;

    const SQInteger inputs_pos = 2;
    const SQInteger outputs_pos = 3;

    SQInteger asize_inputs = sq_getsize(v, inputs_pos);
    SQInteger asize_outputs = sq_getsize(v, outputs_pos);

    if((ilen != asize_inputs) || (oargs != asize_outputs))
        return sq_throwerror(v, _SC( "number of arguments does not "
            "match the number of " _PRINT_INT_FMT " inputs and " _PRINT_INT_FMT " outputs in the neural network"),
                             ilen, oargs);

    const SQInteger inputs_alloc_size = sizeof(float)*ilen;
    const SQInteger outputs_alloc_size = sizeof(float)*olen;

    float *inputs = (float*)sq_malloc(inputs_alloc_size);
    for(SQInteger i=0; i < ilen; ++i)
    {
        sq_pushinteger(v, i);
        sq_get(v, inputs_pos);
        SQFloat fnum;
        SQRESULT rc = sq_getfloat(v, -1, &fnum);
        if(rc != SQ_OK)
        {
            sq_free(inputs, inputs_alloc_size);
            return sq_throwerror(v, _SC("only numbers expected on input array"));
        }
        inputs[i] = fnum;
        sq_poptop(v);
    }

    float *outputs = (float*)sq_malloc(outputs_alloc_size);
    for(SQInteger i=0; i < oargs; ++i)
    {
        sq_pushinteger(v, i);
        sq_get(v, outputs_pos);
        SQFloat fnum;
        SQRESULT rc = sq_getfloat(v, -1, &fnum);
        if(rc != SQ_OK)
        {
            sq_free(inputs, inputs_alloc_size);
            sq_free(outputs, outputs_alloc_size);
            return sq_throwerror(v, _SC("only numbers expected on output array"));
        }
        if (self->flags & NR_FLAG_CLASSIFIER) {
            int classid = fnum;
            if (classid != fnum || fnum >= olen || fnum < 0) {
                sq_free(inputs, inputs_alloc_size);
                sq_free(outputs, outputs_alloc_size);
                return sq_throwerror(v, _SC("classifier network output must be an integer "
                    "in the range from 0 to outputs-1."));
            }
            memset(outputs,0, outputs_alloc_size);
            outputs[classid] = 1;
        } else {
            outputs[i] = fnum;
        }

        sq_poptop(v);
    }

    NRTypeInsertData(self,inputs,outputs,target);
    sq_free(inputs, inputs_alloc_size);
    sq_free(outputs, outputs_alloc_size);

	return 0;
}

static SQRESULT sq_nn_train(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_NN_INSTANCE(v, 1);
    SQ_OPT_INTEGER(v, 2, opt_max_cycles, 0);
    SQ_OPT_INTEGER(v, 3, opt_max_ms, 10000);
    SQ_OPT_INTEGER(v, 4, opt_flags, 0);

    NRTypeObject *nr = self;

    nr->training_max_cycles = opt_max_cycles;
    nr->training_max_ms = opt_max_ms;
    if(opt_flags & NR_FLAG_AUTO_STOP) nr->flags |= NR_FLAG_AUTO_STOP;
    if(opt_flags & NR_FLAG_BACKTRACK) nr->flags |= NR_FLAG_BACKTRACK;

    /* Overfitting detection compares error rate in testing/training data,
     * so does not work without entries in the testing dataset. */
    if (nr->flags & NR_FLAG_AUTO_STOP && nr->test.len == 0) {
        return sq_throwerror(v, _SC("Can't start training with AUTOSTOP option: "
            "overfitting detection requires a non zero length testing dataset"));
    }

    int training_iterations = 1;
    float train_error = 0;
    float test_error = 0;
    float class_error = 0;
    float past_train_error = 1.0/0.0;
    float past_test_error = 1.0/0.0;
    int auto_stop = nr->flags & NR_FLAG_AUTO_STOP;
    int backtrack = nr->flags & NR_FLAG_BACKTRACK;

    uint64_t cycles = 0;
    long long start = NRMilliseconds();
    long long cycle_time;
    int overfitting_count = 0;
    int overfitting_limit = 5;
    float best_test_error = 1.0/0.0;

    nr->flags &= ~NR_FLAG_TO_TRANSFER;

    /* If the network is auto normalized, we need to trasnform the inputs
     * in a way that's acceptable for the NN. We just find the maximum
     * absolute value, and divide for it, to get a -1,1 range. There
     * are more advanced transformations that are usually performed that
     * could be implemented in the future.
     *
     * Note that we compute the normalization vectors for all the inputs
     * and outputs, however if the network is a classifier, flagged with
     * (NR_FLAG_CLASSIFIER), no output normalization will be done since
     * the data is already in 0/1 format. */
    if ((nr->flags & NR_FLAG_NORMALIZE) && nr->dataset.len) {
        int ilen = ANN_INPUT_UNITS(nr->nn);
        int olen = ANN_OUTPUT_UNITS(nr->nn);
        float *imax = nr->inorm;
        float *omax = nr->onorm;
        float *inputs = nr->dataset.inputs;
        float *outputs = nr->dataset.outputs;
        for (int i = 0; i < ilen; i++) imax[i] = 1;
        for (int i = 0; i < olen; i++) omax[i] = 1;

        /* Compute the max values vectors. */
        for (uint32_t j = 0; j < nr->dataset.len; j++) {
            for (int i = 0; i < ilen; i++)
                if (fabs(inputs[i]) > imax[i]) imax[i] = fabs(inputs[i]);
            for (int i = 0; i < olen; i++)
                if (fabs(outputs[i]) > omax[i]) omax[i] = fabs(outputs[i]);
            inputs += ilen;
            outputs += olen;
        }

        /* Likely we are not seeing what will really be the true input/output
         * maximum value, so we multiply the maximum values found by a constant.
         * However if the max is exactly "1" we assume it's a classification
         * input and don't alter it. */
        for (int i = 0; i < ilen; i++) if (imax[i] != 1) imax[i] *= 1.2;
        for (int i = 0; i < olen; i++) if (omax[i] != 1) omax[i] *= 1.2;

        /* We can normalize the dataset directly: after the training it will
         * be discarded anyway. */
        inputs = nr->dataset.inputs;
        outputs = nr->dataset.outputs;
        for (uint32_t j = 0; j < nr->dataset.len; j++) {
            for (int i = 0; i < ilen; i++) inputs[i] /= nr->inorm[i];
            if (!(nr->flags & NR_FLAG_CLASSIFIER))
                for (int i = 0; i < olen; i++) outputs[i] /= nr->onorm[i];
            inputs += ilen;
            outputs += olen;
        }

        inputs = nr->test.inputs;
        outputs = nr->test.outputs;
        for (uint32_t j = 0; j < nr->test.len; j++) {
            for (int i = 0; i < ilen; i++) inputs[i] /= nr->inorm[i];
            if (!(nr->flags & NR_FLAG_CLASSIFIER))
                for (int i = 0; i < olen; i++) outputs[i] /= nr->onorm[i];
            inputs += ilen;
            outputs += olen;
        }
    }

    AnnRprop *saved = NULL;  /* Saved to recover on overfitting. */
    float saved_error;          /* The test error of the saved NN. */
    float saved_train_error;    /* The training dataset error of the saved NN */
    float saved_class_error;    /* The % of classification errors of saved NN */

    while(1) {
        long long cycle_start = NRMilliseconds();

        train_error = AnnTrain(nr->nn,
                               nr->dataset.inputs,
                               nr->dataset.outputs,
                               0,
                               training_iterations,
                               nr->dataset.len,
                               ANN_ALGO_BPROP);
        cycle_time = NRMilliseconds() - cycle_start;
        nr->training_total_steps += nr->dataset.len*training_iterations;

        /* Evaluate the error in the case of auto training, stop it
         * once we see that the error in the traning set is decreasing
         * while the one in the test set is not. */
        if (auto_stop) {
            AnnTestError(nr->nn,
                         nr->test.inputs,
                         nr->test.outputs,
                         nr->test.len, &test_error, &class_error);

            if (train_error < past_train_error &&
                test_error > past_test_error)
            {
                overfitting_count++;
                #ifdef NR_TRAINING_DEBUG
                printf("+YCLE %lld: [%d] %f VS %f\n", (long long)cycles,
                    overfitting_count, train_error, test_error);
                #endif
                if (overfitting_count == overfitting_limit) {
                    nr->flags |= NR_FLAG_OF_DETECTED;
                    break;
                }
            } else if (overfitting_count > 0) {
                #ifdef NR_TRAINING_DEBUG
                printf("-YCLE %lld: [%d] %f VS %f\n", (long long)cycles,
                    overfitting_count, train_error, test_error);
                #endif
                overfitting_count--;
            }

            /* Save all the networks with a score better than the currently
             * saved network. This can be a bit costly, but is safe: one
             * cycle of training more and overfitting can ruin it all. */
            if (backtrack && (saved == NULL || test_error < saved_error)) {
                #ifdef NR_TRAINING_DEBUG
                printf("SAVED! %f < %f\n", test_error, saved_error);
                #endif
                saved_error = test_error;
                saved_train_error = train_error;
                saved_class_error = class_error;
                if (saved) AnnFree(saved);
                saved = AnnClone(nr->nn);
            }

            /* Best network found? Reset the overfitting hints counter. */
            if (test_error < best_test_error) {
                overfitting_count = 0;
                best_test_error = test_error;
                #ifdef NR_TRAINING_DEBUG
                printf("BEST! %lld: <%d> %f VS %f\n", (long long)cycles,
                    overfitting_limit,train_error, test_error);
                #endif
            }

           /* Also stop if the loss is zero in both datasets. */
            if (train_error < 0.000000000000001 &&
                test_error  < 0.000000000000001) break;
        }

        cycles++;
        long long total_time = NRMilliseconds()-start;

        /* Cycles and milliseconds stop conditions. */
        if (nr->training_max_cycles && cycles == nr->training_max_cycles)
            break;
        if (nr->training_max_ms && total_time > (long long)nr->training_max_ms)
            break;

        /* If this is a long training, to do just a single training iteration
         * for each cycle is not optimal: tune the number of iterations to
         * at least take 100 milliseconds. */
        if (total_time > 10000 && cycle_time < 100) training_iterations++;

        past_train_error = train_error;
        past_test_error = test_error;
    }

    /* If auto stop is disabled, we still need to compute the test error
     * in order to return this information to the main thread. */
    if (!auto_stop) {
        AnnTestError(nr->nn,
                     nr->test.inputs,
                     nr->test.outputs,
                     nr->test.len, &test_error, &class_error);
    }

    /* If both autostop and backtracking are enabled, we may have
     * a better network saved! */
    if (auto_stop && backtrack) {
        if (saved && saved_error < test_error) {
            #ifdef NR_TRAINING_DEBUG
            printf("BACKTRACK: Saved network used!\n");
            #endif
            AnnFree(nr->nn);
            nr->nn = saved;
            test_error = saved_error;
            train_error = saved_train_error;
            class_error = saved_class_error;
        } else if (saved) {
            AnnFree(saved);
        }
    }

    if (nr->flags & NR_FLAG_CLASSIFIER) nr->test_class_error = class_error;
    nr->dataset_error = train_error;
    nr->test_error = test_error;
    nr->training_total_ms += NRMilliseconds()-start;

	return 0;
}

static SQRESULT sq_nn_run(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_NN_INSTANCE(v, 1);

    SQInteger asize_inputs = sq_getsize(v, 2);
    SQInteger ilen = ANN_INPUT_UNITS(self->nn);
    if(ilen != asize_inputs)
        return sq_throwerror(v, _SC("wrong number of inputs " _PRINT_INT_FMT " for expected " _PRINT_INT_FMT), asize_inputs, ilen);

    for(SQInteger i=0; i < ilen; ++i)
    {
        sq_pushinteger(v, i);
        sq_get(v, 2);
        SQFloat fnum;
        SQRESULT rc = sq_getfloat(v, -1, &fnum);
        if(rc != SQ_OK)
        {
            return sq_throwerror(v, _SC("only numbers expected on input array"));
        }
        if (self->flags & NR_FLAG_NORMALIZE) fnum /= self->inorm[i];
        ANN_INPUT_NODE(self->nn,i) = fnum;
        sq_poptop(v);
    }
    AnnSimulate(self->nn);

    /* Output the raw net output or the class ID if the network
     * is a classifier and the command invoked was NR.CLASS. */
    int olen = ANN_OUTPUT_UNITS(self->nn);
    sq_newarray(v, olen);
    for(int j = 0; j < olen; j++) {
        float output = ANN_OUTPUT_NODE(self->nn,j);
        if (!(self->flags & NR_FLAG_CLASSIFIER) &&
             (self->flags & NR_FLAG_NORMALIZE))
        {
            output *= self->onorm[j];
        }
        sq_pushfloat(v, output);
        sq_arrayset(v, -2, j);
    }

    return 1;
}

static SQRESULT sq_nn_classify(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_NN_INSTANCE(v, 1);

    if (!(self->flags & NR_FLAG_CLASSIFIER))
        return sq_throwerror(v, _SC("you can't call classify with a regressor network."));

    SQInteger asize_inputs = sq_getsize(v, 2);
    SQInteger ilen = ANN_INPUT_UNITS(self->nn);
    if(ilen != asize_inputs)
        return sq_throwerror(v, _SC("wrong number of inputs %d for expected %d"), (int)asize_inputs, (int)ilen);

    for(SQInteger i=0; i < ilen; ++i)
    {
        sq_pushinteger(v, i);
        sq_get(v, 2);
        SQFloat fnum;
        SQRESULT rc = sq_getfloat(v, -1, &fnum);
        if(rc != SQ_OK)
        {
            return sq_throwerror(v, _SC("only numbers expected on input array"));
        }
        if (self->flags & NR_FLAG_NORMALIZE) fnum /= self->inorm[i];
        ANN_INPUT_NODE(self->nn,i) = fnum;
        sq_poptop(v);
    }
    AnnSimulate(self->nn);

    /* Output the raw net output or the class ID if the network
     * is a classifier and the command invoked was NR.CLASS. */
    int olen = ANN_OUTPUT_UNITS(self->nn);
    float fmax = ANN_OUTPUT_NODE(self->nn,0);
    int max_class = 0;
    for(int j = 1; j < olen; j++) {
        float output = ANN_OUTPUT_NODE(self->nn,j);
        if (output > fmax) {
            fmax = output;
            max_class = j;
        }
    }
    sq_pushinteger(v, max_class);

    return 1;
}

#define ADD_T_TABLE_STR(sk, sv) \
    sq_pushstring(v, sk, -1); \
    sq_pushstring(v, sv, -1); \
    sq_rawset(v, -3);

#define ADD_T_TABLE_INT(sk, sv) \
    sq_pushstring(v, sk, -1); \
    sq_pushinteger(v, sv); \
    sq_rawset(v, -3);

#define ADD_T_TABLE_FLOAT(sk, sv) \
    sq_pushstring(v, sk, -1); \
    sq_pushfloat(v, sv); \
    sq_rawset(v, -3);

static SQRESULT sq_nn_info(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_NN_INSTANCE(v, 1);

    sq_newtable(v);

    ADD_T_TABLE_INT("id", self->id);
    ADD_T_TABLE_STR("type", (self->flags & NR_FLAG_CLASSIFIER) ? "classifier" : "regressor");
    ADD_T_TABLE_INT("auto-normalization", !!(self->flags & NR_FLAG_NORMALIZE));
    ADD_T_TABLE_INT("training", !!(self->flags & NR_FLAG_TRAINING));

    sq_pushliteral(v, _SC("layout"));
    sq_newarray(v, ANN_LAYERS(self->nn));
    for (int ai=0, i = ANN_LAYERS(self->nn)-1; i >= 0; i--, ++ai) {
        int units = ANN_UNITS(self->nn,i);
        if (i != 0) units--; /* Don't count the bias unit. */
        sq_pushinteger(v, units);
        sq_arrayset(v, -2, ai);
    }
    sq_rawset(v, -3);

    ADD_T_TABLE_INT("training-dataset-maxlen", self->dataset.maxlen);
    ADD_T_TABLE_INT("training-dataset-len", self->dataset.len);
    ADD_T_TABLE_INT("test-dataset-maxlen", self->test.maxlen);
    ADD_T_TABLE_INT("test-dataset-len", self->test.len);
    ADD_T_TABLE_INT("training-total-steps", self->training_total_steps);
    ADD_T_TABLE_INT("training-total-cycles", self->dataset.len ?
            (self->training_total_steps / self->dataset.len) : 0);

    float tms = (float)self->training_total_ms/1000;
    ADD_T_TABLE_FLOAT("training-total-seconds", tms);
    ADD_T_TABLE_FLOAT("dataset-error", self->dataset_error);
    ADD_T_TABLE_FLOAT("test-error", self->test_error);

    if (self->flags & NR_FLAG_CLASSIFIER) {
        ADD_T_TABLE_FLOAT("classification-errors-perc", self->test_class_error);
    }

    ADD_T_TABLE_STR("overfitting-detected", (self->flags & NR_FLAG_OF_DETECTED) ? "yes" : "no");

	return 1;
}

static SQRESULT sq_nn_clone(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_NN_INSTANCE(v, 1);
    AnnRprop *clone = AnnClone(self->nn);
    if(clone)
    {
        sq_pushstring(v, sq_nn_TAG, -1);
        if(sq_getonregistrytable(v) == SQ_ERROR) return SQ_ERROR;
        sq_createinstance(v, -1);
        sq_setinstanceup(v, -1, clone);
        sq_setreleasehook(v, -1, sq_nn_release_hook);
    }
    else sq_pushnull(v);
	return 1;
}

#define SQ_NN_GET_SET_FLOAT(func_name) \
static SQRESULT sq_nn_##func_name(HSQUIRRELVM v)\
{\
    SQ_FUNC_VARS(v);\
    SQ_GET_NN_INSTANCE(v, 1);\
    if(_top_ == 1)\
    {\
        sq_pushfloat(v, self->nn->func_name);\
        return 1;\
    }\
    SQ_GET_FLOAT(v, 2, func_name);\
    self->nn->func_name = func_name;\
	return 0;\
}

SQ_NN_GET_SET_FLOAT(learn_rate);
SQ_NN_GET_SET_FLOAT(rprop_nminus);
SQ_NN_GET_SET_FLOAT(rprop_nplus);
SQ_NN_GET_SET_FLOAT(rprop_maxupdate);
SQ_NN_GET_SET_FLOAT(rprop_minupdate);

static SQRESULT sq_nn_flags(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_NN_INSTANCE(v, 1);
    if(_top_ == 1)
    {
        sq_pushinteger(v, self->nn->flags);
        return 1;
    }
    SQ_GET_INTEGER(v, 2, flags);
    self->nn->flags = flags;
	return 0;
}

static SQRESULT sq_nn_weights(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_NN_INSTANCE(v, 1);
    sq_pushfloat(v, AnnCountWeights(self->nn));
    return 1;
}

static SQRESULT sq_nn_weight(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_NN_INSTANCE(v, 1);
    SQ_GET_INTEGER(v, 2, layer);
    SQ_GET_INTEGER(v, 3, i);
    SQ_GET_INTEGER(v, 4, j);

    if(layer < 0 && layer >= self->nn->layers) return sq_throwerror(v, _SC("layer out of range"));
    //if(i < 0 && i >= self->layer[layer]) return sq_throwerror(v, _("layer out of range"));

    float *weight = &ANN_WEIGHT(self->nn, layer, i, j);
    if(_top_ == 4)
    {
        sq_pushfloat(v, *weight);
        return 1;
    }
    SQ_GET_FLOAT(v, 5, new_weight);
    *weight = new_weight;
	return 0;
}

static SQRESULT sq_nn_Ann2Tcl(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_NN_INSTANCE(v, 1);
    Ann2Tcl(self->nn);
	return 0;
}

static SQRESULT sq_nn_Ann2Js(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_NN_INSTANCE(v, 1);
    Ann2Js(self->nn);
	return 0;
}

static SQRESULT sq_nn_AnnPrint(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_NN_INSTANCE(v, 1);
    AnnPrint(self->nn);
	return 0;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),sq_nn_##name,nparams,tycheck}
static SQRegFunction sq_nn_methods[] =
{
    _DECL_FUNC(constructor, -5,_SC("xiiaiii")),
    _DECL_FUNC(clone, 1,_SC("x")),
    _DECL_FUNC(Ann2Tcl, 1,_SC("x")),
    _DECL_FUNC(Ann2Js, 1,_SC("x")),
    _DECL_FUNC(AnnPrint, 1,_SC("x")),
    _DECL_FUNC(flags, -1,_SC("xi")),
    _DECL_FUNC(learn_rate, -1,_SC("xf")),
    _DECL_FUNC(rprop_nminus, -1,_SC("xf")),
    _DECL_FUNC(rprop_nplus, -1,_SC("xf")),
    _DECL_FUNC(rprop_maxupdate, -1,_SC("xf")),
    _DECL_FUNC(rprop_minupdate, -1,_SC("xf")),
    _DECL_FUNC(weights, 1,_SC("x")),
    _DECL_FUNC(weight, -4,_SC("xiiif")),
    _DECL_FUNC(observe, -3,_SC("xaai")),
    _DECL_FUNC(train, -1,_SC("xiii")),
    _DECL_FUNC(run, 2,_SC("xa")),
    _DECL_FUNC(classify, 2,_SC("xa")),
    _DECL_FUNC(info, 1,_SC("x")),
    {0,0}
};
#undef _DECL_FUNC

typedef struct {
  const SQChar *Str;
  SQInteger Val;
} KeyIntType, * KeyIntPtrType;

static KeyIntType sqpcre2_constants[] = {
    #define MK_CONST(c) {_SC(#c), NR_##c}
    #define MK_CONST_FLAG(c) {_SC(#c), NR_FLAG_##c}

	MK_CONST_FLAG(NONE),
	MK_CONST_FLAG(TRAINING),
	MK_CONST_FLAG(REGRESSOR),
	MK_CONST_FLAG(CLASSIFIER),
	MK_CONST_FLAG(NORMALIZE),
	MK_CONST_FLAG(AUTO_STOP),
	MK_CONST_FLAG(OF_DETECTED),
	MK_CONST_FLAG(BACKTRACK),
	MK_CONST_FLAG(TO_PERSIST),
	MK_CONST_FLAG(TO_TRANSFER),
	MK_CONST(MAX_LAYERS),
	MK_CONST(RDB_ENC_VER),
	MK_CONST(INSERT_TRAIN),
	MK_CONST(INSERT_TEST),
    {0,0}
};

#ifdef __cplusplus
extern "C" {
#endif
/* This defines a function that opens up your library. */
SQRESULT sqext_register_nn (HSQUIRRELVM v) {
	sq_pushstring(v,sq_nn_TAG,-1);
	sq_newclass(v,SQFalse);
	sq_settypetag(v,-1,(void*)sq_nn_TAG);
    sq_insert_reg_funcs(v, sq_nn_methods);

	//add constants
	KeyIntPtrType KeyIntPtr;
	for (KeyIntPtr = sqpcre2_constants; KeyIntPtr->Str; KeyIntPtr++) {
		sq_pushstring(v, KeyIntPtr->Str, -1);    //first the key
		sq_pushinteger(v, KeyIntPtr->Val);       //then the value
		sq_newslot(v, -3, SQFalse);              //store then
	}

	sq_newslot(v,-3,SQTrue);

	return SQ_OK;
}

#ifdef __cplusplus
}
#endif


