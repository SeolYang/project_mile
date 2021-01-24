#include "Core/Layer.h"

namespace Mile
{
   Layer::Layer(Context* context, const String& name /* = TEXT("Layer") */) :
      m_context(context),
      m_name(name)
   {
   }
}