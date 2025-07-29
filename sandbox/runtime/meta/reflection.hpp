#pragma once

// https://github.com/rttrorg/rttr
// https://preshing.com/20180116/a-primitive-reflection-system-in-cpp-part-1/

#include <any>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "runtime/meta/argwrap.hpp"

#if defined(__REFLECTION_PARSER__)
#define META(...) __attribute__((annotate(#__VA_ARGS__)))
#define CLASS(class_name, ...) class __attribute__((annotate(#__VA_ARGS__))) class_name
#define STRUCT(struct_name, ...) struct __attribute__((annotate(#__VA_ARGS__))) struct_name
//#define CLASS(class_name,...) class __attribute__((annotate(#__VA_ARGS__))) class_name:public Reflection::object
#else
#define META(...)
#define CLASS(class_name, ...) class class_name
#define STRUCT(struct_name, ...) struct struct_name
//#define CLASS(class_name,...) class class_name:public Reflection::object
#endif // __REFLECTION_PARSER__

namespace reflection {
namespace details {

using ArgWrap = td::impl::ArgWrap;

class ConstructorDefault{
 public:
  ConstructorDefault() = default;

  template <typename C>
  static ConstructorDefault MakeConstructorDefault() {
    ConstructorDefault ctor;
    ctor.fn_ = []() -> std::any {
      return new C;
    };
    return ctor;
  }

  std::any Invoke() {
    return fn_();
  }

 private:
  friend class RawTypeDescriptorBuilder;

  std::function<std::any()> fn_{nullptr};
};

class MemberVariable {
 public:
  MemberVariable() = default;

  template <typename C, typename T>
  MemberVariable(T C::*var) {
    getter_ = [var](std::any obj) -> std::any {
      return std::any_cast<const C *>(obj)->*var;
    };
    setter_ = [var](std::any obj, std::any val) {
      // Syntax: https://stackoverflow.com/a/670744/12003165
      // `obj.*member_var`
      auto *self = std::any_cast<C *>(obj);
      self->*var = std::any_cast<T>(val);
    };
  }

  const std::string &name() const {
    return name_;
  }

  template <typename T, typename C>
  T GetValue(const C &c) const {
    return std::any_cast<T>(getter_(&c));
  }

  template <typename C, typename T>
  void SetValue(C &c, T val) {
    setter_(&c, val);
  }

 private:
  friend class RawTypeDescriptorBuilder;

  std::string name_;
  std::function<std::any(std::any)> getter_{nullptr};
  std::function<void(std::any, std::any)> setter_{nullptr};
};

class MemberFunction {
 public:
  MemberFunction() = default;

  template <typename C, typename R, typename... Args>
  explicit MemberFunction(R (C::*func)(Args...)) {
    fn_ = [func](void *args_ptr) -> std::any {
      using array_t = std::array<impl::ArgWrap, sizeof...(Args)+1>;
      auto &args_arr = *static_cast<array_t *>(args_ptr);
      auto args_tuple = impl::AsTuple<C, Args...>(args_arr);
      return std::apply(func, args_tuple);
    };
  }

  template <typename C, typename... Args>
  explicit MemberFunction(void (C::*func)(Args...)) {
    fn_ = [func](void *args_ptr) -> std::any {
      using array_t = std::array<impl::ArgWrap, sizeof...(Args)+1>;
      auto &args_arr = *static_cast<array_t *>(args_ptr);
      auto args_tuple = impl::AsTuple<C, Args...>(args_arr);
      std::apply(func, args_tuple);
      return std::any{};
    };
  }

  template <typename C, typename R, typename... Args>
  explicit MemberFunction(R (C::*func)(Args...) const) {
    fn_ = [func](void *args_ptr) -> std::any {
      using array_t = std::array<impl::ArgWrap, sizeof...(Args)+1>;
      auto &args_arr = *static_cast<array_t *>(args_ptr);
      auto args_tuple = impl::AsTuple<const C, Args...>(args_arr);
      return std::apply(func, args_tuple);
    };
    is_const_ = true;
  }

