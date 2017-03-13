#pragma once
#include "Mile.h"

namespace Mile
{
    class MileTimer;
    class MileWindow;
    class MILE_API MileApplication
    {
    public:
        MileApplication( std::unique_ptr<MileWindow> NewTargetWindow );
        virtual ~MileApplication( );

        virtual void Excute( ) = 0;

        static MileApplication& GetInstance( );
        MileWindow& GetWindow( ) const;

    private:
        std::unique_ptr<MileWindow> Window;

    private:
        static MileApplication* Instance;

    };
}