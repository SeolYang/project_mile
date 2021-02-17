#include "Layers/GameViewLayer.h"
#include "Core/ImGuiHelper.h"
#include "Core/Window.h"
#include "Core/Engine.h"
#include "Resource/ResourceManager.h"
#include "Resource/RenderTexture.h"
#include "Rendering/RenderTargetDX11.h"
#include "Rendering/Texture2dDX11.h"
#include "Rendering/RendererDX11.h"
#include "Component/CameraComponent.h"
#include "GameFramework/World.h"

namespace Mile
{
   namespace Editor
   {
      GameViewLayer::GameViewLayer(Context* context) :
         m_editorCameraRenderTex(nullptr),
         Layer(context)
      {
      }

      bool GameViewLayer::Init()
      {
         auto resMng = Engine::GetResourceManager();
         auto world = Engine::GetWorld();
         m_editorCameraRenderTex = resMng->Create<RenderTexture>(EDITOR_GAME_VIEW_RENDER_TEXTURE, false);
         m_editorCameraRenderTex->SetWidth(1920);
         m_editorCameraRenderTex->SetHeight(1080);
         //m_editorCamera = world->CreateEntity(TEXT("GameViewCamera"));
         //m_editorCamera->SetVisibleOnHierarchy(true);
         //m_editorCamera->SetSerializable(false);
         //m_editorCameraComponent = m_editorCamera->AddComponent<CameraComponent>();
         //m_editorCameraComponent->SetRenderTexture(m_editorCameraRenderTex);

         return true;
      }

      void GameViewLayer::OnIMGUIRender()
      {
         ImGuiWindowFlags windowFlag = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
         auto world = Engine::GetWorld();
         auto window = Engine::GetWindow();

         ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
         ImGui::Begin("Game", nullptr, windowFlag);
         if (true)//(renderer->IsRenderedFrame())
         {
            if (m_editorCameraRenderTex != nullptr)
            {
               auto contentRegion = ImGui::GetContentRegionAvail();

               ImVec2 windowSize = ImGui::GetWindowSize();
               float titleBarHeight = ImGui::GetCurrentWindow()->TitleBarHeight();
               ImVec2 actualContentArea = ImVec2{ windowSize.x, windowSize.y - titleBarHeight };

               auto renderTexWidth = (float)m_editorCameraRenderTex->GetWidth();
               auto renderTexHeight = (float)m_editorCameraRenderTex->GetHeight();
               Vector2 relativeOutputRes = FindResolutionWithAspectRatio(actualContentArea.x, actualContentArea.y, renderTexWidth/renderTexHeight);
               ImVec2 outputRes{ relativeOutputRes.x, relativeOutputRes.y };
               //m_editorCameraRenderTex->SetWidth((UINT32)referenceRes.x);
               //m_editorCameraRenderTex->SetHeight((UINT32)referenceRes.y);
               //m_editorCameraComponent->SetFov(GameViewDefaultFOV * ((outputRes.x + outputRes.y) / (GameViewDefaultWidth + GameViewDefaultHeight)));
               ImGui::SetCursorPosX((actualContentArea.x - outputRes.x) * 0.5f);
               ImGui::SetCursorPosY((actualContentArea.y - outputRes.y) * 0.5f + titleBarHeight);
               ImGui::Image((void*)m_editorCameraRenderTex->GetRenderTarget()->GetTexture()->GetSRV(), outputRes);
            }
         }
         ImGui::End();
         ImGui::PopStyleColor();
      }
   }
}