  template <typename C, typename... Args>
  explicit MemberFunction(void (C::*func)(Args...) const) {
    fn_ = [func](void *args_ptr) -> std::any {
      using array_t = std::array<impl::ArgWrap, sizeof...(Args)+1>;
      auto &args_arr = *static_cast<array_t *>(args_ptr);
      auto args_tuple = impl::AsTuple<const C, Args...>(args_arr);
      std::apply(func, args_tuple);
      return std::any{};
    };
    is_const_ = true;
  }

  const std::string &name() const {
    return name_;
  }

  bool is_const() const {
    return is_const_;
  }

  template <typename C, typename... Args>
  std::any Invoke(C &c, Args &&... args) {
    if (n_args_ != sizeof...(Args)) {
      throw std::runtime_error("Mismatching number of arguments!");
    }
    std::array<impl::ArgWrap, sizeof...(Args)+1> args_arr = {
        impl::ArgWrap(c),
        impl::ArgWrap{std::forward<Args>(args)}...};
    return fn_(&args_arr);
  }

 private:
  friend class RawTypeDescriptorBuilder;

  std::string name_;
  bool is_const_{false};
  std::function<std::any(std::any)> fn_{nullptr};
};

class TypeDescriptor {
 public:
  const std::string &name() const {
    return name_;
  }

  const ConstructorDefault& GetConstructorDefault() const {
    return constructor_default_;
  }

  const std::vector<MemberVariable> &GetMemberVars() const {
    return member_vars_;
  }

  const std::vector<MemberFunction> &GetMemberFuncs() const {
    return member_funcs_;
  }

  MemberVariable GetMemberVar(const std::string &name) const {
    for (const auto &mv : member_vars_) {
      if (mv.name() == name) {
        return mv;
      }
    }
    return MemberVariable{};
  }

  MemberFunction GetMemberFunc(const std::string &name) const {
    for (const auto &mf : member_funcs_) {
      if (mf.name() == name) {
        return mf;
      }
    }
    return MemberFunction{};
  }

 private:
  friend class RawTypeDescriptorBuilder;

  std::string name_;
  ConstructorDefault constructor_default_;
  std::vector<MemberVariable> member_vars_;
  std::vector<MemberFunction> member_funcs_;
};

class RawTypeDescriptorBuilder {
 public:
  explicit RawTypeDescriptorBuilder(const std::string &name);

  ~RawTypeDescriptorBuilder();
  RawTypeDescriptorBuilder(const RawTypeDescriptorBuilder &) = delete;
  RawTypeDescriptorBuilder &operator=(const RawTypeDescriptorBuilder &) =
      delete;
  RawTypeDescriptorBuilder(RawTypeDescriptorBuilder &&) = default;
  RawTypeDescriptorBuilder &operator=(RawTypeDescriptorBuilder &&) = default;

  template <typename C>
  void SetConstructorDefault() {
    ConstructorDefault ctor;
    ctor = ConstructorDefault::MakeConstructorDefault<C>();
    desc_->constructor_default_ = ctor;
  }

  template <typename C, typename T>
  void AddMemberVar(const std::string &name, T C::*var) {
    MemberVariable mv{var};
    mv.name_ = name;
    desc_->member_vars_.push_back(std::move(mv));
  }

  template <typename FUNC>
  void AddMemberFunc(const std::string &name, FUNC func) {
    MemberFunction mf{func};
    mf.name_ = name;
    desc_->member_funcs_.push_back(std::move(mf));
  }

 private:
  std::unique_ptr<TypeDescriptor> desc_{nullptr};
};

template <typename T>
class TypeDescriptorBuilder {
 public:
  explicit TypeDescriptorBuilder(const std::string &name) : raw_builder_(name) {
  }

  template <typename C>
  TypeDescriptorBuilder &SetConstructorDefault() {
    raw_builder_.SetConstructorDefault<C>();
    return *this;
  }

  template <typename V>
  TypeDescriptorBuilder &AddMemberVar(const std::string &name, V T::*var) {
    raw_builder_.AddMemberVar(name, var);
    return *this;
  }

  template <typename FUNC>
  TypeDescriptorBuilder &AddMemberFunc(const std::string &name, FUNC func) {
    raw_builder_.AddMemberFunc(name, func);
    return *this;
  }

 private:
  RawTypeDescriptorBuilder raw_builder_;
};

class Registry {
 public:
  static Registry &instance() {
    static Registry inst;
    return inst;
  }

  TypeDescriptor *Find(const std::string &name);

