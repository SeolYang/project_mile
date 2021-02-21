#pragma once
#include "Core/CoreMinimal.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Quaternion.h"
#include "Math/Matrix.h"

namespace Mile
{
   class Entity;
   class MEAPI alignas(16) Transform
   {
   public:
      Transform(Entity* entity, const Vector3& position, const Vector3& scale, const Quaternion& rotation) :
         m_entity(entity),
         m_position(position),
         m_scale(scale),
         m_rotation(rotation),
         m_parent(nullptr)
      {
      }

      Transform(Entity* entity) :
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

      void DeSerialize(const json& jsonObj)
      {
         m_position.DeSerialize(jsonObj["Position"]);
         m_scale.DeSerialize(jsonObj["Scale"]);
         m_rotation.DeSerialize(jsonObj["Rotation"]);
      }

      Vector3 GetPosition(ETransformSpace space = ETransformSpace::Local) const
      {
         if (space == ETransformSpace::World && HasParent())
         {
            return (m_position * m_parent->GetWorldRotTransMatrix());
         }

         return m_position;
      }

      void SetPosition(const Vector3& position, ETransformSpace space = ETransformSpace::Local)
      {
         if (space == ETransformSpace::World && HasParent())
         {
            m_position = m_parent->GetRotation(ETransformSpace::World).Inverse().RotateVector(position);
            m_position -= m_parent->GetPosition(ETransformSpace::World);
         }
         else
         {
            m_position = position;
         }
      }

      Vector3 GetScale(ETransformSpace space = ETransformSpace::Local) const
      {
         if (space == ETransformSpace::World && HasParent())
         {
            return m_scale * m_parent->GetScale(space);
         }

         return m_scale;
      }

      void SetScale(const Vector3& scale, ETransformSpace space = ETransformSpace::Local)
      {
         if (space == ETransformSpace::World && HasParent())
         {
            Vector3 parentWorldScale = m_parent->GetScale(ETransformSpace::World);
            m_scale = Vector3(scale.x / parentWorldScale.x, scale.y / parentWorldScale.y, scale.z / parentWorldScale.z);
         }
         else
         {
            m_scale = scale;
         }
      }

      void SetRotation(const Quaternion& rot, ETransformSpace space = ETransformSpace::Local)
      {
         if (space == ETransformSpace::World && HasParent())
         {
            m_rotation = rot.Rotated(m_parent->GetRotation(ETransformSpace::World).Inverse());
         }
         else
         {
            m_rotation = rot;
         }
      }

      Quaternion GetRotation(ETransformSpace space = ETransformSpace::Local) const
      {
         if (space == ETransformSpace::World && HasParent())
         {
            return m_parent->GetRotation(ETransformSpace::World).Rotated(m_rotation);
         }
         else
         {
            return m_rotation;
         }
      }

      void Translate(const Vector3& vec, ETransformSpace space = ETransformSpace::Local)
      {
         SetPosition(GetPosition(space) + vec, space);
      }

      void Scale(const Vector3& scale, ETransformSpace space = ETransformSpace::Local)
      {
         SetScale(GetScale(space) * scale, space);
      }

      void Scale(float factor, ETransformSpace space = ETransformSpace::Local)
      {
         SetScale(GetScale(space) * factor, space);
      }

      void Rotate(const Quaternion& rot, ETransformSpace space = ETransformSpace::Local)
      {
         SetRotation(GetRotation(space).Rotate(rot), space);
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
            Vector3(1.0f / m_scale.x, 1.0f / m_scale.y, 1.0f / m_scale.z),
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

      Vector3 GetForward(ETransformSpace space = ETransformSpace::World) const;
      Vector3 GetUp(ETransformSpace space = ETransformSpace::World) const;

   private:
      void SetParent(Transform* parent)
      {
         if (parent != m_parent)
         {
            Vector3 oldWorldPos = GetPosition(ETransformSpace::World);
            Vector3 oldWorldScale = GetScale(ETransformSpace::World);
            Quaternion oldWorldRotation = GetRotation(ETransformSpace::World);

            if (parent == nullptr)
            {
               SetPosition(oldWorldPos);
               SetScale(oldWorldScale);
               SetRotation(oldWorldRotation);
            }
            else
            {
               m_parent = parent;
               SetPosition(oldWorldPos, ETransformSpace::World);
               SetScale(oldWorldScale, ETransformSpace::World);
               SetRotation(oldWorldRotation, ETransformSpace::World);
            }
         }
      }

   private:
      /* Local Transform **/
      Vector3     m_position;
      Vector3     m_scale;
      Quaternion  m_rotation;

      Entity* m_entity;
      Transform* m_parent;

      friend Entity;
   };

   using TransformPtr = std::unique_ptr<Transform>;
   using WeakTransformPtr = std::weak_ptr<Transform>;
}