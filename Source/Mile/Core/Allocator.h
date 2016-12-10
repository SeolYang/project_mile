#pragma once
#include <atomic>
#include "Memory.h"

namespace Mile
{
	class MILE_API MAllocator
	{
	public:
		explicit MAllocator( )
		{
		}

		virtual ~MAllocator( )
		{
		}

		virtual void* Allocate( uint64 Size ) = 0;
		virtual void Deallocate( void* Target ) = 0;
		virtual void* Reallocate( void* Target, uint64 Size ) 
		{
			ASSERT_MSG( false, "This Allocator is not support Reallocate method!" );
			UNUSED_PARAM( Target );;
			UNUSED_PARAM( Size ); 
			return nullptr; 
		}

		virtual void* AllocateAligned( uint64 SizeBytes, uint64 Alignment = 4 ) = 0;
		virtual void DeallocateAligned( void* Target ) = 0;

		template <class T, typename... Args>
		T* New( Args&&... Params )
		{
			void* AllocatedMemory = Allocate( sizeof( T ) );
			return new ( AllocatedMemory ) T( std::forward<Args>( Params )... );
		}

		template <class T, typename... Args>
		T* NewAligned( Args&&... Params )
		{
			void* AllocatedMemory = AllocateAligned( sizeof( T ), , __alignof( T ) );
			return new ( AllocatedMemory ) T( std::forward<Args>( Params )... );
		}

		template <class T>
		void Delete( T* Target )
		{
			if ( Target != nullptr )
			{
				Target->~T( );
				Deallocate( Target );
			}
		}

		template <class T>
		void DeleteAligned( T* Target )
		{
			if ( Target != nullptr )
			{
				Target->~T( );
				DeallocateAligned( Target );
			}
		}
	};
}