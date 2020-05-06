#pragma once
#include "Math/Vector4.h"
#include "Math/Quaternion.h"

namespace Mile
{
   class MEAPI alignas( 16 ) Matrix
   {
   public:
      Matrix( float _m11, float _m12, float _m13, float _m14,
              float _m21, float _m22, float _m23, float _m24,
              float _m31, float _m32, float _m33, float _m34,
              float _m41, float _m42, float _m43, float _m44 )
      {
         m11 = _m11; m12 = _m12; m13 = _m13; m14 = _m14;
         m21 = _m21; m22 = _m22; m23 = _m23; m24 = _m24;
         m31 = _m31; m32 = _m32; m33 = _m33; m34 = _m34;
         m41 = _m41; m42 = _m42; m43 = _m43; m44 = _m44;
      }

      Matrix( float val ) :
         Matrix( val, val, val, val,
                 val, val, val, val,
                 val, val, val, val,
                 val, val, val, val )
      {
      }

      Matrix( ) :
         Matrix( 1.0f, 0.0f, 0.0f, 0.0f,
                 0.0f, 1.0f, 0.0f, 0.0f,
                 0.0f, 0.0f, 1.0f, 0.0f,
                 0.0f, 0.0f, 0.0f, 1.0f )
      {
      }

      Matrix operator+( const Matrix& mat ) const
      {
         return Matrix( m11 + mat.m11, m12 + mat.m12, m13 + mat.m13, m14 + mat.m14,
                        m21 + mat.m21, m22 + mat.m22, m23 + mat.m23, m24 + mat.m24,
                        m31 + mat.m31, m32 + mat.m32, m33 + mat.m33, m34 + mat.m34,
                        m41 + mat.m41, m42 + mat.m42, m43 + mat.m43, m44 + mat.m44 );
      }

      Matrix& operator+=( const Matrix& mat )
      {
         m11 += mat.m11; m12 += mat.m12; m13 += mat.m13; m14 += mat.m14;
         m21 += mat.m21; m22 += mat.m22; m23 += mat.m23; m24 += mat.m24;
         m31 += mat.m31; m32 += mat.m32; m33 += mat.m33; m34 += mat.m34;
         m41 += mat.m41; m42 += mat.m42; m43 += mat.m43; m44 += mat.m44;
         return ( *this );
      }

      Matrix operator-( const Matrix& mat ) const
      {
         return Matrix( m11 - mat.m11, m12 - mat.m12, m13 - mat.m13, m14 - mat.m14,
                        m21 - mat.m21, m22 - mat.m22, m23 - mat.m23, m24 - mat.m24,
                        m31 - mat.m31, m32 - mat.m32, m33 - mat.m33, m34 - mat.m34,
                        m41 - mat.m41, m42 - mat.m42, m43 - mat.m43, m44 - mat.m44 );
      }

      Matrix& operator-=( const Matrix& mat )
      {
         m11 -= mat.m11; m12 -= mat.m12; m13 -= mat.m13; m14 -= mat.m14;
         m21 -= mat.m21; m22 -= mat.m22; m23 -= mat.m23; m24 -= mat.m24;
         m31 -= mat.m31; m32 -= mat.m32; m33 -= mat.m33; m34 -= mat.m34;
         m41 -= mat.m41; m42 -= mat.m42; m43 -= mat.m43; m44 -= mat.m44;
         return ( *this );
      }

