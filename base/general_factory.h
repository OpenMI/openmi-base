/*!
 *  Copyright 2017 by Contributors
 *  \file general_factory.h
 *  \brief general factory class impl
 *  \author ZhouYong
 */
#ifndef OPENMI_BASE_GENERAL_FACTORY_H_
#define OPENMI_BASE_GENERAL_FACTORY_H_ 

#include <err.h>
#include <errno.h>
#include <memory>
#include <string>
#include <unordered_map>
#include "base/logging.h"

namespace openmi {

/*!
code example:

std::string& sub_class = item->sub_class;
OpPtr op = openmi::GeneralFactory<FeatureOp>::Instance().CreateObj(sub_class);
if (op == nullptr) {
  LOG(ERROR) << "sub class init failed. sub_class: " << sub_class;
  return -1;
}

*/

template<typename BaseClass>
class GeneralClassFactory {
public:
  virtual std::shared_ptr<BaseClass> CreateObj() = 0;
};

template<typename BaseClass>
class GeneralFactory {
public:
  static GeneralFactory& Instance() {
    static GeneralFactory general_factory;
    return general_factory;
  }

  virtual std::shared_ptr<BaseClass> CreateObj(const std::string& class_name) {
    if (class_mapper_.find(class_name) == class_mapper_.end()) {
      LOG(ERROR) << "class_name[" << class_name << "] not in class_mapper_, return nullptr";
      return nullptr;
    }
    return class_mapper_[class_name]->CreateObj();
  }

  virtual void RegisterClass(const std::string& class_name, GeneralClassFactory<BaseClass>* factory) {
    if (class_mapper_.find(class_name) != class_mapper_.end()) {
      LOG(WARN) << "class_name[" << class_name << "] has already registered. return;";
      return;
    }
    class_mapper_[class_name] = factory;
  }

private:
  GeneralFactory() {}
  GeneralFactory(const GeneralFactory<BaseClass>&);
  GeneralFactory* operator=(const GeneralFactory<BaseClass>&);

private:
  std::unordered_map<std::string, GeneralClassFactory<BaseClass>* > class_mapper_;
}; // GeneralFactory

template<typename SubClass, typename BaseClass> 
class GeneralClassFactoryImpl: public GeneralClassFactory<BaseClass> {
public:
  virtual std::shared_ptr<BaseClass> CreateObj() {
    return std::make_shared<SubClass>();
  }
}; // GeneralClassFactoryImpl

template<typename BaseClass>
class GeneralClassRegister {
public:
  GeneralClassRegister(const std::string& class_name, 
                       GeneralClassFactory<BaseClass>* general_class_factory, 
                       GeneralFactory<BaseClass>& general_factory) {
    LOG(WARNING) << "register class. class_name[" << class_name << "] begining ...";
    general_factory.RegisterClass(class_name, general_class_factory);
  }
}; // class GeneralClassRegister

#define REGISTER_CLASS(sub_class, base_class) \
    static GeneralClassFactoryImpl<sub_class, base_class> general_class##_general_factory;\
    static GeneralClassRegister<base_class> general_class##general_register(#sub_class, &general_class##_general_factory, GeneralFactory<base_class>::Instance());

} // namespace
#endif // OPENMI_BASE_GENERAL_FACTORY_H_
