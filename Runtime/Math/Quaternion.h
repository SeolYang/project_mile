#pragma once

#include "Core/Helper.h"
#include "Vector3.h"

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

      Quaternion( float real, const Vector3& imaginary ) :
         Quaternion( real, imaginary.x, imaginary.y, imaginary.z )
      {
      }

      Quaternion( const Vector3& imaginary ) :
         Quaternion( 0.0f, imaginary )
      {
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
         return Quaternion( w * quat.w - ( x*quat.x + y*quat.y + z*quat.z ),
                          ( w * quat.x + quat.w * x ) + ( y * quat.z - z * quat.y ),
                          ( w * quat.y + quat.w * y ) + ( z * quat.x - x * quat.z ),
                          ( w * quat.z + quat.w * z ) + ( x * quat.y - y * quat.x ) );
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

      std::string Serialize( ) const
      {
         return "{ \"x\": " + std::to_string( w ) + ", "
            + "\"y\": " + std::to_string( x ) + ", "
            + "\"z\": " + std::to_string( y ) + ", "
            + "\"w\": " + std::to_string( z ) + " }";
      }

      void DeSerialize( const json& jsonData )
      {
         w = jsonData[ "w" ];
         x = jsonData[ "x" ];
         y = jsonData[ "y" ];
         z = jsonData[ "z" ];
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
         return Quaternion( w * normInv,
                            x * normInv,
                            y * normInv,
                            z * normInv );
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

      Quaternion& Rotate( const Quaternion& rot )
      {
         ( *this ) = ( rot * ( *this ) * rot.Conjugate( ) );
         return ( *this );
      }

      Quaternion Rotated( const Quaternion& rot ) const
      {
         return ( rot * ( *this ) * rot.Conjugate( ) );
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
