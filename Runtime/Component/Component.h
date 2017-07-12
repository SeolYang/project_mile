#pragma once

#include "..\Core\Helper.h"

namespace Mile
{
    class Context;
    class Transform;
    class MEAPI Component
    {
        friend class Entity;
    public:
        Component( Context* context );
        virtual ~Component( ) { }

        bool IsActive( ) const { return m_bIsActive; }
        void SetActive( bool bIsActive );

        virtual void Reset( ) = 0;
        virtual void Start( ) { }
        virtual void Update( ) { }
        virtual void OnEnable( ) { }
        virtual void OnDisable( ) { }

    protected:
        Context*        m_context;
        Transform*      m_transform;
        bool            m_bIsActive;

    };
}