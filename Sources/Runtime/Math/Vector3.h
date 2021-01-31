#pragma once
#include "Math/MathCore.h"

namespace Mile
{
   class MEAPI Vector3
   {
   public:
      Vector3( float xx, float yy, float zz );
      Vector3( );
      Vector3( const Vector3& vec );

      Vector3 operator+( const Vector3& vec ) const;
      Vector3 operator+( float val ) const;
      Vector3& operator+=( const Vector3& vec );
      Vector3& operator+=( float val );

      Vector3 operator-( const Vector3& vec ) const;
      Vector3 operator-( float val ) const;
      Vector3& operator-=( const Vector3& vec );
      Vector3& operator-=( float val );

      Vector3 operator*( const Vector3& vec ) const;
      Vector3 operator*( float factor ) const;
      Vector3& operator*=( const Vector3& vec );
      Vector3& operator*=( float factor );

      Vector3 operator/( const Vector3& vec ) const;
      Vector3 operator/( float div ) const;
      Vector3& operator/=( const Vector3& vec );
      Vector3& operator/=( float div );

      Vector3& operator=( const Vector3& vec );

      bool operator==( const Vector3& vec ) const;
      bool operator!=( const Vector3& vec ) const;

      float operator|( const Vector3& vec ) const;
      Vector3 operator^( const Vector3& vec ) const;

	  json Serialize() const;
      void DeSerialize( const json& jsonData );

      float GetX( ) const;
      float GetY( ) const;
      float GetZ( ) const;

      void SetX( float xx );
      void SetY( float yy );
      void SetZ( float zz );

      float Dot( const Vector3& vec ) const;
      Vector3 Cross( const Vector3& vec ) const;

      float SizeSquared( ) const;
      float Size( ) const;

      void Normalize( );
      Vector3 GetNormalized( ) const;

   public:
      union
      {
         struct
         {
            float x;
            float y;
            float z;
         };

         float elements[ 3 ];
      };

      static Vector3 Up( )
      {
         return Vector3( 0.0f, 1.0f, 0.0f );
      }

      static Vector3 Forward( )
      {
         return Vector3( 0.0f, 0.0f, 1.0f );
      }

      static Vector3 Right( )
      {
         return Vector3( 1.0f, 0.0f, 0.0f );
      }

      static Vector3 Zero( )
      {
         return Vector3( 0.0f, 0.0f, 0.0f );
      }

      static Vector3 One( )
      {
         return Vector3( 1.0f, 1.0f, 1.0f );
      }
   };
}