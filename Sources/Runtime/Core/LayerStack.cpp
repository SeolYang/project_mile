#include "Core/LayerStack.h"
#include "Core/Layer.h"

namespace Mile
{
   LayerStack::LayerStack() :
      m_layerInsertIdx(0)
   {
   }

   LayerStack::~LayerStack()
   {
      for (Layer* layer : m_layers)
      {
         layer->OnDetach();
         SafeDelete(layer);
      }
   }

   void LayerStack::Push(Layer* layer)
   {
      if (layer != nullptr)
      {
         m_layers.emplace(m_layers.begin() + m_layerInsertIdx, layer);
         ++m_layerInsertIdx;
      }
   }

   void LayerStack::PushOverlay(Layer* overlay)
   {
      if (overlay != nullptr)
      {
         m_layers.emplace_back(overlay);
      }
   }

   void LayerStack::Pop(Layer* layer)
   {
      if (layer != nullptr)
      {
         auto itr = std::find(m_layers.begin(), m_layers.begin() + m_layerInsertIdx, layer);
         if (itr != (m_layers.begin() + m_layerInsertIdx))
         {
            layer->OnDetach();
            m_layers.erase(itr);
            --m_layerInsertIdx;
         }
      }
   }

   void LayerStack::PopOverlay(Layer* overlay)
   {
      if (overlay != nullptr)
      {
         auto itr = std::find(m_layers.begin() + m_layerInsertIdx, m_layers.end(), overlay);
         if (itr != m_layers.end())
         {
            overlay->OnDetach();
            m_layers.erase(itr);
         }
      }
   }
}