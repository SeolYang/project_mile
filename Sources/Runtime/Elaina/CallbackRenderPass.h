#pragma once
#include <Elaina/RenderPass.h>

namespace Elaina
{
   template <typename DataType>
   class CallbackRenderPass : public RenderPass
   {
      using SetupCallbackType = std::function<void(RenderPassBuilder&, DataType&)>;
      using ExecuteCallbackType = std::function<void(const DataType&)>;

   public:
      explicit CallbackRenderPass(const StringType& name,
         const SetupCallbackType& setupCallback, const ExecuteCallbackType& executeCallback,
         size_t distributionGruop = 0) :
         Data(DataType()),
         SetupCallback(setupCallback),
         ExecuteCallback(executeCallback),
         RenderPass(name, distributionGruop)
      {
      }

      virtual ~CallbackRenderPass() = default;

      const DataType& GetData() const { return Data; }

   protected:
      virtual void Setup(RenderPassBuilder& builder) override
      {
         SetupCallback(builder, Data);
      }

      virtual void Execute() override
      {
         ExecuteCallback(Data);
      }

   protected:
      DataType Data;
      const SetupCallbackType SetupCallback;
      const ExecuteCallbackType ExecuteCallback;

      friend FrameGraph;

   };
}