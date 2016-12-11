#pragma once
#pragma once
#include "MileMath.h"

/**
* 동차 좌표계를 나타내는 벡터 클래스. (w = 1.0f => Point, w = 0.0f => Vector)
*/
struct MILE_API ALGIN_16 MVector4
{
public:
    float X;
    float Y;
    float Z;
    float W;

public:
    MVector4( )
    {
    }

    /**
    * 4차원 벡터의 생성자
    *
    * @param InX X 좌표.
    * @param InY Y 좌표.
    * @param InZ Z 좌표.
    * @param InW W 좌표.
    */
    explicit MVector4( float InX, float InY, float InZ, float InW = 1.0f )
        : X( InX ), Y( InY ), Z( InZ ), W( InW )
    {
    }

    /**
    * 다른 벡터의 성분들과 각각 덧셈연산을 하여 그 결과를 반환함.
    *
    * @param Rhs 더할 벡터
    */
    MVector4 operator+( const MVector4& Rhs )
    {
        return MVector4( X + Rhs.X, Y + Rhs.Y, Z + Rhs.Z, W + Rhs.W );
    }

    /**
    * 다른 벡터의 성분들과 각각 뺄셈연산을 하여 그 결과를 반환함.
    *
    * @param Rhs 뺄 벡터.
    */
    MVector4 operator-( const MVector4& Rhs )
    {
        return MVector4( X - Rhs.X, Y - Rhs.Y, Z - Rhs.Z, W - Rhs.W );
    }

    /**
    * 다른 벡터의 성분들과 각각 곱셈연산을 하여 그 결과를 반환함.
    *
    * @param Rhs 곱할 벡터.
    */
    MVector4 operator*( const MVector4& Rhs )
    {
        return MVector4( X * Rhs.X, Y * Rhs.Y, Z * Rhs.Z, W * Rhs.W );
    }

    /**
    * 벡터의 성분들에 각각 실수를 곱한결과를 반환함.
    *
    * @param Rhs 곱할 실수.
    */
    MVector4 operator*( float Rhs )
    {
        return MVector4( X * Rhs, Y * Rhs, Z * Rhs, W * Rhs );
    }

    /**
    * 다른 벡터의 성분들과 각각 나눗셈연산을 하여 그 결과를 반환함.
    *
    * @param Rhs 나눌 벡터.
    */
    MVector4 operator/( const MVector4& Rhs )
    {
        return MVector4( X / Rhs.X, Y / Rhs.Y, Z / Rhs.Z, W / Rhs.W );
    }

    /**
    * 벡터의 성분들에 각각 실수를 나누어준 결과를 반환함.
    *
    * @param Rhs 나누어줄 실수.
    */
    MVector4 operator/( float Rhs )
    {
        float InvRhs = 1.0f / Rhs;
        return MVector4( X * InvRhs, Y * InvRhs, Z * InvRhs, W * InvRhs );
    }

    /**
    * 다른 벡터의 성분들을 자신의 성분들에다 더해줌.
    *
    * @param Rhs 더할 벡터.
    */
    MVector4 operator+=( const MVector4& Rhs )
    {
        X += Rhs.X;
        Y += Rhs.Y;
        Z += Rhs.Z;
        W += Rhs.W;
        return *this;
    }

    /**
    * 다른 벡터의 성분들을 자신의 성분들에서 빼줌.
    *
    * @param Rhs 뺄 벡터.
    */
    MVector4 operator-=( const MVector4& Rhs )
    {
        X -= Rhs.X;
        Y -= Rhs.Y;
        Z -= Rhs.Z;
        W -= Rhs.W;
        return *this;
    }

    /**
    * 다른 벡터의 성분들을 자신의 성분들에 곱해줌.
    *
    * @param Rhs 곱할 벡터.
    */
    MVector4 operator*=( const MVector4& Rhs )
    {
        X *= Rhs.X;
        Y *= Rhs.Y;
        Z *= Rhs.Z;
        W *= Rhs.W;
        return *this;
    }

