#include "libA.h"
#include "libC/libC.h"

#include "miro/NamingRepository.h"

#include <ace/Singleton.h>

void * libA()
{
  return (void *) TestIntSingleton::instance();
}
void * libAFoo()
{
  return (void *) Foo::instance();
}
void * libAFooRepository()
{
  return (void *) FooRepository::instance();
}
void * libANamingRepository()
{
  return (void *) Miro::NamingRepository::instance();
}
