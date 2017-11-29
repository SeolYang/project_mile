#include "LightComponent.h"
#include "Core/Entity.h"
#include "Core/Transform.h"

namespace Mile
{
   LightComponent::LightComponent( Entity* entity ) :
      Component( entity )
   {
   }

   Vector3 LightComponent::GetLightDirection( ) const
   {
      auto transform = m_entity->GetTransform( );
      return transform->GetForward( );
   }

   Vector3 LightComponent::GetLightPosition( ) const
   {
      auto transform = m_entity->GetTransform( );
      return transform->GetPosition( TransformSpace::World );
   }

   std::string LightComponent::Serialize( ) const
   {
      auto res = "{ \"Type\": " + LightTypeToString(m_type) + 
         ", \"Color\": " + m_color.Serialize( ) + 
         ", \"SpotlightAngles\": " + m_spotlightAngles.Serialize( )
         + " }";

      return res;
   }

   void LightComponent::DeSerialize( const json& jsonData )
   {
      m_type = StringToLightType( jsonData[ "Type" ] );
      m_color.DeSerialize( jsonData[ "Color" ] );
      m_spotlightAngles.DeSerialize( jsonData[ "SpotlightAngles" ] );
   }
}