  void Register(std::unique_ptr<TypeDescriptor> desc);

  void Clear();

 private:
  std::unordered_map<std::string, std::unique_ptr<TypeDescriptor>> type_descs_;
};

}  // namespace details

template <typename T>
details::TypeDescriptorBuilder<T> AddClass(const std::string &name) {
  details::TypeDescriptorBuilder<T> b{name};
  return b;
}

details::TypeDescriptor &GetByName(const std::string &name);

void ClearRegistry();

template<typename T>
class ReflectionPtr
{
    template<typename U>
    friend class ReflectionPtr;

public:
    ReflectionPtr(std::string type_name, T* instance) : m_type_name(type_name), m_instance(instance) {}
    ReflectionPtr() : m_type_name(), m_instance(nullptr) {}

    ReflectionPtr(const ReflectionPtr& dest) : m_type_name(dest.m_type_name), m_instance(dest.m_instance) {}

    template<typename U /*, typename = typename std::enable_if<std::is_safely_castable<T*, U*>::value>::type */>
    ReflectionPtr<T>& operator=(const ReflectionPtr<U>& dest)
    {
        if (this == static_cast<void*>(&dest))
        {
            return *this;
        }
        m_type_name = dest.m_type_name;
        m_instance  = static_cast<T*>(dest.m_instance);
        return *this;
    }

    template<typename U /*, typename = typename std::enable_if<std::is_safely_castable<T*, U*>::value>::type*/>
    ReflectionPtr<T>& operator=(ReflectionPtr<U>&& dest)
    {
        if (this == static_cast<void*>(&dest))
        {
            return *this;
        }
        m_type_name = dest.m_type_name;
        m_instance  = static_cast<T*>(dest.m_instance);
        return *this;
    }

    ReflectionPtr<T>& operator=(const ReflectionPtr<T>& dest)
    {
        if (this == &dest)
        {
            return *this;
        }
        m_type_name = dest.m_type_name;
        m_instance  = dest.m_instance;
        return *this;
    }

    ReflectionPtr<T>& operator=(ReflectionPtr<T>&& dest)
    {
        if (this == &dest)
        {
            return *this;
        }
        m_type_name = dest.m_type_name;
        m_instance  = dest.m_instance;
        return *this;
    }

    std::string getTypeName() const { return m_type_name; }

    void setTypeName(std::string name) { m_type_name = name; }

    bool operator==(const T* ptr) const { return (m_instance == ptr); }

    bool operator!=(const T* ptr) const { return (m_instance != ptr); }

    bool operator==(const ReflectionPtr<T>& rhs_ptr) const { return (m_instance == rhs_ptr.m_instance); }

    bool operator!=(const ReflectionPtr<T>& rhs_ptr) const { return (m_instance != rhs_ptr.m_instance); }

    template<
        typename T1 /*, typename = typename std::enable_if<std::is_safely_castable<T*, T1*>::value>::type*/>
    explicit operator T1*()
    {
        return static_cast<T1*>(m_instance);
    }

    template<
        typename T1 /*, typename = typename std::enable_if<std::is_safely_castable<T*, T1*>::value>::type*/>
    operator ReflectionPtr<T1>()
    {
        return ReflectionPtr<T1>(m_type_name, (T1*)(m_instance));
    }

    template<
        typename T1 /*, typename = typename std::enable_if<std::is_safely_castable<T*, T1*>::value>::type*/>
    explicit operator const T1*() const
    {
        return static_cast<T1*>(m_instance);
    }

    template<
        typename T1 /*, typename = typename std::enable_if<std::is_safely_castable<T*, T1*>::value>::type*/>
    operator const ReflectionPtr<T1>() const
    {
        return ReflectionPtr<T1>(m_type_name, (T1*)(m_instance));
    }

    T* operator->() { return m_instance; }

    T* operator->() const { return m_instance; }

    T& operator*() { return *(m_instance); }

    T* getPtr() { return m_instance; }

    T* getPtr() const { return m_instance; }

    const T& operator*() const { return *(static_cast<const T*>(m_instance)); }

    T*& getPtrReference() { return m_instance; }

    operator bool() const { return (m_instance != nullptr); }

private:
    std::string m_type_name {""};
    typedef T   m_type;
    T*          m_instance {nullptr};
};

}  // namespace reflection
