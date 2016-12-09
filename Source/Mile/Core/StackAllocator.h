#pragma once
#include "Allocator.h"

namespace Mile
{
	template<uint64 SIZE_BYTES>
	class MILE_API StackAllocator : public Allocator
	{
	public:
		explicit StackAllocator( ) :
			Top( 0 )
		{
		}

		virtual void* Allocate( uint64 SizeBytes ) override
		{
			ASSERT_MSG( ( SizeBytes != 0 ), "Size is must not can be 0!" );
			ASSERT_MSG( ( ( SizeBytes + Top ) <= SIZE_BYTES ), "Stack Overflow" );

			uint64 Result = Data + Top;
			Top += SizeBytes;

			return Result;
		}

		virtual void Deallocate( void* Target ) override
		{
			if ( Target != nullptr )
			{
				ASSERT_MSG( ( Target >= Data && Target < Data + Top ), "Stack Underflow" );
				/** Alloc Memory Address - Allocator Start Address = Used Memory Size */
				Top = ( reinterpret_cast<uint8*>( Target ) - Data );
			}
		}

		virtual void* AllocateAligned( uint64 SizeBytes, uint64 Alignment ) override
		{
			UNUSED_PARAM( Alignment );
			return Allocate( SizeBytes );
		}

		virtual void DeallocateAligned( void* Target ) override
		{
			Deallocate( Target );
		}

	private:
		uint64	Top;
		uint8	Data[SIZE_BYTES];

	};
}