#pragma once
#include "MileMath.h"

namespace Mile
{
	/**
	* 3차원 공간상의 위치 또는 방향을 나타내는 벡터 클래스.
	*/
	struct MILE_API ALGIN_16 MVector
	{
	public:
		/** 벡터의 X 성분. */
		float X;
		/** 벡터의 Y 성분. */
		float Y;
		/** 벡터의 Z 성분. */
		float Z;

	public:
		static const MVector One;
		static const MVector Zero;
		static const MVector Up;
		static const MVector Left;

	public:
		MVector( )
		{
		}

		/**
		* 3차원 벡터의 생성자
		*
		* @param InX X 좌표.
		* @param InY Y 좌표.
		* @param InZ Z 좌표.
		*/
		explicit MVector( float InX, float InY, float InZ )
			: X( InX ), Y( InY ), Z( InZ )
		{
		}

		/**
		* 다른 벡터의 성분들과 각각 덧셈연산을 하여 그 결과를 반환함.
		*
		* @param Rhs 더할 벡터
		*/
		MVector operator+( const MVector& Rhs )
		{
			return MVector( X + Rhs.X, Y + Rhs.Y, Z + Rhs.Z );
		}

		/**
		* 다른 벡터의 성분들과 각각 뺄셈연산을 하여 그 결과를 반환함.
		*
		* @param Rhs 뺄 벡터.
		*/
		MVector operator-( const MVector& Rhs )
		{
			return MVector( X - Rhs.X, Y - Rhs.Y, Z - Rhs.Z );
		}

		/**
		* 다른 벡터의 성분들과 각각 곱셈연산을 하여 그 결과를 반환함.
		*
		* @param Rhs 곱할 벡터.
		*/
		MVector operator*( const MVector& Rhs )
		{
			return MVector( X * Rhs.X, Y * Rhs.Y, Z * Rhs.Z );
		}

		/**
		* 벡터의 성분들에 각각 실수를 곱한결과를 반환함.
		*
		* @param Rhs 곱할 실수.
		*/
		MVector operator*( float Rhs )
		{
			return MVector( X * Rhs, Y * Rhs, Z * Rhs );
		}

		/**
		* 다른 벡터의 성분들과 각각 나눗셈연산을 하여 그 결과를 반환함.
		*
		* @param Rhs 나눌 벡터.
		*/
		MVector operator/( const MVector& Rhs )
		{
			return MVector( X / Rhs.X, Y / Rhs.Y, Z / Rhs.Z );
		}

		/**
		* 벡터의 성분들에 각각 실수를 나누어준 결과를 반환함.
		*
		* @param Rhs 나누어줄 실수.
		*/
		MVector operator/( float Rhs )
		{
			float InvRhs = 1.0f / Rhs;
			return MVector( X * InvRhs, Y * InvRhs, Z * InvRhs );
		}

		/**
		* 다른 벡터의 성분들을 자신의 성분들에다 더해줌.
		*
		* @param Rhs 더할 벡터.
		*/
		MVector operator+=( const MVector& Rhs )
		{
			X += Rhs.X;
			Y += Rhs.Y;
			Z += Rhs.Z;
			return *this;
		}

		/**
		* 다른 벡터의 성분들을 자신의 성분들에서 빼줌.
		*
		* @param Rhs 뺄 벡터.
		*/
		MVector operator-=( const MVector& Rhs )
		{
			X -= Rhs.X;
			Y -= Rhs.Y;
			Z -= Rhs.Z;
			return *this;
		}

		/**
		* 다른 벡터의 성분들을 자신의 성분들에 곱해줌.
		*
		* @param Rhs 곱할 벡터.
		*/
		MVector operator*=( const MVector& Rhs )
		{
			X *= Rhs.X;
			Y *= Rhs.Y;
			Z *= Rhs.Z;
			return *this;
		}

		/**
		* 실수값을 자신의 성분에 각각 곱해줌.
		*
		* @param Rhs 곱할 실수.
		*/
		MVector operator*=( float Rhs )
		{
			X *= Rhs;
			Y *= Rhs;
			Z *= Rhs;
			return *this;
		}

		/**
		* 다른 벡터의 성분들로 자신의 성분들을 나누어줌.
		*
		* @param Rhs 나누어줄 벡터.
		*/
		MVector operator/=( const MVector& Rhs )
		{
			X /= Rhs.X;
			Y /= Rhs.Y;
			Z /= Rhs.Z;
			return *this;
		}

