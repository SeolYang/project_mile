#pragma once
#include <new>
#include "Mile.h"

namespace Mile
{
	namespace MMemory
	{
		inline void* AlignForward( void* Address, uint64 Alignment )
		{
			return reinterpret_cast<void*>(
				( reinterpret_cast<uint64>( Address ) 
					+ static_cast<uint64>( Alignment - 1 ) ) 
					& static_cast<uint64>( ~( Alignment - 1 ) ) );
		}

		inline uint64 AlignForwardAdjustment( const void* Address, uint64 Alignment )
		{
			uint64 Adjustment = Alignment - ( reinterpret_cast<uint64>( Address ) & static_cast<uint64>( Alignment - 1 ) );

			if ( Adjustment == Alignment )
			{
				return 0;
			}

			return Adjustment;
		}
	}
}