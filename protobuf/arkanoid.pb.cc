// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: arkanoid.proto

#include "arkanoid.pb.h"

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
extern PROTOBUF_INTERNAL_EXPORT_arkanoid_2eproto ::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<1> scc_info_Element_arkanoid_2eproto;
extern PROTOBUF_INTERNAL_EXPORT_arkanoid_2eproto ::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_Position_arkanoid_2eproto;
class PositionDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<Position> _instance;
} _Position_default_instance_;
class ElementDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<Element> _instance;
} _Element_default_instance_;
class GameDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<Game> _instance;
} _Game_default_instance_;
static void InitDefaultsscc_info_Element_arkanoid_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::_Element_default_instance_;
    new (ptr) ::Element();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Element::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<1> scc_info_Element_arkanoid_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 1, 0, InitDefaultsscc_info_Element_arkanoid_2eproto}, {
      &scc_info_Position_arkanoid_2eproto.base,}};

static void InitDefaultsscc_info_Game_arkanoid_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::_Game_default_instance_;
    new (ptr) ::Game();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Game::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<1> scc_info_Game_arkanoid_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 1, 0, InitDefaultsscc_info_Game_arkanoid_2eproto}, {
      &scc_info_Element_arkanoid_2eproto.base,}};

static void InitDefaultsscc_info_Position_arkanoid_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::_Position_default_instance_;
    new (ptr) ::Position();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Position::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_Position_arkanoid_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_Position_arkanoid_2eproto}, {}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_arkanoid_2eproto[3];
static const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* file_level_enum_descriptors_arkanoid_2eproto[1];
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_arkanoid_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_arkanoid_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Position, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Position, x_),
  PROTOBUF_FIELD_OFFSET(::Position, y_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Element, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Element, id_),
  PROTOBUF_FIELD_OFFSET(::Element, type_),
  PROTOBUF_FIELD_OFFSET(::Element, position_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Game, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Game, element_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::Position)},
  { 7, -1, sizeof(::Element)},
  { 15, -1, sizeof(::Game)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_Position_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_Element_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_Game_default_instance_),
};

const char descriptor_table_protodef_arkanoid_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\016arkanoid.proto\" \n\010Position\022\t\n\001x\030\001 \001(\005\022"
  "\t\n\001y\030\002 \001(\005\"N\n\007Element\022\n\n\002id\030\001 \001(\005\022\032\n\004typ"
  "e\030\002 \001(\0162\014.ElementType\022\033\n\010position\030\003 \001(\0132"
  "\t.Position\"!\n\004Game\022\031\n\007element\030\001 \003(\0132\010.El"
  "ement*.\n\013ElementType\022\010\n\004BALL\020\000\022\n\n\006PADDLE"
  "\020\001\022\t\n\005BRICK\020\002b\006proto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_arkanoid_2eproto_deps[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_arkanoid_2eproto_sccs[3] = {
  &scc_info_Element_arkanoid_2eproto.base,
  &scc_info_Game_arkanoid_2eproto.base,
  &scc_info_Position_arkanoid_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_arkanoid_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_arkanoid_2eproto = {
  false, false, descriptor_table_protodef_arkanoid_2eproto, "arkanoid.proto", 221,
  &descriptor_table_arkanoid_2eproto_once, descriptor_table_arkanoid_2eproto_sccs, descriptor_table_arkanoid_2eproto_deps, 3, 0,
  schemas, file_default_instances, TableStruct_arkanoid_2eproto::offsets,
  file_level_metadata_arkanoid_2eproto, 3, file_level_enum_descriptors_arkanoid_2eproto, file_level_service_descriptors_arkanoid_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_arkanoid_2eproto = (static_cast<void>(::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_arkanoid_2eproto)), true);
const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* ElementType_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_arkanoid_2eproto);
  return file_level_enum_descriptors_arkanoid_2eproto[0];
}
bool ElementType_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}


// ===================================================================

void Position::InitAsDefaultInstance() {
}
class Position::_Internal {
 public:
};

Position::Position(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:Position)
}
Position::Position(const Position& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::memcpy(&x_, &from.x_,
    static_cast<size_t>(reinterpret_cast<char*>(&y_) -
    reinterpret_cast<char*>(&x_)) + sizeof(y_));
  // @@protoc_insertion_point(copy_constructor:Position)
}

