
#include <stdio.h>
#include "squirrel.h"
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
//#include <pthread.h>

SQ_OPT_STRING_STRLEN();

#include "floatfann.h"

struct SQFannTrainData
{
    fann_train_data *data;
};

static const SQChar sq_fann_training_data_TAG[] = _SC("SQFannTrainData");
#define SQ_GET_FANN_TRAINING_DATA_INSTANCE_NAME_AT(v, at, vname) \
    SQ_GET_INSTANCE_VAR(v, at, SQFannTrainData, vname, sq_fann_training_data_TAG)
#define SQ_GET_FANN_TRAINING_DATA_INSTANCE(v, at) \
    SQ_GET_FANN_TRAINING_DATA_INSTANCE_NAME_AT(v, at, self)

static SQRESULT sq_fann_training_data_release_hook(SQUserPointer p, SQInteger size, void */*ep*/) {
    SQFannTrainData *self = (SQFannTrainData *)p;
    if(self->data) fann_destroy_train(self->data);
    return 0;
}

/*
** Creates a new fann_train_data.
*/
static SQRESULT sq_fann_training_data_constructor (HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    fann_train_data *data = NULL;
    SQObjectType otype = sq_gettype(v, 2);
    if(otype == OT_STRING)
    {
        SQ_GET_STRING(v, 2, data_fname);
        data = fann_read_train_from_file(data_fname);
    }
    else if(otype == OT_INTEGER)
    {
        SQ_GET_INTEGER(v, 2, num_data);
        SQ_GET_INTEGER(v, 3, num_input);
        SQ_GET_INTEGER(v, 4, num_output);
        if(num_data <= 0 || num_input <= 0 || num_output <= 0)
            return sq_throwerror(v, _SC("expect only dimensions > 0"));
        data = fann_create_train(num_data, num_input, num_output);
    }
    if(!data) return sq_throwerror(v, _SC("could not create train data"));

    SQFannTrainData *self = (SQFannTrainData*)sq_malloc(sizeof(*self));
    self->data = data;
    sq_setinstanceup(v, 1, self);
    sq_setreleasehook(v, 1, sq_fann_training_data_release_hook);
    return 1;
}

static SQRESULT sq_fann_training_data_shuffle(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_TRAINING_DATA_INSTANCE(v, 1);
    if(self->data) fann_shuffle_train_data(self->data);
    else return sq_throwerror(v, _SC("train data not initialized"));
	return 0;
}

static SQRESULT sq_fann_training_data_get_errstr(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_TRAINING_DATA_INSTANCE(v, 1);
    sq_pushstring(v, fann_get_errstr((struct fann_error *)self->data), -1);
    return 1;
}

static SQRESULT sq_fann_training_data_get_errno(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_TRAINING_DATA_INSTANCE(v, 1);
    sq_pushinteger(v, fann_get_errno((struct fann_error *)self->data));
    return 1;
}

static SQRESULT sq_fann_training_data_reset_errno(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_TRAINING_DATA_INSTANCE(v, 1);
    fann_reset_errno((struct fann_error *)self->data);
    return 0;
}

static SQRESULT sq_fann_training_data_save(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_TRAINING_DATA_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, fname);
    sq_pushinteger(v, fann_save_train(self->data, fname));
    return 1;
}

#define SQ_FANN_TRAINING_DATA_GET_INT(field)\
static SQRESULT sq_fann_training_data_##field(HSQUIRRELVM v)\
{\
    SQ_FUNC_VARS_NO_TOP(v);\
    SQ_GET_FANN_TRAINING_DATA_INSTANCE(v, 1);\
    sq_pushinteger(v, self->data->field);\
    return 1;\
}

SQ_FANN_TRAINING_DATA_GET_INT(num_data);
SQ_FANN_TRAINING_DATA_GET_INT(num_input);
SQ_FANN_TRAINING_DATA_GET_INT(num_output);

static SQRESULT sq_fann_training_data_set_input_at(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_TRAINING_DATA_INSTANCE(v, 1);
    SQ_GET_INTEGER(v, 2, row);
    if(row < 0 || row >= self->data->num_data) return sq_throwerror(v, _SC("index out fo bounds"));
    SQInteger cols = sq_getsize(v, 3);
    if(cols != self->data->num_input) return sq_throwerror(v, _SC("cols mismatch"));

    //fill input
    fann_type **input = self->data->input;
    for(SQInteger i=0; i < cols; ++i)
    {
        sq_arrayget(v, 3, i);
        SQ_GET_FLOAT(v, -1, value);
        input[row][i] = value;
        sq_poptop(v);
    }
    return 0;
}

