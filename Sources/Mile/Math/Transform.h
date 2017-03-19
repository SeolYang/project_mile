#pragma once
#include "Vector.h"

namespace Mile
{
    /**
    * 위치, 회전, 크기 정보를 포함하고 이를 이용한 여러가지 연산을
    * 지원해주는 구조체이다.
    */
    struct MILE_API Transform
    {
    public:
        inline Transform( ) :
            bIsDirty( true ),
            Position( Vector::Zero ),
            Rotation( Vector::Zero ),
            Scale( Vector::One )
        {
        }

        inline Transform( const Vector& InPosition, const Vector& InRotation, const Vector& InScale ) :
            bIsDirty( true ),
            Position( InPosition ),
            Rotation( InRotation ),
            Scale( InScale )
        {
        }

        Transform operator-( ) const
        {
            return Transform( -Position, -Rotation, -Scale );
        }

        Transform operator+( const Transform& T ) const
        {
            return Transform( Position + T.Position, Rotation + T.Rotation, Scale + T.Scale );
        }

        Transform operator-( const Transform& T ) const
        {
            return ( *this ) + ( -T );
        }

        Transform operator*( const Transform& T ) const
        {
            return Transform( Position * T.Position, Rotation * T.Rotation, Scale * T.Scale );
        }

        Transform operator/( const Transform& T ) const
        {
            return Transform( Position / T.Position, Rotation / T.Rotation, Scale / T.Scale );
        }

        Transform& operator+=( const Transform& T )
        {
            Position += T.Position;
            Rotation += T.Rotation;
            Scale += T.Scale;

            return ( *this );
        }

        Transform& operator-=( const Transform& T )
        {
            return ( ( *this ) += ( -T ) );
        }

        Transform& operator*=( const Transform& T )
        {
            Position *= T.Position;
            Rotation *= T.Rotation;
            Scale *= T.Scale;

            return ( *this );
        }

        Transform& operator/=( const Transform& T )
        {
            Position /= T.Position;
            Rotation /= T.Rotation;
            Scale /= T.Scale;

            return ( *this );
        }

        FORCEINLINE void SetPosition( const Vector& Target )
        {
            bIsDirty = true;
            Position = Target;
        }

        Vector GetPosition( ) const
        {
            return Position;
        }

        FORCEINLINE void SetRotation( const Vector& Target )
        {
            bIsDirty = true;
            Rotation = Target;
        }

        Vector GetRotation( ) const
        {
            return Rotation;
        }

        FORCEINLINE void SetScale( const Vector& Target )
        {
            bIsDirty = true;
            Scale = Target;
        }

        Vector GetScale( ) const
        {
            return Scale;
        }

        FORCEINLINE void SetIsDirty( bool bNewDirtyState = true )
        {
            bIsDirty = bNewDirtyState;
        }

        FORCEINLINE bool IsDirty( ) const
        {
            return bIsDirty;
        }


    private:
        bool   bIsDirty;
        Vector Position;
        Vector Rotation;
        Vector Scale;

    };
}