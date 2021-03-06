#include "management.h"

namespace kagami::management {
///////////////////////////////////////////////////////////////
//Inteface management

  auto &GetFunctionImplCollections() {
    static map<string, FunctionImplCollection> collection_base;
    return collection_base;
  }

  auto &GetFunctionImplCache() {
    static unordered_map<string, FunctionHashMap> cache;
    return cache;
  }

  void BuildFunctionImplCache(string domain) {
    auto &base = GetFunctionImplCache();
    auto &col = GetFunctionImplCollections().at(domain);

    for (auto &unit : col) {
      base[domain].insert(make_pair(unit.first, &unit.second));
    }
  }

  void CreateImpl(FunctionImpl impl, string domain) {
    auto &collection_base = GetFunctionImplCollections();
    auto it = collection_base.find(domain);

    if (it != collection_base.end()) {
      it->second.insert(std::make_pair(impl.GetId(), impl));
    }
    else {
      collection_base.insert(make_pair(domain, FunctionImplCollection()));
      collection_base[domain].insert(std::make_pair(impl.GetId(), impl));
    }

    BuildFunctionImplCache(domain);
  }

  FunctionImpl *FindFunction(string id, string domain) {
    auto &cache = GetFunctionImplCache();
    auto it = cache.find(domain);

    if (it != cache.end()) {
      auto dest = it->second.find(id);
      if (dest != it->second.end()) {
        return dest->second;
      }
    }

    return nullptr;
  }
  ////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////
  //Constant object management

  auto &GetConstantBase() {
    static ObjectContainer base;
    return base;
  }

  Object *CreateConstantObject(string id, Object &object) {
    ObjectContainer &base = GetConstantBase();

    if (base.Find(id) != nullptr) return nullptr;

    base.Add(id, object);
    auto result = base.Find(id);
    return result;
  }

  Object *CreateConstantObject(string id, Object && object) {
    return CreateConstantObject(id, object);
  }

  Object GetConstantObject(string id) {
    ObjectContainer &base = GetConstantBase();
    auto ptr = base.Find(id);

    if (ptr != nullptr) {
      Object obj = type::CreateObjectCopy(*ptr);
      return obj;
    }

    return Object();
  }

  /////////////////////////////////////////////////////////////
}

namespace kagami::management::type {
  auto &GetObjectTraitsCollection() {
    static unordered_map<string, ObjectTraits> base;
    return base;
  }

  vector<string> GetMethods(string id) {
    vector<string> result;
    const auto it = GetObjectTraitsCollection().find(id);

    if (it != GetObjectTraitsCollection().end()) {
      result = it->second.GetMethods();
    }
    return result;
  }

  bool CheckMethod(string func_id, string domain) {
    bool result = false;
    const auto it = GetObjectTraitsCollection().find(domain);

    if (it != GetObjectTraitsCollection().end()) {
      result = find_in_vector(func_id, it->second.GetMethods());
    }

    return result;
  }

  size_t GetHash(Object &obj) {
    auto &base = GetObjectTraitsCollection();
    const auto it = base.find(obj.GetTypeId());
    auto hasher = it->second.GetHasher();
    return hasher(obj.Get());
  }

  bool IsHashable(Object &obj) {
    bool result = false;
    auto &base = GetObjectTraitsCollection();
    const auto it = base.find(obj.GetTypeId());

    if (it != base.end()) {
      result = (it->second.GetHasher() != nullptr);
    }

    return result;
  }

  bool IsCopyable(Object &obj) {
    bool result = false;
    auto &base = GetObjectTraitsCollection();
    const auto it = base.find(obj.GetTypeId());

    if (it != base.end()) {
      result = (it->second.GetDeliver() != ShallowDelivery);
    }

    return result;
  }

  void CreateObjectTraits(string id, ObjectTraits temp) {
    GetObjectTraitsCollection().insert(pair<string, ObjectTraits>(id, temp));
  }

  Object CreateObjectCopy(Object &object) {
    if (object.GetDeliverFlag()) {
      return object;
    }

    Object result;
    const auto it = GetObjectTraitsCollection().find(object.GetTypeId());
    if (it != GetObjectTraitsCollection().end()) {
      auto deliver = it->second.GetDeliver();
      result.PackContent(deliver(object.Get()), object.GetTypeId());
    }

    return result;
  }

  bool CheckBehavior(Object obj, string method_str) {
    auto obj_methods = GetMethods(obj.GetTypeId());
    auto sample = BuildStringVector(method_str);
    bool result = true;

    for (auto &unit : sample) {
      if (!find_in_vector(unit, obj_methods)) {
        result = false;
        break;
      }
    }

    return result;
  }

  bool CompareObjects(Object &lhs, Object &rhs) {
    if (lhs.GetTypeId() != rhs.GetTypeId()) return false;
    auto &collection = GetObjectTraitsCollection();
    const auto it = collection.find(lhs.GetTypeId());
    bool value = false;

    if (it != collection.end()) {
      auto comparator = it->second.GetComparator();
      if (comparator != nullptr) value = comparator(lhs, rhs);
      else value = (lhs.Get() == rhs.Get());
    }

    return value;
  }

  ObjectTraitsSetup &ObjectTraitsSetup::InitMethods(initializer_list<FunctionImpl> && rhs) {
    impl_ = rhs;
    string method_list("");

    for (auto &unit : impl_) {
      method_list.append(unit.GetId()).append("|");
    }

    if (method_list != "") {
      methods_ = method_list.substr(0, method_list.size() - 1);
    }

    return *this;
  }

  ObjectTraitsSetup::~ObjectTraitsSetup() {
    CreateObjectTraits(type_id_, ObjectTraits(dlvy_, methods_, hasher_, comparator_));
    CreateImpl(do_not_copy_);
    for (auto &unit : impl_) {
      CreateImpl(unit, type_id_);
    }
  }
}