static SQRESULT sq_fann_training_data_set_output_at(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_TRAINING_DATA_INSTANCE(v, 1);
    SQ_GET_INTEGER(v, 2, row);
    if(row < 0 || row >= self->data->num_data) return sq_throwerror(v, _SC("index out fo bounds"));
    SQInteger cols = sq_getsize(v, 3);
    if(cols != self->data->num_output) return sq_throwerror(v, _SC("cols mismatch"));

    //fill input
    fann_type **output = self->data->output;
    for(SQInteger i=0; i < cols; ++i)
    {
        sq_arrayget(v, 3, i);
        SQ_GET_FLOAT(v, -1, value);
        output[row][i] = value;
        sq_poptop(v);
    }
    return 0;
}

#define SCALE_ALL 0
#define SCALE_INPUT 1
#define SCALE_OUTPUT 2

static SQRESULT sq_fann_training_data_scale_iot(HSQUIRRELVM v, int iot)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_TRAINING_DATA_INSTANCE(v, 1);
    SQ_GET_FLOAT(v, 2, new_min);
    SQ_GET_FLOAT(v, 3, new_max);

    switch(iot)
    {
        case SCALE_ALL:
            fann_scale_train_data(self->data, new_min, new_max);
            break;
        case SCALE_INPUT:
            fann_scale_input_train_data(self->data, new_min, new_max);
            break;
        case SCALE_OUTPUT:
            fann_scale_output_train_data(self->data, new_min, new_max);
            break;
    }
    return 0;
}

static SQRESULT sq_fann_training_data_scale(HSQUIRRELVM v)
{
    return sq_fann_training_data_scale_iot(v, SCALE_ALL);
}

static SQRESULT sq_fann_training_data_scale_input(HSQUIRRELVM v)
{
    return sq_fann_training_data_scale_iot(v, SCALE_INPUT);
}

static SQRESULT sq_fann_training_data_scale_output(HSQUIRRELVM v)
{
    return sq_fann_training_data_scale_iot(v, SCALE_OUTPUT);
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),sq_fann_training_data_##name,nparams,tycheck}
static SQRegFunction sq_fann_training_data_methods[] =
{
    _DECL_FUNC(constructor, -2,_SC("x s|i ii")),
    _DECL_FUNC(shuffle, 1,_SC("x")),
    _DECL_FUNC(get_errstr, 1,_SC("x")),
    _DECL_FUNC(get_errno, 1,_SC("x")),
    _DECL_FUNC(reset_errno, 1,_SC("x")),
    _DECL_FUNC(num_data, 1,_SC("x")),
    _DECL_FUNC(num_input, 1,_SC("x")),
    _DECL_FUNC(num_output, 1,_SC("x")),
    _DECL_FUNC(save, 2,_SC("xs")),
    _DECL_FUNC(set_input_at, 3,_SC("xia")),
    _DECL_FUNC(set_output_at, 3,_SC("xia")),
    _DECL_FUNC(scale, 3,_SC("xnn")),
    _DECL_FUNC(scale_input, 3,_SC("xnn")),
    _DECL_FUNC(scale_output, 3,_SC("xnn")),

    {0,0}
};
#undef _DECL_FUNC

struct SQFann
{
    fann *ann;
};

struct SQFannCallback
{
    HSQUIRRELVM v;
    HSQOBJECT cb;
    HSQOBJECT udata;
};

static const SQChar sq_fann_TAG[] = _SC("SQFann");
#define SQ_GET_FANN_INSTANCE_NAME_AT(v, at, vname) SQ_GET_INSTANCE_VAR(v, at, SQFann, vname, sq_fann_TAG)
#define SQ_GET_FANN_INSTANCE(v, at) SQ_GET_FANN_INSTANCE_NAME_AT(v, at, self)

static void release_sq_fann_callback(SQFannCallback *cb)
{
    sq_release(cb->v, &cb->cb);
    sq_release(cb->v, &cb->udata);
    sq_free(cb, sizeof(*cb));
}

