#include "Core\Context.h"
#include "Core\Engine.h"

using namespace Mile;

int main( )
{
   auto context = std::make_unique<Context>( );
   auto engine = new Engine( context.get( ) );

   if ( !engine->Init( ) )
   {
      return 1;
   }

   return engine->Execute( );
}