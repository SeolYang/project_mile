#include "TestApp.h"
#include "ImGUIDemoLayer.h"

TestApp::TestApp(Mile::Context* context) :
   Mile::Application(context, TEXT("TestApplication"))
{
}

bool TestApp::Init()
{
   if (Mile::Application::Init())
   {
      PushLayer(new ImGUIDemoLayer(GetContext()));
      return true;
   }

   return false;
}