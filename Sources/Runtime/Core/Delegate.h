#pragma once
#include "Core/Logger.h"

namespace Mile
{
   DECLARE_LOG_CATEGORY_EXTERN(MileDelegate, Log);

   template<typename... Params>
   class MulticastDelegate;

   /**
   * @todo const function 을 지원하는 delegate 추가
   */
   template<typename RetType, typename... Params>
   class MEAPI Delegate
   {
   public:
      using FuncType = std::function<RetType(Params...)>;

   public:
      Delegate() :
         m_bIsBound(false)
      {
      }

      ~Delegate() = default;

      void BindLambda(const FuncType& lambda)
      {
         m_callback = lambda;
         m_bIsBound = true;
      }

      template <typename T>
      void Bind(RetType (T::*func)(Params...), T* ptr)
      {
         BindLambda([ptr, func](Params... args) {return (ptr->*func)(args...); });
      }

      void Unbind()
      {
         if (m_bIsBound)
         {
            m_callback = FuncType();
            m_bIsBound = false;
         }
      }

      RetType Execute(Params... args)
      {
         return m_callback(args...);
      }

      RetType ExecuteIfBound(Params... args)
      {
         if (IsBound())
         {
            return Execute(args...);
         }

         return RetType();
      }

      bool IsBound() const { return m_bIsBound; }

   private:
      FuncType m_callback;
      bool m_bIsBound;

   };

   template<typename... Params>
   class MEAPI Delegate<void, Params...>
   {
   public:
      using FuncType = std::function<void(Params...)>;

   public:
      Delegate() :
         m_bIsBound(false)
      {
      }

      virtual ~Delegate()
      {
         for (auto caster : m_casters)
         {
            caster->Remove(this);
         }
      }

      void BindLambda(const FuncType& lambda)
      {
         m_callback = lambda;
         m_bIsBound = true;
      }

      template <typename T>
      void Bind(void(T::* func)(Params...), T* ptr)
      {
         BindLambda([ptr, func](Params... args) { (ptr->*func)(args...); });
      }

      void Unbind()
      {
         if (m_bIsBound)
         {
            m_callback = FuncType();
            m_bIsBound = false;
         }
      }

      void Execute(Params... args)
      {
         m_callback(args...);
      }

      void ExecuteIfBound(Params... args)
      {
         if (IsBound())
         {
            Execute(args...);
         }
      }

      bool IsBound() const { return m_bIsBound; }

   private:
      void SubscribeCaster(MulticastDelegate<Params...>* caster)
      {
         if (caster != nullptr)
         {
            auto itr = std::find(m_casters.begin(), m_casters.end(), caster);
            if (itr == m_casters.end())
            {
               m_casters.push_back(caster);
            }
         }
      }
      void UnsubscribeCaster(MulticastDelegate<Params...>* caster)
      {
         if (caster != nullptr)
         {
            auto itr = std::find(m_casters.begin(), m_casters.end(), caster);
            if (itr != m_casters.end())
            {
               m_casters.erase(itr);
            }
         }
      }

   private:
      FuncType m_callback;
      bool m_bIsBound;

      friend MulticastDelegate<Params...>;
      std::vector<MulticastDelegate<Params...>*> m_casters;

   };

   template<typename... Params>
   class MEAPI MulticastDelegate
   {
   public:
      using FuncType = std::function<void(Params...)>;
      using Listener = Delegate<void, Params...>;

   public:
      MulticastDelegate()
      {
      }

      virtual ~MulticastDelegate()
      {
         for (auto listener : m_listeners)
         {
            listener->UnsubscribeCaster(this);
         }
      }

      void Add(Listener* target)
      {
         if (!HasListener(target) && target != nullptr)
         {
            m_listeners.push_back(target);
            target->SubscribeCaster(this);
         }
         else
         {
            ME_LOG(MileDelegate, Warning, TEXT("Trying to add duplicated or null listener to MulticastDelegate."));
         }
      }

      void Remove(Listener* target)
      {
         if (target != nullptr)
         {
            for (auto itr = m_listeners.begin(); itr != m_listeners.end(); ++itr)
            {
               if ((*itr) == target)
               {
                  m_listeners.erase(itr);
                  target->UnsubscribeCaster(this);
                  return;
               }
            }
         }
         else
         {
            ME_LOG(MileDelegate, Warning, TEXT("Trying to remove null listener from MulticastDelegate."));
         }
      }

      void Broadcast(Params... args)
      {
         for (auto listener : m_listeners)
         {
            listener->ExecuteIfBound(args...);
         }
      }

      bool HasListener(Listener* target) const
      {
         for (auto listener : m_listeners)
         {
            if (target == listener)
            {
               return true;
            }
         }

         return false;
      }

   private:
      std::vector<Listener*> m_listeners;

   };
}

#define DECLARE_DELEGATE_RetVal_Params(DelegateName, RetVal, ...) \
class MEAPI DelegateName##Delegate : public Mile::Delegate<RetVal, __VA_ARGS__> \
{ \
}
#define DECLARE_DELEGATE_Params(DelegateName, ...) DECLARE_DELEGATE_RetVal_Params(DelegateName, void, __VA_ARGS__)
#define DECLARE_DELEGATE(DelegateName) DECLARE_DELEGATE_RetVal_Params(DelegateName, void)

#define DECLARE_MULTICAST_DELEGATE(DelegateName) DECLARE_MULTICAST_DELEGATE_Params(DelegateName)
#define DECLARE_MULTICAST_DELEGATE_Params(DelegateName, ...) \
DECLARE_DELEGATE_Params(DelegateName, __VA_ARGS__); \
class MEAPI DelegateName##MulticastDelegate : public Mile::MulticastDelegate<__VA_ARGS__> \
{ \
}