static SQRESULT sq_fann_release_hook(SQUserPointer p, SQInteger size, void */*ep*/) {
    SQFann *self = (SQFann *)p;
    if(self->ann)
    {
        SQFannCallback *cb = (SQFannCallback*)fann_get_user_data(self->ann);
        if(cb) release_sq_fann_callback(cb);
        fann_destroy(self->ann);
    }
    return 0;
}

/*
** Creates a new fann.
*/
static SQRESULT sq_fann_constructor (HSQUIRRELVM v) {
    SQ_FUNC_VARS(v);
    fann *ann;
    SQObjectType otype = sq_gettype(v, 2);
    if(otype == OT_STRING)
    {
        SQ_GET_STRING(v, 2, net_fname);
        ann = fann_create_from_file(net_fname);
    }
    else
    {
        int create_type = FANN_NETTYPE_LAYER;
        if(_top_ > 2)
        {
            SQ_GET_INTEGER(v, 3, nt);
            create_type = nt;
        }
        switch(create_type)
        {
        case FANN_NETTYPE_LAYER:
        case FANN_NETTYPE_SHORTCUT:
            break;
        default:
            return sq_throwerror(v, _SC("invalid net type"));
        }

        SQFloat connection_rate;
        SQInteger array_pos = 2;
        if(otype == OT_FLOAT)
        {
            ++array_pos;
            SQ_GET_FLOAT(v, 2, cr);
            connection_rate = cr;
        }
        SQInteger num_layers = sq_getsize(v, array_pos);
        unsigned int *layers = (unsigned int *)
            sq_getscratchpad(v, num_layers*sizeof(unsigned int));
        for(SQInteger i=0; i < num_layers; ++i)
        {
            sq_arrayget(v, array_pos, i);
            SQ_GET_INTEGER(v, -1, value);
            layers[i] = value;
            sq_poptop(v);
        }
        if(array_pos == 2)
        {
            if(create_type == FANN_NETTYPE_LAYER)
                ann = fann_create_standard_array(num_layers, layers);
            else if(create_type == FANN_NETTYPE_SHORTCUT)
                ann = fann_create_shortcut_array(num_layers, layers);
        }
        else ann = fann_create_sparse_array(connection_rate, num_layers, layers);
    }

    if(ann)
    {
        SQFann *self = (SQFann*)sq_malloc(sizeof(*self));
        self->ann = ann;
        sq_setinstanceup(v, 1, self);
        sq_setreleasehook(v, 1, sq_fann_release_hook);
        return 1;
    }
    return sq_throwerror(v, _SC("failed to create SQFann"));
}

static SQRESULT sq_fann_copy(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    fann *ann = fann_copy(self->ann);
    if(ann)
    {
        SQFann *new_self = (SQFann*)sq_malloc(sizeof(*self));
        new_self->ann = ann;
        sq_pushstring(v, sq_fann_TAG, -1);
        sq_getonroottable(v);
        sq_createinstance(v, -1);
        sq_setinstanceup(v, 1, new_self);
        sq_setreleasehook(v, 1, sq_fann_release_hook);
        return 1;
    }
    return sq_throwerror(v, _SC("failed to create SQFann"));
}

static int sq_fann_callback_c(fann *ann, fann_train_data *train,
                    unsigned int max_epochs, unsigned int epochs_between_reports,
                    float desired_error, unsigned int epochs)
{
    SQFannCallback *cb = (SQFannCallback*)fann_get_user_data(ann);
    if(cb)
    {
        /* ensure there is enough space in the stack */
        sq_reservestack(cb->v, 20);
        SQInteger top = sq_gettop(cb->v);

        sq_pushobject(cb->v, cb->cb);
        sq_pushroottable(cb->v);
        sq_pushobject(cb->v, cb->udata);
        sq_pushinteger(cb->v, max_epochs);
        sq_pushinteger(cb->v, epochs_between_reports);
        sq_pushfloat(cb->v, desired_error);
        sq_pushinteger(cb->v, epochs);

        /* call squilu function */
        SQInteger rc = 0;
        if (sq_call(cb->v, 6, SQTrue, SQFalse) == SQ_OK)
            sq_getinteger(cb->v, -1, &rc);

        sq_settop(cb->v, top);
        return rc;
    }
    return 0;
}

