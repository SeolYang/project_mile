#pragma once
#include "Core/Layer.h"
#include "Core/Logger.h"
#include "Math/MathHelper.h"

namespace Mile
{
   class World;
   class Entity;
   namespace Editor
   {
      DECLARE_LOG_CATEGORY_EXTERN(MileWorldHierarchyLayer, Log);

      class WorldHierarchyLayer : public Layer
      {
         public:
            WorldHierarchyLayer(Context* context);
            virtual void OnIMGUIRender();

            void SetTargetWorld(World* world) { m_target = world; }

      private:
            void DrawEntityNode(Entity* targetRoot);

            void DrawHierarchyPanel();
            void DrawPropertiesPanel();

      private:
         World* m_target;
         Entity* m_selectedEntity;
         Vector3 m_tempEulerRotation;

      };
   }
}