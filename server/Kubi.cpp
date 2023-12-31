// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Kubi.proto

#include "Kubi.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG

namespace _pb = ::PROTOBUF_NAMESPACE_ID;
namespace _pbi = _pb::internal;

namespace kubi {
PROTOBUF_CONSTEXPR KNode::KNode(
    ::_pbi::ConstantInitialized)
  : uid_(&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{})
  , route_(&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{})
  , data_str_(&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{})
  , data_bytes_(&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{})
  , pkg_type_(0)
  , pkg_id_(0){}
struct KNodeDefaultTypeInternal {
  PROTOBUF_CONSTEXPR KNodeDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~KNodeDefaultTypeInternal() {}
  union {
    KNode _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 KNodeDefaultTypeInternal _KNode_default_instance_;
}  // namespace kubi
static ::_pb::Metadata file_level_metadata_Kubi_2eproto[1];
static constexpr ::_pb::EnumDescriptor const** file_level_enum_descriptors_Kubi_2eproto = nullptr;
static constexpr ::_pb::ServiceDescriptor const** file_level_service_descriptors_Kubi_2eproto = nullptr;

const uint32_t TableStruct_Kubi_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::kubi::KNode, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::kubi::KNode, pkg_type_),
  PROTOBUF_FIELD_OFFSET(::kubi::KNode, pkg_id_),
  PROTOBUF_FIELD_OFFSET(::kubi::KNode, uid_),
  PROTOBUF_FIELD_OFFSET(::kubi::KNode, route_),
  PROTOBUF_FIELD_OFFSET(::kubi::KNode, data_str_),
  PROTOBUF_FIELD_OFFSET(::kubi::KNode, data_bytes_),
};
static const ::_pbi::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, -1, sizeof(::kubi::KNode)},
};

static const ::_pb::Message* const file_default_instances[] = {
  &::kubi::_KNode_default_instance_._instance,
};

const char descriptor_table_protodef_Kubi_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\nKubi.proto\022\004kubi\"k\n\005KNode\022\020\n\010pkg_type\030"
  "\001 \001(\005\022\016\n\006pkg_id\030\002 \001(\005\022\013\n\003uid\030\003 \001(\t\022\r\n\005ro"
  "ute\030\004 \001(\t\022\020\n\010data_str\030\005 \001(\t\022\022\n\ndata_byte"
  "s\030\006 \001(\014b\006proto3"
  ;
static ::_pbi::once_flag descriptor_table_Kubi_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_Kubi_2eproto = {
    false, false, 135, descriptor_table_protodef_Kubi_2eproto,
    "Kubi.proto",
    &descriptor_table_Kubi_2eproto_once, nullptr, 0, 1,
    schemas, file_default_instances, TableStruct_Kubi_2eproto::offsets,
    file_level_metadata_Kubi_2eproto, file_level_enum_descriptors_Kubi_2eproto,
    file_level_service_descriptors_Kubi_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_Kubi_2eproto_getter() {
  return &descriptor_table_Kubi_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_Kubi_2eproto(&descriptor_table_Kubi_2eproto);
namespace kubi {

// ===================================================================

class KNode::_Internal {
 public:
};

KNode::KNode(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor();
  // @@protoc_insertion_point(arena_constructor:kubi.KNode)
}
KNode::KNode(const KNode& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  uid_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    uid_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_uid().empty()) {
    uid_.Set(from._internal_uid(), 
      GetArenaForAllocation());
  }
  route_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    route_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_route().empty()) {
    route_.Set(from._internal_route(), 
      GetArenaForAllocation());
  }
  data_str_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    data_str_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_data_str().empty()) {
    data_str_.Set(from._internal_data_str(), 
      GetArenaForAllocation());
  }
  data_bytes_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    data_bytes_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_data_bytes().empty()) {
    data_bytes_.Set(from._internal_data_bytes(), 
      GetArenaForAllocation());
  }
  ::memcpy(&pkg_type_, &from.pkg_type_,
    static_cast<size_t>(reinterpret_cast<char*>(&pkg_id_) -
    reinterpret_cast<char*>(&pkg_type_)) + sizeof(pkg_id_));
  // @@protoc_insertion_point(copy_constructor:kubi.KNode)
}

inline void KNode::SharedCtor() {
uid_.InitDefault();
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  uid_.Set("", GetArenaForAllocation());
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
route_.InitDefault();
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  route_.Set("", GetArenaForAllocation());
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
data_str_.InitDefault();
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  data_str_.Set("", GetArenaForAllocation());
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
data_bytes_.InitDefault();
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  data_bytes_.Set("", GetArenaForAllocation());
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&pkg_type_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&pkg_id_) -
    reinterpret_cast<char*>(&pkg_type_)) + sizeof(pkg_id_));
}

KNode::~KNode() {
  // @@protoc_insertion_point(destructor:kubi.KNode)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void KNode::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  uid_.Destroy();
  route_.Destroy();
  data_str_.Destroy();
  data_bytes_.Destroy();
}

