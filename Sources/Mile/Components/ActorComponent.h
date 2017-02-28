#pragma once
#include "Component.h"

namespace Mile
{
    /**
    * ActorComponent는 Actor에 추가될 수 있는 재사용 가능한 동작을 정의하는 Component의 기본 클래스입니다.
    */
    class MILE_API ActorComponent : public Component
    {
    public:
        ActorComponent( Object* Owner ) :
            Component( Owner )
        {
        }

    private:
        std::vector<ActorComponent*> Children;

    };
}