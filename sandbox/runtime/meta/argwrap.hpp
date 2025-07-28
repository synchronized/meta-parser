#include <any>
#include <array>
#include <functional>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <utility>

namespace td {
namespace impl {

template <typename T>
struct RefTrait {
  static constexpr int value = 0;
};

template <typename T>
struct RefTrait<T &> {
  static constexpr int value = 1;
};

// T&& not supported for simplicity
template <typename T>
struct RefTrait<T &&> {};

template <typename T>
struct IsConst : std::false_type {};

template <typename T>
struct IsConst<T &> : std::false_type {};

template <typename T>
struct IsConst<T *> : std::false_type {};

template <typename T>
struct IsConst<const T> : std::true_type {};

template <typename T>
struct IsConst<const T &> : std::true_type {};

template <typename T>
struct IsConst<const T *> : std::true_type {};

class ArgWrap {
 public:
  template <typename T>
  ArgWrap(T &&val) {
    // Debug type T
    // static_assert(std::is_same<T, void>::value, "Hoi!");
    ref_type_ = RefTrait<T>::value;
    constness_ = IsConst<T>::value;
    if (ref_type_ == 1) {
      storage_ = &val;
    } else {
      storage_ = val;
    }
  }

  template <typename T>
  T Cast() {
    using RawT = std::remove_cv_t<std::remove_reference_t<T>>;
    constexpr int kRefType = RefTrait<T>::value;
    constexpr bool kConstness = IsConst<T>::value;
    if constexpr (kRefType == 0) {
      if (ref_type_ == 1) {
        if (constness_) {
          std::cout << "want copy, self is const-ref\n";
          return *std::any_cast<const RawT *>(storage_);
        } else {
          std::cout << "want copy, self is mut-ref\n";
          return *std::any_cast<RawT *>(storage_);
        }
      }
      std::cout << "want copy, self is copy\n";
      return std::any_cast<RawT>(storage_);
    }

    if (ref_type_ == 0) {
      if constexpr (kConstness) {
        std::cout << "want const-ref, self is copy\n";
      } else {
        std::cout << "want mut-ref, self is copy\n";
      }

      return *std::any_cast<RawT>(&storage_);
    }
    if constexpr (kConstness) {
      if (constness_) {
        std::cout << "want const-ref, self is const-ref\n";
        return *std::any_cast<const RawT *>(storage_);
      } else {
        std::cout << "want const-ref, self is mut-ref\n";
        return *std::any_cast<RawT *>(storage_);
      }
    } else {
      if (constness_) {
        throw std::runtime_error("Cannot cast const-ref to non-const ref");
      }
      std::cout << "want mut-ref, self is mut-ref\n";
      return *std::any_cast<RawT *>(storage_);
    }
  }

 private:
  std::any storage_{};
  int ref_type_{0};
  bool constness_{false};
};

template <typename... Args, size_t N, size_t... Is>
std::tuple<Args...> AsTuple(std::array<ArgWrap, N> &arr,
                            std::index_sequence<Is...>) {
  // Dont use std::make_tuple, which can't easily support reference.
  return std::forward_as_tuple(arr[Is].template Cast<Args>()...);
}

template <typename... Args, size_t N,
          typename = std::enable_if_t<(N == sizeof...(Args))>>
std::tuple<Args...> AsTuple(std::array<ArgWrap, N> &arr) {
  return AsTuple<Args...>(arr, std::make_index_sequence<N>{});
}

}  // namespace impl

class FuncWrap {
 public:
  template <typename... Args>
  explicit FuncWrap(void (*func)(Args...)) {
    n_args_ = sizeof...(Args);
    fn_ = [func](void *args_ptr) -> std::any {
      using array_t = std::array<impl::ArgWrap, sizeof...(Args)>;
      auto &args_arr = *static_cast<array_t *>(args_ptr);
      auto args_tuple = impl::AsTuple<Args...>(args_arr);
      std::apply(func, args_tuple);
      return std::any{};
    };
  }

  template <typename C, typename R, typename... Args>
  explicit FuncWrap(R (C::*func)(Args...)) {
    fn_ = [func](void *args_ptr) -> std::any {
      using array_t = std::array<impl::ArgWrap, sizeof...(Args)+1>;
      auto &args_arr = *static_cast<array_t *>(args_ptr);
      auto args_tuple = impl::AsTuple<C, Args...>(args_arr);
      return std::apply(func, args_tuple);
    };
  }

  template <typename C, typename... Args>
  explicit FuncWrap(void (C::*func)(Args...)) {
    fn_ = [func](void *args_ptr) -> std::any {
      using array_t = std::array<impl::ArgWrap, sizeof...(Args)+1>;
      auto &args_arr = *static_cast<array_t *>(args_ptr);
      auto args_tuple = impl::AsTuple<C, Args...>(args_arr);
      std::apply(func, args_tuple);
      return std::any{};
    };
  }

  template <typename C, typename R, typename... Args>
  explicit FuncWrap(R (C::*func)(Args...) const) {
    fn_ = [func](void *args_ptr) -> std::any {
      using array_t = std::array<impl::ArgWrap, sizeof...(Args)+1>;
      auto &args_arr = *static_cast<array_t *>(args_ptr);
      auto args_tuple = impl::AsTuple<const C, Args...>(args_arr);
      return std::apply(func, args_tuple);
    };
    is_const_ = true;
  }

  template <typename C, typename... Args>
  explicit FuncWrap(void (C::*func)(Args...) const) {
    fn_ = [func](void *args_ptr) -> std::any {
      using array_t = std::array<impl::ArgWrap, sizeof...(Args)+1>;
      auto &args_arr = *static_cast<array_t *>(args_ptr);
      auto args_tuple = impl::AsTuple<const C, Args...>(args_arr);
      std::apply(func, args_tuple);
      return std::any{};
    };
    is_const_ = true;
  }


  template <typename... Args>
  void Invoke(Args &&... args) {
    if (n_args_ != sizeof...(Args)) {
      throw std::runtime_error("Mismatching number of args!");
    }
    std::array<impl::ArgWrap, sizeof...(Args)> args_arr = {
        impl::ArgWrap{std::forward<Args>(args)}...};
    fn_(&args_arr);
  }

  template <typename C, typename... Args>
  std::any Invoke(C &c, Args &&... args) {
    if (n_args_ != sizeof...(Args)) {
      throw std::runtime_error("Mismatching number of args!");
    }
    std::array<impl::ArgWrap, sizeof...(Args)+1> args_arr = {
        impl::ArgWrap(c),
        impl::ArgWrap{std::forward<Args>(args)}...};
    return fn_(&args_arr);
  }

  bool is_const() const {
    return is_const_;
  }

  bool is_member() const {
    return is_member_;
  }

 private:
  std::function<std::any(void *)> fn_{nullptr};
  int n_args_{0};
  bool is_const_{false};
  bool is_member_{false};
};

}  // namespace td
