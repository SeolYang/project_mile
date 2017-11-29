#pragma once

#include "Core/Helper.h"
#include "Math.h"

namespace Mile
{
   class MEAPI alignas(16) Vector2
   {
   public:
      Vector2( float xx, float yy ) :
         x( xx ),
         y( yy )
      {
      }

      Vector2( ) :
         Vector2( 0.0f, 0.0f )
      {
      }

      Vector2( const Vector2& vec ) :
         Vector2( vec.x, vec.y )
      {
      }

      Vector2 operator+( const Vector2& vec ) const 
      {
         return Vector2( x + vec.x,
                         y + vec.y );
      }

      Vector2 operator+( float val ) const
      {
         return Vector2( x + val,
                         y + val );
      }

      Vector2& operator+=( const Vector2& vec )
      {
         x += vec.x;
         y += vec.y;
         return ( *this );
      }

      Vector2& operator+=( float val )
      {
         x += val;
         y += val;
         return ( *this );
      }

      Vector2 operator-( const Vector2& vec ) const
      {
         return Vector2( x - vec.x,
                         y - vec.y );
      }

      Vector2 operator-( float val ) const
      {
         return Vector2( x - val,
                         y - val );
      }

      Vector2& operator-=( const Vector2& vec )
      {
         x -= vec.x;
         y -= vec.y;
         return ( *this );
      }

      Vector2& operator-=( float val )
      {
         x -= val;
         y -= val;
         return ( *this );
      }

      Vector2 operator*( const Vector2& vec ) const
      {
         return Vector2( x * vec.x,
                         y * vec.y );
      }

      Vector2 operator*( float factor ) const
      {
         return Vector2( x * factor,
                         y * factor );
      }

      Vector2& operator*=( const Vector2& vec )
      {
         x *= vec.x;
         y *= vec.y;
         return ( *this );
      }

      Vector2& operator*=( float factor )
      {
         x *= factor;
         y *= factor;
         return ( *this );
      }

      Vector2 operator/( const Vector2& vec ) const
      {
         return Vector2( x / vec.x,
                         y / vec.y );
      }

      Vector2 operator/( float div ) const
      {
         float factor = 1.0f / div;
         return Vector2( x * factor,
                         y * factor );
      }

      Vector2& operator/=( const Vector2& vec )
      {
         x /= vec.x;
         y /= vec.y;
         return ( *this );
      }

      Vector2& operator/=( float div )
      {
         float factor = 1.0f / div;
         x *= factor;
         y *= factor;
         return ( *this );
      }

      Vector2& operator=( const Vector2& vec )
      {
         x = vec.x;
         y = vec.y;
         return ( *this );
      }

      std::string Serialize( ) const
      {
         return "{ \"x\": " + std::to_string( x ) + ", "
            + "\"y\": " + std::to_string( y ) + " }";
      }

      void DeSerialize( const json& jsonData )
      {
         x = jsonData[ "x" ];
         y = jsonData[ "y" ];
      }

      bool operator==( const Vector2& vec ) const
      {
         return ( x == vec.x ) && ( y == vec.y );
      }

      bool operator!=( const Vector2& vec ) const
      {
         return ( x != vec.x ) || ( y != vec.y );
      }

      float operator|( const Vector2& vec ) const
      {
         return Dot( vec );
      }

      float GetX( ) const
      {
         return x;
      }

      float GetY( ) const
      {
         return y;
      }

      void SetX( float xx )
      {
         x = xx;
      }

      void SetY( float yy )
      {
         y = yy;
      }

      float SizeSquared( ) const
      {
         return ( x * x + y * y );
      }

      float Size( ) const
      {
         return std::sqrt( SizeSquared( ) );
      }

      void Normalize( )
      {
         float factor = ( 1.0f / Size( ) );
         x *= factor;
         y *= factor;
      }

      Vector2 GetNormalized( ) const
      {
         Vector2 temp = ( *this );
         temp.Normalize( );
         return temp;
      }

      float Dot( const Vector2& vec ) const
      {
         return ( ( x*vec.x ) + ( y*vec.y ) );
      }

   public:
      union
      {
         struct
         {
            float x;
            float y;
         };

         float elements[ 2 ];
      };
      

   };
}

