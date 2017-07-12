#pragma once

#include "SubSystem.h"

namespace Mile
{
    class World;
    class MEAPI Engine : public SubSystem
    {
    public:
        Engine( Context* context );
        virtual ~Engine( ) { ShutDown( ); }

        /**
        * @brief Engine의 Subsystem 들을 초기화합니다.
        * @return 초기화 성공 여부
        */
        virtual bool Initialize( ) override;

        /**
        * @brief Engine의 Subsystem 들을 업데이트합니다.
        */
        void Update( );

        /**
        * @brief Engine의 Subsystem 들을 해제합니다.
        */
        void ShutDown( );

    private:
        World*  m_world;

    };
}