void Position::SharedCtor() {
  ::memset(&x_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&y_) -
      reinterpret_cast<char*>(&x_)) + sizeof(y_));
}

Position::~Position() {
  // @@protoc_insertion_point(destructor:Position)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void Position::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void Position::ArenaDtor(void* object) {
  Position* _this = reinterpret_cast< Position* >(object);
  (void)_this;
}
void Position::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void Position::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const Position& Position::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_Position_arkanoid_2eproto.base);
  return *internal_default_instance();
}


void Position::Clear() {
// @@protoc_insertion_point(message_clear_start:Position)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  ::memset(&x_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&y_) -
      reinterpret_cast<char*>(&x_)) + sizeof(y_));
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* Position::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  ::PROTOBUF_NAMESPACE_ID::Arena* arena = GetArena(); (void)arena;
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // int32 x = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          x_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // int32 y = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          y_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* Position::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:Position)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 x = 1;
  if (this->x() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(1, this->_internal_x(), target);
  }

  // int32 y = 2;
  if (this->y() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(2, this->_internal_y(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Position)
  return target;
}

size_t Position::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Position)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // int32 x = 1;
  if (this->x() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
        this->_internal_x());
  }

  // int32 y = 2;
  if (this->y() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
        this->_internal_y());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void Position::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:Position)
  GOOGLE_DCHECK_NE(&from, this);
  const Position* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<Position>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:Position)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:Position)
    MergeFrom(*source);
  }
}

void Position::MergeFrom(const Position& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:Position)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.x() != 0) {
    _internal_set_x(from._internal_x());
  }
  if (from.y() != 0) {
    _internal_set_y(from._internal_y());
  }
}

void Position::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:Position)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Position::CopyFrom(const Position& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Position)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Position::IsInitialized() const {
  return true;
}

void Position::InternalSwap(Position* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(Position, y_)
      + sizeof(Position::y_)
      - PROTOBUF_FIELD_OFFSET(Position, x_)>(
          reinterpret_cast<char*>(&x_),
          reinterpret_cast<char*>(&other->x_));
}

::PROTOBUF_NAMESPACE_ID::Metadata Position::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

void Element::InitAsDefaultInstance() {
  ::_Element_default_instance_._instance.get_mutable()->position_ = const_cast< ::Position*>(
      ::Position::internal_default_instance());
}
class Element::_Internal {
 public:
  static const ::Position& position(const Element* msg);
};

const ::Position&
Element::_Internal::position(const Element* msg) {
  return *msg->position_;
}
Element::Element(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:Element)
}
Element::Element(const Element& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  if (from._internal_has_position()) {
    position_ = new ::Position(*from.position_);
  } else {
    position_ = nullptr;
  }
  ::memcpy(&id_, &from.id_,
    static_cast<size_t>(reinterpret_cast<char*>(&type_) -
    reinterpret_cast<char*>(&id_)) + sizeof(type_));
  // @@protoc_insertion_point(copy_constructor:Element)
}

void Element::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_Element_arkanoid_2eproto.base);
  ::memset(&position_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&type_) -
      reinterpret_cast<char*>(&position_)) + sizeof(type_));
}

Element::~Element() {
  // @@protoc_insertion_point(destructor:Element)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void Element::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  if (this != internal_default_instance()) delete position_;
}

void Element::ArenaDtor(void* object) {
  Element* _this = reinterpret_cast< Element* >(object);
  (void)_this;
}
void Element::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void Element::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const Element& Element::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_Element_arkanoid_2eproto.base);
  return *internal_default_instance();
}


void Element::Clear() {
// @@protoc_insertion_point(message_clear_start:Element)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  if (GetArena() == nullptr && position_ != nullptr) {
    delete position_;
  }
  position_ = nullptr;
  ::memset(&id_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&type_) -
      reinterpret_cast<char*>(&id_)) + sizeof(type_));
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* Element::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  ::PROTOBUF_NAMESPACE_ID::Arena* arena = GetArena(); (void)arena;
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // int32 id = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // .ElementType type = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          ::PROTOBUF_NAMESPACE_ID::uint64 val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          _internal_set_type(static_cast<::ElementType>(val));
        } else goto handle_unusual;
        continue;
      // .Position position = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 26)) {
          ptr = ctx->ParseMessage(_internal_mutable_position(), ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* Element::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:Element)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 id = 1;
  if (this->id() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(1, this->_internal_id(), target);
  }

  // .ElementType type = 2;
  if (this->type() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteEnumToArray(
      2, this->_internal_type(), target);
  }

  // .Position position = 3;
  if (this->has_position()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        3, _Internal::position(this), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Element)
  return target;
}

