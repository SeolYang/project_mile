#pragma once
#include "Component/ComponentRegister.h"
#include "Math/Vector4.h"

namespace Mile
{
   enum class MEAPI EMeteringMode
   {
      Manual,
      AutoExposureBasic,
      AutoExposureHistogram
   };

   class RenderTexture;
   /**
    * @brief	카메라의 속성들을 가지고 있는 컴포넌트 입니다.
    *			기본적으로 FOV 값, 평면들과의 거리, 프레임 버퍼를 초기화 시킬 색상을 속성으로 가지고 있습니다.
    */
   class MEAPI CameraComponent : public Component
   {
      DeclareComponent(CameraComponent);

   public:
      CameraComponent(Entity* entity);

      /**
       * @brief 카메라의 field of view(FOV)값을 가져옵니다.
       */
      float GetFov() const { return m_fov; }
      /**
       * @brief 카메라의 field of view(FOV)값을 설정합니다.
       * @param FOV 값 (Degrees)
       */
      void SetFov(float fov) { m_fov = fov; }

      /**
      * @brief	카메라에서 가까운 평면과의 거리를 가져옵니다.
      */
      float GetNearPlane() const { return m_nearPlane; }
      /**
       * @brief	카메라에서 가장 가까운 평면과의 거리를 설정합니다.
       * @param	거리 값
       */
      void SetNearPlane(float nearPlane) { m_nearPlane = nearPlane; }

      /**
       * @brief 카메라에서 가장 먼 평면과의 거리를 가져옵니다.
       */
      float GetFarPlane() const { return m_farPlane; }
      /**
       * @brief 카메라에서 가장 먼 평면과의 거리를 설정합니다.
       * @param 거리 값
       */
      void SetFarPlane(float farPlane) { m_farPlane = farPlane; }

      /**
       * @brief 렌더링이 끝나고 프레임 버퍼를 덮어 씌울 색을 가져옵니다.
       */
      Vector4 GetClearColor() const { return m_clearColor; }
      /**
      * @brief 렌더링이 끝나고 프레임 버퍼를 덮어 씌울 색을 가져옵니다.
      * @param 덮어 씌울 색상
      */
      void SetClearColor(const Vector4& clearColor) { m_clearColor = clearColor; }

      void SetRenderTexture(RenderTexture* renderTexture) { m_renderTexture = renderTexture; }
      RenderTexture* GetRenderTexture() const { return m_renderTexture; }

      virtual json Serialize() const override;
      virtual void DeSerialize(const json& jsonData) override;

      float Aperture() const { return m_aperture; }
      float& Aperture() { return m_aperture; }

      float ShutterSpeed() const { return m_shutterSpeed; }
      float& ShutterSpeed() { return m_shutterSpeed; }

      float Sensitivity() const { return m_sensitivity; }
      float& Sensitivity() { return m_sensitivity; }

      EMeteringMode& MeteringMode() { return m_meteringMode; }
      EMeteringMode MeteringMode() const { return m_meteringMode; }

      float& ExposureCompoensation() { return m_expComp; }
      float ExposureCompoensation() const { return m_expComp; }

      float Exposure() const;

      float GetLightAdaptionSpeed() const { return m_lightAdaptionSpeed; }
      void SetLightAdaptionSpeed(float newSpeed)
      {
         m_lightAdaptionSpeed = std::max(newSpeed, 0.0001f);
      }

      float GetDarkAdaptionSpeed() const { return m_darkAdaptionSpeed; }
      void SetDarkAdaptionSpeed(float newSpeed)
      {
         m_darkAdaptionSpeed = std::max(newSpeed, 0.0001f);
      }

      float GetMinBrightness() const { return m_minBrightness; }
      void SetMinBrightness(float brightness)
      {
         m_minBrightness = std::max(brightness, 0.0f);
      }

      float GetMaxBrightness() const { return m_maxBrightness; }
      void SetMaxBrightness(float brightness)
      {
         m_maxBrightness = std::max(brightness, 0.0f);
      }

      void OnGUI() override;

   private:
      float m_fov;
      float m_nearPlane;
      float m_farPlane;

      EMeteringMode m_meteringMode;
      float m_aperture;
      float m_shutterSpeed;
      float m_sensitivity;
      float m_expComp;

      float m_lightAdaptionSpeed;
      float m_darkAdaptionSpeed;
      float m_minBrightness;
      float m_maxBrightness;

      Vector4 m_clearColor;

      RenderTexture* m_renderTexture;

   };
}