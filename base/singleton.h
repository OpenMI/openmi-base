#ifndef OPENMI_BASE_SINGLETON_H_
#define OPENMI_BASE_SINGLETON_H_ 

namespace openmi {

template <typename T>
class Singleton {
public:
  static T& Instance() {
    static T instance;
    return instance;
  }
}; // class Singleton

} // namespace openmi
#endif // OPENMI_BASE_SINGLETON_H_