static SQRESULT sq_fann_set_callback(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    SQFannCallback *cb = (SQFannCallback*)fann_get_user_data(self->ann);
    if(cb) release_sq_fann_callback(cb);
    cb = (SQFannCallback*)sq_malloc(sizeof(*cb));
    cb->v = v;
    sq_resetobject(&cb->cb);
    sq_getstackobj(v, 2, &cb->cb);
    sq_addref(v, &cb->cb);
    sq_resetobject(&cb->udata);
    if(_top_ > 2)
    {
        sq_getstackobj(v, 3, &cb->udata);
        sq_addref(v, &cb->udata);
    }
    fann_set_user_data(self->ann, cb);
    fann_set_callback(self->ann, sq_fann_callback_c);
    return 0;
}

static SQRESULT sq_fann_learning_rate(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    if(_top_ > 1)
    {
        SQ_GET_FLOAT(v, 2, learn_rate);
        fann_set_learning_rate(self->ann, learn_rate);
        return 0;
    }
    sq_pushfloat(v, fann_get_learning_rate(self->ann));
	return 1;
}

static SQRESULT sq_fann_learning_momentum(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    if(_top_ > 1)
    {
        SQ_GET_FLOAT(v, 2, value);
        fann_set_learning_momentum(self->ann, value);
        return 0;
    }
    sq_pushfloat(v, fann_get_learning_momentum(self->ann));
	return 1;
}

static SQRESULT sq_fann_training_algorithm(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    if(_top_ > 1)
    {
        SQ_GET_INTEGER(v, 2, value);
        fann_set_training_algorithm(self->ann, (fann_train_enum)value);
        return 0;
    }
    sq_pushinteger(v, fann_get_training_algorithm(self->ann));
	return 1;
}

static SQRESULT sq_fann_train_error_function(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    if(_top_ > 1)
    {
        SQ_GET_INTEGER(v, 2, value);
        fann_set_train_error_function(self->ann, (fann_errorfunc_enum)value);
        return 0;
    }
    sq_pushinteger(v, fann_get_train_error_function(self->ann));
	return 1;
}

static SQRESULT sq_fann_train_stop_function(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    if(_top_ > 1)
    {
        SQ_GET_INTEGER(v, 2, value);
        fann_set_train_stop_function(self->ann, (fann_stopfunc_enum)value);
        return 0;
    }
    sq_pushinteger(v, fann_get_train_stop_function(self->ann));
	return 1;
}

static SQRESULT sq_fann_activation_steepness(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    if(_top_ > 3)
    {
        SQ_GET_FLOAT(v, 2, value);
        SQ_GET_INTEGER(v, 3, layer);
        SQ_GET_INTEGER(v, 4, neuron);
        fann_set_activation_steepness(self->ann, (fann_type)value, layer, neuron);
        return 0;
    }
    SQ_GET_INTEGER(v, 2, layer);
    SQ_GET_INTEGER(v, 3, neuron);
    sq_pushfloat(v, fann_get_activation_steepness(self->ann, layer, neuron));
	return 1;
}

#define SQFANN_GET_FLOAT_OR_INT(stype, func_name) \
static SQRESULT sq_fann_##func_name(HSQUIRRELVM v) \
{ \
    SQ_FUNC_VARS_NO_TOP(v); \
    SQ_GET_FANN_INSTANCE(v, 1); \
    sq_push##stype(v, fann_##func_name(self->ann)); \
    return 1; \
}

#define SQFANN_GET_FLOAT(func_name) \
    SQFANN_GET_FLOAT_OR_INT(float, func_name)

#define SQFANN_GET_INTEGER(func_name) \
    SQFANN_GET_FLOAT_OR_INT(integer, func_name)

#define SQFANN_SET_FLOAT_OR_INT(stype, func_name, cast_type) \
static SQRESULT sq_fann_##func_name(HSQUIRRELVM v) \
{ \
    SQ_FUNC_VARS_NO_TOP(v); \
    SQ_GET_FANN_INSTANCE(v, 1); \
    SQ_GET_##stype(v, 2, value); \
    fann_##func_name(self->ann, (cast_type)value); \
    return 0; \
}

