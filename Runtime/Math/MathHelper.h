#pragma once
#include "MathCore.h"
#include "Math/Quaternion.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace Mile
{
   namespace Math
   {
      static Quaternion EulerToQuaternion(const Vector3& eulerAngles)
      {
         float cy = cos(eulerAngles.z * 0.5f);
         float sy = sin(eulerAngles.z * 0.5f);
         float cp = cos(eulerAngles.y * 0.5f);
         float sp = sin(eulerAngles.y * 0.5f);
         float cr = cos(eulerAngles.x * 0.5f);
         float sr = sin(eulerAngles.x * 0.5f);

         Quaternion q;
         q.w = cr * cp * cy + sr * sp * sy;
         q.x = sr * cp * cy - cr * sp * sy;
         q.y = cr * sp * cy + sr * cp * sy;
         q.z = cr * cp * sy - sr * sp * cy;

         return q;
      }

      static Vector3 QuaternionToEulerAngles(const Quaternion& q)
      {
         Vector3 angles;
         float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
         float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
         angles.x = std::atan2(sinr_cosp, cosr_cosp);

         float sinp = 2.0f * (q.w * q.y - q.z * q.x);
         if (std::abs(sinp) >= 1.0f)
         {
            angles.y = std::copysign(Math::Pi / 2.0f, sinp);
         }
         else
         {
            angles.y = std::asin(sinp);
         }

         float siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
         float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
         angles.z = std::atan2(siny_cosp, cosy_cosp);

         return angles;
      }

      static Vector3 RadEulerAnglesToDegEulerAngles(const Vector3& radEuler)
      {
         return Vector3(
            RadianToDegree(radEuler.x),
            RadianToDegree(radEuler.y),
            RadianToDegree(radEuler.z));
      }

      static Vector3 DegEulerAnglesToRadEulerAngles(const Vector3& degEuler)
      {
         return Vector3(
            DegreeToRadian(degEuler.x),
            DegreeToRadian(degEuler.y),
            DegreeToRadian(degEuler.z));
      }
   }
}
