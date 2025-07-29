#include "serializer.hpp"
#include <assert.h>

namespace serializer
{

template<>
Json Serializer::write(const char& instance)
{
    return Json(instance);
}
template<>
char& Serializer::read(const Json& json_context, char& instance)
{
    assert(json_context.is_number());
    return instance = json_context.number_value();
}

template<>
Json Serializer::write(const int& instance)
{
    return Json(instance);
}
template<>
int& Serializer::read(const Json& json_context, int& instance)
{
    assert(json_context.is_number());
    return instance = static_cast<int>(json_context.number_value());
}

template<>
Json Serializer::write(const unsigned int& instance)
{
    return Json(static_cast<int>(instance));
}
template<>
unsigned int& Serializer::read(const Json& json_context, unsigned int& instance)
{
    assert(json_context.is_number());
    return instance = static_cast<unsigned int>(json_context.number_value());
}

template<>
Json Serializer::write(const float& instance)
{
    return Json(instance);
}
template<>
float& Serializer::read(const Json& json_context, float& instance)
{
    assert(json_context.is_number());
    return instance = static_cast<float>(json_context.number_value());
}

template<>
Json Serializer::write(const double& instance)
{
    return Json(instance);
}
template<>
double& Serializer::read(const Json& json_context, double& instance)
{
    assert(json_context.is_number());
    return instance = static_cast<float>(json_context.number_value());
}

template<>
Json Serializer::write(const bool& instance)
{
    return Json(instance);
}
template<>
bool& Serializer::read(const Json& json_context, bool& instance)
{
    assert(json_context.is_bool());
    return instance = json_context.bool_value();
}

template<>
Json Serializer::write(const std::string& instance)
{
    return Json(instance);
}
template<>
std::string& Serializer::read(const Json& json_context, std::string& instance)
{
    assert(json_context.is_string());
    return instance = json_context.string_value();
}

    // template<>
    // Json Serializer::write(const Reflection::object& instance)
    //{
    // return Json::object();
    //}
    // template<>
    // Reflection::object& Serializer::read(const Json& json_context, Reflection::object& instance)
    //{
    //	return instance;
    //}

    //////////////////////////////////
    // template of generation coder
    //////////////////////////////////

    /*template<>
    Json Serializer::write(const ss& instance)
    {
        return Json();
    }
    template<>
    Json Serializer::write(const jkj& instance)
    {
        return Json();
    }

    template<>
    Json Serializer::write(const test_class& instance)
    {
        Json::array aa;
        for(auto& item: instance.c)
        {
            aa.emplace_back(Serializer::write(item));
        }
        ss jj;
        reflection::object* jjj1 = &jj;
        auto kkk = Serializer::write(jjj1);
        auto jjj = kkk.dump();
        return Json::object{
            {"a",Serializer::write(instance.a)},
            {"b",Serializer::write(instance.b)},
            {"c",aa}
        };
    }
    template<>
    test_class& Serializer::read(const Json& json_context, test_class& instance)
    {
        assert(json_context.is_object());
        Serializer::read(json_context["a"], instance.a);
        Serializer::read(json_context["b"], instance.b);

        assert(json_context["c"].is_array());
        Json::array cc = json_context["c"].array_items();
        instance.c.resize(cc.size());
        for (size_t index=0; index < cc.size();++index)
        {
            Serializer::read(cc[index], instance.c[index]);
        }
        return instance;
    }*/


    ////////////////////////////////////

namespace details {

RawTypeDescriptorBuilder::RawTypeDescriptorBuilder(const std::string &name)
    : desc_(std::make_unique<TypeDescriptor>()) {
  desc_->name_ = name;
}

RawTypeDescriptorBuilder::~RawTypeDescriptorBuilder() {
  Registry::instance().Register(std::move(desc_));
}

TypeDescriptor *Registry::Find(const std::string &name) {
  return type_descs_.find(name)->second.get();
}

void Registry::Register(std::unique_ptr<TypeDescriptor> desc) {
  auto name = desc->name();
  type_descs_[name] = std::move(desc);
}

void Registry::Clear() {
  decltype(type_descs_) tmp;
  tmp.swap(type_descs_);
}

}  // namespace details

details::TypeDescriptor &GetByName(const std::string &name) {
  return *details::Registry::instance().Find(name);
}

void ClearRegistry() {
  details::Registry::instance().Clear();
}

} // namespace serializer