#define SQFANN_SET_FLOAT(func_name, cast_type) \
    SQFANN_SET_FLOAT_OR_INT(FLOAT, func_name, cast_type)

#define SQFANN_SET_INTEGER(func_name, cast_type) \
    SQFANN_SET_FLOAT_OR_INT(INTEGER, func_name, cast_type)

SQFANN_SET_FLOAT(set_activation_steepness_hidden, fann_type);
SQFANN_SET_FLOAT(set_activation_steepness_output, fann_type);
SQFANN_SET_FLOAT(set_quickprop_decay, fann_type);
SQFANN_SET_FLOAT(set_quickprop_mu, fann_type);
SQFANN_SET_FLOAT(set_rprop_increase_factor, fann_type);
SQFANN_SET_FLOAT(set_rprop_decrease_factor, fann_type);
SQFANN_SET_FLOAT(set_rprop_delta_min, fann_type);
SQFANN_SET_FLOAT(set_rprop_delta_max, fann_type);
SQFANN_SET_FLOAT(set_cascade_output_change_fraction, fann_type);
SQFANN_SET_INTEGER(set_cascade_output_stagnation_epochs, unsigned);
SQFANN_SET_FLOAT(set_cascade_candidate_change_fraction, fann_type);
SQFANN_SET_INTEGER(set_cascade_candidate_stagnation_epochs, unsigned);
SQFANN_SET_FLOAT(set_cascade_weight_multiplier, fann_type);
SQFANN_SET_FLOAT(set_cascade_candidate_limit, fann_type);
SQFANN_SET_INTEGER(set_cascade_max_out_epochs, unsigned);
SQFANN_SET_INTEGER(set_cascade_max_cand_epochs, unsigned);
SQFANN_SET_INTEGER(set_cascade_num_candidate_groups, unsigned);
SQFANN_SET_INTEGER(set_activation_function_hidden, fann_activationfunc_enum);
SQFANN_SET_INTEGER(set_activation_function_output, fann_activationfunc_enum);


static SQRESULT sq_fann_randomize_weights(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    SQ_GET_FLOAT(v, 2, min_weight);
    SQ_GET_FLOAT(v, 3, max_weight);
    fann_randomize_weights(self->ann, min_weight, max_weight);
    return 0;
}

static SQRESULT sq_fann_reset_MSE(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    fann_reset_MSE(self->ann);
    return 0;
}

SQFANN_GET_FLOAT(get_MSE);

static SQRESULT sq_fann_get_errstr(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    sq_pushstring(v, fann_get_errstr((struct fann_error *)self->ann), -1);
    return 1;
}

static SQRESULT sq_fann_get_errno(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    sq_pushinteger(v, fann_get_errno((struct fann_error *)self->ann));
    return 1;
}

static SQRESULT sq_fann_reset_errno(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    fann_reset_errno((struct fann_error *)self->ann);
    return 0;
}

SQFANN_GET_INTEGER(get_num_input);
SQFANN_GET_INTEGER(get_num_output);
SQFANN_GET_INTEGER(get_bit_fail);

static SQRESULT sq_fann_train_on_data_type(HSQUIRRELVM v, bool isCascade)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    SQ_GET_FANN_TRAINING_DATA_INSTANCE_NAME_AT(v, 2, data);
    SQ_GET_INTEGER(v, 3, max_epochs);
    SQ_GET_INTEGER(v, 4, epochs_between_reports);
    SQ_GET_FLOAT(v, 5, desired_error);
    if(isCascade)
        fann_cascadetrain_on_data(self->ann, data->data, max_epochs,
                           epochs_between_reports, desired_error);
    else
        fann_train_on_data(self->ann, data->data, max_epochs,
                           epochs_between_reports, desired_error);
    return 0;
}

static SQRESULT sq_fann_train_on_data(HSQUIRRELVM v)
{
    return sq_fann_train_on_data_type(v, false);
}

static SQRESULT sq_fann_cascadetrain_on_data(HSQUIRRELVM v)
{
    return sq_fann_train_on_data_type(v, true);
}

static SQRESULT sq_fann_train_on_file(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, data_fn);
    SQ_GET_INTEGER(v, 3, max_epochs);
    SQ_GET_INTEGER(v, 4, epochs_between_reports);
    SQ_GET_FLOAT(v, 5, desired_error);
    fann_train_on_file(self->ann, data_fn, max_epochs,
                       epochs_between_reports, desired_error);
    return 0;
}