    /**
    * 실수값을 자신의 성분에 각각 곱해줌.
    *
    * @param Rhs 곱할 실수.
    */
    MVector4 operator*=( float Rhs )
    {
        X *= Rhs;
        Y *= Rhs;
        Z *= Rhs;
        W *= Rhs;
        return *this;
    }

    /**
    * 다른 벡터의 성분들로 자신의 성분들을 나누어줌.
    *
    * @param Rhs 나누어줄 벡터.
    */
    MVector4 operator/=( const MVector4& Rhs )
    {
        X /= Rhs.X;
        Y /= Rhs.Y;
        Z /= Rhs.Z;
        W /= Rhs.W;
        return *this;
    }

    /**
    * 실수값으로 자신의 성분들을 나누어줌.
    *
    * @param Rhs 나누어줄 실수.
    */
    MVector4 operator/=( float Rhs )
    {
        float InvRhs = 1.0f / Rhs;
        X *= InvRhs;
        Y *= InvRhs;
        Z *= InvRhs;
        W *= InvRhs;
        return *this;
    }

    /**
    * 자신의 역원(Inverse Element)를 구함.
    */
    MVector4 operator-( )
    {
        return MVector4( -X, -Y, -Z, -W );
    }

    /**
    * 성분들이 같은지 비교함.
    *
    * @param Rhs 비교할 3차원 벡터.
    */
    bool operator==( const MVector4& Rhs )
    {
        return ( X == Rhs.X ) && ( Y == Rhs.Y ) && ( Z == Rhs.Z ) && ( W == Rhs.W );
    }

    /**
    * 성분들이 다른지 비교함.
    *
    * @param Rhs 비교할 3차원 벡터.
    */
    bool operator!=( const MVector4& Rhs )
    {
        return ( X != Rhs.X ) && ( Y != Rhs.Y ) && ( Z != Rhs.Z ) && ( W != Rhs.W );
    }

    /**
    * 3차원 벡터 요소에 대해 내적 연산을함.
    *
    * @param Rhs 내적연산을할 4차원 벡터.
    */
    float operator|( const MVector4& Rhs )
    {
        return this->DotProduct3( Rhs );
    }

    /**
    * 3차원 벡터 요소에 대해 외적 연산을함.
    *
    * @param Rhs 외적연산을할 4차원 벡터(우항).
    */
    MVector4 operator^( const MVector4& Rhs )
    {
        return this->CrossProduct3( Rhs );
    }

    /**
    * 3차원 벡터 요소에 대해 정규화된 벡터를 반환함. (자기자신에게 영향을 끼치지 않음.)
    */
    MVector4 Normalized3( ) const
    {
        float X = this->X;
        float Y = this->Y;
        float Z = this->Z;
        float InvLength = 1.0f / Math::Sqrt( ( X * X ) + ( Y * Y ) + ( Z * Z ) );
        return MVector4( X * InvLength, Y * InvLength, Z * InvLength, W );
    }

    /**
    * 3차원 벡터 요소에 대해 자신을 정규화함. (자기자신에게 영향을 끼침.)
    */
    void Normalize3( )
    {
        float InvLength = 1.0f / Math::Sqrt( ( X * X ) + ( Y * Y ) + ( Z * Z ) );
        X *= InvLength;
        Y *= InvLength;
        Z *= InvLength;
    }

    /**
    * 성분들에 새로운 값을 설정함.
    *
    * @param NewX 새로운 X 값.
    * @param NewY 새로운 Y 값.
    * @param NewZ 새로운 Z 값.
    * @param NewW 새로운 W 값.
    */
    void Set( float NewX, float NewY, float NewZ, float NewW )
    {
        X = NewX;
        Y = NewY;
        Z = NewZ;
        W = NewW;
    }

    /**
    * X 성분에 새로운 값을 설정함.
    *
    * @param NewX 새로운 X 값.
    */
    void SetX( float NewX )
    {
        X = NewX;
    }

    /**
    * Y 성분에 새로운 값을 설정함.
    *
    * @param NewY 새로운 Y 값.
    */
    void SetY( float NewY )
    {
        Y = NewY;
    }

