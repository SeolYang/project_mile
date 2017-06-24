#pragma once
#include "Mile.h"
#include "MileString.h"
#include "Rumia/DefaultAllocator.h"

namespace Mile
{
    class MILE_API Object
    {
    public:
        template <typename Ty, typename TAllocator = Rumia::DefaultAllocator, typename... Args >
        static Ty* NewObject( TAllocator& Allocator, Args&&... Params )
        {
            return RUMIA_NEW( Allocator, Ty, Params );
        }

    protected:
        inline Object( const MString& NewName = MString( TEXT( "UNKNOWN" ) ), bool IsValid = true ) :
            Name( NewName ),
            bIsValid( IsValid ),
            ObjectID( NumOfAllocatedObject ),
            bIsTick( false ),
            bIsTickFuncRegistered( false ),
            TickPriority( 0 )
        {
            ++NumOfAllocatedObject;
            ++ObjectCounting;
        }

        Object( const Object& CopiedObject ) :
            Name( CopiedObject.Name ),
            bIsValid( CopiedObject.bIsValid ),
            ObjectID( NumOfAllocatedObject ),
            bIsTick( CopiedObject.bIsTick ),
            bIsTickFuncRegistered( false ),
            TickPriority( CopiedObject.TickPriority )
        {
            SetIsTick( bIsTick );
            ++NumOfAllocatedObject;
            ++ObjectCounting;
        }

    public:
        Object( Object&& MovedObject ) :
            Name( std::move( MovedObject.Name ) ),
            bIsValid( MovedObject.bIsValid ),
            bIsTick( MovedObject.bIsTick ),
            bIsTickFuncRegistered( MovedObject.bIsTickFuncRegistered ),
            TickPriority( MovedObject.TickPriority )
        {
            MovedObject.SetIsTick( false );
            this->SetIsTick( bIsTick );
        }

        virtual ~Object( )
        {
            --ObjectCounting;
        }

        void SetName( const MString& NewName )
        {
            Name = NewName;
        }

        MString GetName( ) const
        {
            return Name;
        }

        uint64 GetObjectID( ) const
        {
            return ObjectID;
        }

        static uint64 GetObjectCount( )
        {
            return ObjectCounting;
        }

        /**
        * 매 프레임마다 호출되는 메소드
        * @param DeltaTime 프레임과 프레임사이에 흐른 시간
        */
        virtual void Tick( float DeltaTime ) { UNUSED_PARAM( DeltaTime ); }

        void SetIsTick( bool bNewIsTick );
        FORCEINLINE bool IsTick( ) const { return bIsTick; }

        void SetTickPriority( uint64 NewTickPriority );
        FORCEINLINE uint64 GetTickPriority( ) const { return TickPriority; }

    private:
        static uint64                NumOfAllocatedObject;
        static uint64                ObjectCounting;
        /* ObjectID 는 이때까지 만들어진 Object의 갯수로 할당됨. */
        uint64                       ObjectID;
        MString                      Name;
        bool                         bIsValid;

        bool                         bIsTick;
        bool                         bIsTickFuncRegistered;
        uint64                       TickPriority;

    };
}