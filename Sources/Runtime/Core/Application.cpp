#include "Core/Application.h"
#include "Core/Layer.h"
#include "Core/IMGUILayer.h"
#include "Core/Context.h"
#include "Core/Window.h"

namespace Mile
{
   DEFINE_LOG_CATEGORY(MileApplication);

   Application::Application(Context* context, const String& name) :
      m_name(name),
      m_imguiLayer(nullptr),
      SubSystem(context)
   {
   }

   Application::~Application()
   {
      DeInit();
   }

   bool Application::Init()
   {
      if (SubSystem::Init())
      {
         Context* context = GetContext();
         Window* window = context->GetSubSystem<Window>();
         if (window != nullptr)
         {
            window->SetTitle(m_name);
         }

         m_imguiLayer = new IMGUILayer(context);
         PushOverlay(m_imguiLayer);

         ME_LOG(MileApplication, Log, TEXT("Application initialized."));
         SubSystem::InitSucceed();
         return true;
      }

      return false;
   }

   void Application::DeInit()
   {
      if (IsInitialized())
      {
         ME_LOG(MileApplication, Log, TEXT("Application deinitialized."));
         SubSystem::DeInit();
      }
   }

   void Application::Update()
   {
      for (auto layer : m_layers)
      {
         layer->OnUpdate();
      }
   }

   void Application::RenderIMGUI()
   {
      OPTICK_EVENT();
      m_imguiLayer->Begin();
      for (auto layer : m_layers)
      {
         layer->OnIMGUIRender();
      }
      m_imguiLayer->End();
   }

   void Application::PushLayer(Layer* layer)
   {
      if (layer != nullptr)
      {
         ME_LOG(MileApplication, Log, TEXT("Layer pushed : ") + layer->GetName());
         m_layers.Push(layer);
         layer->OnAttach();
      }
   }

   void Application::PushOverlay(Layer* overlay)
   {
      if (overlay != nullptr)
      {
         ME_LOG(MileApplication, Log, TEXT("Overlay pushed : ") + overlay->GetName());
         m_layers.PushOverlay(overlay);
         overlay->OnAttach();
      }
   }
}