static SQRESULT sq_fann_test_data(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    SQ_GET_FANN_TRAINING_DATA_INSTANCE_NAME_AT(v, 2, data);
    sq_pushfloat(v, fann_test_data(self->ann, data->data));
    return 1;
}

static SQRESULT sq_fann_test(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    SQInteger isize = sq_getsize(v, 2);
    SQInteger ann_num_input = fann_get_num_input(self->ann);
    if(isize != ann_num_input) return sq_throwerror(v, _SC("wrong number of inputs"));
    SQInteger osize = sq_getsize(v, 3);
    SQInteger ann_num_output = fann_get_num_output(self->ann);
    if(osize != ann_num_output) return sq_throwerror(v, _SC("wrong number of outputs"));

    fann_type *data = (fann_type*)sq_getscratchpad(v, (osize+isize)*sizeof(fann_type));
    fann_type *input = data;
    fann_type *output = data+isize;

    for(SQInteger i=0; i < isize; ++i)
    {
        sq_arrayget(v, 2, i);
        SQ_GET_FLOAT(v, -1, value);
        input[i] = value;
        sq_poptop(v);
    }

    for(SQInteger i=0; i < osize; ++i)
    {
        sq_arrayget(v, 3, i);
        SQ_GET_FLOAT(v, -1, value);
        output[i] = value;
        sq_poptop(v);
    }

    fann_type *calc_output = fann_test(self->ann, input, output);

    sq_newarray(v, ann_num_output);
    for(SQInteger i=0; i < ann_num_output; ++i)
    {
        sq_pushfloat(v, calc_output[i]);
        sq_arrayset(v, -2, i);
    }

    return 1;
}

static SQRESULT sq_fann_run(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    SQInteger isize = sq_getsize(v, 2);
    SQInteger ann_num_input = fann_get_num_input(self->ann);
    if(isize != ann_num_input) return sq_throwerror(v, _SC("wrong number of inputs"));
    SQInteger ann_num_output = fann_get_num_output(self->ann);
    fann_type *input = (fann_type*)sq_getscratchpad(v, isize*sizeof(fann_type));
    fann_type *calc_output;

    for(SQInteger i=0; i < isize; ++i)
    {
        sq_arrayget(v, 2, i);
        SQ_GET_FLOAT(v, -1, value);
        input[i] = value;
        sq_poptop(v);
    }
    calc_output = fann_run(self->ann, input);
    if(!calc_output) return sq_throwerror(v, _SC("error running ann"));

    sq_newarray(v, ann_num_output);
    for(SQInteger i=0; i < ann_num_output; ++i)
    {
        sq_pushfloat(v, calc_output[i]);
        sq_arrayset(v, -2, i);
    }
    return 1;
}

static SQRESULT sq_fann_set_scaling_params(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    SQ_GET_FANN_TRAINING_DATA_INSTANCE_NAME_AT(v, 2, data);
    SQ_GET_FLOAT(v, 3, new_input_min);
    SQ_GET_FLOAT(v, 4, new_input_max);
    SQ_GET_FLOAT(v, 5, new_output_min);
    SQ_GET_FLOAT(v, 6, new_output_max);
    fann_set_scaling_params(self->ann, data->data,
            new_input_min, new_input_max, new_output_min, new_output_max);
    return 0;
}

static SQRESULT sq_fann_set_input_ouput_scaling_params(HSQUIRRELVM v, bool isInput)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    SQ_GET_FANN_TRAINING_DATA_INSTANCE_NAME_AT(v, 2, data);
    SQ_GET_FLOAT(v, 3, new_min);
    SQ_GET_FLOAT(v, 4, new_max);
    if(isInput) fann_set_input_scaling_params(self->ann, data->data, new_min, new_max);
    else fann_set_output_scaling_params(self->ann, data->data, new_min, new_max);
    return 0;
}

static SQRESULT sq_fann_set_input_scaling_params(HSQUIRRELVM v)
{
    return sq_fann_set_input_ouput_scaling_params(v, true);
}

static SQRESULT sq_fann_set_output_scaling_params(HSQUIRRELVM v)
{
    return sq_fann_set_input_ouput_scaling_params(v, false);
}