    /**
    * Z 성분에 새로운 값을 설정함.
    *
    * @param NewZ 새로운 Z 값.
    */
    void SetZ( float NewZ )
    {
        Z = NewZ;
    }

    /**
    * W 성분에 새로운 값을 설정함.
    *
    * @param NewW 새로운 W 값.
    */
    void SetW( float NewW )
    {
        W = NewW;
    }

    /**
    * X 성분의 값을 반환함.
    */
    float GetX( ) const
    {
        return X;
    }

    /**
    * Y 성분의 값을 반환함.
    */
    float GetY( ) const
    {
        return Y;
    }

    /**
    * Z 성분의 값을 반환함.
    */
    float GetZ( ) const
    {
        return Z;
    }

    /**
    * W 성분의 값을 반환함.
    */
    float GetW( ) const
    {
        return W;
    }

    /**
    * 벡터의 길이(크기)를 반환함.
    */
    float Length( ) const
    {
        return Math::Sqrt( ( X * X ) + ( Y * Y ) + ( Z * Z ) + ( W * W ) );
    }

    /**
    * 3차원 벡터 요소에 대해 벡터의 길이(크기)를 반환함.
    */
    float Length3( ) const
    {
        return Math::Sqrt( ( X * X ) + ( Y * Y ) + ( Z * Z ) );
    }

    /**
    * 벡터의 길이(크기)의 제곱을 반환함.
    */
    float SquaredLength( ) const
    {
        return ( X * X ) + ( Y * Y ) + ( Z * Z ) + ( W * W );
    }

    /**
    * 3차원 벡터 요소에 대해 벡터의 길이(크기)의 제곱을 반환함.
    */
    float SquaredLength3( ) const
    {
        return ( X * X ) + ( Y * Y ) + ( Z * Z );
    }

    /**
    * 3차원 벡터 요소에 대해 내적 연산의 결과값을 반환함.
    *
    * @param Rhs 내적 연산을할 4차원 벡터.
    */
    float DotProduct3( const MVector4& Rhs )
    {
        return ( X * Rhs.X ) + ( Y * Rhs.Y ) + ( Z * Rhs.Z );
    }

    /**
    * 3차원 벡터 요소에 대해 외적 연산의 결과값을 반환함.
    *
    * @param Rhs 외적 연산을할 4차원 벡터.
    */
    MVector4 CrossProduct3( const MVector4& Rhs )
    {
        float NewX = ( Y * Rhs.Z ) - ( Z * Rhs.Z );
        float NewY = ( Z * Rhs.X ) - ( X * Rhs.Z );
        float NewZ = ( X * Rhs.Y ) - ( Y * Rhs.X );
        return MVector4( NewX, NewY, NewZ, 0.0f );
    }

    /**
    * 각 성분의 절대값을 성분으로가지는 벡터를 반환함.
    */
    MVector4 GetAbs( ) const
    {
        return MVector4( Math::Abs( X ), Math::Abs( Y ), Math::Abs( Z ), Math::Abs( W ) );
    }

    /**
    * 요소들이 모두 1인지 확인함.
    */
    bool IsOne( ) const
    {
        return ( X == 1.0f ) && ( Y == 1.0f ) && ( Z == 1.0f ) && ( W == 1.0f );
    }

    /**
    * 영 벡터(Zero Vector)인지 확인함.
    */
    bool IsZero( ) const
    {
        return ( X == 0.0f ) && ( Y == 0.0f ) && ( Z == 0.0f ) && ( W == 0.0f );
    }

    /**
    * 단위 벡터(Unit Vector)인지 확인함.
    */
    bool IsUnit( ) const
    {
        float Length = Math::Sqrt( ( X * X ) + ( Y * Y ) + ( Z * Z ) + ( W * W ) );
        return ( Length == 1.0f );
    }

    /**
    * 3차원 벡터 요소에 대해 단위 벡터(Unit Vector)인지 확인함.
    */
    bool IsUnit3( ) const
    {
        float Length = Math::Sqrt( ( X * X ) + ( Y * Y ) + ( Z * Z ) );
        return ( Length == 1.0f );
    }
};