#include "faster_mri.h"

#define Array LongArray
#define Array_(x) LongArray_##x
#define Item long
#include "Array_defs.h"
#undef Array
#undef Array_
#undef Item