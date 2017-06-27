#include "Component.h"
#include "Entity.h"

namespace Mile
{
    void Component::SetOwner( Entity* NewOwner )
    {
        ASSERT_MSG( ( ( Owner != nullptr ) && ( NewOwner != nullptr ) ), TEXT( "This component already has Owner!" ) );
        Owner = NewOwner;
    }
}
