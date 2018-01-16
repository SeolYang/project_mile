#include "Vector4.h"

namespace Mile
{
    const Vector4 Vector4::Up( 0.0f, 1.0f, 0.0f, 1.0f );
    const Vector4 Vector4::Down( 0.0f, -1.0f, 0.0f, 1.0f );
    const Vector4 Vector4::Left( -1.0f, 0.0f, 0.0f, 1.0f );
    const Vector4 Vector4::Right( 1.0f, 0.0f, 0.0f, 1.0f );
    const Vector4 Vector4::Forward( 0.0f, 0.0f, 1.0f, 1.0f );
    const Vector4 Vector4::Backward( 0.0f, 0.0f, -1.0f, 1.0f );
    const Vector4 Vector4::One( 1.0f, 1.0f, 1.0f, 1.0f );
    const Vector4 Vector4::Zero( 0.0f, 0.0f, 0.0f, 1.0f );

    Vector4::Vector4( float xx, float yy, float zz, float ww ) :
       x( xx ),
       y( yy ),
       z( zz ),
       w( ww )
    {
    }

    Vector4::Vector4( ) :
       Vector4( 0.0f, 0.0f, 0.0f, 1.0f )
    {
    }

    Vector4::Vector4( const Vector4& vec ) :
       Vector4( vec.x, vec.y, vec.z, vec.w )
    {
    }

    Vector4 Vector4::operator+( const Vector4& vec ) const
    {
       return Vector4( x + vec.x,
                       y + vec.y,
                       z + vec.z,
                       w + vec.w );
    }

    Vector4 Vector4::operator+( float val ) const
    {
       return Vector4( x + val,
                       y + val,
                       z + val,
                       w + val );
    }

    Vector4& Vector4::operator+=( const Vector4& vec )
    {
       x += vec.x;
       y += vec.y;
       z += vec.z;
       w += vec.w;
       return ( *this );
    }

    Vector4& Vector4::operator+=( float val )
    {
       x += val;
       y += val;
       z += val;
       w += val;
       return ( *this );
    }

    Vector4 Vector4::operator-( const Vector4& vec ) const
    {
       return Vector4( x - vec.x,
                       y - vec.y,
                       z - vec.z,
                       w - vec.w );
    }

    Vector4 Vector4::operator-( float val ) const
    {
       return Vector4( x - val,
                       y - val,
                       z - val,
                       w - val );
    }

    Vector4& Vector4::operator-=( const Vector4& vec )
    {
       x -= vec.x;
       y -= vec.y;
       z -= vec.z;
       w -= vec.w;
       return ( *this );
    }

    Vector4& Vector4::operator-=( float val )
    {
       x -= val;
       y -= val;
       z -= val;
       w -= val;
       return ( *this );
    }

    Vector4 Vector4::operator*( const Vector4& vec ) const
    {
       return Vector4( x * vec.x,
                       y * vec.y,
                       z * vec.z,
                       w * vec.w );
    }

    Vector4 Vector4::operator*( float factor ) const
    {
       return Vector4( x * factor,
                       y * factor,
                       z * factor,
                       w * factor );
    }

    Vector4& Vector4::operator*=( const Vector4& vec )
    {
       x *= vec.x;
       y *= vec.y;
       z *= vec.z;
       w *= vec.w;
       return ( *this );
    }

    Vector4& Vector4::operator*=( float factor )
    {
       x *= factor;
       y *= factor;
       z *= factor;
       w *= factor;
       return ( *this );
    }

    Vector4 Vector4::operator/( const Vector4& vec ) const
    {
       return Vector4( x / vec.x,
                       y / vec.y,
                       z / vec.z,
                       w / vec.w );
    }

    Vector4 Vector4::operator/( float div ) const
    {
       float factor = 1.0f / div;
       return Vector4( x * factor,
                       y * factor,
                       z * factor,
                       w * factor );
    }