      Matrix operator*( const Matrix& mat ) const
      {
         return Matrix( ( m11 * mat.m11 + m12 * mat.m21 + m13 * mat.m31 + m14 * mat.m41 ),
                        ( m11 * mat.m12 + m12 * mat.m22 + m13 * mat.m32 + m14 * mat.m42 ),
                        ( m11 * mat.m13 + m12 * mat.m23 + m13 * mat.m33 + m14 * mat.m43 ),
                        ( m11 * mat.m14 + m12 * mat.m24 + m13 * mat.m34 + m14 * mat.m44 ),
                        ( m21 * mat.m11 + m22 * mat.m21 + m23 * mat.m31 + m24 * mat.m41 ),
                        ( m21 * mat.m12 + m22 * mat.m22 + m23 * mat.m32 + m24 * mat.m42 ),
                        ( m21 * mat.m13 + m22 * mat.m23 + m23 * mat.m33 + m24 * mat.m43 ),
                        ( m21 * mat.m14 + m22 * mat.m24 + m23 * mat.m34 + m24 * mat.m44 ),
                        ( m31 * mat.m11 + m32 * mat.m21 + m33 * mat.m31 + m34 * mat.m41 ),
                        ( m31 * mat.m12 + m32 * mat.m22 + m33 * mat.m32 + m34 * mat.m42 ),
                        ( m31 * mat.m13 + m32 * mat.m23 + m33 * mat.m33 + m34 * mat.m43 ),
                        ( m31 * mat.m14 + m32 * mat.m24 + m33 * mat.m34 + m34 * mat.m44 ),
                        ( m41 * mat.m11 + m42 * mat.m21 + m43 * mat.m31 + m44 * mat.m41 ),
                        ( m41 * mat.m12 + m42 * mat.m22 + m43 * mat.m32 + m44 * mat.m42 ),
                        ( m41 * mat.m13 + m42 * mat.m23 + m43 * mat.m33 + m44 * mat.m43 ),
                        ( m41 * mat.m14 + m42 * mat.m24 + m43 * mat.m34 + m44 * mat.m44 ) );
      }

      Matrix operator*( float factor ) const
      {
         return Matrix( m11 * factor, m12 * factor, m13 * factor, m14 * factor,
                        m21 * factor, m22 * factor, m23 * factor, m24 * factor,
                        m31 * factor, m32 * factor, m33 * factor, m34 * factor,
                        m41 * factor, m42 * factor, m43 * factor, m44 * factor );
      }

      Matrix& operator*=( const Matrix& mat )
      {
         m11 = ( m11 * mat.m11 + m12 * mat.m21 + m13 * mat.m31 + m14 * mat.m41 );
         m12 = ( m11 * mat.m12 + m12 * mat.m22 + m13 * mat.m32 + m14 * mat.m42 );
         m13 = ( m11 * mat.m13 + m12 * mat.m23 + m13 * mat.m33 + m14 * mat.m43 );
         m14 = ( m11 * mat.m14 + m12 * mat.m24 + m13 * mat.m34 + m14 * mat.m44 );
         m21 = ( m21 * mat.m11 + m22 * mat.m21 + m23 * mat.m31 + m24 * mat.m41 );
         m22 = ( m21 * mat.m12 + m22 * mat.m22 + m23 * mat.m32 + m24 * mat.m42 );
         m23 = ( m21 * mat.m13 + m22 * mat.m23 + m23 * mat.m33 + m24 * mat.m43 );
         m24 = ( m21 * mat.m14 + m22 * mat.m24 + m23 * mat.m34 + m24 * mat.m44 );
         m31 = ( m31 * mat.m11 + m32 * mat.m21 + m33 * mat.m31 + m34 * mat.m41 );
         m32 = ( m31 * mat.m12 + m32 * mat.m22 + m33 * mat.m32 + m34 * mat.m42 );
         m33 = ( m31 * mat.m13 + m32 * mat.m23 + m33 * mat.m33 + m34 * mat.m43 );
         m34 = ( m31 * mat.m14 + m32 * mat.m24 + m33 * mat.m34 + m34 * mat.m44 );
         m41 = ( m41 * mat.m11 + m42 * mat.m21 + m43 * mat.m31 + m44 * mat.m41 );
         m42 = ( m41 * mat.m12 + m42 * mat.m22 + m43 * mat.m32 + m44 * mat.m42 );
         m43 = ( m41 * mat.m13 + m42 * mat.m23 + m43 * mat.m33 + m44 * mat.m43 );
         m44 = ( m41 * mat.m14 + m42 * mat.m24 + m43 * mat.m34 + m44 * mat.m44 );
         return ( *this );
      }

