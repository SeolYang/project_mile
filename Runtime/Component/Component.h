#pragma once

#include "../Core/Helper.h"

namespace Mile
{
   class Context;
   class MEAPI Component
   {
   public:
      Component( Context* context );
      virtual ~Component( ) { }

      template <typename Ty>
      static Ty* Create( Context* context )
      {
         return new Ty( context );
      }

      std::string Serialize( ) const;
      void DeSerialize( const json& jsonData );

      bool IsActive( ) const { return m_bIsActive; }
      void SetActive( bool bIsActive );

      virtual void Reset( ) { }
      virtual void Start( ) { }
      virtual void Update( ) { }
      virtual void OnEnable( ) { }
      virtual void OnDisable( ) { }

   protected:
      Component( ) :
         Component( nullptr )
      {
      }

   protected:
      Context*        m_context;
      bool            m_bIsActive;

   };
}