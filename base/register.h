#ifndef OPENMI_BASE_REGISTER_H_
#define OPENMI_BASE_REGISTER_H_ 

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "logging.h"

namespace openmi {

// register global singletons 
template<typename T>
class Register {
public:
  inline static const std::vector<const T*>& List() {
    return Get()->const_list_;
  }

  inline static std::vector<std::string> ListAllNames() {
    const std::map<std::string, T*>& func_mapper = Get()->func_mapper_;
    typename std::map<std::string, T*>::const_iterator p;
    std::vector<std::string> names;
    for (p = func_mapper.begin(); p != func_mapper.end(); ++p) {
      names.push_back(p->first);
    }
    return names;
  }

  inline static const T* Find(const std::string& name) {
    const std::map<std::string, T*>& func_mapper = Get()->func_mapper_;
    typename std::map<std::string, T*>::const_iterator p = func_mapper.find(name);
    if (p != func_mapper.end()) {
      return p->second;
    } else {
      std::string names;
      for (std::string name: ListAllNames()) {
        names += name + ",";
      }
      LOG(ERROR) << name << " in register not exists. registered names: [" << names << "]";
      return NULL;
    }
  }

  inline void AddAlias(const std::string& key_name, const std::string& alias) {
    T* t = func_mapper_.at(key_name);
    if (func_mapper_.count(alias)) {
      if (t != func_mapper_.at(alias)) {
        std::runtime_error("Try to register alias " + alias + " for key " + key_name + " but " + alias + " is already taken.");
      }
    } else {
      func_mapper_[alias] = t;
    }
  }

  inline T& __REGISTER__(const std::string& name) {
    printf("__REGISTER__ name: %s\n", name.c_str());
    if (func_mapper_.count(name) != 0U) {
      std::runtime_error(name + " already registered");
    }
    T* t = new T();
    t->name = name;
    func_mapper_[name] = t;
    const_list_.push_back(t);
    entry_list_.push_back(t);
    return *t;
  }

  inline T* __REGISTER_OR_GET__(const std::string& name) {
    if (func_mapper_.count(name) == 0) {
      return __REGISTER__(name);
    } else {
      return *func_mapper_.at(name);
    }
  }

  // get a singleton of the registry. this can be defined by OPENMI_REGISTER_ENABLE 
  static Register* Get();

private:
  Register() {}

  ~Register() {
    for (size_t i = 0; i < entry_list_.size(); ++i) {
      delete entry_list_[i];
    }
  }

private:
  std::vector<T*> entry_list_;
  std::vector<const T*> const_list_;
  // a map from name to function type
  std::map<std::string, T*> func_mapper_;
}; // class Register 

/*!
 * A abstraction class used for registry
 */
template<typename T, typename FunctionType>
class FunctionRegisterBase {
public:
  inline T& SetFunction(FunctionType func) {
    this->func = func;
    return this->Self();
  }

  inline T& SetReturnType(const std::string& type) {
    this->return_type = type;
    return this->Self();
  }

  inline T& Self() {
    return *(static_cast<T*>(this));
  }

  inline FunctionType& Function() {
    return this->func;
  }

public:
  std::string name;
  FunctionType func;
  std::string return_type;
  // TODO add argument to factory function 
}; // class FunctionRegisterBase

// Macro to enable the register of Type. 
// This macro must be used under namespace openmi, and only used once in cc file. 
#define OPENMI_REGISTER_ENABLE(Type)            \
  template<>                                    \
  Register<Type>* Register<Type>::Get() {       \
    static Register<Type> inst;                 \
    return &inst;                               \
  }                                             \
/*
#define OPENMI_REGISTER_OBJECT(Type, TypeName, Name)        \
  static Type & __make_ ## TypeName ## _ ## Name ## __ =    \
    ::openmi::Register<Type>::Get()->__REGISTER__(#Name)    \
*/

#define OPENMI_REGISTER_OBJECT(Type, TypeName, Object)        \
  OPENMI_REGISTER_OBJECT_HELPER(Type, TypeName, Object, Object)

#define OPENMI_REGISTER_OBJECT_HELPER(Type, TypeName, Object, name) \
  static Type& __make_ ## TypeName ## _ ## Object ## name ## __ = ::openmi::Register<Type>::Get()->__REGISTER__(#name)

// (Optional) Declare a file tag to current file that contains object registrations. 
#define OPENMI_REGISTER_FILE_TAG(UniqueTag)  \
  int __openmi_register_file_tag_ ## UniqueTag ## __() { return 0; }

// (Optional) Force link to all the objects registered in file tag. 
// This macro must be used in the same file as OPENMI_REGISTER_ENABLE 
// and in the same namespace as OPENMI_REGISTER_FILE_TAG 
// 
// Notice!!! This is very importent to static linking registered object. 
//
// This can be used to enforce link of certain file into during static linking. 
//
// This is mainly used to solve problem during statically link a library which contains backward registration. 
// Specifically, this avoids the object in these file tags to be ignored by compiler. 
//
// For dynamic linking, this problem will not occure as EVERYTHING is loaded by default.
// Use of this optional as it will create an error when a file tag do not exist.
// An alternative solution is always ask user to enable '--whole-archieve' during static linking.
#define OPENMI_REGISTER_LINK_TAG(UniqueTag)             \
  int __openmi_register_file_tag_ ## UniqueTag ## __();  \
  static int __register_file_tag_ ## UniqueTag ## __ =  \
    __openmi_register_file_tag_ ## UniqueTag ## __();

} // namespace openmi 
#endif // OPENMI_BASE_REGISTER_H_
