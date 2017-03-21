#include "Matrix.h"
#include "Vector.h"
#include "Transform.h"

namespace Mile
{
    Matrix Matrix::Identity = Matrix(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f );

    Matrix::Matrix(
        float M11, float M12, float M13, float M14,
        float M21, float M22, float M23, float M24,
        float M31, float M32, float M33, float M34,
        float M41, float M42, float M43, float M44 ) :
        M11( M11 ), M12( M12 ), M13( M13 ), M14( M14 ),
        M21( M21 ), M22( M22 ), M23( M23 ), M24( M24 ),
        M31( M31 ), M32( M32 ), M33( M33 ), M34( M34 ),
        M41( M41 ), M42( M42 ), M43( M43 ), M44( M44 )
    {
    }

    Matrix::Matrix( const Matrix& Mat ) :
        Matrix(
            Mat.M11, Mat.M12, Mat.M13, Mat.M14,
            Mat.M21, Mat.M22, Mat.M23, Mat.M24,
            Mat.M31, Mat.M32, Mat.M33, Mat.M34,
            Mat.M41, Mat.M42, Mat.M43, Mat.M44 )
    {
    }

    Matrix::Matrix(
        const Vector4& Column1,
        const Vector4& Column2,
        const Vector4& Column3,
        const Vector4& Column4 ) :
        Matrix(
            Column1.X, Column2.X, Column3.X, Column4.X,
            Column1.Y, Column2.Y, Column3.Y, Column4.Y,
            Column1.Z, Column2.Z, Column3.Z, Column4.Z,
            Column1.W, Column2.W, Column3.W, Column4.W )

    {
    }

    Matrix& Matrix::operator=( const Matrix& Mat )
    {
        M11 = Mat.M11; M12 = Mat.M12; M13 = Mat.M13; M14 = Mat.M14;
        M21 = Mat.M21; M22 = Mat.M22; M23 = Mat.M23; M24 = Mat.M24;
        M31 = Mat.M31; M32 = Mat.M32; M33 = Mat.M33; M34 = Mat.M34;
        M41 = Mat.M41; M42 = Mat.M42; M43 = Mat.M43; M44 = Mat.M44;
        return ( *this );
    }

    Matrix Matrix::operator+( const Matrix& Mat ) const
    {
        return Matrix(
            M11 + Mat.M11, M12 + Mat.M12, M13 + Mat.M13, M14 + Mat.M14,
            M21 + Mat.M21, M22 + Mat.M22, M23 + Mat.M23, M24 + Mat.M24,
            M31 + Mat.M31, M32 + Mat.M32, M33 + Mat.M33, M34 + Mat.M34,
            M41 + Mat.M41, M42 + Mat.M42, M43 + Mat.M43, M44 + Mat.M44 );
    }

    void Matrix::operator+=( const Matrix& Mat )
    {
        ( *this ) = ( ( *this ) + Mat );
    }

    Matrix Matrix::operator-( const Matrix& Mat ) const
    {
        return Matrix(
            M11 - Mat.M11, M12 - Mat.M12, M13 - Mat.M13, M14 - Mat.M14,
            M21 - Mat.M21, M22 - Mat.M22, M23 - Mat.M23, M24 - Mat.M24,
            M31 - Mat.M31, M32 - Mat.M32, M33 - Mat.M33, M34 - Mat.M34,
            M41 - Mat.M41, M42 - Mat.M42, M43 - Mat.M43, M44 - Mat.M44 );
    }

    void Matrix::operator-=( const Matrix& Mat )
    {
        ( *this ) = ( ( *this ) - Mat );
    }

    Matrix Matrix::operator*( float Scale ) const
    {
        return Matrix(
            M11 * Scale, M12 * Scale, M13 * Scale, M14 * Scale,
            M21 * Scale, M22 * Scale, M23 * Scale, M24 * Scale,
            M31 * Scale, M32 * Scale, M33 * Scale, M34 * Scale,
            M41 * Scale, M42 * Scale, M43 * Scale, M44 * Scale );
    }

