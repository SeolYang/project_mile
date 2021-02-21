#pragma once
#pragma warning(push, 0)
#include <cmath>
#pragma warning(pop)
#include "Core/CoreMinimal.h"

namespace Mile
{
   namespace Math
   {
      constexpr float Pi = 3.14159265f;

      inline static float DegreeToRadian(float degree)
      {
         return (degree / 180.0f) * Pi;
      }

      inline static float RadianToDegree(float radian)
      {
         return (radian / Pi) * 180.0f;
      }

      inline static float Lerp(float a, float b, float t)
      {
         return a + (t * (b - a));
      }

      inline static float ClampAngle(float degree)
      {
         degree = std::fmod(degree, 360.0f);
         if (degree < 0.0f)
         {
            degree += 360.0f;
         }

         return degree;
      }

      inline static float NormalizeAngle(float degree)
      {
         degree = ClampAngle(degree);
         if (degree > 180.0f)
         {
            degree -= 360.0f;
         }

         return degree;
      }

      inline static float Steradian(float distance, float area)
      {
         return (area / distance);
      }
   }
}
