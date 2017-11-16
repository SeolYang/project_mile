#pragma once

#include "../Core/Helper.h"

namespace Mile
{
    class Context;
    class MEAPI Component
    {
        friend class Entity;
    public:
        Component( Context* context );
        virtual ~Component( ) { }

        String Serialize( ) const;
        void DeSerialize( const String& json );

        bool IsActive( ) const { return m_bIsActive; }
        void SetActive( bool bIsActive );

        virtual void Reset( ) { }
        virtual void Start( ) { }
        virtual void Update( ) { }
        virtual void OnEnable( ) { }
        virtual void OnDisable( ) { }

    protected:
        Context*        m_context;
        bool            m_bIsActive;

    };
}