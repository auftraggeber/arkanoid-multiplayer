// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: arkanoid.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_arkanoid_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_arkanoid_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3012000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3012004 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_arkanoid_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_arkanoid_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[3]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_arkanoid_2eproto;
class Element;
class ElementDefaultTypeInternal;
extern ElementDefaultTypeInternal _Element_default_instance_;
class Game;
class GameDefaultTypeInternal;
extern GameDefaultTypeInternal _Game_default_instance_;
class Position;
class PositionDefaultTypeInternal;
extern PositionDefaultTypeInternal _Position_default_instance_;
PROTOBUF_NAMESPACE_OPEN
template<> ::Element* Arena::CreateMaybeMessage<::Element>(Arena*);
template<> ::Game* Arena::CreateMaybeMessage<::Game>(Arena*);
template<> ::Position* Arena::CreateMaybeMessage<::Position>(Arena*);
PROTOBUF_NAMESPACE_CLOSE

enum ElementType : int {
  BALL = 0,
  PADDLE = 1,
  BRICK = 2,
  ElementType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  ElementType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool ElementType_IsValid(int value);
constexpr ElementType ElementType_MIN = BALL;
constexpr ElementType ElementType_MAX = BRICK;
constexpr int ElementType_ARRAYSIZE = ElementType_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* ElementType_descriptor();
template<typename T>
inline const std::string& ElementType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, ElementType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function ElementType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    ElementType_descriptor(), enum_t_value);
}
inline bool ElementType_Parse(
    const std::string& name, ElementType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<ElementType>(
    ElementType_descriptor(), name, value);
}
// ===================================================================

class Position PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:Position) */ {
 public:
  inline Position() : Position(nullptr) {};
  virtual ~Position();

  Position(const Position& from);
  Position(Position&& from) noexcept
    : Position() {
    *this = ::std::move(from);
  }

