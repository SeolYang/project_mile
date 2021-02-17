#pragma once
#include "Rendering/RenderObject.h"
#include "Rendering/RendererDX11.h"
#include "Rendering/VertexBufferDX11.h"
#include "Rendering/IndexBufferDX11.h"

namespace Mile
{
   class RendererDX11;
   class MEAPI Mesh : public RenderObject
   {
   public:
      Mesh(RendererDX11* renderer, const std::wstring& name, const String& modelPath, EStaticMeshType type = EStaticMeshType::External) :
         m_type(type),
         m_name(name),
         m_vertexBuffer(nullptr),
         m_indexBuffer(nullptr),
         m_vertexNum(0),
         m_indexNum(0),
         m_modelPath(modelPath),
         RenderObject(renderer)
      {
      }

      virtual ~Mesh()
      {
         SafeDelete(m_vertexBuffer);
         SafeDelete(m_indexBuffer);
      }

      template <typename Vertex>
      bool Init(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
      {
         if (RenderObject::IsInitializable())
         {
            RendererDX11* renderer = GetRenderer();
            m_indexBuffer = new IndexBufferDX11(renderer);
            m_indexNum = static_cast<unsigned int>(indices.size());
            if (!m_indexBuffer->Init(indices))
            {
               SafeDelete(m_indexBuffer);
               return false;
            }

            m_vertexBuffer = new VertexBufferDX11(renderer);
            m_vertexNum = static_cast<unsigned int>(vertices.size());
            if (!m_vertexBuffer->Init<Vertex>(vertices))
            {
               SafeDelete(m_vertexBuffer);
               return false;
            }

            RenderObject::ConfirmInit();
            return true;
         }

         return false;
      }

      bool Bind(ID3D11DeviceContext& deviceContext, unsigned int startSlot);

      IndexBufferDX11* GetIndexBuffer() { return m_indexBuffer; }
      VertexBufferDX11* GetVertexBuffer() { return m_vertexBuffer; }

      unsigned int GetVertexCount() const { return m_vertexNum; }
      unsigned int GetIndexCount() const { return m_indexNum; }

      std::wstring GetName() const { return m_name; }
      String GetModelPath() const { return m_modelPath; }

      EStaticMeshType GetMeshType() const { return m_type; }

   private:
      EStaticMeshType   m_type;
      IndexBufferDX11*  m_indexBuffer;
      VertexBufferDX11* m_vertexBuffer;

      std::wstring      m_name;
      String            m_modelPath;

      unsigned int      m_vertexNum;
      unsigned int      m_indexNum;

   };
}