static SQRESULT sq_fann_init_weights(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    SQ_GET_FANN_TRAINING_DATA_INSTANCE_NAME_AT(v, 2, data);
    fann_init_weights(self->ann, data->data);
    return 0;
}

static SQRESULT sq_fann_scale_train(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    SQ_GET_FANN_TRAINING_DATA_INSTANCE_NAME_AT(v, 2, data);
    fann_scale_train(self->ann, data->data);
    return 0;
}

static SQRESULT sq_fann_scale_input(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    SQ_GET_FLOAT(v, 2, value);
    fann_type fv = value;
    fann_scale_input(self->ann, &fv);
    sq_pushfloat(v, fv);
    return 1;
}

static SQRESULT sq_fann_descale_output(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    SQ_GET_FLOAT(v, 2, value);
    fann_type fv = value;
    fann_descale_input(self->ann, &fv);
    sq_pushfloat(v, fv);
    return 1;
}

static SQRESULT sq_fann_save(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, net_fname);
    fann_save(self->ann, net_fname);
    return 0;
}

static SQRESULT sq_fann_print_connections(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    fann_print_connections(self->ann);
    return 0;
}

static SQRESULT sq_fann_print_parameters(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FANN_INSTANCE(v, 1);
    fann_print_parameters(self->ann);
    return 0;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),sq_fann_##name,nparams,tycheck}
static SQRegFunction sq_fann_methods[] =
{
    _DECL_FUNC(constructor, -2,_SC("x s|f|a i")),
    _DECL_FUNC(copy, 1,_SC("x")),
    _DECL_FUNC(save, 2,_SC("xs")),
    _DECL_FUNC(learning_rate, -1,_SC("xf")),
    _DECL_FUNC(learning_momentum, -1,_SC("xf")),
    _DECL_FUNC(training_algorithm, -1,_SC("xi")),
    _DECL_FUNC(train_error_function, -1,_SC("xi")),
    _DECL_FUNC(train_stop_function, -1,_SC("xi")),
    _DECL_FUNC(set_activation_function_hidden, 2,_SC("xi")),
    _DECL_FUNC(set_activation_function_output, 2,_SC("xi")),
    _DECL_FUNC(activation_steepness, -3,_SC("xnnn")),
    _DECL_FUNC(set_activation_steepness_hidden, 2,_SC("xn")),
    _DECL_FUNC(set_activation_steepness_output, 2,_SC("xn")),
    _DECL_FUNC(set_quickprop_decay, 2,_SC("xn")),
    _DECL_FUNC(set_quickprop_mu, 2,_SC("xn")),
    _DECL_FUNC(set_rprop_increase_factor, 2,_SC("xn")),
    _DECL_FUNC(set_rprop_decrease_factor, 2,_SC("xn")),
    _DECL_FUNC(set_rprop_delta_min, 2,_SC("xn")),
    _DECL_FUNC(set_rprop_delta_max, 2,_SC("xn")),
    _DECL_FUNC(set_cascade_output_change_fraction, 2,_SC("xn")),
    _DECL_FUNC(set_cascade_output_stagnation_epochs, 2,_SC("xi")),
    _DECL_FUNC(set_cascade_candidate_change_fraction, 2,_SC("xn")),
    _DECL_FUNC(set_cascade_candidate_stagnation_epochs, 2,_SC("xi")),
    _DECL_FUNC(set_cascade_weight_multiplier, 2,_SC("xn")),
    _DECL_FUNC(set_cascade_candidate_limit, 2,_SC("xn")),
    _DECL_FUNC(set_cascade_max_out_epochs, 2,_SC("xi")),
    _DECL_FUNC(set_cascade_max_cand_epochs, 2,_SC("xi")),
    _DECL_FUNC(set_cascade_num_candidate_groups, 2,_SC("xi")),
    _DECL_FUNC(randomize_weights, 3,_SC("xnn")),
    _DECL_FUNC(reset_MSE, 1,_SC("x")),
    _DECL_FUNC(get_MSE, 1,_SC("x")),
    _DECL_FUNC(get_errstr, 1,_SC("x")),
    _DECL_FUNC(get_errno, 1,_SC("x")),
    _DECL_FUNC(reset_errno, 1,_SC("x")),
    _DECL_FUNC(get_num_input, 1,_SC("x")),
    _DECL_FUNC(get_num_output, 1,_SC("x")),
    _DECL_FUNC(get_bit_fail, 1,_SC("x")),
    _DECL_FUNC(print_connections, 1,_SC("x")),
    _DECL_FUNC(print_parameters, 1,_SC("x")),
    _DECL_FUNC(train_on_data, 5,_SC("xxiif")),
    _DECL_FUNC(cascadetrain_on_data, 5,_SC("xxiif")),
    _DECL_FUNC(train_on_file, 5,_SC("xsiif")),
    _DECL_FUNC(test, 3,_SC("xaa")),
    _DECL_FUNC(test_data, 2,_SC("xx")),
    _DECL_FUNC(run, 2,_SC("xa")),
    _DECL_FUNC(set_scaling_params, 6,_SC("xxnnnn")),
    _DECL_FUNC(set_input_scaling_params, 4,_SC("xxnn")),
    _DECL_FUNC(set_output_scaling_params, 4,_SC("xxnn")),
    _DECL_FUNC(init_weights, 2,_SC("xx")),
    _DECL_FUNC(scale_train, 2,_SC("xx")),
    _DECL_FUNC(scale_input, 2,_SC("xn")),
    _DECL_FUNC(descale_output, 2,_SC("xn")),
    _DECL_FUNC(set_callback, -2,_SC("xc.")),

    {0,0}
};
#undef _DECL_FUNC

