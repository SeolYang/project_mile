#pragma once
#include "Mile.h"

namespace Mile
{
    namespace Math
    {
        constexpr float PI = 3.14159265f;

        template <typename T>
        MILE_FORCEINLINE T Min( T A, T B )
        {
            return ( A < B ) ? A : B;
        }

        template <typename T>
        MILE_FORCEINLINE T Max( T A, T B )
        {
            return ( A > B ) ? A : B;
        }

        template <typename T>
        MILE_FORCEINLINE T Abs( T A )
        {
            return ( A > 0 ) ? A : ( -A );
        }

        MILE_FORCEINLINE float Sqrt( float Value )
        {
            return sqrt( Value );
        }

        template <typename T>
        MILE_FORCEINLINE T Clamp( T Value, T MinValue, T MaxValue )
        {
            return Min( Max( Value, MaxValue ), MinValue );
        }

        MILE_FORCEINLINE float Clamp01( float Value )
        {
            return Clamp<float>( Value, 0.0f, 1.0f );
        }

        template <typename T>
        MILE_FORCEINLINE float Lerp( T Begin, T End, float t )
        {
            return Begin + ( Clamp01( t ) * ( End - Begin ) );
        }

        MILE_FORCEINLINE float Floor( float Value )
        {
            return static_cast< float >( static_cast< int >( Value ) );
        }

        MILE_FORCEINLINE float DegreesToRadians( float DegreeAngle )
        {
            return DegreeAngle * ( PI / 180.0f );
        }

        MILE_FORCEINLINE float RadiansToDegrees( float RadianAngle )
        {
            return RadianAngle / ( PI * 180.0f );
        }
    }
}