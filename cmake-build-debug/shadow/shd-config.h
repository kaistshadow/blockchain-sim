#ifndef BYTE_ORDER
# define BIG_ENDIAN 1
# define LITTLE_ENDIAN 2
/* #undef IS_BIG_ENDIAN */
# ifdef IS_BIG_ENDIAN
#  define BYTE_ORDER BIG_ENDIAN
# else
#  define BYTE_ORDER LITTLE_ENDIAN
# endif
#endif

#define SHADOW_VERSION_STRING "Shadow 2020-03-12 (built 2020-03-19)"
#define SHADOW_INFO_STRING "For more information, visit https://shadow.github.io or https://github.com/shadow"