      Matrix& operator*=( float factor )
      {
         m11 *= factor; m12 *= factor; m13 *= factor; m14 *= factor;
         m21 *= factor; m22 *= factor; m23 *= factor; m24 *= factor;
         m31 *= factor; m32 *= factor; m33 *= factor; m34 *= factor;
         m41 *= factor; m42 *= factor; m43 *= factor; m44 *= factor;
         return ( *this );
      }

      Matrix operator/( float div ) const
      {
         float factor = 1.0f / div;
         return Matrix( m11 * factor, m12 * factor, m13 * factor, m14 * factor,
                        m21 * factor, m22 * factor, m23 * factor, m24 * factor,
                        m31 * factor, m32 * factor, m33 * factor, m34 * factor,
                        m41 * factor, m42 * factor, m43 * factor, m44 * factor );
      }

      Matrix& operator/=( float div )
      {
         float factor = 1.0f / div;
         m11 *= factor; m12 *= factor; m13 *= factor; m14 *= factor;
         m21 *= factor; m22 *= factor; m23 *= factor; m24 *= factor;
         m31 *= factor; m32 *= factor; m33 *= factor; m34 *= factor;
         m41 *= factor; m42 *= factor; m43 *= factor; m44 *= factor;
         return ( *this );
      }

      Matrix& operator=( const Matrix& mat )
      {
         m11 = mat.m11; m12 = mat.m12; m13 = mat.m13; m14 = mat.m14;
         m21 = mat.m21; m22 = mat.m22; m23 = mat.m23; m24 = mat.m24;
         m31 = mat.m31; m32 = mat.m32; m33 = mat.m33; m34 = mat.m34;
         m41 = mat.m41; m42 = mat.m42; m43 = mat.m43; m44 = mat.m44;
         return ( *this );
      }

      bool operator==( const Matrix& mat ) const
      {
         return ( m11 == mat.m11 && m12 == mat.m12 && m13 == mat.m13 && m14 == mat.m14 ) &&
            ( m21 == mat.m21 && m22 == mat.m22 && m23 == mat.m23 && m24 == mat.m14 ) &&
            ( m31 == mat.m31 && m32 == mat.m32 && m33 == mat.m33 && m34 == mat.m14 ) &&
            ( m41 == mat.m41 && m42 == mat.m42 && m43 == mat.m43 && m44 == mat.m14 );
      }

      bool operator!=( const Matrix& mat ) const
      {
         return ( m11 != mat.m11 || m12 != mat.m12 || m13 != mat.m13 || m14 != mat.m14 ) ||
            ( m21 != mat.m21 || m22 != mat.m22 || m23 != mat.m23 || m24 != mat.m14 ) ||
            ( m31 != mat.m31 || m32 != mat.m32 || m33 != mat.m33 || m34 != mat.m14 ) ||
            ( m41 != mat.m41 || m42 != mat.m42 || m43 != mat.m43 || m44 != mat.m14 );
      }

      float Determinant( ) const
      {
         float res = 0.0f;

         if ( m11 != 0.0f )
         {
            res += ( m11 * ( m22 * ( m33 * m44 - m34 * m43 ) - m23 * ( m32 * m44 - m34 * m43 ) + m24 * ( m32 * m43 - m33 * m42 ) ) );
         }
         if ( m12 != 0.0f )
         {
            res -= ( m12 * ( m21 * ( m33 * m44 - m34 * m43 ) - m23 * ( m31 * m44 - m34 * m41 ) + m24 * ( m31 * m44 - m34 * m41 ) ) );
         }
         if ( m13 != 0.0f )
         {
            res += ( m13 * ( m21 *( m32 * m44 - m34 * m42 ) - m22 * ( m31 * m44 - m34 * m41 ) + m24 * ( m31 * m42 - m32 * m41 ) ) );
         }
         if ( m14 != 0.0f )
         {
            res -= ( m14 * ( m21 * ( m32 * m43 - m33 * m42 ) - m22 * ( m31 * m43 - m33 * m41 ) + m23 * ( m21 * m42 - m32 * m41 ) ) );
         }

         return res;
      }

