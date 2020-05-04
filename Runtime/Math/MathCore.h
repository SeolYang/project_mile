#pragma once
#include <cmath>

namespace Mile
{
   namespace Math
   {
      static const Float Pi = (Float)3.14159265;

      static float DegreeToRadian( float degree )
      {
         return (degree / 180.0f) * Pi;
      }

      static float RadianToDegree( float radian )
      {
         return ( radian / Pi ) * 180.0f;
      }

      static float Lerp(float a, float b, float t)
      {
         return a + (t * (b - a));
      }
   }
}
