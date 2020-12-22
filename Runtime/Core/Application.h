#pragma once

#include "Core/SubSystem.h"
#include "Core/LayerStack.h"

namespace Mile
{
   class Layer;
   class IMGUILayer;
   class MEAPI Application : public SubSystem
   {
   public:
      Application(Context* context, const String& name);
      virtual ~Application() = default;

      virtual bool Init() override;

      void Update();
      void RenderIMGUI();

      void PushLayer(Layer* layer);
      void PushOverlay(Layer* overlay);

      String GetName() const { return m_name; }
      LayerStack& GetLayerStack() { return m_layers; }

   private:
      String      m_name;
      LayerStack  m_layers;
      IMGUILayer* m_imguiLayer;

   };
}