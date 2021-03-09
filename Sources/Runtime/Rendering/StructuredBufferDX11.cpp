#include "Rendering/StructuredBufferDX11.h"

namespace Mile
{
   bool StructuredBufferDX11::Init(unsigned int count, unsigned int structSize, bool bCPUWritable, bool bGPUWritable, const D3D11_SUBRESOURCE_DATA* data)
   {
      D3D11_BUFFER_DESC desc;
      ZeroMemory(&desc, sizeof(desc));
      desc.ByteWidth = count * structSize;
      desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
      desc.StructureByteStride = structSize;
      
      if (!bCPUWritable && !bGPUWritable)
      {
         desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
         desc.Usage = D3D11_USAGE_IMMUTABLE;
         desc.CPUAccessFlags = 0;
      }
      else if (bCPUWritable && !bGPUWritable)
      {
         desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
         desc.Usage = D3D11_USAGE_DYNAMIC;
         desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
      }
      else if (!bCPUWritable && bGPUWritable)
      {
         desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
         desc.Usage = D3D11_USAGE_DEFAULT;
         desc.CPUAccessFlags = 0;
      }
      else
      {
         return false;
      }

      RendererDX11* renderer = GetRenderer();
      ID3D11Device& device = renderer->GetDevice();

      HRESULT result = device.CreateBuffer(&desc, data, reinterpret_cast<ID3D11Buffer**>(&m_resource));
      if (FAILED(result))
      {
         return false;
      }

      D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
      ZeroMemory(&uavDesc, sizeof(uavDesc));
      uavDesc.Format = DXGI_FORMAT_UNKNOWN;
      uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
      uavDesc.Buffer.NumElements = count;
      if (!ResourceDX11::InitUnorderedAccessView(uavDesc))
      {
         return false;
      }

      D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
      ZeroMemory(&srvDesc, sizeof(srvDesc));
      srvDesc.Format = DXGI_FORMAT_UNKNOWN;
      srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
      srvDesc.Buffer.ElementWidth = count;
      if (!ResourceDX11::InitShaderResourceView(srvDesc))
      {
         return false;
      }

      return true;
   }
}