#pragma once
#include "Math/Vector4.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "Math/Quaternion.h"
#include "Math/Matrix.h"

namespace Mile
{
   enum class TransformSpace
   {
      World,
      Local
   };

   class Entity;
   class MEAPI alignas(16) Transform
   {
   public:
      Transform(Entity * entity, const Vector3 & position, const Vector3 & scale, const Quaternion & rotation) :
         m_entity(entity),
         m_position(position),
         m_scale(scale),
         m_rotation(rotation),
         m_parent(nullptr)
      {
      }

      Transform(Entity * entity) :
         Transform(entity, Vector3(), Vector3(1.0f, 1.0f, 1.0f),
            Quaternion(1.0f, 0.0f, 0.0f, 0.0f))
      {
      }

      json Serialize() const
      {
         json serialized;
         serialized["Position"] = m_position.Serialize();
         serialized["Scale"] = m_scale.Serialize();
         serialized["Rotation"] = m_rotation.Serialize();
         return serialized;
      }

      void DeSerialize(const json & jsonObj)
      {
         m_position.DeSerialize(jsonObj["Position"]);
         m_scale.DeSerialize(jsonObj["Scale"]);
         m_rotation.DeSerialize(jsonObj["Rotation"]);
      }

      Vector3 GetPosition(TransformSpace space = TransformSpace::Local) const
      {
         if (space == TransformSpace::World && HasParent())
         {
            return m_position * GetWorldRotTransMatrix();
         }

         return m_position;
      }

      void SetPosition(const Vector3& position)
      {
         m_position = position;
      }

      Vector3 GetScale(TransformSpace space = TransformSpace::Local) const
      {
         if (space == TransformSpace::World && HasParent())
         {
            return m_scale * m_parent->GetScale(space);
         }

         return m_scale;
      }

      void SetScale(const Vector3& scale)
      {
         m_scale = scale;
      }

      void SetRotation(const Quaternion& rot)
      {
         m_rotation = rot;
      }

      Quaternion GetRotation(TransformSpace space = TransformSpace::Local) const
      {
         if (space == TransformSpace::World && HasParent())
         {
            return m_parent->GetRotation(TransformSpace::World).Rotated(m_rotation);
         }
         else
         {
            return m_rotation;
         }
      }

      void Translate(const Vector3& vec)
      {
         m_position += vec;
      }

      void Scale(const Vector3& scale)
      {
         m_scale *= scale;
      }

      void Scale(float factor)
      {
         m_scale *= factor;
      }

      void Rotate(const Quaternion& rot)
      {
         m_rotation.Rotate(rot);
      }

      Transform* GetParent() const;
      bool HasParent() const { return m_parent != nullptr; }
      Entity* GetEntity() const { return m_entity; }

      Matrix GetWorldMatrix() const
      {
         Matrix localMatrix = Matrix::CreateTransformMatrix(m_position, m_scale, m_rotation);
         if (m_parent != nullptr)
         {
            return localMatrix * m_parent->GetWorldMatrix();
         }

         return localMatrix;
      }

      Matrix GetInverseWorldMatrix() const
      {
         Matrix invLocalMatrix = Matrix::CreateTransformMatrix(
            Vector3(-m_position.x, -m_position.y, -m_position.z),
            Vector3(1.0f/m_scale.x, 1.0f/m_scale.y, 1.0f/m_scale.z), 
            m_rotation.Inverse());

         if (m_parent != nullptr)
         {
            return invLocalMatrix * m_parent->GetInverseWorldMatrix();
         }

         return invLocalMatrix;
      }

      Matrix GetWorldRotTransMatrix() const
      {
         Matrix localMatrix = Matrix::CreateRotation(m_rotation) * Matrix::CreateTranslation(m_position);
         if (m_parent != nullptr)
         {
            return localMatrix * m_parent->GetWorldRotTransMatrix();
         }

         return localMatrix;
      }

      Vector3 GetForward(TransformSpace space = TransformSpace::World) const;
      Vector3 GetUp(TransformSpace space = TransformSpace::World) const;

   private:
      void SetParent(Transform* parent)
      {
         if (m_parent != nullptr)
         {
            SetPosition(GetPosition(TransformSpace::World));
            SetScale(GetScale(TransformSpace::World));
            SetRotation(GetRotation(TransformSpace::World));
         }

         m_parent = parent;
         if (m_parent != nullptr)
         {
            SetPosition(GetPosition(TransformSpace::Local));
            SetScale(GetScale(TransformSpace::Local));
            SetRotation(GetRotation(TransformSpace::Local));
         }
      }

   private:
      /* Local Transform **/
      Vector3     m_position;
      Vector3     m_scale;
      Quaternion  m_rotation;

      Entity*     m_entity;
      Transform*  m_parent;

      friend Entity;
   };

   using TransformPtr = std::unique_ptr<Transform>;
   using WeakTransformPtr = std::weak_ptr<Transform>;
}