#pragma once
#include "Rendering/RenderingCore.h"

namespace Mile
{
   class RendererDX11;
   class MEAPI GPUProfiler
   {
   public:
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
               sum += data.second;
            }
         }

         return sum;
      }

   private:
      RendererDX11* m_renderer;
      std::map<std::string, GPUProfileData> m_profiles;
      std::map<std::string, double> m_profileTimes;
      UINT64 m_currentFrame;
      UINT64 m_queryLatency;
      std::vector<UINT64> m_drawCalls;
      std::vector<UINT64> m_vertices;
      std::vector<UINT64> m_triangles;
      UINT64 m_latestDrawCalls;
      UINT64 m_latestDrawVertices;
      UINT64 m_latestDrawTriangles;
      
   };

   class MEAPI ScopedProfile
   {
   public:
      ScopedProfile(GPUProfiler& profiler, const std::string& profileName) :
         m_profiler(profiler),
         m_profileName(profileName)
      {
         m_profiler.Begin(profileName);
      }

      ~ScopedProfile()
      {
         m_profiler.End(m_profileName);
      }

      ScopedProfile() = delete;
      ScopedProfile(const ScopedProfile&) = delete;
      ScopedProfile(ScopedProfile&&) = delete;

   private:
      GPUProfiler& m_profiler;
      std::string m_profileName;

   };

   class MEAPI ScopedContextProfile
   {
   public:
      ScopedContextProfile(GPUProfiler& profiler, const std::string& profileName, ID3D11DeviceContext& context, bool bIsDeferred = false) :
         m_profiler(profiler),
         m_context(context),
         m_profileName(profileName)
      {
         m_profiler.Begin(profileName, m_context, bIsDeferred);
      }

      ~ScopedContextProfile()
      {
         m_profiler.End(m_profileName, m_context);
      }

      ScopedContextProfile() = delete;
      ScopedContextProfile(const ScopedContextProfile&) = delete;
      ScopedContextProfile(ScopedContextProfile&&) = delete;

   private:
      GPUProfiler& m_profiler;
      std::string m_profileName;
      ID3D11DeviceContext& m_context;

   };
}