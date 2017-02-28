#pragma once
#include "Vector.h"

namespace Mile
{
    /**
    * 위치, 회전, 크기 정보를 포함하고 이를 이용한 여러가지 연산을
    * 지원해주는 구조체이다.
    * @todo: Transform 정보를 이용해 변환행렬 계산
    * @todo: 다른 Transform과 Merge 될 수 있도록하기. ( T1 + T2 = T3 )
    * @todo: 계산된 변환행렬을 저장후 dirty 마크가 되있지않다면 저장된 
    *         변환행렬을 반환. (강제적으로 dirty 마킹 가능)
    */
    struct MILE_API Transform
    {
    public:
        inline Transform( ) :
            bIsDirty( true ),
            Translation( Vector::Zero ),
            Rotation( Vector::Zero ),
            Scale( Vector::One )
        {
        }

        FORCEINLINE void SetTranslation( const Vector& Target )
        {
            bIsDirty = true;
            Translation = Target;
        }

        Vector GetTranslation( ) const
        {
            return Translation;
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
        Vector Translation;
        Vector Rotation;
        Vector Scale;
        //Matrix PastMatrix;

    };
}