#pragma once
#include "Core/Layer.h"

namespace Mile
{
   class RenderTexture;
   class CameraComponent;
   class Entity;
   namespace Editor
   {
      class GameViewLayer : public Layer
      {
      public:
         GameViewLayer(Context* context);
         virtual bool Init() override;
         virtual void OnIMGUIRender();

      private:
         RenderTexture* m_editorCameraRenderTex;

      };
   }
}