#pragma once
#include "Vector3.h"

namespace Mile
{
    class MEAPI Vector4
    {
    public:
        Vector4( float xx, float yy, float zz, float ww ) :
            x( xx ),
            y( yy ),
            z( zz ),
            w( ww )
        {
        }

        Vector4( ) :
            Vector4( 0.0f, 0.0f, 0.0f, 1.0f )
        {
        }

        Vector4( const Vector3& vec ) :
            Vector4( vec.x, vec.y, vec.z, 1.0f )
        {
        }

        Vector4( const Vector4& vec ) :
            Vector4( vec.x, vec.y, vec.z, vec.w )
        {
        }

        Vector4 operator+( const Vector4& vec ) const
        {
            return Vector4( x + vec.x,
                            y + vec.y,
                            z + vec.z,
                            w + vec.w );
        }

        Vector4 operator+( float val ) const
        {
            return Vector4( x + val,
                            y + val,
                            z + val,
                            w + val );
        }

        Vector4& operator+=( const Vector4& vec )
        {
            x += vec.x;
            y += vec.y;
            z += vec.z;
            w += vec.w;
            return ( *this );
        }

        Vector4& operator+=( float val )
        {
            x += val;
            y += val;
            z += val;
            w += val;
            return ( *this );
        }

        Vector4 operator-( const Vector4& vec ) const
        {
            return Vector4( x - vec.x,
                            y - vec.y,
                            z - vec.z,
                            w - vec.w );
        }

        Vector4 operator-( float val ) const
        {
            return Vector4( x - val,
                            y - val,
                            z - val,
                            w - val );
        }

        Vector4& operator-=( const Vector4& vec )
        {
            x -= vec.x;
            y -= vec.y;
            z -= vec.z;
            w -= vec.w;
            return ( *this );
        }

        Vector4& operator-=( float val )
        {
            x -= val;
            y -= val;
            z -= val;
            w -= val;
            return ( *this );
        }

        Vector4 operator*( const Vector4& vec ) const
        {
            return Vector4( x * vec.x,
                            y * vec.y,
                            z * vec.z,
                            w * vec.w );
        }

        Vector4 operator*( float factor ) const
        {
            return Vector4( x * factor,
                            y * factor,
                            z * factor,
                            w * factor );
        }

        Vector4& operator*=( const Vector4& vec )
        {
            x *= vec.x;
            y *= vec.y;
            z *= vec.z;
            w *= vec.w;
            return ( *this );
        }

        Vector4& operator*=( float factor )
        {
            x *= factor;
            y *= factor;
            z *= factor;
            w *= factor;
            return ( *this );
        }

        Vector4 operator/( const Vector4& vec ) const
        {
            return Vector4( x / vec.x,
                            y / vec.y,
                            z / vec.z,
                            w / vec.w );
        }

        Vector4 operator/( float div ) const
        {
            float factor = 1.0f / div;
            return Vector4( x * factor,
                            y * factor,
                            z * factor,
                            w * factor );
        }

        Vector4& operator/=( const Vector4& vec )
        {
            x /= vec.x;
            y /= vec.y;
            z /= vec.z;
            w /= vec.w;
            return ( *this );
        }

        Vector4& operator/=( float div )
        {
            float factor = 1.0f / div;
            x *= factor;
            y *= factor;
            z *= factor;
            w *= factor;
            return ( *this );
        }

        Vector4& operator=( const Vector4& vec )
        {
            x = vec.x;
            y = vec.y;
            z = vec.z;
            w = vec.w;
            return ( *this );
        }

        bool operator==( const Vector4& vec ) const
        {
            return ( x == vec.x ) && ( y == vec.y ) && ( z == vec.z ) && ( w == vec.w );
        }

        bool operator!=( const Vector4& vec ) const
        {
            return ( x != vec.x ) || ( y != vec.y ) || ( z != vec.z ) || ( w != vec.w );
        }

        float operator|( const Vector4& vec ) const
        {
            // Dot product ( 3-Dimension )
            return ( x * vec.x + y * vec.y + z * vec.z );
        }


        Vector4 operator^( const Vector4& vec ) const
        {
            // Cross product ( 3-Dimension )
            return Vector4( y * vec.z,
                            z * vec.x,
                            x * vec.y,
                            1.0f );
        }

        std::string Serialize( ) const
        {
           return "\"Vector4\": { \"x\":" + std::to_string( x ) + ","
              + "\"y\":" + std::to_string( y ) + ","
              + "\"z\":" + std::to_string( z ) + ","
              + "\"w\":" + std::to_string( w ) + "}";
        }

        void DeSerialize( const json& jsonData )
        {
           x = jsonData[ "x" ];
           y = jsonData[ "y" ];
           z = jsonData[ "z" ];
           w = jsonData[ "w" ];
        }

        float GetX( ) const
        {
            return x;
        }

        float GetY( ) const
        {
            return y;
        }

        float GetZ( ) const
        {
            return z;
        }

        float GetW( ) const
        {
            return w;
        }

        void SetX( float xx )
        {
            x = xx;
        }

        void SetY( float yy )
        {
            y = yy;
        }

        void SetZ( float zz )
        {
            z = zz;
        }

        void SetW( float ww )
        {
            w = ww;
        }

        float Dot3( const Vector4& vec ) const
        {
            return ( x * vec.x + y * vec.y + z * vec.z );
        }

        float Dot( const Vector4& vec ) const
        {
            return ( x * vec.x + y * vec.y + z * vec.z + w * vec.w );
        }

        Vector4 Cross( const Vector4& vec ) const
        {
            return Vector4( y * vec.z,
                            z * vec.x,
                            x * vec.y,
                            1.0f );
        }

        float Size3Sqaured( ) const
        {
            return ( x * x + y * y + z * z );
        }

        float Size3( ) const
        {
            return std::sqrt( Size3Sqaured( ) );
        }

        float SizeSquared( ) const
        {
            return ( x * x + y * y + z * z + w * w );
        }

        float Size( ) const
        {
            return std::sqrt( SizeSquared( ) );
        }

        void Normalize3( )
        {
            float factor = 1.0f / Size3( );
            x *= factor;
            y *= factor;
            z *= factor;
        }

        Vector4 GetNormalized3( ) const
        {
            Vector4 temp = ( *this );
            temp.Normalize3( );
            return temp;
        }

        void Normalize( )
        {
            float factor = 1.0f / Size( );
            x *= factor;
            y *= factor;
            z *= factor;
            w *= factor;
        }

        Vector4 GetNormalized( ) const
        {
            Vector4 temp = ( *this );
            temp.Normalize( );
            return temp;
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


    public:
        static Vector4 Up;
        static Vector4 Down;
        static Vector4 Left;
        static Vector4 Right;
        static Vector4 Forward;
        static Vector4 Backward;
        static Vector4 One;
        static Vector4 Zero;

    };
}