void KNode::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void KNode::Clear() {
// @@protoc_insertion_point(message_clear_start:kubi.KNode)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  uid_.ClearToEmpty();
  route_.ClearToEmpty();
  data_str_.ClearToEmpty();
  data_bytes_.ClearToEmpty();
  ::memset(&pkg_type_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&pkg_id_) -
      reinterpret_cast<char*>(&pkg_type_)) + sizeof(pkg_id_));
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* KNode::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // int32 pkg_type = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 8)) {
          pkg_type_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // int32 pkg_id = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 16)) {
          pkg_id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // string uid = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 26)) {
          auto str = _internal_mutable_uid();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
          CHK_(::_pbi::VerifyUTF8(str, "kubi.KNode.uid"));
        } else
          goto handle_unusual;
        continue;
      // string route = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 34)) {
          auto str = _internal_mutable_route();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
          CHK_(::_pbi::VerifyUTF8(str, "kubi.KNode.route"));
        } else
          goto handle_unusual;
        continue;
      // string data_str = 5;
      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 42)) {
          auto str = _internal_mutable_data_str();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
          CHK_(::_pbi::VerifyUTF8(str, "kubi.KNode.data_str"));
        } else
          goto handle_unusual;
        continue;
      // bytes data_bytes = 6;
      case 6:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 50)) {
          auto str = _internal_mutable_data_bytes();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* KNode::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:kubi.KNode)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 pkg_type = 1;
  if (this->_internal_pkg_type() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteInt32ToArray(1, this->_internal_pkg_type(), target);
  }

  // int32 pkg_id = 2;
  if (this->_internal_pkg_id() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteInt32ToArray(2, this->_internal_pkg_id(), target);
  }

  // string uid = 3;
  if (!this->_internal_uid().empty()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_uid().data(), static_cast<int>(this->_internal_uid().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "kubi.KNode.uid");
    target = stream->WriteStringMaybeAliased(
        3, this->_internal_uid(), target);
  }

  // string route = 4;
  if (!this->_internal_route().empty()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_route().data(), static_cast<int>(this->_internal_route().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "kubi.KNode.route");
    target = stream->WriteStringMaybeAliased(
        4, this->_internal_route(), target);
  }

  // string data_str = 5;
  if (!this->_internal_data_str().empty()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_data_str().data(), static_cast<int>(this->_internal_data_str().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "kubi.KNode.data_str");
    target = stream->WriteStringMaybeAliased(
        5, this->_internal_data_str(), target);
  }

  // bytes data_bytes = 6;
  if (!this->_internal_data_bytes().empty()) {
    target = stream->WriteBytesMaybeAliased(
        6, this->_internal_data_bytes(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:kubi.KNode)
  return target;
}

size_t KNode::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:kubi.KNode)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string uid = 3;
  if (!this->_internal_uid().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_uid());
  }

  // string route = 4;
  if (!this->_internal_route().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_route());
  }

  // string data_str = 5;
  if (!this->_internal_data_str().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_data_str());
  }

  // bytes data_bytes = 6;
  if (!this->_internal_data_bytes().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
        this->_internal_data_bytes());
  }

  // int32 pkg_type = 1;
  if (this->_internal_pkg_type() != 0) {
    total_size += ::_pbi::WireFormatLite::Int32SizePlusOne(this->_internal_pkg_type());
  }

  // int32 pkg_id = 2;
  if (this->_internal_pkg_id() != 0) {
    total_size += ::_pbi::WireFormatLite::Int32SizePlusOne(this->_internal_pkg_id());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData KNode::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSizeCheck,
    KNode::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*KNode::GetClassData() const { return &_class_data_; }

void KNode::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to,
                      const ::PROTOBUF_NAMESPACE_ID::Message& from) {
  static_cast<KNode *>(to)->MergeFrom(
      static_cast<const KNode &>(from));
}


void KNode::MergeFrom(const KNode& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:kubi.KNode)
  GOOGLE_DCHECK_NE(&from, this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (!from._internal_uid().empty()) {
    _internal_set_uid(from._internal_uid());
  }
  if (!from._internal_route().empty()) {
    _internal_set_route(from._internal_route());
  }
  if (!from._internal_data_str().empty()) {
    _internal_set_data_str(from._internal_data_str());
  }
  if (!from._internal_data_bytes().empty()) {
    _internal_set_data_bytes(from._internal_data_bytes());
  }
  if (from._internal_pkg_type() != 0) {
    _internal_set_pkg_type(from._internal_pkg_type());
  }
  if (from._internal_pkg_id() != 0) {
    _internal_set_pkg_id(from._internal_pkg_id());
  }
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void KNode::CopyFrom(const KNode& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:kubi.KNode)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool KNode::IsInitialized() const {
  return true;
}

void KNode::InternalSwap(KNode* other) {
  using std::swap;
  auto* lhs_arena = GetArenaForAllocation();
  auto* rhs_arena = other->GetArenaForAllocation();
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &uid_, lhs_arena,
      &other->uid_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &route_, lhs_arena,
      &other->route_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &data_str_, lhs_arena,
      &other->data_str_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &data_bytes_, lhs_arena,
      &other->data_bytes_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(KNode, pkg_id_)
      + sizeof(KNode::pkg_id_)
      - PROTOBUF_FIELD_OFFSET(KNode, pkg_type_)>(
          reinterpret_cast<char*>(&pkg_type_),
          reinterpret_cast<char*>(&other->pkg_type_));
}

::PROTOBUF_NAMESPACE_ID::Metadata KNode::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_Kubi_2eproto_getter, &descriptor_table_Kubi_2eproto_once,
      file_level_metadata_Kubi_2eproto[0]);
}

// @@protoc_insertion_point(namespace_scope)
}  // namespace kubi
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::kubi::KNode*
Arena::CreateMaybeMessage< ::kubi::KNode >(Arena* arena) {
  return Arena::CreateMessageInternal< ::kubi::KNode >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