  inline Position& operator=(const Position& from) {
    CopyFrom(from);
    return *this;
  }
  inline Position& operator=(Position&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const Position& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Position* internal_default_instance() {
    return reinterpret_cast<const Position*>(
               &_Position_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(Position& a, Position& b) {
    a.Swap(&b);
  }
  inline void Swap(Position* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(Position* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline Position* New() const final {
    return CreateMaybeMessage<Position>(nullptr);
  }

  Position* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<Position>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const Position& from);
  void MergeFrom(const Position& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Position* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "Position";
  }
  protected:
  explicit Position(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_arkanoid_2eproto);
    return ::descriptor_table_arkanoid_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kXFieldNumber = 1,
    kYFieldNumber = 2,
  };
  // int32 x = 1;
  void clear_x();
  ::PROTOBUF_NAMESPACE_ID::int32 x() const;
  void set_x(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_x() const;
  void _internal_set_x(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // int32 y = 2;
  void clear_y();
  ::PROTOBUF_NAMESPACE_ID::int32 y() const;
  void set_y(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_y() const;
  void _internal_set_y(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // @@protoc_insertion_point(class_scope:Position)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::int32 x_;
  ::PROTOBUF_NAMESPACE_ID::int32 y_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_arkanoid_2eproto;
};
// -------------------------------------------------------------------

class Element PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:Element) */ {
 public:
  inline Element() : Element(nullptr) {};
  virtual ~Element();

  Element(const Element& from);
  Element(Element&& from) noexcept
    : Element() {
    *this = ::std::move(from);
  }

  inline Element& operator=(const Element& from) {
    CopyFrom(from);
    return *this;
  }
  inline Element& operator=(Element&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const Element& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Element* internal_default_instance() {
    return reinterpret_cast<const Element*>(
               &_Element_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(Element& a, Element& b) {
    a.Swap(&b);
  }
  inline void Swap(Element* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(Element* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline Element* New() const final {
    return CreateMaybeMessage<Element>(nullptr);
  }

  Element* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<Element>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const Element& from);
  void MergeFrom(const Element& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Element* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "Element";
  }
  protected:
  explicit Element(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_arkanoid_2eproto);
    return ::descriptor_table_arkanoid_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kPositionFieldNumber = 3,
    kIdFieldNumber = 1,
    kTypeFieldNumber = 2,
  };
  // .Position position = 3;
  bool has_position() const;
  private:
  bool _internal_has_position() const;
  public:
  void clear_position();
  const ::Position& position() const;
  ::Position* release_position();
  ::Position* mutable_position();
  void set_allocated_position(::Position* position);
  private:
  const ::Position& _internal_position() const;
  ::Position* _internal_mutable_position();
  public:
  void unsafe_arena_set_allocated_position(
      ::Position* position);
  ::Position* unsafe_arena_release_position();

  // int32 id = 1;
  void clear_id();
  ::PROTOBUF_NAMESPACE_ID::int32 id() const;
  void set_id(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_id() const;
  void _internal_set_id(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // .ElementType type = 2;
  void clear_type();
  ::ElementType type() const;
  void set_type(::ElementType value);
  private:
  ::ElementType _internal_type() const;
  void _internal_set_type(::ElementType value);
  public:

  // @@protoc_insertion_point(class_scope:Element)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::Position* position_;
  ::PROTOBUF_NAMESPACE_ID::int32 id_;
  int type_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_arkanoid_2eproto;
};
// -------------------------------------------------------------------

class Game PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:Game) */ {
 public:
  inline Game() : Game(nullptr) {};
  virtual ~Game();

  Game(const Game& from);
  Game(Game&& from) noexcept
    : Game() {
    *this = ::std::move(from);
  }

  inline Game& operator=(const Game& from) {
    CopyFrom(from);
    return *this;
  }
  inline Game& operator=(Game&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const Game& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Game* internal_default_instance() {
    return reinterpret_cast<const Game*>(
               &_Game_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    2;

  friend void swap(Game& a, Game& b) {
    a.Swap(&b);
  }
  inline void Swap(Game* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(Game* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline Game* New() const final {
    return CreateMaybeMessage<Game>(nullptr);
  }

  Game* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<Game>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const Game& from);
  void MergeFrom(const Game& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Game* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "Game";
  }
  protected:
  explicit Game(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_arkanoid_2eproto);
    return ::descriptor_table_arkanoid_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kElementFieldNumber = 1,
  };
  // repeated .Element element = 1;
  int element_size() const;
  private:
  int _internal_element_size() const;
  public:
  void clear_element();
  ::Element* mutable_element(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Element >*
      mutable_element();
  private:
  const ::Element& _internal_element(int index) const;
  ::Element* _internal_add_element();
  public:
  const ::Element& element(int index) const;
  ::Element* add_element();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Element >&
      element() const;

  // @@protoc_insertion_point(class_scope:Game)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Element > element_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_arkanoid_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Position

// int32 x = 1;
inline void Position::clear_x() {
  x_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 Position::_internal_x() const {
  return x_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 Position::x() const {
  // @@protoc_insertion_point(field_get:Position.x)
  return _internal_x();
}
inline void Position::_internal_set_x(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  x_ = value;
}
inline void Position::set_x(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_x(value);
  // @@protoc_insertion_point(field_set:Position.x)
}

// int32 y = 2;
inline void Position::clear_y() {
  y_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 Position::_internal_y() const {
  return y_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 Position::y() const {
  // @@protoc_insertion_point(field_get:Position.y)
  return _internal_y();
}
inline void Position::_internal_set_y(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  y_ = value;
}
inline void Position::set_y(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_y(value);
  // @@protoc_insertion_point(field_set:Position.y)
}

// -------------------------------------------------------------------

// Element

// int32 id = 1;
inline void Element::clear_id() {
  id_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 Element::_internal_id() const {
  return id_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 Element::id() const {
  // @@protoc_insertion_point(field_get:Element.id)
  return _internal_id();
}
inline void Element::_internal_set_id(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  id_ = value;
}
inline void Element::set_id(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_id(value);
  // @@protoc_insertion_point(field_set:Element.id)
}

// .ElementType type = 2;
inline void Element::clear_type() {
  type_ = 0;
}
inline ::ElementType Element::_internal_type() const {
  return static_cast< ::ElementType >(type_);
}
inline ::ElementType Element::type() const {
  // @@protoc_insertion_point(field_get:Element.type)
  return _internal_type();
}
inline void Element::_internal_set_type(::ElementType value) {
  
  type_ = value;
}
inline void Element::set_type(::ElementType value) {
  _internal_set_type(value);
  // @@protoc_insertion_point(field_set:Element.type)
}

// .Position position = 3;
inline bool Element::_internal_has_position() const {
  return this != internal_default_instance() && position_ != nullptr;
}
inline bool Element::has_position() const {
  return _internal_has_position();
}
inline void Element::clear_position() {
  if (GetArena() == nullptr && position_ != nullptr) {
    delete position_;
  }
  position_ = nullptr;
}
inline const ::Position& Element::_internal_position() const {
  const ::Position* p = position_;
  return p != nullptr ? *p : *reinterpret_cast<const ::Position*>(
      &::_Position_default_instance_);
}
inline const ::Position& Element::position() const {
  // @@protoc_insertion_point(field_get:Element.position)
  return _internal_position();
}
inline void Element::unsafe_arena_set_allocated_position(
    ::Position* position) {
  if (GetArena() == nullptr) {
    delete reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(position_);
  }
  position_ = position;
  if (position) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:Element.position)
}
inline ::Position* Element::release_position() {
  auto temp = unsafe_arena_release_position();
  if (GetArena() != nullptr) {
    temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  }
  return temp;
}
inline ::Position* Element::unsafe_arena_release_position() {
  // @@protoc_insertion_point(field_release:Element.position)
  
  ::Position* temp = position_;
  position_ = nullptr;
  return temp;
}
inline ::Position* Element::_internal_mutable_position() {
  
  if (position_ == nullptr) {
    auto* p = CreateMaybeMessage<::Position>(GetArena());
    position_ = p;
  }
  return position_;
}
inline ::Position* Element::mutable_position() {
  // @@protoc_insertion_point(field_mutable:Element.position)
  return _internal_mutable_position();
}
inline void Element::set_allocated_position(::Position* position) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArena();
  if (message_arena == nullptr) {
    delete position_;
  }
  if (position) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena =
      ::PROTOBUF_NAMESPACE_ID::Arena::GetArena(position);
    if (message_arena != submessage_arena) {
      position = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, position, submessage_arena);
    }
    
  } else {
    
  }
  position_ = position;
  // @@protoc_insertion_point(field_set_allocated:Element.position)
}

// -------------------------------------------------------------------

// Game

// repeated .Element element = 1;
inline int Game::_internal_element_size() const {
  return element_.size();
}
inline int Game::element_size() const {
  return _internal_element_size();
}
inline void Game::clear_element() {
  element_.Clear();
}
inline ::Element* Game::mutable_element(int index) {
  // @@protoc_insertion_point(field_mutable:Game.element)
  return element_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Element >*
Game::mutable_element() {
  // @@protoc_insertion_point(field_mutable_list:Game.element)
  return &element_;
}
inline const ::Element& Game::_internal_element(int index) const {
  return element_.Get(index);
}
inline const ::Element& Game::element(int index) const {
  // @@protoc_insertion_point(field_get:Game.element)
  return _internal_element(index);
}
inline ::Element* Game::_internal_add_element() {
  return element_.Add();
}
inline ::Element* Game::add_element() {
  // @@protoc_insertion_point(field_add:Game.element)
  return _internal_add_element();
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Element >&
Game::element() const {
  // @@protoc_insertion_point(field_list:Game.element)
  return element_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)


PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::ElementType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::ElementType>() {
  return ::ElementType_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_arkanoid_2eproto
