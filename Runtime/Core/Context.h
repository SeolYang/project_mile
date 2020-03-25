#pragma once

#include "Core/SubSystem.h"
#include <vector>

namespace Mile
{
   /**
    * @brief	모든 subsystem 들을 가지고있는 일종의 subsystem 들의 컨테이너 입니다. 엔진에서 사용할 모든 서브 시스템을 Context가
    *			소유하고 있어야하며, 최종적으로 할당 해제 책임도 지게 됩니다.
    */
   class MEAPI Context
   {
   public:
      Context();
      ~Context();

   public:
      /**
      * @brief 새로운 Subsystem을 Context에 등록합니다.
      * @param newSubSystem 새로 등록할 Subsystem의 메모리 주소
      */
      void RegisterSubSystem(SubSystem* newSubSystem);
      template <typename T> T* GetSubSystem();

      /**
      * @brief 주어진 타입의 Subsystem 이 Context에 등록되어있는지 알 수 있습니다.
      * @return Subsystem 소유 여부
      */
      template <typename T> bool HasSubSystem() const;

   private:
      std::vector<SubSystem*>  m_subSystems;

   };

   template <typename T>
   T* Context::GetSubSystem()
   {
      for (const auto& subSys : m_subSystems)
      {
         if (typeid(T) == typeid(*subSys))
         {
            return static_cast<T*>(subSys);
         }
      }

      return nullptr;
   }

   template <typename T>
   bool Context::HasSubSystem() const
   {
      return (GetSubSystem<T>() != nullptr);
   }
}