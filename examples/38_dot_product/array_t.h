#pragma once

#include "auto_gen_fields.h"

template<typename T,int SIZE>
struct array_t
{
  
  T data[SIZE];

  AUTO_GEN_FIELD_METHODS(array_t, ( \
     data \
  ) )
  //
};

