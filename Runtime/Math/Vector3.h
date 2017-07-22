#pragma once
#include "Core\Helper.h"
#include <cmath>

namespace Mile
{
    class MEAPI Vector3
    {
    public:
        Vector3( float xx, float yy, float zz ) :
            x( xx ),
            y( yy ),
            z( zz )
        {
        }

        Vector3( ) :
            Vector3( 0.0f, 0.0f, 0.0f )
        {
        }

        Vector3( const Vector3& vec ) :
            Vector3( vec.x, vec.y, vec.z )
        {
        }

        Vector3 operator+( const Vector3& vec ) const
        {
            return Vector3( x + vec.x,
                            y + vec.y,
                            z + vec.z );
        }

        Vector3 operator+( float val ) const
        {
            return Vector3( x + val,
                            y + val,
                            z + val );
        }

        Vector3& operator+=( const Vector3& vec )
        {
            x += vec.x;
            y += vec.y;
            z += vec.z;
            return ( *this );
        }

        Vector3& operator+=( float val )
        {
            x += val;
            y += val;
            z += val;
            return ( *this );
        }

        Vector3 operator-( const Vector3& vec ) const
        {
            return Vector3( x - vec.x,
                            y - vec.y,
                            z - vec.z );
        }

        Vector3 operator-( float val ) const
        {
            return Vector3( x - val,
                            y - val,
                            z - val );
        }

        Vector3& operator-=( const Vector3& vec )
        {
            x -= vec.x;
            y -= vec.y;
            z -= vec.z;
            return ( *this );
        }

        Vector3& operator-=( float val )
        {
            x -= val;
            y -= val;
            z -= val;
            return ( *this );
        }

        Vector3 operator*( const Vector3& vec ) const
        {
            return Vector3( x * vec.x,
                            y * vec.y,
                            z * vec.z );
        }

        Vector3 operator*( float factor ) const
        {
            return Vector3( x * factor,
                            y * factor,
                            z * factor );
        }

        Vector3& operator*=( const Vector3& vec )
        {
            x *= vec.x;
            y *= vec.y;
            z *= vec.z;
            return ( *this );
        }

        Vector3& operator*=( float factor )
        {
            x *= factor;
            y *= factor;
            z *= factor;
            return ( *this );
        }

        Vector3 operator/( const Vector3& vec ) const
        {
            return Vector3( x / vec.x,
                            y / vec.y,
                            z / vec.z );
        }

        Vector3 operator/( float div ) const
        {
            float factor = 1.0f / div;
            return Vector3( x * factor,
                            y * factor,
                            z * factor );
        }

        Vector3& operator/=( const Vector3& vec )
        {
            x /= vec.x;
            y /= vec.y;
            z /= vec.z;
            return ( *this );
        }

        Vector3& operator/=( float div )
        {
            float factor = 1.0f / div;
            x *= factor;
            y *= factor;
            z *= factor;
            return ( *this );
        }

        Vector3& operator=( const Vector3& vec )
        {
            x = vec.x;
            y = vec.y;
            z = vec.z;
            return ( *this );
        }

        bool operator==( const Vector3& vec ) const
        {
            return ( x == vec.x ) && ( y == vec.y ) && ( z == vec.z );
        }

        bool operator!=( const Vector3& vec ) const
        {
            return ( x != vec.x ) || ( y != vec.y ) || ( z != vec.z );
        }

        float operator|( const Vector3& vec ) const
        {
            // Dot product
            return Dot( vec );
        }

        Vector3 operator^( const Vector3& vec ) const
        {
            // Cross product
            return Cross( vec );
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

        float Dot( const Vector3& vec ) const
        {
            return ( x * vec.x ) + ( y * vec.y ) + ( z * vec.z );
        }

        Vector3 Cross( const Vector3& vec ) const
        {
            return Vector3( y * vec.z, z * vec.x, x * vec.y );
        }

        float SizeSquared( ) const
        {
            return ( x * x + y * y + z * z );
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
            z *= factor;
        }

        Vector3 GetNormalized( ) const
        {
            Vector3 temp = ( *this );
            temp.Normalize( );
            return temp;
        }

    public:
        float x;
        float y;
        float z;

    };
}