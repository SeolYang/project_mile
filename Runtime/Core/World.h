#pragma once

#include "SubSystem.h"

namespace Mile
{
    class Entity;
    class MEAPI World : public SubSystem
    {
    public:
        World( Context* context );
        virtual ~World( );

        /**
        * @brief    World를 초기화합니다.
        */
        virtual bool Init( ) override;

        /**
        * @brief    World에 생성되어있는 Entity들의 Start 함수를 호출합니다.
        */
        void Start( );

        /**
        * @brief    World에 생성되어있는 Entity들을 Update 합니다.
        */
        void Update( );

        /**
        * @brief    World에 새로운 Entity를 생성합니다.
        * @return   생성된 Entity
        */
        Entity* CreateEntity( );

        /**
        * @brief    World에 생성되어있는 Entitiy중 주어진 이름을 가진 Entity를 반환합니다.
        *           ( 단 같은 이름을 가지는 Entity중 제일 먼저 추가된 Entity를 반환함. )
        * @param    해당하는 이름을 가지는 Entity
        */
        Entity* GetEntityByName( const String& name );

        /**
        * @brief    World에 생성어있는 모든 Entity들의 배열을 반환합니다.
        * @param    Entity 배열
        */
        std::vector<Entity*> GetEntities( );

    private:
        std::vector<Entity*> m_entities;

    };
}