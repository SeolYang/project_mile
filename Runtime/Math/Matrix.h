#pragma once
#include "Vector4.h"

namespace Mile
{
    class MEAPI Matrix
    {
    public:
        Matrix( float m11, float m12, float m13, float m14,
                float m21, float m22, float m23, float m24,
                float m31, float m32, float m33, float m34,
                float m41, float m42, float m43, float m44 )
        {
            m[ 0 ][ 0 ] = m11; m[ 0 ][ 1 ] = m12; m[ 0 ][ 2 ] = m13; m[ 0 ][ 3 ] = m14;
            m[ 1 ][ 0 ] = m21; m[ 1 ][ 1 ] = m22; m[ 1 ][ 2 ] = m23; m[ 1 ][ 3 ] = m24;
            m[ 2 ][ 0 ] = m31; m[ 2 ][ 1 ] = m32; m[ 2 ][ 2 ] = m33; m[ 2 ][ 3 ] = m34;
            m[ 3 ][ 0 ] = m41; m[ 3 ][ 1 ] = m42; m[ 3 ][ 2 ] = m43; m[ 3 ][ 3 ] = m44;
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

    public:
        union
        {
            struct
            {
                float m[ 4 ][ 4 ];
            };

            float m11, m12, m13, m14;
            float m21, m22, m23, m24;
            float m31, m32, m33, m34;
            float m41, m42, m43, m44;
        };

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
}