
#pragma once

#if defined(__SYNTHESIS__) || defined(CCS_SYSC)
#include <ac_shared.h>
#else

template <typename T>
using ac_shared = T;

#endif
