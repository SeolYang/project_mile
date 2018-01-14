#include "Material.h"
#include "json.hpp"
#include "Core\Context.h"
#include "ResourceManager.h"
#include "Texture2D.h"

//{
// "BaseColor": ....,
// "TextureType Name": Texture File path ....,
// "Specular Exponential": .....,
// "Specular Intensity": .... }
namespace Mile
{
   bool Material::Init( )
   {
      std::ifstream stream( this->m_path );
      if ( !stream.is_open( ) )
      {
         return false;
      }

      std::string jsonStr;
      std::string temp;
      while ( std::getline( stream, temp ) )
      {
         jsonStr += temp;
         jsonStr += '\n';
      }
      stream.close( );

      auto resourceMng = m_context->GetSubSystem<ResourceManager>( );
      json jsonData = json::parse( jsonStr );
      m_specExp = jsonData[ "SpecExp" ];
      m_specAlbedo.DeSerialize(jsonData[ "SpecAlbedo" ]);
      m_diffuseMap = resourceMng->Load<Texture2D>( String2WString( jsonData[ "DiffuseMap" ] ) );
      m_normalMap = resourceMng->Load<Texture2D>( String2WString( jsonData[ "NormalMap" ] ) );

      // @TODO: Load engine default textures if load texture does not exist
      return true;
   }

   bool Material::SaveTo( const String& filePath )
   {
      std::string diffuseMapPath = "\"None\"";
      if ( m_diffuseMap != nullptr )
      {
         diffuseMapPath = "\"" + WString2String( m_diffuseMap->GetPath( ) ) + "\"";
      }

      std::string normalMapPath = "\"None\"";
      if ( m_normalMap != nullptr )
      {
         normalMapPath = "\"" + WString2String( m_normalMap->GetPath( ) ) + "\"";
      }

      std::string res = "{ \"SpecExp\": " + std::to_string( m_specExp ) +
         ", \"SpecAlbedo\": " + m_specAlbedo.Serialize( ) +
         ", \"DiffuseMap\": " + diffuseMapPath +
         ", \"NormalMap\":" + normalMapPath + " }";

      std::ofstream stream( filePath );
      stream << res;
      stream.close( );

      return true;
   }
}