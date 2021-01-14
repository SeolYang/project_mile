#pragma once
#include "Core/Layer.h"
using namespace Mile;

class ImGUIDemoLayer : public Layer
{
public:
   ImGUIDemoLayer(Context* context);

   virtual void OnIMGUIRender();

};