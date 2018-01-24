#pragma once

#include "Rendering.h"

namespace Mile
{
   enum class CullMode
   {
      CULL_NONE,
      CULL_FRONT,
      CULL_BACK
   };

   class RendererDX11;
   class MEAPI RasterizerState
   {
   public:
      RasterizerState( RendererDX11* renderer );
      ~RasterizerState( );

      bool Init( );

      bool Bind( ID3D11DeviceContext& deviceContext );

      void SetWireframeRender( bool bIsWireframe ) 
      { 
         if ( m_bIsWireframe != bIsWireframe )
         {
            m_bIsDirty = true;
         }

         m_bIsWireframe = bIsWireframe; 
      }

      bool IsWireframeRender( ) const { return m_bIsWireframe; }

      void SetCullMode( CullMode cullMode ) 
      { 
         if ( m_cullMode != cullMode )
         {
            m_bIsDirty = true;
         }

         m_cullMode = cullMode;
      }

      CullMode GetCullMode( ) const { return m_cullMode; }

      void SetFrontCounterClockwise( bool bFrontCounterClockwise )
      { 
         if ( m_bFrontCounterClockwise != bFrontCounterClockwise )
         {
            m_bIsDirty = true;
         }

         m_bFrontCounterClockwise = bFrontCounterClockwise; 
      }

      bool IsFrontCounterClockwise( ) const { return m_bFrontCounterClockwise; }

      void SetDepthBias( int depthBias )
      { 
         if ( depthBias != m_depthBias )
         {
            m_bIsDirty = true;
         }

         m_depthBias = depthBias;
      }

      int GetDepthBias( ) const { return m_depthBias; }

      void SetSlopeScaledDepthBias( float slopeScaledDepthBias ) 
      {
         if ( m_slopeScaledDepthBias != slopeScaledDepthBias )
         {
            m_bIsDirty = true;
         }

         m_slopeScaledDepthBias = slopeScaledDepthBias;
      }

      float GetSlopeScaledDepthBias( ) const { return m_slopeScaledDepthBias; }

      void SetDepthBiasClamp( float depthBiasClamp ) 
      {
         if ( m_depthBiasClamp != depthBiasClamp )
         {
            m_bIsDirty = true;
         }

         m_depthBiasClamp = depthBiasClamp; 
      }

      float GetDepthBiasClamp( ) const { return m_depthBiasClamp; }

      void SetDepthClipEnable( bool bIsDepthClipEnable ) 
      { 
         if ( m_bIsDepthClipEnable = bIsDepthClipEnable )
         {
            m_bIsDirty = true;
         }

         m_bIsDepthClipEnable = bIsDepthClipEnable;
      }

      bool IsDepthClipEnabled( ) const { return m_bIsDepthClipEnable; }

   private:
      RendererDX11 * m_renderer;
      bool           m_bIsDirty;

      bool           m_bIsWireframe;
      CullMode       m_cullMode;
      bool           m_bFrontCounterClockwise;
      int            m_depthBias;
      float          m_slopeScaledDepthBias;
      float          m_depthBiasClamp;
      bool           m_bIsDepthClipEnable;

      ID3D11RasterizerState*  m_rasterizerState;

   };
}