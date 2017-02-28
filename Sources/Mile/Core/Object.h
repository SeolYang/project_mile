#pragma once
#include "Mile.h"
#include "MileString.h"

namespace Mile
{
    /*
    * @todo: 기본 능력 확장 ( 직렬화, UUID, RTTI, 메모리관리<고민해보기> ) 
    */
    class MILE_API Object
    {
    public:
        inline Object( const MString& NewName = MString( TEXT("UNKNOWN") ), bool IsValid = true ) :
            Name( NewName ),
            bIsValid( IsValid )
        {
        }

        void SetName( const MString& NewName )
        {
            Name = NewName;
        }

        MString GetName( ) const
        {
            return Name;
        }

    private:
        MString                      Name;
        bool                         bIsValid;

    };
}