#pragma once
#include "Vector4.h"

namespace Mile
{
    struct Vector;
    /**
    * 4x4 행렬을 나타내는 행렬  test구조체
    * 수학에서의 곱을 따름
    * M(Row, Column)
    */
    struct MILE_API Matrix
    {
    public:
        Matrix(
            float M11, float M12, float M13, float M14,
            float M21, float M22, float M23, float M24,
            float M31, float M32, float M33, float M34,
            float M41, float M42, float M43, float M44 );
        Matrix( const Matrix& Mat );
        Matrix(
            const Vector4& Column1,
            const Vector4& Column2,
            const Vector4& Column3,
            const Vector4& Column4 );

        Matrix& operator=( const Matrix& Mat );

        Matrix operator+( const Matrix& Mat ) const;
        void operator+=( const Matrix& Mat );

        Matrix operator-( const Matrix& Mat ) const;
        void operator-=( const Matrix& Mat );

        Matrix operator*( float Scale ) const;
        Matrix operator*( const Matrix& Mat ) const;
        Vector4 operator*( const Vector4& Vector ) const;
        void operator*=( float Scale );
        void operator*=( const Matrix& Mat );

        Matrix GetTranspose( ) const;
        float GetDeterminant( ) const { return 1.0f; }
        Matrix GetAdjugate( ) const { return Identity; }
        Matrix GetInverse( ) const;

        static Matrix CreateTranslation( float XPos, float YPos, float ZPos );
        static Matrix CreateTranslation( const Vector& Position );
        static Matrix CreateScale( float Roll, float Pitch, float Yaw );
        static Matrix CreateScale( const Vector& Scale );
        static Matrix CreateRotationRoll( float Rad );
        static Matrix CreateRotationPitch( float Rad );
        static Matrix CreateRotationYaw( float Rad );
        static Matrix CreateRotation( float Roll, float Pitch, float Yaw );
        static Matrix CreateRotation( const Vector& Rotation );

    public:
        /* M[ROW][COLUMN] */
        float M11, M12, M13, M14;
        float M21, M22, M23, M24;
        float M31, M32, M33, M34;
        float M41, M42, M43, M44;

    public:
        static Matrix Identity;

    };
}