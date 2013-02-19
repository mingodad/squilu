#ifndef DYNAMIC_LIBRAY_H
#define DYNAMIC_LIBRAY_H

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
