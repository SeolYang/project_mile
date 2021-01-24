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
   DECLARE_LOG_CATEGORY_EXTERN(MileThreadPool, ELogVerbosity::Log);

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
         Context* context = GetContext();
         if (SubSystem::Init())
         {
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
                                       this->IsInitialized();
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

            ME_LOG(MileThreadPool, Log, TEXT("Thread Pool initialized."));
            SubSystem::InitSucceed();
            return true;
         }

         ME_LOG(MileThreadPool, Fatal, TEXT("Failed to initialize Thread Pool."));
         return false;
      }

      virtual void DeInit() override
      {
         if (IsInitialized())
         {
            m_workers.clear();
            ME_LOG(MileThreadPool, Log, TEXT("Thread Pool deinitialized."));
            SubSystem::DeInit();
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
      std::queue<std::function<void()>> m_tasks;

   };
}