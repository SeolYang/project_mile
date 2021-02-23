#pragma once
#include "Rendering/RenderingCore.h"

namespace Mile
{
   class RendererDX11;
   class MEAPI GPUProfiler
   {
   public:
      struct MEAPI GPUProfilDataeAccumulation
      {
         double RecentElapsed = 0.0;
         double AccumulatedTime = 0.0;
         double Samples = 0.0;
         double AvgTime = 0.0f;
         double MaxTime = 0.0f;
         double MinTime = DBL_MAX;
      };

      struct MEAPI GPUProfileData
      {
         bool bIsOnQuery = false;
         bool bIsDeferred = false;
         ID3D11Query* Disjoint = nullptr;
         ID3D11Query* Begin = nullptr;
         ID3D11Query* End = nullptr;
         double ElapsedTime = 0.0;
         UINT64 ElapsedTimeHz = 0;
         UINT64 QueryBeginFrame = 0;

         ~GPUProfileData()
         {
            Reset();
         }

         void Reset()
         {
            bIsOnQuery = false;
            bIsDeferred = false;
            SafeRelease(Disjoint);
            SafeRelease(Begin);
            SafeRelease(End);
            ElapsedTime = 0;
            QueryBeginFrame = 0;
         }
      };

   public:
      GPUProfiler(RendererDX11* renderer);
      ~GPUProfiler();

      GPUProfiler() = delete;
      GPUProfiler(const GPUProfiler&) = delete;
      GPUProfiler(GPUProfiler&&) = delete;

      void Begin(const std::string& name);
      void End(const std::string& name);
      void EndFrame();

      void Begin(const std::string& name, ID3D11DeviceContext& context, bool bIsDeferred = false);
      void End(const std::string& name, ID3D11DeviceContext& context);

      /** Thread-safe draw call count increment, Thread0 == Main Thread */
      void DrawCall(UINT64 vertices, UINT64 triangles, size_t threadIdx = 0) { ++m_drawCalls[threadIdx]; m_vertices[threadIdx] += vertices; m_triangles[threadIdx] += triangles; }
      UINT64 GetLatestDrawCalls() const { return m_latestDrawCalls; }
      UINT64 GetLatestVertices() const { return m_latestDrawVertices; }
      UINT64 GetLatestTriangles() const { return m_latestDrawTriangles; }

      UINT64 GetCurrentFrame() const { return m_currentFrame; }

      void SetQueryLatency(UINT64 queryLatency) { m_queryLatency = queryLatency; }
      UINT64 GetQueryLatency() const { return m_queryLatency; }

      const auto& GetProfileTimes() const { return m_profileTimes; }
      double GetProfileOverallTime() const
      {
         double sum = 0.0;
         for (const auto& data : m_profileTimes)
         {
            if (!(*m_profiles.find(data.first)).second.bIsDeferred)
            {
               sum += data.second.RecentElapsed;
            }
         }

         return sum;
      }

      void ClearDatas() 
      {
         m_profileTimes.clear();
         m_drawCalls.resize(0);
         m_vertices.resize(0); 
         m_triangles.resize(0); 
      }

   private:
      RendererDX11* m_renderer;
      std::unordered_map<std::string, GPUProfileData> m_profiles;
      std::map<std::string, GPUProfilDataeAccumulation> m_profileTimes;
      UINT64 m_currentFrame;
      UINT64 m_queryLatency;
      std::vector<UINT64> m_drawCalls;
      std::vector<UINT64> m_vertices;
      std::vector<UINT64> m_triangles;
      UINT64 m_latestDrawCalls;
      UINT64 m_latestDrawVertices;
      UINT64 m_latestDrawTriangles;
      
   };

   class MEAPI ScopedGPUProfile
   {
   public:
      ScopedGPUProfile(GPUProfiler& profiler, const std::string& profileName) :
         m_profiler(profiler),
         m_profileName(profileName)
      {
         m_profiler.Begin(profileName);
      }

      ~ScopedGPUProfile()
      {
         m_profiler.End(m_profileName);
      }

      ScopedGPUProfile() = delete;
      ScopedGPUProfile(const ScopedGPUProfile&) = delete;
      ScopedGPUProfile(ScopedGPUProfile&&) = delete;

   private:
      GPUProfiler& m_profiler;
      std::string m_profileName;

   };

   class MEAPI ScopedDeferredGPUProfile
   {
   public:
      ScopedDeferredGPUProfile(GPUProfiler& profiler, const std::string& profileName, ID3D11DeviceContext& deferredContext) :
         m_profiler(profiler),
         m_context(deferredContext),
         m_profileName(profileName)
      {
         m_globalMutex.lock();
         m_profiler.Begin(profileName, m_context, true);
         m_globalMutex.unlock();
      }

      ~ScopedDeferredGPUProfile()
      {
         m_globalMutex.lock();
         m_profiler.End(m_profileName, m_context);
         m_globalMutex.unlock();
      }

      ScopedDeferredGPUProfile() = delete;
      ScopedDeferredGPUProfile(const ScopedDeferredGPUProfile&) = delete;
      ScopedDeferredGPUProfile(ScopedDeferredGPUProfile&&) = delete;

   private:
      static std::mutex m_globalMutex;
      GPUProfiler& m_profiler;
      std::string m_profileName;
      ID3D11DeviceContext& m_context;

   };
}