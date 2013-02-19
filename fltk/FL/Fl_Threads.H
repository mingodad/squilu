#ifndef Fl_Threads_H
#define Fl_Threads_H

typedef void *(*tread_func_t) (void *);

#if !defined( _WIN32) || defined(__CYGWIN__)
// pthreads:

#include <pthread.h>

/// \name fltk/Threads.h
//@{

/** Hides whatever the system uses to identify a thread. Used so
  the "toy" interface is portable. */
typedef pthread_t Thread;

/** Fork a new thread and make it run \a f(p). Returns negative number
  on error, otherwise \a t is set to the new thread. */
inline int create_thread(Thread& t, tread_func_t f, void* p) {
  return pthread_create((pthread_t*)&t, 0, f, p);
}

/**
  "Mutual-exclusion lock" for simple multithreaded programs.  Calling
  lock() will wait until nobody else has the lock and then will
  return. <i>Calling lock() more than once will "deadlock"!</i>
  To avoid this, use RecursiveMutex.
*/
class Fl_Mutex {
  friend class Fl_SignalMutex;
  pthread_mutex_t mutex;
  Fl_Mutex(const Fl_Mutex&);
  Fl_Mutex& operator=(const Fl_Mutex&);
protected:
  Fl_Mutex(const pthread_mutexattr_t* a) {pthread_mutex_init(&mutex, a);}
public:
  Fl_Mutex() {pthread_mutex_init(&mutex, 0);}
  void lock() {pthread_mutex_lock(&mutex);}
  void unlock() {pthread_mutex_unlock(&mutex);}
  bool trylock() {return pthread_mutex_trylock(&mutex) == 0;}
  ~Fl_Mutex() {pthread_mutex_destroy(&mutex);}
};

/**
  A portable "semaphore". A thread that holds this lock() can call
  wait(), which will unlock it, then wait for another thread to
  call signal(), then lock() it again.

  The other thread can call signal() at any time, though usually
  it will have called lock() as well, as the lock can be used to
  protect the data that is actually being shared between the threads.

  If more than one thread is in wait(), then calling signal_one()
  will only wake one of them up. This may be more efficient, and
  can be done safely if all threads that call wait() also call
  signal_one() just before calling unlock().

  Warning: wait() can return even if signal() was not called. You
  must then check other data (protected by the lock()) to see if
  the condition really is fulfilled. In many cases this is the
  best implementation, it is also necessary to work around design
  errors in Windows, where always returns after 1/2 second to
  avoid a deadlock due to the non-atomic nature of Windows calls.
*/
class Fl_SignalMutex : public Fl_Mutex {
  pthread_cond_t cond;
public:
  Fl_SignalMutex() : Fl_Mutex() {pthread_cond_init(&cond, 0);}
  void signal() {pthread_cond_broadcast(&cond);}
  void signal_one() {pthread_cond_signal(&cond);}
  void wait() {pthread_cond_wait(&cond, &mutex);}
};

// Linux supports recursive locks, use them directly, with some cheating:
#if defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP) || defined(PTHREAD_MUTEX_RECURSIVE)

class Fl_RecursiveMutex : public Fl_Mutex {
public:
  Fl_RecursiveMutex();
};

#else // standard pthread mutexes need a bit of work to be recursive:

/**
  "Mutual exclusion lock" to protect data in multithreaded programs.
  This is a "recursive lock". Calling lock() will wait until nobody
  else has the lock and then will take it. Calling lock() multiple
  times by the same thread is allowed, and unlock() must then be
  called the same number of times before another thread can get the
  lock.
*/
class Fl_RecursiveMutex : public Fl_Mutex {
  pthread_t owner;
  int counter;
public:
  Fl_RecursiveMutex() : Fl_Mutex(), counter(0) {}
  void lock() {
    if (!counter || owner != pthread_self()) {
      Fl_Mutex::lock();
      owner = pthread_self();
      counter = 1;
    } else {
      ++counter;
    }
  }
  bool trylock() {
    if (!counter || owner != pthread_self()) {
      if (!Fl_Mutex::trylock()) return false;
      owner = pthread_self();
    }
    counter++;
    return true;
  }
  void unlock() {if (!--counter) Fl_Mutex::unlock();}
};

#endif

#else // _WIN32:

//# define _WIN32_WINNT 0x0500
# include <windows.h>
# include <process.h>
// undefine some of the more annoying crap:
# undef DELETE
# undef ERROR
# undef IN
# undef OUT
# undef POINT
# undef far
# undef max
# undef min
# undef near

typedef unsigned long Thread;

inline int create_thread(Thread& t, tread_func_t f, void* p) {
  return t = (Thread)_beginthread((void( __cdecl * )( void * ))f, 0, p);
}

class Fl_Mutex {
  CRITICAL_SECTION cs;
  Fl_Mutex(const Fl_Mutex&);
  Fl_Mutex& operator=(const Fl_Mutex&);
public:
  Fl_Mutex() {InitializeCriticalSection(&cs);}
  void lock() {while (!TryEnterCriticalSection(&cs)) SwitchToThread();}
  void unlock() {LeaveCriticalSection(&cs);}
  bool trylock() {return TryEnterCriticalSection(&cs);}
  ~Fl_Mutex() {DeleteCriticalSection(&cs);}
};

// After many experiments we have determined that this very stupid
// implementation has the lowest overhead:
class Fl_SignalMutex : public Fl_Mutex {
public:
  Fl_SignalMutex() : Fl_Mutex() {}
  void signal() {}
  void signal_one() {}
  void wait() {
    // the following three calls should be atomic, sigh...
    unlock();
    SwitchToThread();
    lock();
  }
};

typedef Fl_Mutex Fl_RecursiveMutex;

#endif

/**
   C++ convienence object for locking a Fl_Mutex.
   Creating a local one of these will lock() the mutex and it means
   unlock() will be called no matter how a function exits, because
   the destructor ~Fl_Mutex_Guard() does an unlock().

\code
   static Fl_Mutex mutex;
   function() {
     Fl_Mutex:Guard guard(mutex);
     do_stuff;
     throw_exceptions;
     if (test()) return;
     etc;
   }
\endcode

*/
class Fl_Mutex_Guard {
  Fl_Mutex& lock;
 public:
  Fl_Mutex_Guard(Fl_Mutex& m) : lock(m) {lock.lock();}
  Fl_Mutex_Guard(Fl_Mutex* m) : lock(*m) {lock.lock();}
  ~Fl_Mutex_Guard() {lock.unlock();}
};

//@}

#endif
