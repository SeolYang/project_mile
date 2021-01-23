#include "Layers/GameViewLayer.h"
#include "Core/imguiHelper.h"
#include "Core/Window.h"
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
      constexpr float GameViewDefaultWidth = 1280.0f;
      constexpr float GameViewDefaultHeight = 720.0f;
      constexpr float GameViewDefaultFOV = 45.0f;
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
         m_editorCameraRenderTex->SetWidth(GameViewDefaultWidth);
         m_editorCameraRenderTex->SetHeight(GameViewDefaultHeight);
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
         Vector2 maxRes = window->GetResolution();

         ImGui::SetNextWindowSizeConstraints(ImVec2{ 50.0f, 50.0f }, ImVec2{ maxRes.x, maxRes.y });
         ImGui::Begin("Game", nullptr, windowFlag);
         if (m_editorCameraRenderTex != nullptr)
         {
            WindowFocusedEffect(0, 255, 0, 0.9f);
            auto contentRegion = ImGui::GetContentRegionAvail();

            ImVec2 windowSize = ImGui::GetWindowSize();
            float titleBarHeight = ImGui::GetCurrentWindow()->TitleBarHeight();
            ImVec2 actualContentArea = ImVec2{ windowSize.x, windowSize.y - titleBarHeight };

            Vector2 relativeOutputRes = FindResolutionWithAspectRatio(actualContentArea.x, actualContentArea.y, (maxRes.x / maxRes.y));
            ImVec2 outputRes{ relativeOutputRes.x, relativeOutputRes.y };
            m_editorCameraRenderTex->SetWidth(maxRes.x);
            m_editorCameraRenderTex->SetHeight(maxRes.y);
            //m_editorCameraComponent->SetFov(GameViewDefaultFOV * ((outputRes.x + outputRes.y) / (GameViewDefaultWidth + GameViewDefaultHeight)));
            ImGui::SetCursorPosX((actualContentArea.x - outputRes.x) * 0.5f);
            ImGui::SetCursorPosY((actualContentArea.y - outputRes.y ) * 0.5f + titleBarHeight);
            ImGui::Image((void*)m_editorCameraRenderTex->GetRenderTarget()->GetTexture()->GetSRV(), outputRes);
         }
         ImGui::End();
      }
   }
}

