#include "MileObject.h"
#include "TickManager.h"

namespace Mile
{
    uint64 Object::NumOfAllocatedObject = 0;
    uint64 Object::ObjectCounting = 0;

    void Object::SetIsTick( bool bNewIsTick )
    {
        TickManager& Manager = TickManager::Instance( Allocator );
        switch ( bNewIsTick )
        {
        case true:
            if ( !bIsTickFuncRegistered )
            {
                Manager.AddEvent( std::bind( &Object::Tick, this, std::placeholders::_1 ), ObjectID, TickPriority );
                bIsTickFuncRegistered = true;
            }

            break;

        case false:
            if ( bIsTickFuncRegistered )
            {
                Manager.RemoveEvent( ObjectID );
                bIsTickFuncRegistered = false;
            }

            break;
        }
    }

    void Object::SetTickPriority( uint64 NewTickPriority )
    {
        if ( bIsTickFuncRegistered )
        {
            TickManager::Instance( Allocator ).ModifyPriority( ObjectID, NewTickPriority );
        }

        TickPriority = NewTickPriority;
    }
}