size_t Element::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Element)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // .Position position = 3;
  if (this->has_position()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
        *position_);
  }

  // int32 id = 1;
  if (this->id() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
        this->_internal_id());
  }

  // .ElementType type = 2;
  if (this->type() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::EnumSize(this->_internal_type());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void Element::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:Element)
  GOOGLE_DCHECK_NE(&from, this);
  const Element* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<Element>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:Element)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:Element)
    MergeFrom(*source);
  }
}

void Element::MergeFrom(const Element& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:Element)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.has_position()) {
    _internal_mutable_position()->::Position::MergeFrom(from._internal_position());
  }
  if (from.id() != 0) {
    _internal_set_id(from._internal_id());
  }
  if (from.type() != 0) {
    _internal_set_type(from._internal_type());
  }
}

void Element::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:Element)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Element::CopyFrom(const Element& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Element)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Element::IsInitialized() const {
  return true;
}

void Element::InternalSwap(Element* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(Element, type_)
      + sizeof(Element::type_)
      - PROTOBUF_FIELD_OFFSET(Element, position_)>(
          reinterpret_cast<char*>(&position_),
          reinterpret_cast<char*>(&other->position_));
}

::PROTOBUF_NAMESPACE_ID::Metadata Element::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

void Game::InitAsDefaultInstance() {
}
class Game::_Internal {
 public:
};

Game::Game(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena),
  element_(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:Game)
}
Game::Game(const Game& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      element_(from.element_) {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:Game)
}

void Game::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_Game_arkanoid_2eproto.base);
}

Game::~Game() {
  // @@protoc_insertion_point(destructor:Game)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void Game::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void Game::ArenaDtor(void* object) {
  Game* _this = reinterpret_cast< Game* >(object);
  (void)_this;
}
void Game::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void Game::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const Game& Game::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_Game_arkanoid_2eproto.base);
  return *internal_default_instance();
}


void Game::Clear() {
// @@protoc_insertion_point(message_clear_start:Game)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  element_.Clear();
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* Game::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  ::PROTOBUF_NAMESPACE_ID::Arena* arena = GetArena(); (void)arena;
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // repeated .Element element = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          ptr -= 1;
          do {
            ptr += 1;
            ptr = ctx->ParseMessage(_internal_add_element(), ptr);
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<10>(ptr));
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* Game::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:Game)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // repeated .Element element = 1;
  for (unsigned int i = 0,
      n = static_cast<unsigned int>(this->_internal_element_size()); i < n; i++) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(1, this->_internal_element(i), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Game)
  return target;
}

size_t Game::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Game)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // repeated .Element element = 1;
  total_size += 1UL * this->_internal_element_size();
  for (const auto& msg : this->element_) {
    total_size +=
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(msg);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void Game::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:Game)
  GOOGLE_DCHECK_NE(&from, this);
  const Game* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<Game>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:Game)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:Game)
    MergeFrom(*source);
  }
}

void Game::MergeFrom(const Game& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:Game)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  element_.MergeFrom(from.element_);
}

void Game::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:Game)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Game::CopyFrom(const Game& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Game)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Game::IsInitialized() const {
  return true;
}

void Game::InternalSwap(Game* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  element_.InternalSwap(&other->element_);
}

::PROTOBUF_NAMESPACE_ID::Metadata Game::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::Position* Arena::CreateMaybeMessage< ::Position >(Arena* arena) {
  return Arena::CreateMessageInternal< ::Position >(arena);
}
template<> PROTOBUF_NOINLINE ::Element* Arena::CreateMaybeMessage< ::Element >(Arena* arena) {
  return Arena::CreateMessageInternal< ::Element >(arena);
}
template<> PROTOBUF_NOINLINE ::Game* Arena::CreateMaybeMessage< ::Game >(Arena* arena) {
  return Arena::CreateMessageInternal< ::Game >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
