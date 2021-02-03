#pragma once
#include <string>

namespace Elaina
{
  using StringType = std::string;

  template <typename Type>
  static void SafeDelete(Type& pointer)
  {
     if (pointer != nullptr)
     {
        delete pointer;
        pointer = nullptr;
     }
  }

  template <typename Type>
  static void SafeDeleteArray(Type& pointer)
  {
     if (pointer != nullptr)
     {
        delete[] pointer;
        pointer = nullptr;
     }
  }
}

#if defined(DEBUG) || defined(_DEBUG)
#define ELAINA_ASSERT(condition, message) \
do \
{ \
   if (!(condition)) \
   { \
      std::cerr << "Assertion failed in " << __FILE__ \
                << " line " << __LINE__ << ": " << message << std::endl; \
      std::terminate(); \
   } \
} while(false)
#else
#define ELAINA_ASSERT(condition, message) do { } while(false)
#endif