		/**
		* 실수값으로 자신의 성분들을 나누어줌.
		*
		* @param Rhs 나누어줄 실수.
		*/
		MVector operator/=( float Rhs )
		{
			float InvRhs = 1.0f / Rhs;
			X *= InvRhs;
			Y *= InvRhs;
			Z *= InvRhs;
			return *this;
		}

		/**
		* 자신의 역원(Inverse Element)를 구함.
		*/
		MVector operator-( )
		{
			return MVector( -X, -Y, -Z );
		}

		/**
		* 성분들이 같은지 비교함.
		*
		* @param Rhs 비교할 3차원 벡터.
		*/
		bool operator==( const MVector& Rhs )
		{
			return ( X == Rhs.X ) && ( Y == Rhs.Y ) && ( Z == Rhs.Z );
		}

		/**
		* 성분들이 다른지 비교함.
		*
		* @param Rhs 비교할 3차원 벡터.
		*/
		bool operator!=( const MVector& Rhs )
		{
			return ( X != Rhs.X ) && ( Y != Rhs.Y ) && ( Z != Rhs.Z );
		}

		/**
		* 내적 연산을함.
		*
		* @param Rhs 내적연산을할 3차원 벡터.
		*/
		float operator|( const MVector& Rhs )
		{
			return this->DotProduct( Rhs );
		}

		/**
		* 외적 연산을함.
		*
		* @param Rhs 외적연산을할 3차원 벡터(우항).
		*/
		MVector operator^( const MVector& Rhs )
		{
			return this->CrossProduct( Rhs );
		}

		/**
		* 정규화된 벡터를 반환함. (자기자신에게 영향을 끼치지 않음.)
		*/
		MVector Normalized( ) const
		{
			float X = this->X;
			float Y = this->Y;
			float Z = this->Z;
			float InvLength = 1.0f / Math::Sqrt( ( X * X ) + ( Y * Y ) + ( Z * Z ) );
			return MVector( X * InvLength, Y * InvLength, Z * InvLength );
		}

		/**
		* 자신을 정규화함. (자기자신에게 영향을 끼침.)
		*/
		void Normalize( )
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
		*/
		void Set( float NewX, float NewY, float NewZ )
		{
			X = NewX;
			Y = NewY;
			Z = NewZ;
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
		* 벡터의 길이(크기)를 반환함.
		*/
		float Length( ) const
		{
			return Math::Sqrt( ( X * X ) + ( Y * Y ) + ( Z * Z ) );
		}

		/**
		* 벡터의 길이(크기)의 제곱을 반환함.
		*/
		float SquaredLength( ) const
		{
			return ( X * X ) + ( Y * Y ) + ( Z * Z );
		}

		/**
		* 내적 연산의 결과값을 반환함.
		*
		* @param Rhs 내적 연산을할 3차원 벡터.
		*/
		float DotProduct( const MVector& Rhs )
		{
			return ( X * Rhs.X ) + ( Y * Rhs.Y ) + ( Z * Rhs.Z );
		}

		/**
		* 외적 연산의 결과값을 반환함.
		*
		* @param Rhs 외적 연산을할 3차원 벡터.
		*/
		MVector CrossProduct( const MVector& Rhs )
		{
			float NewX = ( Y * Rhs.Z ) - ( Z * Rhs.Z );
			float NewY = ( Z * Rhs.X ) - ( X * Rhs.Z );
			float NewZ = ( X * Rhs.Y ) - ( Y * Rhs.X );
			return MVector( NewX, NewY, NewZ );
		}

		/**
		* 각 성분의 절대값을 성분으로가지는 벡터를 반환함.
		*/
		MVector GetAbs( ) const
		{
			return MVector( Math::Abs( X ), Math::Abs( Y ), Math::Abs( Z ) );
		}

		/**
		* 요소들이 모두 1인지 확인함.
		*/
		bool IsOne( ) const
		{
			return ( X == 1.0f ) && ( Y == 1.0f ) && ( Z == 1.0f );
		}

		/**
		* 영 벡터(Zero Vector)인지 확인함.
		*/
		bool IsZero( ) const
		{
			return ( X == 0.0f ) && ( Y == 0.0f ) && ( Z == 0.0f );
		}

		/**
		* 단위 벡터(Unit Vector)인지 확인함.
		*/
		bool IsUnit( ) const
		{
			float Length = Math::Sqrt( ( X * X ) + ( Y * Y ) + ( Z * Z ) );
			return ( Length == 1.0f );
		}
	};
}
