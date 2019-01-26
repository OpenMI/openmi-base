#ifndef BASE_FACTORY_H_ 
#define BASE_FACTORY_H_ 

#include "base.h"
#include <functional>

#include "register.h"

class BaseFactory: public openmi::FunctionRegisterBase<BaseFactory, std::function<Base*()> > {

};

#define REGISTER_BASE(Name)                                   \
  OPENMI_REGISTER_OBJECT(::BaseFactory, BaseFactory, Name)    \
  .SetFunction([]() { return new Name(); }) 

#endif 
