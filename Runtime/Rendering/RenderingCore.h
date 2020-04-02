#pragma once
#include "Core/Helper.h"
#include "Core/SubSystem.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix.h"

#include <d3d11.h>
#include <d3dcompiler.h>

#define SAFE_TEX_BIND(TEXTURE, DEVICE_CONTEXT, SLOT, SHADER_TYPE) if (TEXTURE != nullptr) { TEXTURE->Bind(DEVICE_CONTEXT, SLOT, SHADER_TYPE); }
#define SAFE_TEX_UNBIND(TEXTURE, DEVICE_CONTEXT) if (TEXTURE != nullptr) { TEXTURE->Unbind(DEVICE_CONTEXT); }

enum class EShaderType
{
   VertexShader,
   HullShader,
   DomainShader,
   GeometryShader,
   PixelShader,
   EnumSize
};