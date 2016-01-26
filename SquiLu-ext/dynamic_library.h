#ifndef DYNAMIC_LIBRAY_H
#define DYNAMIC_LIBRAY_H

#if defined(WIN32)
#define DYNLIB_FOR_OS(fn) #fn ".dll"
#elif defined(__APPLE__)
#define DYNLIB_FOR_OS(fn) #fn ".dylib"
#else
#define DYNLIB_FOR_OS(fn) #fn ".so"
#endif

class DynamicLibrary
{
protected:
  void * handleLib;
public:
  DynamicLibrary ();
  virtual ~ DynamicLibrary ();
  bool open (char const * libname);
  bool close ();
  void * dlsym (char const * sym_name);
};

#endif //DYNAMIC_LIBRAY_H
