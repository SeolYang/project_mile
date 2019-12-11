#pragma once
#include "Core/Helper.h"
#include "Math.h"

namespace Mile
{
    class MEAPI Vector4
    {
    public:
       Vector4( float xx, float yy, float zz, float ww );
       Vector4( );
       Vector4( const Vector4& vec );

       Vector4 operator+( const Vector4& vec ) const;
       Vector4 operator+( float val ) const;
       Vector4& operator+=( const Vector4& vec );
       Vector4& operator+=( float val );

       Vector4 operator-( const Vector4& vec ) const;
       Vector4 operator-( float val ) const;
       Vector4& operator-=( const Vector4& vec );
       Vector4& operator-=( float val );

       Vector4 operator*( const Vector4& vec ) const;
       Vector4 operator*( float factor ) const;
       Vector4& operator*=( const Vector4& vec );
       Vector4& operator*=( float factor );

       Vector4 operator/( const Vector4& vec ) const;
       Vector4 operator/( float div ) const;
       Vector4& operator/=( const Vector4& vec );
       Vector4& operator/=( float div );

       Vector4& operator=( const Vector4& vec );

       bool operator==( const Vector4& vec ) const;
       bool operator!=( const Vector4& vec ) const;

       float operator|( const Vector4& vec ) const;
       Vector4 operator^( const Vector4& vec ) const;

	   json Serialize() const;
       void DeSerialize( const json& jsonData );

       float GetX( ) const;
       float GetY( ) const;
       float GetZ( ) const;
       float GetW( ) const;

       void SetX( float xx );
       void SetY( float yy );
       void SetZ( float zz );
       void SetW( float ww );

       float Dot3( const Vector4& vec ) const;
       float Dot( const Vector4& vec ) const;
       Vector4 Cross( const Vector4& vec ) const;

       float Size3Sqaured( ) const;
       float Size3( ) const;
       float SizeSquared( ) const;
        float Size( ) const;

        void Normalize3( );
        Vector4 GetNormalized3( ) const;
        void Normalize( );
        Vector4 GetNormalized( ) const;

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

    public:
       static Vector4 Up( )
       {
          return Vector4( 0.0f, 1.0f, 0.0f, 0.0f );
       }

       static Vector4 Forward( )
       {
          return Vector4( 0.0f, 0.0f, 1.0f, 0.0f );
       }

       static Vector4 Right( )
       {
          return Vector4( 1.0f, 0.0f, 0.0f, 0.0f );
       }

       static Vector4 Zero( )
       {
          return Vector4( 0.0f, 0.0f, 0.0f, 0.0f );
       }

       static Vector4 One( )
       {
          return Vector4( 1.0f, 1.0f, 1.0f, 0.0f );
       }
    };

}
