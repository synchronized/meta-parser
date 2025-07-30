#pragma once
#include "runtime/meta/json.h"
#include "runtime/meta/reflection.hpp"

#include <cassert>

namespace serializer {

namespace details {

class TypeDescriptor {
public:
    const std::string &name() const {
        return name_;
    }

    const std::function<void*(const Json&)> &GetConstructorWithJson() const {
        return constructorWithJson_;
    }

    const std::function<Json(void*)> &GetWriteToJson() const {
        return writeToJson_;
    }

private:
    friend class RawTypeDescriptorBuilder;

    std::string name_;
    std::function<void*(const Json&)> constructorWithJson_;
    std::function<Json(void*)> writeToJson_;
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

    void SetConstructorWithJson(std::function<void*(const Json&)> func) {
        desc_->constructorWithJson_ = func;
    }

    void SetWriteToJson(const std::function<Json(void*)> func) {
        desc_->writeToJson_ = func;
    }


private:
    std::unique_ptr<TypeDescriptor> desc_{nullptr};
};

template <typename T>
class TypeDescriptorBuilder {
public:
    explicit TypeDescriptorBuilder(const std::string &name) : raw_builder_(name) {
    }

    TypeDescriptorBuilder &SetConstructorWithJson(std::function<void*(const Json&)> func) {
        raw_builder_.SetConstructorWithJson(func);
        return *this;
    }

    TypeDescriptorBuilder &SetWriteToJson(const std::function<Json(void*)> func) {
        raw_builder_.SetWriteToJson(func);
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

////////////////////////////////////

template<typename...>
inline constexpr bool always_false = false;

class Serializer
{
public:
    template<typename T>
    static Json writePointer(T* instance)
    {
        return Json::object {{"$typeName", Json {"*"}}, {"$context", Serializer::write(*instance)}};
    }

    template<typename T>
    static T*& readPointer(const Json& json_context, T*& instance)
    {
        assert(instance == nullptr);
        std::string type_name = json_context["$typeName"].string_value();
        assert(!type_name.empty());
        if ('*' == type_name[0])
        {
            instance = new T;
            read(json_context["$context"], *instance);
        }
        else
        {
            auto& classDesc = GetByName(type_name);
            auto& ctorWithJson = classDesc.GetConstructorWithJson();
            instance = static_cast<C*>(ctorWithJson(json_context["$context"]));
        }
        return instance;
    }

    template<typename T>
    static Json write(const reflection::ReflectionPtr<T>& instance)
    {
        T*          instance_ptr = static_cast<T*>(instance.operator->());
        std::string type_name    = instance.getTypeName();
        auto& classDesc = GetByName(type_name);
        auto& writeToJson = classDesc.GetWriteToJson();
        return Json::object {
            {"$typeName", Json(type_name)},
            {"$context", writeToJson(instance_ptr)},
        };
    }

    template<typename T>
    static T*& read(const Json& json_context, reflection::ReflectionPtr<T>& instance)
    {
        std::string type_name = json_context["$typeName"].string_value();
        instance.setTypeName(type_name);
        return readPointer(json_context, instance.getPtrReference());
    }

    template<typename T>
    static Json write(const T& instance)
    {

        if constexpr (std::is_pointer<T>::value)
        {
            return writePointer((T)instance);
        }
        else
        {
            static_assert(always_false<T>, "Serializer::write<T> has not been implemented yet!");
            return Json();
        }
    }

    template<typename T>
    static T& read(const Json& json_context, T& instance)
    {
        if constexpr (std::is_pointer<T>::value)
        {
            return readPointer(json_context, instance);
        }
        else
        {
            static_assert(always_false<T>, "Serializer::read<T> has not been implemented yet!");
            return instance;
        }
    }
};

// implementation of base types
template<>
Json Serializer::write(const char& instance);
template<>
char& Serializer::read(const Json& json_context, char& instance);

template<>
Json Serializer::write(const int& instance);
template<>
int& Serializer::read(const Json& json_context, int& instance);

template<>
Json Serializer::write(const unsigned int& instance);
template<>
unsigned int& Serializer::read(const Json& json_context, unsigned int& instance);

template<>
Json Serializer::write(const float& instance);
template<>
float& Serializer::read(const Json& json_context, float& instance);

template<>
Json Serializer::write(const double& instance);
template<>
double& Serializer::read(const Json& json_context, double& instance);

template<>
Json Serializer::write(const bool& instance);
template<>
bool& Serializer::read(const Json& json_context, bool& instance);

template<>
Json Serializer::write(const std::string& instance);
template<>
std::string& Serializer::read(const Json& json_context, std::string& instance);

// template<>
// Json Serializer::write(const Reflection::object& instance);
// template<>
// Reflection::object& Serializer::read(const Json& json_context, Reflection::object& instance);

////////////////////////////////////
////sample of generation coder
////////////////////////////////////
// class test_class
//{
// public:
//     int a;
//     unsigned int b;
//     std::vector<int> c;
// };
// class ss;
// class jkj;
// template<>
// Json Serializer::write(const ss& instance);
// template<>
// Json Serializer::write(const jkj& instance);

/*REFLECTION_TYPE(jkj)
CLASS(jkj,Fields)
{
    REFLECTION_BODY(jkj);
    int jl;
};

REFLECTION_TYPE(ss)
CLASS(ss:public jkj,WhiteListFields)
{
    REFLECTION_BODY(ss);
    int jl;
};*/

////////////////////////////////////
////template of generation coder
////////////////////////////////////
// template<>
// Json Serializer::write(const test_class& instance);
// template<>
// test_class& Serializer::read(const Json& json_context, test_class& instance);

//
////////////////////////////////////

} // namespace serializer
