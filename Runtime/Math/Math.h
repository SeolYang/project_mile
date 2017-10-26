#pragma once
#include <cmath>

namespace Mile
{
#ifdef _FLOAT_AS_DOUBLE_
   using Float = double;
#else
   using Float = float;
#endif

   namespace Math
   {
      static const Float Pi = 3.1415926535897932384;
   }
}
