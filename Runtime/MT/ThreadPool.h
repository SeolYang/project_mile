#pragma once
#include "Core/SubSystem.h"
#include "Core/Logger.h"
#include <thread>
#include <future>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <vector>
#include <queue>
#include <forward_list>
#include <memory>
#include <stdexcept>

namespace Mile
{
   // Default ThreadPool size : ( Physical Core + Logical Core ) - 1
   class MEAPI ThreadPool : public SubSystem
   {
   public:
      ThreadPool(Context* context, size_t numberOfThreads) :
         m_threadNum(numberOfThreads),
         m_bStop(false),
         SubSystem(context)
      {
      }

      ThreadPool(Context* context) :
         ThreadPool(context, std::thread::hardware_concurrency() - 1)
      {
      }

      ~ThreadPool()
      {
         {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_bStop = true;
            // Compel initialized to ended up every threads.
            m_bIsInitialized = true;
         }

         // Wake up all threads to stop thread pool.
         m_condition.notify_all();

         for (std::thread& worker : m_workers)
         {
            // Wait for join all threads.
            worker.join();
         }

         DeInit();
      }

      virtual bool Init() override
      {
         if (m_context == nullptr || m_bIsInitialized)
         {
            MELog(m_context, TEXT("ThreadPool"), ELogType::WARNING, TEXT("Thread Pool already initialized."), true);
            return false;
         }

         // Thread Pool must init once.
         // But its able to mark as Deinitialized.
         m_bStop = false;
         if (m_workers.empty())
         {
            for (size_t idx = 0; idx < m_threadNum; ++idx)
            {
               // Actual thread initialize.
               m_workers.emplace_front([this]
                  {
                     while (true)
                     {
                        std::function<void()> task;

                        // Mutex Scope
                        {
                           std::unique_lock<std::mutex> lock(this->m_mutex);

                           // Waiting for task.
                           this->m_condition.wait(lock, [this]
                              {
                                 return (this->m_bStop ||
                                    !this->m_tasks.empty()) &&
                                    this->m_bIsInitialized;
                              });

                           if (this->m_bStop && this->m_tasks.empty())
                           {
                              return;
                           }

                           task = std::move(this->m_tasks.front());
                           this->m_tasks.pop();
                        }

                        // Running task in thread!
                        task();
                     }
                  });
            }
         }

         MELog(m_context, TEXT("ThreadPool"), ELogType::MESSAGE, TEXT("Thread Pool initialized."), true);
         m_bIsInitialized = true;
         return true;
      }

      virtual void DeInit() override
      {
         if (m_bIsInitialized)
         {
            SubSystem::DeInit();
            MELog(m_context, TEXT("ThreadPool"), ELogType::MESSAGE, TEXT("Thread Pool deinitialized."), true);
         }
      }

      template <typename FUNC, typename... Args>
      auto AddTask(FUNC&& func, Args&& ... args)
      {
         using return_type = typename std::result_of<FUNC(Args...)>::type;

         auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(
               std::forward<FUNC>(func),
               std::forward<Args>(args)...));

         std::future<return_type> result = task->get_future();
         {
            std::unique_lock<std::mutex> lock(m_mutex);

            if (m_bStop)
            {
               throw std::runtime_error("Add on stopped Thread Pool!");
            }

            m_tasks.emplace([task] { (*task)(); });
         }

         // Wake up one thread in pool.
         m_condition.notify_one();
         return result;
      }

   private:
      size_t      m_threadNum;
      bool        m_bStop;

      std::mutex                     m_mutex;
      std::condition_variable        m_condition;
      std::forward_list<std::thread> m_workers;
      std::queue< std::function<void()>> m_tasks;

   };
}