    Matrix Matrix::operator*( const Matrix& Mat ) const
    {
        return Matrix(
            ( ( M11 * Mat.M11 ) + ( M12 * Mat.M21 ) + ( M13 * Mat.M31 ) + ( M14 * Mat.M41 ) ), // (1, 1)
            ( ( M11 * Mat.M12 ) + ( M12 * Mat.M22 ) + ( M13 * Mat.M32 ) + ( M14 * Mat.M42 ) ), // (1, 2)
            ( ( M11 * Mat.M13 ) + ( M12 * Mat.M23 ) + ( M13 * Mat.M33 ) + ( M14 * Mat.M43 ) ), // (1, 3)
            ( ( M11 * Mat.M14 ) + ( M12 * Mat.M24 ) + ( M13 * Mat.M34 ) + ( M14 * Mat.M44 ) ), // (1, 4)

            ( ( M21 * Mat.M11 ) + ( M22 * Mat.M21 ) + ( M23 * Mat.M31 ) + ( M24 * Mat.M41 ) ), // (2, 1)
            ( ( M21 * Mat.M12 ) + ( M22 * Mat.M22 ) + ( M23 * Mat.M32 ) + ( M24 * Mat.M42 ) ), // (2, 2)
            ( ( M21 * Mat.M13 ) + ( M22 * Mat.M23 ) + ( M23 * Mat.M33 ) + ( M24 * Mat.M43 ) ), // (2, 3)
            ( ( M21 * Mat.M14 ) + ( M22 * Mat.M24 ) + ( M23 * Mat.M34 ) + ( M24 * Mat.M44 ) ), // (2, 4)

            ( ( M31 * Mat.M11 ) + ( M32 * Mat.M21 ) + ( M33 * Mat.M31 ) + ( M34 * Mat.M41 ) ), // (3, 1)
            ( ( M31 * Mat.M12 ) + ( M32 * Mat.M22 ) + ( M33 * Mat.M32 ) + ( M34 * Mat.M42 ) ), // (3, 2)
            ( ( M31 * Mat.M13 ) + ( M32 * Mat.M23 ) + ( M33 * Mat.M33 ) + ( M34 * Mat.M43 ) ), // (3, 3)
            ( ( M31 * Mat.M14 ) + ( M32 * Mat.M24 ) + ( M33 * Mat.M34 ) + ( M34 * Mat.M44 ) ), // (3, 4)

            ( ( M41 * Mat.M11 ) + ( M42 * Mat.M21 ) + ( M43 * Mat.M31 ) + ( M44 * Mat.M41 ) ), // (4, 1)
            ( ( M41 * Mat.M12 ) + ( M42 * Mat.M22 ) + ( M43 * Mat.M32 ) + ( M44 * Mat.M42 ) ), // (4, 2)
            ( ( M41 * Mat.M13 ) + ( M42 * Mat.M23 ) + ( M43 * Mat.M33 ) + ( M44 * Mat.M43 ) ), // (4, 3)
            ( ( M41 * Mat.M14 ) + ( M42 * Mat.M24 ) + ( M43 * Mat.M34 ) + ( M44 * Mat.M44 ) )  // (4, 4)
        );
    }

    Vector4 Matrix::operator*( const Vector4& Vector ) const
    {
        float NewX = ( M11 * Vector.X ) + ( M12 * Vector.Y ) + ( M13 * Vector.Z ) + ( M14 * Vector.W ); // (1, 1) => X
        float NewY = ( M21 * Vector.X ) + ( M22 * Vector.Y ) + ( M23 * Vector.Z ) + ( M24 * Vector.W ); // (2, 1) => Y
        float NewZ = ( M31 * Vector.X ) + ( M32 * Vector.Y ) + ( M33 * Vector.Z ) + ( M34 * Vector.W ); // (3, 1) => Z
        float NewW = ( M41 * Vector.X ) + ( M42 * Vector.Y ) + ( M43 * Vector.Z ) + ( M44 * Vector.W ); // (4, 1) => W
        return Vector4( NewX, NewY, NewZ, NewW );
    }

