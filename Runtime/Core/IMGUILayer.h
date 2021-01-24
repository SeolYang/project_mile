#pragma once
#include "Core/Layer.h"

namespace Mile
{
   DECLARE_LOG_CATEGORY_EXTERN(MileIMGUILayer, Log);
   class MEAPI IMGUILayer : public Layer
   {
   public:
      IMGUILayer(Context* context);
      ~IMGUILayer() = default;

      virtual void OnAttach() override;
      virtual void OnDetach() override;

      void Begin();
      void End();

   };
}