    Vector4& Vector4::operator/=( const Vector4& vec )
    {
       x /= vec.x;
       y /= vec.y;
       z /= vec.z;
       w /= vec.w;
       return ( *this );
    }

    Vector4& Vector4::operator/=( float div )
    {
       float factor = 1.0f / div;
       x *= factor;
       y *= factor;
       z *= factor;
       w *= factor;
       return ( *this );
    }

    Vector4& Vector4::operator=( const Vector4& vec )
    {
       x = vec.x;
       y = vec.y;
       z = vec.z;
       w = vec.w;
       return ( *this );
    }

    bool Vector4::operator==( const Vector4& vec ) const
    {
       return ( x == vec.x ) && ( y == vec.y ) && ( z == vec.z ) && ( w == vec.w );
    }

    bool Vector4::operator!=( const Vector4& vec ) const
    {
       return ( x != vec.x ) || ( y != vec.y ) || ( z != vec.z ) || ( w != vec.w );
    }

    float Vector4::operator|( const Vector4& vec ) const
    {
       // Dot product ( 3-Dimension )
       return ( x * vec.x + y * vec.y + z * vec.z );
    }


    Vector4 Vector4::operator^( const Vector4& vec ) const
    {
       // Cross product ( 3-Dimension )
       return Vector4( y * vec.z,
                       z * vec.x,
                       x * vec.y,
                       1.0f );
    }

    std::string Vector4::Serialize( ) const
    {
       return "{ \"x\": " + std::to_string( x ) + ", "
          + "\"y\": " + std::to_string( y ) + ", "
          + "\"z\": " + std::to_string( z ) + ", "
          + "\"w\": " + std::to_string( w ) + " }";
    }

    void Vector4::DeSerialize( const json& jsonData )
    {
       x = jsonData[ "x" ];
       y = jsonData[ "y" ];
       z = jsonData[ "z" ];
       w = jsonData[ "w" ];
    }

    float Vector4::GetX( ) const
    {
       return x;
    }

    float Vector4::GetY( ) const
    {
       return y;
    }

    float Vector4::GetZ( ) const
    {
       return z;
    }

    float Vector4::GetW( ) const
    {
       return w;
    }

    void Vector4::SetX( float xx )
    {
       x = xx;
    }

    void Vector4::SetY( float yy )
    {
       y = yy;
    }

    void Vector4::SetZ( float zz )
    {
       z = zz;
    }

    void Vector4::SetW( float ww )
    {
       w = ww;
    }

    float Vector4::Dot3( const Vector4& vec ) const
    {
       return ( x * vec.x + y * vec.y + z * vec.z );
    }

    float Vector4::Dot( const Vector4& vec ) const
    {
       return ( x * vec.x + y * vec.y + z * vec.z + w * vec.w );
    }

    Vector4 Vector4::Cross( const Vector4& vec ) const
    {
       return Vector4( y * vec.z,
                       z * vec.x,
                       x * vec.y,
                       1.0f );
    }

    float Vector4::Size3Sqaured( ) const
    {
       return ( x * x + y * y + z * z );
    }

    float Vector4::Size3( ) const
    {
       return std::sqrt( Size3Sqaured( ) );
    }

    float Vector4::SizeSquared( ) const
    {
       return ( x * x + y * y + z * z + w * w );
    }

    float Vector4::Size( ) const
    {
       return std::sqrt( SizeSquared( ) );
    }

    void Vector4::Normalize3( )
    {
       float factor = 1.0f / Size3( );
       x *= factor;
       y *= factor;
       z *= factor;
    }

    Vector4 Vector4::GetNormalized3( ) const
    {
       Vector4 temp = ( *this );
       temp.Normalize3( );
       return temp;
    }

    void Vector4::Normalize( )
    {
       float factor = 1.0f / Size( );
       x *= factor;
       y *= factor;
       z *= factor;
       w *= factor;
    }

    Vector4 Vector4::GetNormalized( ) const
    {
       Vector4 temp = ( *this );
       temp.Normalize( );
       return temp;
    }
}