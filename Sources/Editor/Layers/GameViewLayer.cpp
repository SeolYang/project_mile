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
         auto renderer = Engine::GetRenderer();
         auto renderRes = renderer->GetRenderResolution();
         m_editorCameraRenderTex = resMng->Create<RenderTexture>(EDITOR_GAME_VIEW_RENDER_TEXTURE, false);
         m_editorCameraRenderTex->SetWidth((UINT32)renderRes.x);
         m_editorCameraRenderTex->SetHeight((UINT32)renderRes.y);
         return true;
      }

      void GameViewLayer::OnIMGUIRender()
      {
         OPTICK_EVENT();
         ImGuiWindowFlags windowFlag = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
         auto world = Engine::GetWorld();
         auto window = Engine::GetWindow();
         auto renderer = Engine::GetRenderer();

         ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
         ImGui::Begin("Game", nullptr, windowFlag);
         if (m_editorCameraRenderTex != nullptr)
         {
            auto renderTarget = m_editorCameraRenderTex->GetRenderTarget();
            Vector2 imageRes{ (float)renderTarget->GetWidth(), (float)renderTarget->GetHeight() };

            GUI::ImageRelativeToWindow(m_editorCameraRenderTex->GetRenderTarget()->GetTexture()->GetSRV(), imageRes);

            Vector2 renderRes = renderer->GetRenderResolution();
            m_editorCameraRenderTex->SetWidth((UINT32)renderRes.x);
            m_editorCameraRenderTex->SetHeight((UINT32)renderRes.y);
         }
         ImGui::End();
         ImGui::PopStyleColor();
      }
   }
}

