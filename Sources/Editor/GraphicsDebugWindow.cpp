#include "GraphicsDebugWindow.h"
#include "Core/ImGuiHelper.h"
#include "Core/Engine.h"
#include "Rendering/RendererDX11.h"
#include "Rendering/RendererPBR.h"
#include "Rendering/GBuffer.h"
#include "Rendering/RenderTargetDX11.h"
#include "Rendering/DepthStencilBufferDX11.h"
#include "Rendering/Texture2dDX11.h"

namespace Mile
{
   namespace Editor
   {
      GraphicsDebugWindow::GraphicsDebugWindow() :
         m_selected(EGraphicsDebug::Position)
      {
      }

      void GraphicsDebugWindow::OnGUI()
      {
         /** @todo: code clean up */
         ImGuiWindowFlags windowFlag = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
         ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
         if (ImGui::Begin("Graphics Debugger", nullptr, windowFlag))
         {
            RendererDX11* renderer = Engine::GetRenderer();
            if (renderer != nullptr)
            {
               Vector2 renderRes = renderer->GetRenderResolution();
               ID3D11ShaderResourceView* outputSRV = nullptr;
               RendererPBR* rendererPBR = dynamic_cast<RendererPBR*>(renderer);

               if (rendererPBR != nullptr)
               {
                  GBuffer* gBuffer = rendererPBR->GetGBuffer();
                  RenderTargetDX11* ssaoBuffer = rendererPBR->GetSSAOBuffer();

                  if (gBuffer != nullptr)
                  {
                     if (ImGui::BeginMenuBar())
                     {
                        if (ImGui::BeginMenu("Views"))
                        {
                           bool bSelected = false;
                           if (gBuffer != nullptr)
                           {
                              bSelected = EGraphicsDebug::Position == m_selected;
                              if (ImGui::MenuItem("Position", nullptr, &bSelected, !bSelected))
                              {
                                 m_selected = EGraphicsDebug::Position;
                              }
                              bSelected = EGraphicsDebug::Albedo == m_selected;
                              if (ImGui::MenuItem("Albedo", nullptr, &bSelected, !bSelected))
                              {
                                 m_selected = EGraphicsDebug::Albedo;
                              }
                              bSelected = EGraphicsDebug::Normal == m_selected;
                              if (ImGui::MenuItem("Normal", nullptr, &bSelected, !bSelected))
                              {
                                 m_selected = EGraphicsDebug::Normal;
                              }
                              bSelected = EGraphicsDebug::Depth == m_selected;
                              if (ImGui::MenuItem("Depth", nullptr, &bSelected, !bSelected))
                              {
                                 m_selected = EGraphicsDebug::Depth;
                              }
                              bSelected = EGraphicsDebug::SSAO == m_selected;
                              if (ImGui::MenuItem("SSAO", nullptr, &bSelected, !bSelected))
                              {
                                 m_selected = EGraphicsDebug::SSAO;
                              }
                              bSelected = EGraphicsDebug::Lighting == m_selected;
                              if (ImGui::MenuItem("Lighting", nullptr, &bSelected, !bSelected))
                              {
                                 m_selected = EGraphicsDebug::Lighting;
                              }
                           }
                           ImGui::EndMenu();
                        }
                        ImGui::EndMenuBar();

                        switch (m_selected)
                        {
                        case Mile::Editor::EGraphicsDebug::Position:
                           outputSRV = gBuffer->GetPositionBuffer()->GetTexture()->GetShaderResourceView();
                           break;
                        case Mile::Editor::EGraphicsDebug::Albedo:
                           outputSRV = gBuffer->GetAlbedoBuffer()->GetTexture()->GetShaderResourceView();
                           break;
                        case Mile::Editor::EGraphicsDebug::Normal:
                           outputSRV = gBuffer->GetNormalBuffer()->GetTexture()->GetShaderResourceView();
                           break;
                        case Mile::Editor::EGraphicsDebug::Depth:
                           outputSRV = rendererPBR->GetDebugDepthBuffer()->GetTexture()->GetShaderResourceView();
                           break;
                        case Mile::Editor::EGraphicsDebug::Lighting:
                           outputSRV = rendererPBR->GetDebugLightingBuffer()->GetTexture()->GetShaderResourceView();
                           break;
                        case Mile::Editor::EGraphicsDebug::SSAO:
                           if (ssaoBuffer != nullptr)
                           {
                              outputSRV = rendererPBR->GetDebugSSAOBuffer()->GetTexture()->GetShaderResourceView();
                           }
                           break;
                        }

                        if (outputSRV != nullptr)
                        {
                           GUI::ImageRelativeToWindow(outputSRV, renderer->GetRenderResolution());
                        }
                     }
                  }
               }
            }
         }

         ImGui::End();
         ImGui::PopStyleColor();
      }
   }
}