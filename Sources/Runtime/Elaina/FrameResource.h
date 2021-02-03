#pragma once
#include <vector>
#include <Elaina/Elaina.config.h>
#include <Elaina/Realize.h>

namespace Elaina
{
   class RenderPass;
   class FrameGraph;
   class RenderPassBuilder;

   /** Resource Handler */
   class FrameResourceBase
   {
   public:
      virtual ~FrameResourceBase() = default;
      RenderPass* GetCreator() const { return Creator; }

      size_t GetID() const { return Identifier; }
      StringType GetName() const { return Name; }

      size_t GetRefCount() const { return RefCount; }
      /** Resource which does not reference to any Render pass and even not a external permanent resource. */
      bool IsExpired() const { return (RefCount == 0) && IsTransient(); }

      bool IsTransient() const { return (Creator != nullptr); }
      virtual bool IsRealized() const = 0;
      bool IsExternalPermanent() const { return (!IsTransient()); }

   protected:
      explicit FrameResourceBase(const StringType& name, RenderPass* creator) :
         Name(name),
         Creator(creator),
         RefCount(0)
      {
         static size_t IdentifierCounter = 0;
         Identifier = IdentifierCounter;
         ++IdentifierCounter;
      }

      virtual void Realize() = 0;
      virtual void Derealize() = 0;

   private:
      /**
      * @brief Calculate Reference count
      * 자신을 읽는 RenderPass에게 참조된다
      * (RenderPass가 리소스를 참조해야하는 경우; RefCount = RenderPass의 수)
      */
      void OnCompile()
      {
         RefCount = Readers.size();
      }

   protected:
      size_t      Identifier;
      StringType  Name;

      RenderPass* Creator;
      std::vector<RenderPass*> Readers;
      std::vector<RenderPass*> Writers;
      size_t RefCount;

      friend FrameGraph;
      friend RenderPassBuilder;

   };

   template <typename DescriptorType, typename ActualType>
   class FrameResource : public FrameResourceBase
   {
   public:
      /** Deferred-created resources */
      explicit FrameResource(const StringType& name, RenderPass* creator, const DescriptorType& descriptor) :
         Descriptor(descriptor),
         Actual(nullptr),
         FrameResourceBase(name, creator)
      {
      }

      /** External Permanent resources */
      explicit FrameResource(const StringType& name, const DescriptorType& descriptor, ActualType* actual) :
         Descriptor(descriptor),
         Actual(actual),
         FrameResourceBase(name, nullptr)
      {
      }

      virtual ~FrameResource()
      {
         /** Delete 'actual' only if it is transient resource and deferred created(or realized). */
         Derealize();
      }

      DescriptorType GetDescriptor() const { return Descriptor; }
      ActualType* GetActual() const { return Actual; }

      virtual bool IsRealized() const override { return (Actual != nullptr); }

   protected:
      virtual void Realize() override
      {
         if (IsTransient() && !IsRealized())
         {
            Actual = Elaina::Realize<DescriptorType, ActualType>(Descriptor);
         }
      }

      virtual void Derealize() override
      {
         if (IsTransient() && IsRealized())
         {
            SafeDelete(Actual);
         }
      }

   protected:
      DescriptorType Descriptor;
      ActualType* Actual;

   };
}