      Matrix Adjugate( ) const
      {
         float b11 = ( m22 * m33 * m44 ) + ( m23 * m34 * m42 ) + ( m24 * m32 * m43 ) - ( m22 * m34 * m43 ) - ( m23 * m32 * m44 ) - ( m24 * m33 * m42 );
         float b12 = ( m12 * m34 * m43 ) + ( m13 * m32 * m44 ) + ( m14 * m33 * m42 ) - ( m12 * m33 * m44 ) - ( m13 * m34 * m42 ) - ( m14 * m32 * m43 );
         float b13 = ( m12 * m23 * m44 ) + ( m13 * m24 * m42 ) + ( m14 * m22 * m43 ) - ( m12 * m24 * m43 ) - ( m13 * m22 * m44 ) - ( m14 * m23 * m42 );
         float b14 = ( m12 * m24 * m33 ) + ( m13 * m22 * m34 ) + ( m14 * m23 * m32 ) - ( m12 * m23 * m34 ) - ( m13 * m24 * m32 ) - ( m14 * m22 * m33 );
         float b21 = ( m21 * m34 * m43 ) + ( m23 * m31 * m44 ) + ( m24 * m33 * m41 ) - ( m21 * m33 * m44 ) - ( m23 * m34 * m41 ) - ( m24 * m31 * m43 );
         float b22 = ( m11 * m33 * m44 ) + ( m13 * m34 * m41 ) + ( m14 * m31 * m43 ) - ( m11 * m34 * m43 ) - ( m13 * m31 * m44 ) - ( m14 * m33 * m41 );
         float b23 = ( m11 * m24 * m43 ) + ( m13 * m21 * m44 ) + ( m14 * m21 * m33 ) - ( m11 * m23 * m41 ) - ( m13 * m24 * m41 ) - ( m14 * m21 * m43 );
         float b24 = ( m11 * m23 * m34 ) + ( m13 * m24 * m31 ) + ( m14 * m21 * m33 ) - ( m11 * m24 * m33 ) - ( m13 * m21 * m34 ) - ( m14 * m23 * m31 );
         float b31 = ( m21 * m32 * m44 ) + ( m22 * m34 * m41 ) + ( m24 * m31 * m42 ) - ( m21 * m34 * m42 ) - ( m22 * m31 * m44 ) - ( m24 * m32 * m41 );
         float b32 = ( m11 * m34 * m42 ) + ( m12 * m31 * m44 ) + ( m14 * m32 * m41 ) - ( m11 * m32 * m44 ) - ( m12 * m34 * m41 ) - ( m14 * m31 * m42 );
         float b33 = ( m11 * m22 * m44 ) + ( m12 * m24 * m41 ) + ( m14 * m21 * m42 ) - ( m11 * m24 * m42 ) - ( m12 * m21 * m44 ) - ( m14 * m22 * m41 );
         float b34 = ( m11 * m24 * m32 ) + ( m12 * m21 * m34 ) + ( m14 * m22 * m31 ) - ( m11 * m22 * m34 ) - ( m12 * m24 * m31 ) - ( m14 * m21 * m32 );
         float b41 = ( m21 * m33 * m42 ) + ( m22 * m31 * m43 ) + ( m23 * m32 * m41 ) - ( m21 * m32 * m43 ) - ( m22 * m33 * m41 ) - ( m23 * m31 * m42 );
         float b42 = ( m11 * m32 * m43 ) + ( m12 * m33 * m41 ) + ( m13 * m31 * m42 ) - ( m11 * m33 * m42 ) - ( m12 * m31 * m43 ) - ( m13 * m32 * m41 );
         float b43 = ( m11 * m23 * m42 ) + ( m12 * m21 * m43 ) + ( m13 * m22 * m41 ) - ( m11 * m22 * m43 ) - ( m12 * m23 * m41 ) - ( m13 * m21 * m42 );
         float b44 = ( m11 * m22 * m33 ) + ( m12 * m23 * m31 ) + ( m13 * m21 * m32 ) - ( m11 * m23 * m32 ) - ( m12 * m21 * m33 ) - ( m13 * m22 * m31 );

         return Matrix( b11, b12, b13, b14,
                        b21, b22, b23, b24,
                        b31, b32, b33, b34,
                        b41, b42, b43, b44 );
      }