    void Matrix::operator*=( float Scale )
    {
        ( *this ) = ( ( *this ) * Scale );
    }

    void Matrix::operator*=( const Matrix& Mat )
    {
        ( *this ) = ( ( *this ) * Mat );
    }

    Matrix Matrix::GetTranspose( ) const
    {
        return Matrix(
            M11, M21, M31, M41,
            M12, M22, M32, M42,
            M13, M23, M33, M43,
            M14, M24, M34, M44 );
    }

    Matrix Matrix::GetInverse( ) const
    {
        float InvDeterminant = 1.0f / GetDeterminant( );
        return ( GetAdjugate( ) * InvDeterminant );
    }

    Matrix Matrix::CreateTranslation( float XPos, float YPos, float ZPos )
    {
        return Matrix(
            1.0f,   0.0f,   0.0f,   XPos,
            0.0f,   1.0f,   0.0f,   YPos,
            0.0f,   0.0f,   1.0f,   ZPos,
            0.0f,   0.0f,   0.0f,   1.0f);
    }

    Matrix Matrix::CreateTranslation( const Vector& Position )
    {
        return CreateTranslation( Position.X, Position.Y, Position.Z );
    }

    Matrix Matrix::CreateScale( float Roll, float Pitch, float Yaw )
    {
        return Matrix(
            Pitch,  0.0f,   0.0f,   0.0f,
            0.0f,   Yaw,    0.0f,    0.0f,
            0.0f,   0.0f,   Roll,    0.0,
            0.0,    0.0,    0.0,    1.0f );
    }

    Matrix Matrix::CreateScale( const Vector& Scale )
    {
        return CreateScale( Scale.Z, Scale.X, Scale.Y );
    }

    Matrix Matrix::CreateRotationRoll( float Rad )
    {
        return Matrix(
            1.0f,   0.0f,           0.0f,           0.0f,
            0.0f,   cosf( Rad ),    -sinf( Rad ),   0.0f,
            0.0f,   sinf( Rad ),    cosf( Rad ),    0.0f,
            0.0f,   0.0f,           0.0f,           1.0f );
    }

    Matrix Matrix::CreateRotationPitch( float Rad )
    {
        return Matrix(
            cosf( Rad ),    0.0f,   sinf( Rad ),    0.0f,
            0.0f,           1.0f,   0.0f,           0.0f,
            -sinf( Rad ),   0.0f,   cosf( Rad ),    0.0f,
            0.0f,           0.0f,   0.0f,           1.0f );
    }

    Matrix Matrix::CreateRotationYaw( float Rad )
    {
        return Matrix(
            cosf( Rad ),    -sinf( Rad ),   0.0f,   0.0f,
            sinf( Rad ),    cosf( Rad ),    0.0f,   0.0f,
            0.0f,           0.0f,           1.0f,   0.0f,
            0.0f,           0.0f,           0.0f,   1.0f );
    }

    Matrix Matrix::CreateRotation( float Roll, float Pitch, float Yaw )
    {
        return CreateRotationRoll( Roll ) * CreateRotationPitch( Pitch ) * CreateRotationYaw( Yaw );
    }

    Matrix Matrix::CreateRotation( const Vector& Rotation )
    {
        return CreateRotation( Rotation.Z, Rotation.X, Rotation.Y );
    }

    Matrix Matrix::CreateTransform( const Transform & Transform )
    {
        return ( CreateScale( Transform.GetScale( ) ) ) *
            ( CreateRotation( Transform.GetRotation( ) ) ) *
            ( CreateTranslation( Transform.GetPosition( ) ) );
    }
}