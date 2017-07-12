#pragma once

#include "SubSystem.h"
#include "Rumia/Array.h"
#include <vector>

namespace Mile
{
    class MEAPI Context
    {
    public:
        Context( Allocator& allocator );
        ~Context( );

    public:
        /**
        * @brief Context가 소유하고있는 할당자를 가져옵니다.
        * @return Context의 할당자
        */
        Allocator& GetAllocator( ) { return m_allocator; }

        /**
        * @brief 새로운 Subsystem을 Context에 등록합니다.
        * @param newSubSystem 새로 등록할 Subsystem의 메모리 주소
        */
        void RegisterSubSystem( SubSystem* newSubSystem );
        template <typename T> T* GetSubSystem( );

        /**
        * @brief 주어진 타입의 Subsystem 이 Context에 등록되어있는지 알 수 있습니다.
        * @return Subsystem 소유 여부
        */
        template <typename T> bool HasSubSystem( ) const;

    private:
        Allocator& m_allocator;
        Array<SubSystem*> m_subSystems;

    };

    template <typename T>
    T* Context::GetSubSystem( )
    {
        for ( const auto& subSys : m_subSystems )
        {
            if ( typeid( T ) == typeid( *subSys ) )
            {
                return static_cast<T*>( subSys );
            }
        }

        return nullptr;
    }

    template <typename T>
    bool Context::HasSubSystem( ) const
    {
        return ( GetSubSystem<T>( ) != nullptr );
    }
}