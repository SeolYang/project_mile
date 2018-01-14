#include "VertexShaderDX11.h"
#include "InputLayoutDX11.h"

namespace Mile
{
   VertexShaderDX11::VertexShaderDX11( RendererDX11* renderer ) :
      m_shader( nullptr ),
      m_inputLayout( nullptr ),
      ShaderDX11( renderer )
   {
   }

   VertexShaderDX11::~VertexShaderDX11( )
   {
      SafeRelease( m_shader );
   }

   bool VertexShaderDX11::Init( const String& shaderPath )
   {
      if ( m_bIsCompiled || m_renderer == nullptr )
      {
         return false;
      }

      if ( !Compile( shaderPath, ShaderType::VertexShader ) )
      {
         return false;
      }

      auto device = m_renderer->GetDevice( );
      auto result = device->CreateVertexShader( m_blob->GetBufferPointer( ), m_blob->GetBufferSize( ),
                                                nullptr, 
                                                &m_shader );

      if ( FAILED( result ) )
      {
         // Failed to create vertex shader
         return false;
      }
      
      m_inputLayout = std::make_unique<InputLayoutDX11>( m_renderer );
      if ( !m_inputLayout->Init( this->Reflect( ), this ) )
      {
         return false;
      }

      return true;
   }

   bool VertexShaderDX11::Bind( )
   {
      if ( !m_bIsCompiled || m_renderer == nullptr )
      {
         return false;
      }

      // Set Input Layout
      if ( !m_inputLayout->Bind( ) )
      {
         return false;
      }

      m_renderer->GetDeviceContext( )->VSSetShader( m_shader, nullptr, 0 );
      return true;
   }

   void VertexShaderDX11::Unbind( )
   {
      if ( m_renderer != nullptr )
      {
         auto deviceContext = m_renderer->GetDeviceContext( );
         deviceContext->VSSetShader( nullptr, nullptr, 0 );
      }
   }

   std::vector<D3D11_INPUT_ELEMENT_DESC> VertexShaderDX11::Reflect( ) const
   {
      auto inputLayoutDescs = std::vector<D3D11_INPUT_ELEMENT_DESC>( );
      ID3D11ShaderReflection* vsReflection = nullptr;
      auto result = D3DReflect( m_blob->GetBufferPointer( ), m_blob->GetBufferSize( ),
                                IID_ID3D11ShaderReflection, ( void** )( &m_shader ) );

      if ( FAILED( result ) )
      {
         return std::move( inputLayoutDescs );
      }

      D3D11_SHADER_DESC shaderDesc;
      vsReflection->GetDesc( &shaderDesc );

      for ( unsigned int idx = 0; idx < shaderDesc.InputParameters; ++idx )
      {
         D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
         vsReflection->GetInputParameterDesc( idx, &paramDesc );

         D3D11_INPUT_ELEMENT_DESC elementDesc;
         elementDesc.SemanticName = paramDesc.SemanticName;
         elementDesc.SemanticIndex = paramDesc.SemanticIndex;
         elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
         elementDesc.InputSlot = 0;
         elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
         elementDesc.InstanceDataStepRate = 0;

         if ( paramDesc.Mask == 1 )
         {
            if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32_UINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32_SINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
         }
         else if ( paramDesc.Mask < 4 )
         {
            if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
         }
         else if ( paramDesc.Mask < 8 )
         {
            if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
         }
         else if ( paramDesc.Mask < 16 )
         {
            if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
         }

         inputLayoutDescs.push_back( elementDesc );
      }

      return std::move( inputLayoutDescs );
   }
}