      Matrix& Inverse( )
      {
         float determinant = Determinant( );
         if ( determinant == 0.0f )
         {
            return ( *this );
         }

         auto adjugate = Adjugate( );
         ( *this ) = adjugate * ( 1.0f / determinant );
         return ( *this );
      }

      Matrix Inversed( ) const
      {
         Matrix temp = ( *this );
         return temp.Inverse( );
      }

      String ToString( ) const
      {
         return std::to_wstring( m11 ) + TEXT( "\t" ) + std::to_wstring( m12 ) + TEXT( "\t" ) + std::to_wstring( m13 ) + TEXT( "\t" ) + std::to_wstring( m14 ) + TEXT( "\n" ) +
            std::to_wstring( m21 ) + TEXT( "\t" ) + std::to_wstring( m22 ) + TEXT( "\t" ) + std::to_wstring( m23 ) + TEXT( "\t" ) + std::to_wstring( m24 ) + TEXT( "\n" ) +
            std::to_wstring( m31 ) + TEXT( "\t" ) + std::to_wstring( m32 ) + TEXT( "\t" ) + std::to_wstring( m33 ) + TEXT( "\t" ) + std::to_wstring( m34 ) + TEXT( "\n" ) +
            std::to_wstring( m41 ) + TEXT( "\t" ) + std::to_wstring( m42 ) + TEXT( "\t" ) + std::to_wstring( m43 ) + TEXT( "\t" ) + std::to_wstring( m44 ) + TEXT( "\n" );
      }

      Matrix& Transpose( )
      {
         Matrix mat{ ( *this ) };
         m12 = mat.m21; m13 = mat.m31; m14 = mat.m41;
         m21 = mat.m12; m23 = mat.m32; m24 = mat.m42;
         m31 = mat.m13; m32 = mat.m23; m34 = mat.m43;
         m41 = mat.m14; m42 = mat.m24; m43 = mat.m34;
         return ( *this );
      }

      Matrix Transposed( ) const
      {
         Matrix mat{ ( *this ) };
         return mat.Transpose( );
      }

      static Matrix CreateTranslation( float x, float y, float z )
      {
         return Matrix{
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            x,    y,    z,    1.0f };
      }

      static Matrix CreateTranslation( const Vector3& vec )
      {
         return CreateTranslation( vec.x, vec.y, vec.z );
      }

      static Matrix CreateScale( float x, float y, float z )
      {
         return Matrix{
            x,       0.0f,    0.0f,    0.0f,
            0.0f,    y,       0.0f,    0.0f,
            0.0f,    0.0f,    z,       0.0f,
            0.0f,    0.0f,    0.0f,    1.0f };
      }

      static Matrix CreateScale( const Vector3& vec )
      {
         return CreateScale( vec.x, vec.y, vec.z );
      }

      static Matrix CreateRotation( const Quaternion& quat )
      {
         float x = quat.x;
         float y = quat.y;
         float z = quat.z;
         float w = quat.w;
         float xSqrd = quat.x * quat.x;
         float ySqrd = quat.y * quat.y;
         float zSqrd = quat.z * quat.z;
         auto mat = Matrix{
            ( 1.0f - 2.0f * ySqrd - 2.0f * zSqrd ), ( 2.0f * x*y + 2.0f * w*z ), ( 2.0f * x*z - 2.0f * w*y ), 0.0f,
            ( 2.0f * x*y - 2.0f * w*z ), ( 1.0f - 2.0f * xSqrd - 2.0f * zSqrd ), ( 2.0f * y*z + 2.0f * w*x ), 0.0f,
            ( 2.0f * x*z + 2.0f * w*y ), ( 2.0f * y*z - 2.0f * w*x ), ( 1.0f - 2.0f * xSqrd - 2.0f * ySqrd ),   0.0f,
            0.0f, 0.0f, 0.0f, 1.0f };
         return mat.Transpose( );
      }

