#include "libC.h"


// From ace/README:
//ACE_HAS_EXPLICIT_STATIC_TEMPLATE_MEMBER_INSTANTIATION  
//                                        For the GCC compiler
//                                        on AIX, HPUX and VxWorks we have to
//                                        explicitly instantiate static template
//                                        members else we get multiple instances
//                                        of the same static.
#if defined (ACE_HAS_EXPLICIT_STATIC_TEMPLATE_MEMBER_INSTANTIATION)
  template ACE_Singleton <int, ACE_SYNCH_MUTEX> *
  ACE_Singleton <int, ACE_SYNCH_MUTEX>::singleton_;
#endif /* ACE_HAS_EXPLICIT_STATIC_TEMPLATE_MEMBER_INSTANTIATION */

#if defined (ACE_HAS_EXPLICIT_STATIC_TEMPLATE_MEMBER_INSTANTIATION)
  template ACE_Singleton <Foo, ACE_SYNCH_MUTEX> *
  ACE_Singleton <Foo, ACE_SYNCH_MUTEX>::singleton_;
#endif /* ACE_HAS_EXPLICIT_STATIC_TEMPLATE_MEMBER_INSTANTIATION */

#if defined (ACE_HAS_EXPLICIT_STATIC_TEMPLATE_MEMBER_INSTANTIATION)
  template ACE_Singleton <Miro::Repository<Foo>, ACE_SYNCH_MUTEX> *
  ACE_Singleton <Miro::Repository<Foo>, ACE_SYNCH_MUTEX>::singleton_;
#endif /* ACE_HAS_EXPLICIT_STATIC_TEMPLATE_MEMBER_INSTANTIATION */

Miro::Singleton<Foo> Foo::instance;