typedef struct {
  const SQChar *Str;
  SQInteger Val;
} KeyIntType, * KeyIntPtrType;

static KeyIntType module_constants[] = {
    #define MK_CONST(c) {_SC(#c), FANN_##c}

	MK_CONST(LINEAR),
	MK_CONST(THRESHOLD),
	MK_CONST(THRESHOLD_SYMMETRIC),
	MK_CONST(SIGMOID),
	MK_CONST(SIGMOID_STEPWISE),
	MK_CONST(SIGMOID_SYMMETRIC),
	MK_CONST(SIGMOID_SYMMETRIC_STEPWISE),
	MK_CONST(GAUSSIAN),
	MK_CONST(GAUSSIAN_STEPWISE),
	MK_CONST(ELLIOT),
	MK_CONST(ELLIOT_SYMMETRIC),
	MK_CONST(GAUSSIAN_SYMMETRIC),
	MK_CONST(LINEAR_PIECE),
	MK_CONST(LINEAR_PIECE_SYMMETRIC),
	MK_CONST(SIN_SYMMETRIC),
	MK_CONST(COS_SYMMETRIC),
	MK_CONST(SIN),
	MK_CONST(COS),

	MK_CONST(TRAIN_INCREMENTAL),
	MK_CONST(TRAIN_BATCH),
	MK_CONST(TRAIN_RPROP),
	MK_CONST(TRAIN_QUICKPROP),
	MK_CONST(TRAIN_SARPROP),

	MK_CONST(ERRORFUNC_LINEAR),
	MK_CONST(ERRORFUNC_TANH),

	MK_CONST(STOPFUNC_MSE),
	MK_CONST(STOPFUNC_BIT),

	MK_CONST(NETTYPE_LAYER),
	MK_CONST(NETTYPE_SHORTCUT),
    {0,0}
};

#ifdef __cplusplus
extern "C" {
#endif
/* This defines a function that opens up your library. */
SQRESULT sqext_register_fann (HSQUIRRELVM v) {
	sq_pushstring(v,sq_fann_training_data_TAG,-1);
	sq_newclass(v,SQFalse);
	sq_settypetag(v,-1,(void*)sq_fann_training_data_TAG);
    sq_insert_reg_funcs(v, sq_fann_training_data_methods);

	sq_newslot(v,-3,SQTrue);

	sq_pushstring(v,sq_fann_TAG,-1);
	sq_newclass(v,SQFalse);
	sq_settypetag(v,-1,(void*)sq_fann_TAG);
    sq_insert_reg_funcs(v, sq_fann_methods);

	//add constants
	KeyIntPtrType KeyIntPtr;
	for (KeyIntPtr = module_constants; KeyIntPtr->Str; KeyIntPtr++) {
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
