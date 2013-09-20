/* -*- C++ -*- */
#ifndef libC_h
#define libC_h

#include "libC_Export.h"
#include "miro/Singleton.h"
#include "miro/Repository.h"

#include <ace/Singleton.h>

struct libC_Export Foo
{
  static Miro::Singleton<Foo> instance;
};

typedef Miro::Repository<Foo> FooRepository;

typedef ACE_Singleton<FooRepository, ACE_SYNCH_RECURSIVE_MUTEX> FooRepositorySingleton;
LIBC_SINGLETON_DECLARE(ACE_Singleton, FooRepository, ACE_SYNCH_RECURSIVE_MUTEX);

typedef ACE_Singleton<int, ACE_SYNCH_RECURSIVE_MUTEX> TestIntSingleton;
LIBC_SINGLETON_DECLARE(ACE_Singleton, int, ACE_SYNCH_RECURSIVE_MUTEX);

typedef ACE_Singleton<Foo, ACE_SYNCH_RECURSIVE_MUTEX> FooSingleton;
LIBC_SINGLETON_DECLARE(ACE_Singleton, Foo, ACE_SYNCH_RECURSIVE_MUTEX);

#endif // libC_h
