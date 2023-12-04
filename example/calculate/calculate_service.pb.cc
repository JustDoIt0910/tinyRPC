// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: calculate_service.proto

#include "calculate_service.pb.h"

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
constexpr AddQuery::AddQuery(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : a_(0)
  , b_(0){}
struct AddQueryDefaultTypeInternal {
  constexpr AddQueryDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~AddQueryDefaultTypeInternal() {}
  union {
    AddQuery _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT AddQueryDefaultTypeInternal _AddQuery_default_instance_;
constexpr AddResponse::AddResponse(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : result_(0){}
struct AddResponseDefaultTypeInternal {
  constexpr AddResponseDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~AddResponseDefaultTypeInternal() {}
  union {
    AddResponse _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT AddResponseDefaultTypeInternal _AddResponse_default_instance_;
static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_calculate_5fservice_2eproto[2];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_calculate_5fservice_2eproto = nullptr;
static const ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor* file_level_service_descriptors_calculate_5fservice_2eproto[1];

const uint32_t TableStruct_calculate_5fservice_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::AddQuery, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::AddQuery, a_),
  PROTOBUF_FIELD_OFFSET(::AddQuery, b_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::AddResponse, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::AddResponse, result_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, -1, sizeof(::AddQuery)},
  { 8, -1, -1, sizeof(::AddResponse)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_AddQuery_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_AddResponse_default_instance_),
};

const char descriptor_table_protodef_calculate_5fservice_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\027calculate_service.proto\" \n\010AddQuery\022\t\n"
  "\001a\030\001 \001(\005\022\t\n\001b\030\002 \001(\005\"\035\n\013AddResponse\022\016\n\006re"
  "sult\030\001 \001(\00522\n\020CalculateService\022\036\n\003Add\022\t."
  "AddQuery\032\014.AddResponseB\003\200\001\001b\006proto3"
  ;
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_calculate_5fservice_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_calculate_5fservice_2eproto = {
  false, false, 155, descriptor_table_protodef_calculate_5fservice_2eproto, "calculate_service.proto", 
  &descriptor_table_calculate_5fservice_2eproto_once, nullptr, 0, 2,
  schemas, file_default_instances, TableStruct_calculate_5fservice_2eproto::offsets,
  file_level_metadata_calculate_5fservice_2eproto, file_level_enum_descriptors_calculate_5fservice_2eproto, file_level_service_descriptors_calculate_5fservice_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable* descriptor_table_calculate_5fservice_2eproto_getter() {
  return &descriptor_table_calculate_5fservice_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY static ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptorsRunner dynamic_init_dummy_calculate_5fservice_2eproto(&descriptor_table_calculate_5fservice_2eproto);

// ===================================================================

class AddQuery::_Internal {
 public:
};

AddQuery::AddQuery(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor();
  if (!is_message_owned) {
    RegisterArenaDtor(arena);
  }
  // @@protoc_insertion_point(arena_constructor:AddQuery)
}
AddQuery::AddQuery(const AddQuery& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::memcpy(&a_, &from.a_,
    static_cast<size_t>(reinterpret_cast<char*>(&b_) -
    reinterpret_cast<char*>(&a_)) + sizeof(b_));
  // @@protoc_insertion_point(copy_constructor:AddQuery)
}

inline void AddQuery::SharedCtor() {
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&a_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&b_) -
    reinterpret_cast<char*>(&a_)) + sizeof(b_));
}

AddQuery::~AddQuery() {
  // @@protoc_insertion_point(destructor:AddQuery)
  if (GetArenaForAllocation() != nullptr) return;
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

inline void AddQuery::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
}

void AddQuery::ArenaDtor(void* object) {
  AddQuery* _this = reinterpret_cast< AddQuery* >(object);
  (void)_this;
}
void AddQuery::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void AddQuery::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void AddQuery::Clear() {
// @@protoc_insertion_point(message_clear_start:AddQuery)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  ::memset(&a_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&b_) -
      reinterpret_cast<char*>(&a_)) + sizeof(b_));
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* AddQuery::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // int32 a = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 8)) {
          a_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // int32 b = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 16)) {
          b_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
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

uint8_t* AddQuery::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:AddQuery)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 a = 1;
  if (this->_internal_a() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(1, this->_internal_a(), target);
  }

  // int32 b = 2;
  if (this->_internal_b() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(2, this->_internal_b(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:AddQuery)
  return target;
}

size_t AddQuery::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:AddQuery)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // int32 a = 1;
  if (this->_internal_a() != 0) {
    total_size += ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32SizePlusOne(this->_internal_a());
  }

  // int32 b = 2;
  if (this->_internal_b() != 0) {
    total_size += ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32SizePlusOne(this->_internal_b());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData AddQuery::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSizeCheck,
    AddQuery::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*AddQuery::GetClassData() const { return &_class_data_; }

void AddQuery::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to,
                      const ::PROTOBUF_NAMESPACE_ID::Message& from) {
  static_cast<AddQuery *>(to)->MergeFrom(
      static_cast<const AddQuery &>(from));
}


void AddQuery::MergeFrom(const AddQuery& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:AddQuery)
  GOOGLE_DCHECK_NE(&from, this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (from._internal_a() != 0) {
    _internal_set_a(from._internal_a());
  }
  if (from._internal_b() != 0) {
    _internal_set_b(from._internal_b());
  }
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void AddQuery::CopyFrom(const AddQuery& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:AddQuery)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool AddQuery::IsInitialized() const {
  return true;
}

void AddQuery::InternalSwap(AddQuery* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(AddQuery, b_)
      + sizeof(AddQuery::b_)
      - PROTOBUF_FIELD_OFFSET(AddQuery, a_)>(
          reinterpret_cast<char*>(&a_),
          reinterpret_cast<char*>(&other->a_));
}

::PROTOBUF_NAMESPACE_ID::Metadata AddQuery::GetMetadata() const {
  return ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(
      &descriptor_table_calculate_5fservice_2eproto_getter, &descriptor_table_calculate_5fservice_2eproto_once,
      file_level_metadata_calculate_5fservice_2eproto[0]);
}

// ===================================================================

class AddResponse::_Internal {
 public:
};

AddResponse::AddResponse(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor();
  if (!is_message_owned) {
    RegisterArenaDtor(arena);
  }
  // @@protoc_insertion_point(arena_constructor:AddResponse)
}
AddResponse::AddResponse(const AddResponse& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  result_ = from.result_;
  // @@protoc_insertion_point(copy_constructor:AddResponse)
}

inline void AddResponse::SharedCtor() {
result_ = 0;
}

AddResponse::~AddResponse() {
  // @@protoc_insertion_point(destructor:AddResponse)
  if (GetArenaForAllocation() != nullptr) return;
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

inline void AddResponse::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
}

void AddResponse::ArenaDtor(void* object) {
  AddResponse* _this = reinterpret_cast< AddResponse* >(object);
  (void)_this;
}
void AddResponse::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void AddResponse::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void AddResponse::Clear() {
// @@protoc_insertion_point(message_clear_start:AddResponse)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  result_ = 0;
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* AddResponse::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // int32 result = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 8)) {
          result_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
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

uint8_t* AddResponse::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:AddResponse)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 result = 1;
  if (this->_internal_result() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(1, this->_internal_result(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:AddResponse)
  return target;
}

size_t AddResponse::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:AddResponse)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // int32 result = 1;
  if (this->_internal_result() != 0) {
    total_size += ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32SizePlusOne(this->_internal_result());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData AddResponse::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSizeCheck,
    AddResponse::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*AddResponse::GetClassData() const { return &_class_data_; }

void AddResponse::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to,
                      const ::PROTOBUF_NAMESPACE_ID::Message& from) {
  static_cast<AddResponse *>(to)->MergeFrom(
      static_cast<const AddResponse &>(from));
}


void AddResponse::MergeFrom(const AddResponse& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:AddResponse)
  GOOGLE_DCHECK_NE(&from, this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (from._internal_result() != 0) {
    _internal_set_result(from._internal_result());
  }
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void AddResponse::CopyFrom(const AddResponse& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:AddResponse)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool AddResponse::IsInitialized() const {
  return true;
}

void AddResponse::InternalSwap(AddResponse* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(result_, other->result_);
}

::PROTOBUF_NAMESPACE_ID::Metadata AddResponse::GetMetadata() const {
  return ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(
      &descriptor_table_calculate_5fservice_2eproto_getter, &descriptor_table_calculate_5fservice_2eproto_once,
      file_level_metadata_calculate_5fservice_2eproto[1]);
}

// ===================================================================

CalculateService::~CalculateService() {}

const ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor* CalculateService::descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_calculate_5fservice_2eproto);
  return file_level_service_descriptors_calculate_5fservice_2eproto[0];
}

const ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor* CalculateService::GetDescriptor() {
  return descriptor();
}

void CalculateService::Add(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                         const ::AddQuery*,
                         ::AddResponse*,
                         ::google::protobuf::Closure* done) {
  controller->SetFailed("Method Add() not implemented.");
  done->Run();
}

void CalculateService::CallMethod(const ::PROTOBUF_NAMESPACE_ID::MethodDescriptor* method,
                             ::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                             const ::PROTOBUF_NAMESPACE_ID::Message* request,
                             ::PROTOBUF_NAMESPACE_ID::Message* response,
                             ::google::protobuf::Closure* done) {
  GOOGLE_DCHECK_EQ(method->service(), file_level_service_descriptors_calculate_5fservice_2eproto[0]);
  switch(method->index()) {
    case 0:
      Add(controller,
             ::PROTOBUF_NAMESPACE_ID::internal::DownCast<const ::AddQuery*>(
                 request),
             ::PROTOBUF_NAMESPACE_ID::internal::DownCast<::AddResponse*>(
                 response),
             done);
      break;
    default:
      GOOGLE_LOG(FATAL) << "Bad method index; this should never happen.";
      break;
  }
}

const ::PROTOBUF_NAMESPACE_ID::Message& CalculateService::GetRequestPrototype(
    const ::PROTOBUF_NAMESPACE_ID::MethodDescriptor* method) const {
  GOOGLE_DCHECK_EQ(method->service(), descriptor());
  switch(method->index()) {
    case 0:
      return ::AddQuery::default_instance();
    default:
      GOOGLE_LOG(FATAL) << "Bad method index; this should never happen.";
      return *::PROTOBUF_NAMESPACE_ID::MessageFactory::generated_factory()
          ->GetPrototype(method->input_type());
  }
}

const ::PROTOBUF_NAMESPACE_ID::Message& CalculateService::GetResponsePrototype(
    const ::PROTOBUF_NAMESPACE_ID::MethodDescriptor* method) const {
  GOOGLE_DCHECK_EQ(method->service(), descriptor());
  switch(method->index()) {
    case 0:
      return ::AddResponse::default_instance();
    default:
      GOOGLE_LOG(FATAL) << "Bad method index; this should never happen.";
      return *::PROTOBUF_NAMESPACE_ID::MessageFactory::generated_factory()
          ->GetPrototype(method->output_type());
  }
}

CalculateService_Stub::CalculateService_Stub(::PROTOBUF_NAMESPACE_ID::RpcChannel* channel)
  : channel_(channel), owns_channel_(false) {}
CalculateService_Stub::CalculateService_Stub(
    ::PROTOBUF_NAMESPACE_ID::RpcChannel* channel,
    ::PROTOBUF_NAMESPACE_ID::Service::ChannelOwnership ownership)
  : channel_(channel),
    owns_channel_(ownership == ::PROTOBUF_NAMESPACE_ID::Service::STUB_OWNS_CHANNEL) {}
CalculateService_Stub::~CalculateService_Stub() {
  if (owns_channel_) delete channel_;
}

void CalculateService_Stub::Add(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                              const ::AddQuery* request,
                              ::AddResponse* response,
                              ::google::protobuf::Closure* done) {
  channel_->CallMethod(descriptor()->method(0),
                       controller, request, response, done);
}

// @@protoc_insertion_point(namespace_scope)
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::AddQuery* Arena::CreateMaybeMessage< ::AddQuery >(Arena* arena) {
  return Arena::CreateMessageInternal< ::AddQuery >(arena);
}
template<> PROTOBUF_NOINLINE ::AddResponse* Arena::CreateMaybeMessage< ::AddResponse >(Arena* arena) {
  return Arena::CreateMessageInternal< ::AddResponse >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>