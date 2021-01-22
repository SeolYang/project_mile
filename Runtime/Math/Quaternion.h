#pragma once
#include "Core/Helper.h"
#include "Math/Vector3.h"

namespace Mile
{
   class MEAPI Quaternion
   {
   public:
      Quaternion( float ww, float xx, float yy, float zz ) :
         w( ww ),
         x( xx ),
         y( yy ),
         z( zz )
      {
      }

      Quaternion( ) :
         Quaternion( 1.0f, 0.0f, 0.0f, 0.0f )
      {
      }

      Quaternion( float degree, const Vector3& axis )
      {
         Vector3 normalizedAxis = axis.GetNormalized( );
         float radian = (Math::DegreeToRadian(degree / 2.0f));
         float sinVal = sinf( radian );
         float cosVal = cosf( radian );

         this->w = cosVal;
         this->x = sinVal * normalizedAxis.x;
         this->y = sinVal * normalizedAxis.y;
         this->z = sinVal * normalizedAxis.z;
      }

      Quaternion operator+( const Quaternion& quat ) const
      {
         return Quaternion( w + quat.w,
                            x + quat.x,
                            y + quat.y,
                            z + quat.z );
      }

      Quaternion& operator+=( const Quaternion& quat )
      {
         w += quat.w;
         x += quat.x;
         y += quat.y;
         z += quat.z;
         return ( *this );
      }

      Quaternion operator-( const Quaternion& quat ) const
      {
         return Quaternion( w - quat.w,
                            x - quat.x,
                            y - quat.y,
                            z - quat.z );
      }

      Quaternion& operator-=( const Quaternion& quat )
      {
         w -= quat.w;
         x -= quat.x;
         y -= quat.y;
         z -= quat.z;
         return ( *this );
      }

      Quaternion operator*( float factor ) const
      {
         return Quaternion( w * factor,
                            x * factor,
                            y * factor,
                            z * factor );
      }

      Quaternion& operator*=( float factor )
      {
         w *= factor;
         x *= factor;
         y *= factor;
         z *= factor;
         return ( *this );
      }

      Quaternion operator*( const Quaternion& quat ) const
      {
         return Quaternion(
            ( w * quat.w - ( x * quat.x + y * quat.y + z * quat.z ) ),
            ( x * quat.w + y * quat.z - z * quat.y + w * quat.x ),
            ( -x * quat.z + y * quat.w + z * quat.x + w * quat.y ),
            ( x * quat.y - y * quat.x + z * quat.w + w * quat.z ) );
      }

      Vector3 operator*( const Vector3& rhs ) const
      {
         Vector3 quatVec{ x, y, z };
         Vector3 norm{ quatVec ^ rhs };
         Vector3 tangent{ quatVec ^ norm };

         return rhs + ( ( norm * w + tangent ) * 2.0f );
      }

      Quaternion& operator*=( const Quaternion& quat )
      {
         w = ( w * quat.w - ( x*quat.x + y*quat.y + z*quat.z ) );
         x = ( w * quat.x + quat.w * x ) + ( y * quat.z - z * quat.y );
         y = ( w * quat.y + quat.w * y ) + ( z * quat.x - x * quat.z );
         z = ( w * quat.z + quat.w * z ) + ( x * quat.y - y * quat.x );
         return ( *this );
      }

      Quaternion operator/( float div ) const
      {
         float factor = 1.0f / div;
         return Quaternion( w * factor,
                            x * factor,
                            y * factor,
                            z * factor );
      }

      Quaternion& operator/=( float div )
      {
         float factor = 1.0f / div;
         w *= factor;
         x *= factor;
         y *= factor;
         z *= factor;
         return ( *this );
      }

      Quaternion& operator=( const Quaternion& quat )
      {
         w = quat.w;
         x = quat.x;
         y = quat.y;
         z = quat.z;
         return ( *this );
      }

	  json Serialize() const
	  {
		  json serialized;
		  serialized["w"] = w;
		  serialized["x"] = x;
		  serialized["y"] = y;
		  serialized["z"] = z;
		  return serialized;
	  }

      void DeSerialize( const json& jsonData )
      {
         w = GetValueSafelyFromJson(jsonData, "w", 0.0f);
         x = GetValueSafelyFromJson(jsonData, "x", 0.0f);
         y = GetValueSafelyFromJson(jsonData, "y", 0.0f);
         z = GetValueSafelyFromJson(jsonData, "z", 0.0f);
      }

      float Norm( ) const
      {
         return std::sqrt( w * w + x * x + y * y + z * z );
      }

      float NormSquared( ) const
      {
         return ( w * w + x * x + y * y + z * z );
      }

      float Dot( const Quaternion& quat ) const
      {
         return ( x * quat.x + y * quat.y + z * quat.z + w * quat.w );
      }

      Quaternion Conjugate( ) const
      {
         return Quaternion( w, -x, -y, -z );
      }

      Quaternion Inverse( ) const
      {
         float norm = NormSquared( );
         float normInv = 1.0f / norm;
         auto conjugate = Conjugate( );
         return Quaternion( conjugate.w * normInv,
                            conjugate.x * normInv,
                            conjugate.y * normInv,
                            conjugate.z * normInv );
      }

      Quaternion Normalized( ) const
      {
         float norm = Norm( );
         float normInv = 1.0f / norm;
         return Quaternion( w * normInv,
                            x * normInv,
                            y * normInv,
                            z * normInv );
      }

      Quaternion& Normalize( )
      {
         float norm = Norm( );
         float normInv = 1.0f / norm;
         this->w *= normInv;
         this->x *= normInv;
         this->y *= normInv;
         this->z *= normInv;
         return ( *this );
      }

      Quaternion& Rotate( const Quaternion& rot )
      {
         (*this) = rot * (*this);
         return Normalize();
      }

      Quaternion& Rotate(float degree, const Vector3& axis)
      {
         Quaternion newRot;

         Vector3 normalizedAxis = axis.GetNormalized();
         float radian = (Math::DegreeToRadian(degree / 2.0f));
         float sinVal = sinf(radian);
         float cosVal = cosf(radian);

         newRot.w = cosVal;
         newRot.x = sinVal * normalizedAxis.x;
         newRot.y = sinVal * normalizedAxis.y;
         newRot.z = sinVal * normalizedAxis.z;

         return Rotate(newRot);
      }

      Quaternion& Rotate(const Vector3& eulerAnglesDegree)
      {
         this->Rotate(eulerAnglesDegree.x, Vector3(1.0f, 0.0f, 0.0f));
         this->Rotate(eulerAnglesDegree.y, Vector3(0.0f, 1.0f, 0.0f));
         this->Rotate(eulerAnglesDegree.z, Vector3(0.0f, 0.0f, 1.0f));
         return Normalize();
      }

      Quaternion Rotated( const Quaternion& rot ) const
      {
         Quaternion result = (*this);
         return result.Rotate(rot);
      }

      Quaternion Rotated(float degree, const Vector3& axis) const
      {
         Quaternion result = (*this);
         return result.Rotate(degree, axis);
      }

      Quaternion Rotated(const Vector3& eulerAnglesDegree) const
      {
         Quaternion result = (*this);
         return result.Rotate(eulerAnglesDegree);
      }

      Vector3 RotateVector(const Vector3& p) const
      {
         Quaternion conj = Conjugate();
         Vector3 result = (*this) * p;
         result = conj * p;
         return result;
      }

   public:
      union
      {
         struct
         {
            float x;
            float y;
            float z;
            float w;
         };

         float elements[ 4 ];
      };
   };
}
