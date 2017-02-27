#pragma once
#include "ActorComponent.h"

namespace Mile
{
    /**
    * SceneComponent는 Transform 정보를 가지고 서로 붙이는 것을 지원하는 컴포넌트입니다.
    */
    class MILE_API SceneComponent : public ActorComponent
    {
    public:
        SceneComponent( Actor* Owner ) :
            ActorComponent( Owner )
        {
        }

    private:
        std::vector<SceneComponent*> Children;

    };
}