      static Matrix CreateTransformMatrix( const Vector3& position, const Vector3& scale, const Quaternion& rot )
      {
         return CreateRotation(rot) * CreateScale(scale) * CreateTranslation(position);
      }

      /* Left-Handed View Matrix **/
      static Matrix CreateView( const Vector3& camPosition, const Vector3& camDirection, const Vector3& up = Vector3::Up( ) )
      {
         Vector3 upNor = up.GetNormalized( );
         Vector3 zAxis = camDirection.GetNormalized( );
         Vector3 xAxis = ( upNor ^ zAxis ).GetNormalized( );
         Vector3 yAxis = ( zAxis ^ xAxis ).GetNormalized( );

         return Matrix{
            xAxis.x, yAxis.x, zAxis.x, 0.0f,
            xAxis.y, yAxis.y, zAxis.y, 0.0f,
            xAxis.z, yAxis.z, zAxis.z, 0.0f,
            -( xAxis | camPosition ), -( yAxis | camPosition ), -( zAxis | camPosition ), 1.0f };
      }

      /* Left-Handed View Inverse Matrix **/
      static Matrix CreateInverseView(const Vector3& camPosition, const Vector3& camDirection, const Vector3& up = Vector3::Up())
      {
         Vector3 upNor = up.GetNormalized();
         Vector3 zAxis = camDirection.GetNormalized();
         Vector3 xAxis = (upNor ^ zAxis).GetNormalized();
         Vector3 yAxis = (zAxis ^ xAxis).GetNormalized();

         Matrix invRot = Matrix{
            xAxis.x, xAxis.y, xAxis.z, 0.0f,
            yAxis.x, yAxis.y, yAxis.z, 0.0f,
            zAxis.x, zAxis.y, zAxis.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
         };

         Matrix invTrans = Matrix{
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            (xAxis | camPosition), (yAxis | camPosition), (zAxis | camPosition), 1.0f
         };

         return (invTrans * invRot);
      }

      /* Left-Handed Perspective Fov Proj **/
      static Matrix CreatePerspectiveProj( float degreeFov, float aspectRatio, float nearPlane, float farPlane )
      {
         float height = ( 1.0f / std::tan( Math::DegreeToRadian( degreeFov / 2.0f ) ) );
         float width = height / aspectRatio;
         float range = farPlane / ( farPlane - nearPlane );

         return Matrix{
            width,   0.0f, 0.0f, 0.0f,
            0.0f, height,  0.0f, 0.0f,
            0.0f, 0.0f, range,   1.0f,
            0.0f, 0.0f, -range * nearPlane,  0.0f };
      }

   public:
      float m11, m12, m13, m14;
      float m21, m22, m23, m24;
      float m31, m32, m33, m34;
      float m41, m42, m43, m44;

   public:
      static Matrix Identity;

   };

   static Vector4 operator*( const Vector4& vec, const Matrix& mat )
   {
      return Vector4( vec.x * mat.m11 + vec.y * mat.m21 + vec.z * mat.m31 + vec.w * mat.m41,
                      vec.x * mat.m12 + vec.y * mat.m22 + vec.z * mat.m32 + vec.w * mat.m42,
                      vec.x * mat.m13 + vec.y * mat.m23 + vec.z * mat.m33 + vec.w * mat.m43,
                      vec.x * mat.m14 + vec.y * mat.m24 + vec.z * mat.m34 + vec.w * mat.m44 );
   }

   static Vector3 operator*( const Vector3& vec, const Matrix& mat )
   {
      Vector4 vec4 = Vector4( vec.x, vec.y, vec.z, 1.0f ) * mat;
      return Vector3( vec4.x, vec4.y, vec4.z );
   }
}