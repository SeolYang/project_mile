#pragma once

#include "SubSystem.h"

namespace Mile
{
    class Entity;
    template<typename Ty>
    class PlainText;

    class MEAPI World : public SubSystem
    {
    public:
        World( Context* context );
        virtual ~World( );

        //std::string Serialize( ) const;
		json Serialize() const;
        void DeSerialize( const json& jsonData );

        /**
        * @brief    World를 초기화합니다.
        */
        virtual bool Init( ) override;
        virtual void DeInit( ) override;

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
        Entity* CreateEntity( const String& name );

        /**
        * @brief    World에 생성되어있는 Entitiy중 주어진 이름을 가진 Entity를 반환합니다.
        *           ( 단 같은 이름을 가지는 Entity중 제일 먼저 추가된 Entity를 반환함. )
        * @param    해당하는 이름을 가지는 Entity
        */
        Entity* GetEntityByName( const String& name );

        /**
        * @brief    World에 생성되어있는 모든 Entity들의 배열을 반환합니다.
        * @return    Entity 배열
        */
        std::vector<Entity*> GetEntities( );

        /**
        * @brief    World에 생성되어있는 Entity 중 Root Entitiy들의 배열을 반환합니다.
        */
        std::vector<Entity*> GetRootEntities( );

        bool LoadFrom( const String& filePath );
        bool SaveTo( const String& filePath );
        bool Save( );

    private:
        std::vector<Entity*> m_entities;
        PlainText<std::string>* m_loadedData;

    };
}