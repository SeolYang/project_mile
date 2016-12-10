#pragma once
#include "Allocator.h"

namespace Mile
{
	class MILE_API MProxyAllocator : public MAllocator
	{
	public:
		MProxyAllocator( MAllocator& TargetAllocator ) :
			Source( TargetAllocator ),
			NumAllocations( 0 )
		{
		}

		virtual ~MProxyAllocator( )
		{
			ASSERT_MSG( NumAllocations == 0, "Memory leak detected!" );
		}

		uint32 GetNumAllocations( ) const
		{
			return NumAllocations;
		}

		virtual void* Allocate( uint64 Size ) override
		{
			ASSERT( Size != 0 );

			++NumAllocations;
			return Source.Allocate( Size );
		}

		virtual void Deallocate( void* Target ) override
		{
			if ( Target != nullptr )
			{
				--NumAllocations;
				Source.Deallocate( Target );
			}
		}

		virtual void* AllocateAligned( uint64 Size, uint64 Alignment ) override
		{
			ASSERT( Size != 0 && Alignment > 1 );

			++NumAllocations;
			return Source.AllocateAligned( Size, Alignment );
		}

		virtual void DeallocateAligned( void* Target ) override
		{
			if ( Target != nullptr )
			{
				--NumAllocations;
				Source.DeallocateAligned( Target );
			}
		}

		virtual void* Reallocate( void* Target, uint64 Size ) override
		{
			return Source.Reallocate( Target, Size );
		}

	private:
		MAllocator&				Source;
		std::atomic<uint32>		NumAllocations;

	};
}