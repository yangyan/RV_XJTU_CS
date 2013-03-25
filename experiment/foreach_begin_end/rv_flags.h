#ifdef THIS_IS_CPP
#define FLAGS_EXTERN(type,name,value) type name = value
#else
#define FLAGS_EXTERN(type,name,value) extern type name
#endif

FLAGS_EXTERN(bool, fbe_error, false);
