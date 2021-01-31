#pragma once

#include "Core/Application.h"

class TestApp : public Mile::Application
{
public:
   TestApp(Mile::Context* context);
   ~TestApp() = default;

   virtual bool Init() override;

};