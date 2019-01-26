#ifndef TREE_FACTORY_H_ 
#define TREE_FACTORY_H_ 

#include "tree.h"
#include <functional>

#include "register.h"

class TreeFactory: public openmi::FunctionRegisterBase<TreeFactory, std::function<Tree*()> > {

};

#define REGISTER_TREE(Name)                                   \
  OPENMI_REGISTER_OBJECT(::TreeFactory, TreeFactory, Name)    \
  .SetFunction([]() { return new Name(); }) 

#define REGISTER_TREE_WITH_NAME(Object, name)                 \
  OPENMI_REGISTER_OBJECT_HELPER(::TreeFactory, TreeFactory, Object, name)         \
  .SetFunction([]() { return new Object(); })

#endif 
