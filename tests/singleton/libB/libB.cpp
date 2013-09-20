#include "libB.h"
#include "libC/libC.h"

#include "miro/NamingRepository.h"

#include <ace/Singleton.h>

void * libB()
{
  return (void *)TestIntSingleton::instance();
}
void * libBFoo()
{
  return (void *)Foo::instance();
}
void * libBFooRepository()
{
  return (void *)FooRepository::instance();
}
void * libBNamingRepository()
{
  return (void *)Miro::NamingRepository::instance();
}
