#pragma once
#include "Rendering/RenderingCore.h"
#include "Core/Engine.h"

namespace Mile
{
   /**
    * @brief   렌더러를 가지고있는 오브젝트들의 공통 인터페이스.
    *          이 클래스를 상속하는 자식 클래스들은 Init 함수를 구현할때 IsInitializable로 미리
    *          초기화 가능한지 확인하여야 하며, 초기화가 완료되었을시 RenderObject::ConfirmInit() 메서드를 호출하여야 합니다.
    *          Bind 함수를 구현할시에도 IsBindable 함수로, Bind 가능 여부를 체크하여야 합니다.
    */
   class RendererDX11;
   class MEAPI RenderObject
   {
   public:
      RenderObject(RendererDX11* renderer);
      virtual ~RenderObject();

      FORCEINLINE bool IsInitialized() const { return m_bIsInitialized; }
      /**
       * @brief   현재 오브젝트가 소유중인(또는 오브젝트를 소유하는) 렌더러가 엔진에서 사용하고 있는 주 렌더러인지 확인 합니다.
       */
      bool HasAvailableRenderer() const;

      /**
       * @brief   오브젝트를 초기할 수 있는지 체크합니다. 이미 초기화 되어있지 않아야하며, 
       *          엔진의 주 렌더러와 오브젝트의 렌더러가 동일하여야 합니다.
       */
      FORCEINLINE bool IsInitializable() const { return (!IsInitialized() && HasAvailableRenderer()); }

      /**
       * @brief   오브젝트를 바인드할 수 있는지 체크합니다. 초기화가 완료되어있어야 하며,
       *          엔진의 주 렌더러와 오브젝트의 렌더러가 동일하여야 합니다.
       *
       * @warn    오브젝트가 실제로 파이프라인에 바인드되어 있는지 여부와는 관련이 없습니다.   
       */
      FORCEINLINE bool IsBindable() const { return (IsInitialized() && HasAvailableRenderer()); }
      FORCEINLINE RendererDX11* GetRenderer() const { return m_renderer; }

   protected:
      FORCEINLINE void ConfirmInit() { m_bIsInitialized = true; }
      FORCEINLINE void ConfirmDeInit() { m_bIsInitialized = false; }

   private:
      RendererDX11* m_renderer;
      bool m_bIsInitialized;

   };
}