#pragma once
#include <Elaina/Elaina.config.h>

namespace Elaina
{
   template <typename DescriptorType, typename ActualType>
   struct MissingRealizeImplementation : std::false_type
   {
      /** static assert를 이용한 더욱 정확한 compile time error */
   };

   template <typename DescriptorType, typename ActualType>
   ActualType* Realize(const DescriptorType& descriptor)
   {
      static_assert(MissingRealizeImplementation<DescriptorType, ActualType>::value, "Missing realize implementation for given descriptor type and actual type!");
      return nullptr;
   }
}