#include "Core/Context.h"
#include "Core/Engine.h"
#include "EditorApp.h"

#ifdef _DEBUG
#include <iostream>
#include <crtdbg.h>
#endif

int main()
{
#ifdef _DEBUG
   _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
   //_CrtSetBreakAlloc(63982);
#endif

   int execute = 1;
   auto context = new Mile::Context();
   auto app = new Mile::Editor::EditorApp(context);
   auto engine = new Mile::Engine(context, app);

   if (engine->Init())
   {
      execute = engine->Execute();
   }

   SafeDelete(context);
   OPTICK_SHUTDOWN();
   return execute;
}