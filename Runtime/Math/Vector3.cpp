#include "Vector3.h"

namespace Mile
{
   Vector3::Vector3( float xx, float yy, float zz ) :
      x( xx ),
      y( yy ),
      z( zz )
   {
   }

   Vector3::Vector3( ) :
      Vector3( 0.0f, 0.0f, 0.0f )
   {
   }

   Vector3::Vector3( const Vector3& vec ) :
      Vector3( vec.x, vec.y, vec.z )
   {
   }

   Vector3 Vector3::operator+( const Vector3& vec ) const
   {
      return Vector3( x + vec.x,
                      y + vec.y,
                      z + vec.z );
   }

   Vector3 Vector3::operator+( float val ) const
   {
      return Vector3( x + val,
                      y + val,
                      z + val );
   }

   Vector3& Vector3::operator+=( const Vector3& vec )
   {
      x += vec.x;
      y += vec.y;
      z += vec.z;
      return ( *this );
   }

   Vector3& Vector3::operator+=( float val )
   {
      x += val;
      y += val;
      z += val;
      return ( *this );
   }

   Vector3 Vector3::operator-( const Vector3& vec ) const
   {
      return Vector3( x - vec.x,
                      y - vec.y,
                      z - vec.z );
   }

   Vector3 Vector3::operator-( float val ) const
   {
      return Vector3( x - val,
                      y - val,
                      z - val );
   }

   Vector3& Vector3::operator-=( const Vector3& vec )
   {
      x -= vec.x;
      y -= vec.y;
      z -= vec.z;
      return ( *this );
   }

   Vector3& Vector3::operator-=( float val )
   {
      x -= val;
      y -= val;
      z -= val;
      return ( *this );
   }

   Vector3 Vector3::operator*( const Vector3& vec ) const
   {
      return Vector3( x * vec.x,
                      y * vec.y,
                      z * vec.z );
   }

   Vector3 Vector3::operator*( float factor ) const
   {
      return Vector3( x * factor,
                      y * factor,
                      z * factor );
   }

   Vector3& Vector3::operator*=( const Vector3& vec )
   {
      x *= vec.x;
      y *= vec.y;
      z *= vec.z;
      return ( *this );
   }

   Vector3& Vector3::operator*=( float factor )
   {
      x *= factor;
      y *= factor;
      z *= factor;
      return ( *this );
   }

   Vector3 Vector3::operator/( const Vector3& vec ) const
   {
      return Vector3( x / vec.x,
                      y / vec.y,
                      z / vec.z );
   }

   Vector3 Vector3::operator/( float div ) const
   {
      float factor = 1.0f / div;
      return Vector3( x * factor,
                      y * factor,
                      z * factor );
   }

   Vector3& Vector3::operator/=( const Vector3& vec )
   {
      x /= vec.x;
      y /= vec.y;
      z /= vec.z;
      return ( *this );
   }

   Vector3& Vector3::operator/=( float div )
   {
      float factor = 1.0f / div;
      x *= factor;
      y *= factor;
      z *= factor;
      return ( *this );
   }

   Vector3& Vector3::operator=( const Vector3& vec )
   {
      x = vec.x;
      y = vec.y;
      z = vec.z;
      return ( *this );
   }

   bool Vector3::operator==( const Vector3& vec ) const
   {
      return ( x == vec.x ) && ( y == vec.y ) && ( z == vec.z );
   }

   bool Vector3::operator!=( const Vector3& vec ) const
   {
      return ( x != vec.x ) || ( y != vec.y ) || ( z != vec.z );
   }

   float Vector3::operator|( const Vector3& vec ) const
   {
      // Dot product
      return Dot( vec );
   }

   Vector3 Vector3::operator^( const Vector3& vec ) const
   {
      // Cross product
      return Cross( vec );
   }

   //std::string Vector3::Serialize( ) const
   //{
   //   return "{ \"x\": " + std::to_string( x ) + ", "
   //      + "\"y\": " + std::to_string( y ) + ", "
   //      + "\"z\": " + std::to_string( z ) + " }";
   //}

   json Vector3::Serialize() const
   {
	   json serialized;
	   serialized["x"] = x;
	   serialized["y"] = y;
	   serialized["z"] = z;
	   return serialized;
   }

   void Vector3::DeSerialize( const json& jsonData )
   {
      x = jsonData[ "x" ];
      y = jsonData[ "y" ];
      z = jsonData[ "z" ];
   }

   float Vector3::GetX( ) const
   {
      return x;
   }

   float Vector3::GetY( ) const
   {
      return y;
   }

   float Vector3::GetZ( ) const
   {
      return z;
   }

   void Vector3::SetX( float xx )
   {
      x = xx;
   }

   void Vector3::SetY( float yy )
   {
      y = yy;
   }

   void Vector3::SetZ( float zz )
   {
      z = zz;
   }

   float Vector3::Dot( const Vector3& vec ) const
   {
      return ( x * vec.x ) + ( y * vec.y ) + ( z * vec.z );
   }

   Vector3 Vector3::Cross( const Vector3& vec ) const
   {
      return Vector3( y * vec.z - z * vec.y,
                      z * vec.x - x * vec.z,
                      x * vec.y - y * vec.x );
   }

   float Vector3::SizeSquared( ) const
   {
      return ( x * x + y * y + z * z );
   }

   float Vector3::Size( ) const
   {
      return std::sqrt( SizeSquared( ) );
   }

   void Vector3::Normalize( )
   {
      float factor = ( 1.0f / Size( ) );
      x *= factor;
      y *= factor;
      z *= factor;
   }

   Vector3 Vector3::GetNormalized( ) const
   {
      Vector3 temp = ( *this );
      temp.Normalize( );
      return temp;
   }
}