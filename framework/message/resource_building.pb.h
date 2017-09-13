// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: resource_building.proto

#ifndef PROTOBUF_resource_5fbuilding_2eproto__INCLUDED
#define PROTOBUF_resource_5fbuilding_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
#include "common.pb.h"
// @@protoc_insertion_point(includes)

namespace slg {
namespace protocol {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_resource_5fbuilding_2eproto();
void protobuf_AssignDesc_resource_5fbuilding_2eproto();
void protobuf_ShutdownFile_resource_5fbuilding_2eproto();

class ResourceBuildingDTO;
class ResourceBuildingReapRequest;
class ResourceBuildingReapResponse;
class ResourceBuildingSpeedUpRequest;
class ResourceBuildingSpeedUpResponse;
class ResourceBuildingListPush;

enum ResourceBuildingCommandId {
  RESOURCE_BUILDING_REAP = 401,
  RESOURCE_BUILDING_SPEED_UP = 402
};
bool ResourceBuildingCommandId_IsValid(int value);
const ResourceBuildingCommandId ResourceBuildingCommandId_MIN = RESOURCE_BUILDING_REAP;
const ResourceBuildingCommandId ResourceBuildingCommandId_MAX = RESOURCE_BUILDING_SPEED_UP;
const int ResourceBuildingCommandId_ARRAYSIZE = ResourceBuildingCommandId_MAX + 1;

const ::google::protobuf::EnumDescriptor* ResourceBuildingCommandId_descriptor();
inline const ::std::string& ResourceBuildingCommandId_Name(ResourceBuildingCommandId value) {
  return ::google::protobuf::internal::NameOfEnum(
    ResourceBuildingCommandId_descriptor(), value);
}
inline bool ResourceBuildingCommandId_Parse(
    const ::std::string& name, ResourceBuildingCommandId* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ResourceBuildingCommandId>(
    ResourceBuildingCommandId_descriptor(), name, value);
}
enum ResourceBuildingPushId {
  RESOURCE_BUILDING_LIST_PUSH = 50401
};
bool ResourceBuildingPushId_IsValid(int value);
const ResourceBuildingPushId ResourceBuildingPushId_MIN = RESOURCE_BUILDING_LIST_PUSH;
const ResourceBuildingPushId ResourceBuildingPushId_MAX = RESOURCE_BUILDING_LIST_PUSH;
const int ResourceBuildingPushId_ARRAYSIZE = ResourceBuildingPushId_MAX + 1;

const ::google::protobuf::EnumDescriptor* ResourceBuildingPushId_descriptor();
inline const ::std::string& ResourceBuildingPushId_Name(ResourceBuildingPushId value) {
  return ::google::protobuf::internal::NameOfEnum(
    ResourceBuildingPushId_descriptor(), value);
}
inline bool ResourceBuildingPushId_Parse(
    const ::std::string& name, ResourceBuildingPushId* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ResourceBuildingPushId>(
    ResourceBuildingPushId_descriptor(), name, value);
}
// ===================================================================

class ResourceBuildingDTO : public ::google::protobuf::Message {
 public:
  ResourceBuildingDTO();
  virtual ~ResourceBuildingDTO();

  ResourceBuildingDTO(const ResourceBuildingDTO& from);

  inline ResourceBuildingDTO& operator=(const ResourceBuildingDTO& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const ResourceBuildingDTO& default_instance();

  void Swap(ResourceBuildingDTO* other);

  // implements Message ----------------------------------------------

  ResourceBuildingDTO* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ResourceBuildingDTO& from);
  void MergeFrom(const ResourceBuildingDTO& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required int32 buildingId = 1;
  inline bool has_buildingid() const;
  inline void clear_buildingid();
  static const int kBuildingIdFieldNumber = 1;
  inline ::google::protobuf::int32 buildingid() const;
  inline void set_buildingid(::google::protobuf::int32 value);

  // required int32 currentNum = 2;
  inline bool has_currentnum() const;
  inline void clear_currentnum();
  static const int kCurrentNumFieldNumber = 2;
  inline ::google::protobuf::int32 currentnum() const;
  inline void set_currentnum(::google::protobuf::int32 value);

  // required int32 lastReapTime = 3;
  inline bool has_lastreaptime() const;
  inline void clear_lastreaptime();
  static const int kLastReapTimeFieldNumber = 3;
  inline ::google::protobuf::int32 lastreaptime() const;
  inline void set_lastreaptime(::google::protobuf::int32 value);

  // optional int32 speedUpEndTime = 4;
  inline bool has_speedupendtime() const;
  inline void clear_speedupendtime();
  static const int kSpeedUpEndTimeFieldNumber = 4;
  inline ::google::protobuf::int32 speedupendtime() const;
  inline void set_speedupendtime(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:slg.protocol.ResourceBuildingDTO)
 private:
  inline void set_has_buildingid();
  inline void clear_has_buildingid();
  inline void set_has_currentnum();
  inline void clear_has_currentnum();
  inline void set_has_lastreaptime();
  inline void clear_has_lastreaptime();
  inline void set_has_speedupendtime();
  inline void clear_has_speedupendtime();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int32 buildingid_;
  ::google::protobuf::int32 currentnum_;
  ::google::protobuf::int32 lastreaptime_;
  ::google::protobuf::int32 speedupendtime_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(4 + 31) / 32];

  friend void  protobuf_AddDesc_resource_5fbuilding_2eproto();
  friend void protobuf_AssignDesc_resource_5fbuilding_2eproto();
  friend void protobuf_ShutdownFile_resource_5fbuilding_2eproto();

  void InitAsDefaultInstance();
  static ResourceBuildingDTO* default_instance_;
};
// -------------------------------------------------------------------

class ResourceBuildingReapRequest : public ::google::protobuf::Message {
 public:
  ResourceBuildingReapRequest();
  virtual ~ResourceBuildingReapRequest();

  ResourceBuildingReapRequest(const ResourceBuildingReapRequest& from);

  inline ResourceBuildingReapRequest& operator=(const ResourceBuildingReapRequest& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const ResourceBuildingReapRequest& default_instance();

  void Swap(ResourceBuildingReapRequest* other);

  // implements Message ----------------------------------------------

  ResourceBuildingReapRequest* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ResourceBuildingReapRequest& from);
  void MergeFrom(const ResourceBuildingReapRequest& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required int32 buildingId = 1;
  inline bool has_buildingid() const;
  inline void clear_buildingid();
  static const int kBuildingIdFieldNumber = 1;
  inline ::google::protobuf::int32 buildingid() const;
  inline void set_buildingid(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:slg.protocol.ResourceBuildingReapRequest)
 private:
  inline void set_has_buildingid();
  inline void clear_has_buildingid();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int32 buildingid_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_resource_5fbuilding_2eproto();
  friend void protobuf_AssignDesc_resource_5fbuilding_2eproto();
  friend void protobuf_ShutdownFile_resource_5fbuilding_2eproto();

  void InitAsDefaultInstance();
  static ResourceBuildingReapRequest* default_instance_;
};
// -------------------------------------------------------------------

class ResourceBuildingReapResponse : public ::google::protobuf::Message {
 public:
  ResourceBuildingReapResponse();
  virtual ~ResourceBuildingReapResponse();

  ResourceBuildingReapResponse(const ResourceBuildingReapResponse& from);

  inline ResourceBuildingReapResponse& operator=(const ResourceBuildingReapResponse& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const ResourceBuildingReapResponse& default_instance();

  void Swap(ResourceBuildingReapResponse* other);

  // implements Message ----------------------------------------------

  ResourceBuildingReapResponse* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ResourceBuildingReapResponse& from);
  void MergeFrom(const ResourceBuildingReapResponse& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required int32 buildingId = 1;
  inline bool has_buildingid() const;
  inline void clear_buildingid();
  static const int kBuildingIdFieldNumber = 1;
  inline ::google::protobuf::int32 buildingid() const;
  inline void set_buildingid(::google::protobuf::int32 value);

  // required .slg.protocol.PropertyDTO resourcrAdd = 2;
  inline bool has_resourcradd() const;
  inline void clear_resourcradd();
  static const int kResourcrAddFieldNumber = 2;
  inline const ::slg::protocol::PropertyDTO& resourcradd() const;
  inline ::slg::protocol::PropertyDTO* mutable_resourcradd();
  inline ::slg::protocol::PropertyDTO* release_resourcradd();
  inline void set_allocated_resourcradd(::slg::protocol::PropertyDTO* resourcradd);

  // required bool isCrit = 3;
  inline bool has_iscrit() const;
  inline void clear_iscrit();
  static const int kIsCritFieldNumber = 3;
  inline bool iscrit() const;
  inline void set_iscrit(bool value);

  // @@protoc_insertion_point(class_scope:slg.protocol.ResourceBuildingReapResponse)
 private:
  inline void set_has_buildingid();
  inline void clear_has_buildingid();
  inline void set_has_resourcradd();
  inline void clear_has_resourcradd();
  inline void set_has_iscrit();
  inline void clear_has_iscrit();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::slg::protocol::PropertyDTO* resourcradd_;
  ::google::protobuf::int32 buildingid_;
  bool iscrit_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(3 + 31) / 32];

  friend void  protobuf_AddDesc_resource_5fbuilding_2eproto();
  friend void protobuf_AssignDesc_resource_5fbuilding_2eproto();
  friend void protobuf_ShutdownFile_resource_5fbuilding_2eproto();

  void InitAsDefaultInstance();
  static ResourceBuildingReapResponse* default_instance_;
};
// -------------------------------------------------------------------

class ResourceBuildingSpeedUpRequest : public ::google::protobuf::Message {
 public:
  ResourceBuildingSpeedUpRequest();
  virtual ~ResourceBuildingSpeedUpRequest();

  ResourceBuildingSpeedUpRequest(const ResourceBuildingSpeedUpRequest& from);

  inline ResourceBuildingSpeedUpRequest& operator=(const ResourceBuildingSpeedUpRequest& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const ResourceBuildingSpeedUpRequest& default_instance();

  void Swap(ResourceBuildingSpeedUpRequest* other);

  // implements Message ----------------------------------------------

  ResourceBuildingSpeedUpRequest* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ResourceBuildingSpeedUpRequest& from);
  void MergeFrom(const ResourceBuildingSpeedUpRequest& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required int32 buildingId = 1;
  inline bool has_buildingid() const;
  inline void clear_buildingid();
  static const int kBuildingIdFieldNumber = 1;
  inline ::google::protobuf::int32 buildingid() const;
  inline void set_buildingid(::google::protobuf::int32 value);

  // optional bool useDiamond = 2;
  inline bool has_usediamond() const;
  inline void clear_usediamond();
  static const int kUseDiamondFieldNumber = 2;
  inline bool usediamond() const;
  inline void set_usediamond(bool value);

  // optional int32 itemId = 3;
  inline bool has_itemid() const;
  inline void clear_itemid();
  static const int kItemIdFieldNumber = 3;
  inline ::google::protobuf::int32 itemid() const;
  inline void set_itemid(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:slg.protocol.ResourceBuildingSpeedUpRequest)
 private:
  inline void set_has_buildingid();
  inline void clear_has_buildingid();
  inline void set_has_usediamond();
  inline void clear_has_usediamond();
  inline void set_has_itemid();
  inline void clear_has_itemid();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int32 buildingid_;
  bool usediamond_;
  ::google::protobuf::int32 itemid_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(3 + 31) / 32];

  friend void  protobuf_AddDesc_resource_5fbuilding_2eproto();
  friend void protobuf_AssignDesc_resource_5fbuilding_2eproto();
  friend void protobuf_ShutdownFile_resource_5fbuilding_2eproto();

  void InitAsDefaultInstance();
  static ResourceBuildingSpeedUpRequest* default_instance_;
};
// -------------------------------------------------------------------

class ResourceBuildingSpeedUpResponse : public ::google::protobuf::Message {
 public:
  ResourceBuildingSpeedUpResponse();
  virtual ~ResourceBuildingSpeedUpResponse();

  ResourceBuildingSpeedUpResponse(const ResourceBuildingSpeedUpResponse& from);

  inline ResourceBuildingSpeedUpResponse& operator=(const ResourceBuildingSpeedUpResponse& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const ResourceBuildingSpeedUpResponse& default_instance();

  void Swap(ResourceBuildingSpeedUpResponse* other);

  // implements Message ----------------------------------------------

  ResourceBuildingSpeedUpResponse* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ResourceBuildingSpeedUpResponse& from);
  void MergeFrom(const ResourceBuildingSpeedUpResponse& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required int32 buildingId = 1;
  inline bool has_buildingid() const;
  inline void clear_buildingid();
  static const int kBuildingIdFieldNumber = 1;
  inline ::google::protobuf::int32 buildingid() const;
  inline void set_buildingid(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:slg.protocol.ResourceBuildingSpeedUpResponse)
 private:
  inline void set_has_buildingid();
  inline void clear_has_buildingid();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int32 buildingid_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_resource_5fbuilding_2eproto();
  friend void protobuf_AssignDesc_resource_5fbuilding_2eproto();
  friend void protobuf_ShutdownFile_resource_5fbuilding_2eproto();

  void InitAsDefaultInstance();
  static ResourceBuildingSpeedUpResponse* default_instance_;
};
// -------------------------------------------------------------------

class ResourceBuildingListPush : public ::google::protobuf::Message {
 public:
  ResourceBuildingListPush();
  virtual ~ResourceBuildingListPush();

  ResourceBuildingListPush(const ResourceBuildingListPush& from);

  inline ResourceBuildingListPush& operator=(const ResourceBuildingListPush& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const ResourceBuildingListPush& default_instance();

  void Swap(ResourceBuildingListPush* other);

  // implements Message ----------------------------------------------

  ResourceBuildingListPush* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ResourceBuildingListPush& from);
  void MergeFrom(const ResourceBuildingListPush& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated .slg.protocol.ResourceBuildingDTO resourceBuildings = 1;
  inline int resourcebuildings_size() const;
  inline void clear_resourcebuildings();
  static const int kResourceBuildingsFieldNumber = 1;
  inline const ::slg::protocol::ResourceBuildingDTO& resourcebuildings(int index) const;
  inline ::slg::protocol::ResourceBuildingDTO* mutable_resourcebuildings(int index);
  inline ::slg::protocol::ResourceBuildingDTO* add_resourcebuildings();
  inline const ::google::protobuf::RepeatedPtrField< ::slg::protocol::ResourceBuildingDTO >&
      resourcebuildings() const;
  inline ::google::protobuf::RepeatedPtrField< ::slg::protocol::ResourceBuildingDTO >*
      mutable_resourcebuildings();

  // @@protoc_insertion_point(class_scope:slg.protocol.ResourceBuildingListPush)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedPtrField< ::slg::protocol::ResourceBuildingDTO > resourcebuildings_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_resource_5fbuilding_2eproto();
  friend void protobuf_AssignDesc_resource_5fbuilding_2eproto();
  friend void protobuf_ShutdownFile_resource_5fbuilding_2eproto();

  void InitAsDefaultInstance();
  static ResourceBuildingListPush* default_instance_;
};
// ===================================================================


// ===================================================================

// ResourceBuildingDTO

// required int32 buildingId = 1;
inline bool ResourceBuildingDTO::has_buildingid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ResourceBuildingDTO::set_has_buildingid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ResourceBuildingDTO::clear_has_buildingid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ResourceBuildingDTO::clear_buildingid() {
  buildingid_ = 0;
  clear_has_buildingid();
}
inline ::google::protobuf::int32 ResourceBuildingDTO::buildingid() const {
  return buildingid_;
}
inline void ResourceBuildingDTO::set_buildingid(::google::protobuf::int32 value) {
  set_has_buildingid();
  buildingid_ = value;
}

// required int32 currentNum = 2;
inline bool ResourceBuildingDTO::has_currentnum() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ResourceBuildingDTO::set_has_currentnum() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ResourceBuildingDTO::clear_has_currentnum() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ResourceBuildingDTO::clear_currentnum() {
  currentnum_ = 0;
  clear_has_currentnum();
}
inline ::google::protobuf::int32 ResourceBuildingDTO::currentnum() const {
  return currentnum_;
}
inline void ResourceBuildingDTO::set_currentnum(::google::protobuf::int32 value) {
  set_has_currentnum();
  currentnum_ = value;
}

// required int32 lastReapTime = 3;
inline bool ResourceBuildingDTO::has_lastreaptime() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void ResourceBuildingDTO::set_has_lastreaptime() {
  _has_bits_[0] |= 0x00000004u;
}
inline void ResourceBuildingDTO::clear_has_lastreaptime() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void ResourceBuildingDTO::clear_lastreaptime() {
  lastreaptime_ = 0;
  clear_has_lastreaptime();
}
inline ::google::protobuf::int32 ResourceBuildingDTO::lastreaptime() const {
  return lastreaptime_;
}
inline void ResourceBuildingDTO::set_lastreaptime(::google::protobuf::int32 value) {
  set_has_lastreaptime();
  lastreaptime_ = value;
}

// optional int32 speedUpEndTime = 4;
inline bool ResourceBuildingDTO::has_speedupendtime() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void ResourceBuildingDTO::set_has_speedupendtime() {
  _has_bits_[0] |= 0x00000008u;
}
inline void ResourceBuildingDTO::clear_has_speedupendtime() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void ResourceBuildingDTO::clear_speedupendtime() {
  speedupendtime_ = 0;
  clear_has_speedupendtime();
}
inline ::google::protobuf::int32 ResourceBuildingDTO::speedupendtime() const {
  return speedupendtime_;
}
inline void ResourceBuildingDTO::set_speedupendtime(::google::protobuf::int32 value) {
  set_has_speedupendtime();
  speedupendtime_ = value;
}

// -------------------------------------------------------------------

// ResourceBuildingReapRequest

// required int32 buildingId = 1;
inline bool ResourceBuildingReapRequest::has_buildingid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ResourceBuildingReapRequest::set_has_buildingid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ResourceBuildingReapRequest::clear_has_buildingid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ResourceBuildingReapRequest::clear_buildingid() {
  buildingid_ = 0;
  clear_has_buildingid();
}
inline ::google::protobuf::int32 ResourceBuildingReapRequest::buildingid() const {
  return buildingid_;
}
inline void ResourceBuildingReapRequest::set_buildingid(::google::protobuf::int32 value) {
  set_has_buildingid();
  buildingid_ = value;
}

// -------------------------------------------------------------------

// ResourceBuildingReapResponse

// required int32 buildingId = 1;
inline bool ResourceBuildingReapResponse::has_buildingid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ResourceBuildingReapResponse::set_has_buildingid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ResourceBuildingReapResponse::clear_has_buildingid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ResourceBuildingReapResponse::clear_buildingid() {
  buildingid_ = 0;
  clear_has_buildingid();
}
inline ::google::protobuf::int32 ResourceBuildingReapResponse::buildingid() const {
  return buildingid_;
}
inline void ResourceBuildingReapResponse::set_buildingid(::google::protobuf::int32 value) {
  set_has_buildingid();
  buildingid_ = value;
}

// required .slg.protocol.PropertyDTO resourcrAdd = 2;
inline bool ResourceBuildingReapResponse::has_resourcradd() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ResourceBuildingReapResponse::set_has_resourcradd() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ResourceBuildingReapResponse::clear_has_resourcradd() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ResourceBuildingReapResponse::clear_resourcradd() {
  if (resourcradd_ != NULL) resourcradd_->::slg::protocol::PropertyDTO::Clear();
  clear_has_resourcradd();
}
inline const ::slg::protocol::PropertyDTO& ResourceBuildingReapResponse::resourcradd() const {
  return resourcradd_ != NULL ? *resourcradd_ : *default_instance_->resourcradd_;
}
inline ::slg::protocol::PropertyDTO* ResourceBuildingReapResponse::mutable_resourcradd() {
  set_has_resourcradd();
  if (resourcradd_ == NULL) resourcradd_ = new ::slg::protocol::PropertyDTO;
  return resourcradd_;
}
inline ::slg::protocol::PropertyDTO* ResourceBuildingReapResponse::release_resourcradd() {
  clear_has_resourcradd();
  ::slg::protocol::PropertyDTO* temp = resourcradd_;
  resourcradd_ = NULL;
  return temp;
}
inline void ResourceBuildingReapResponse::set_allocated_resourcradd(::slg::protocol::PropertyDTO* resourcradd) {
  delete resourcradd_;
  resourcradd_ = resourcradd;
  if (resourcradd) {
    set_has_resourcradd();
  } else {
    clear_has_resourcradd();
  }
}

// required bool isCrit = 3;
inline bool ResourceBuildingReapResponse::has_iscrit() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void ResourceBuildingReapResponse::set_has_iscrit() {
  _has_bits_[0] |= 0x00000004u;
}
inline void ResourceBuildingReapResponse::clear_has_iscrit() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void ResourceBuildingReapResponse::clear_iscrit() {
  iscrit_ = false;
  clear_has_iscrit();
}
inline bool ResourceBuildingReapResponse::iscrit() const {
  return iscrit_;
}
inline void ResourceBuildingReapResponse::set_iscrit(bool value) {
  set_has_iscrit();
  iscrit_ = value;
}

// -------------------------------------------------------------------

// ResourceBuildingSpeedUpRequest

// required int32 buildingId = 1;
inline bool ResourceBuildingSpeedUpRequest::has_buildingid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ResourceBuildingSpeedUpRequest::set_has_buildingid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ResourceBuildingSpeedUpRequest::clear_has_buildingid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ResourceBuildingSpeedUpRequest::clear_buildingid() {
  buildingid_ = 0;
  clear_has_buildingid();
}
inline ::google::protobuf::int32 ResourceBuildingSpeedUpRequest::buildingid() const {
  return buildingid_;
}
inline void ResourceBuildingSpeedUpRequest::set_buildingid(::google::protobuf::int32 value) {
  set_has_buildingid();
  buildingid_ = value;
}

// optional bool useDiamond = 2;
inline bool ResourceBuildingSpeedUpRequest::has_usediamond() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ResourceBuildingSpeedUpRequest::set_has_usediamond() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ResourceBuildingSpeedUpRequest::clear_has_usediamond() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ResourceBuildingSpeedUpRequest::clear_usediamond() {
  usediamond_ = false;
  clear_has_usediamond();
}
inline bool ResourceBuildingSpeedUpRequest::usediamond() const {
  return usediamond_;
}
inline void ResourceBuildingSpeedUpRequest::set_usediamond(bool value) {
  set_has_usediamond();
  usediamond_ = value;
}

// optional int32 itemId = 3;
inline bool ResourceBuildingSpeedUpRequest::has_itemid() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void ResourceBuildingSpeedUpRequest::set_has_itemid() {
  _has_bits_[0] |= 0x00000004u;
}
inline void ResourceBuildingSpeedUpRequest::clear_has_itemid() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void ResourceBuildingSpeedUpRequest::clear_itemid() {
  itemid_ = 0;
  clear_has_itemid();
}
inline ::google::protobuf::int32 ResourceBuildingSpeedUpRequest::itemid() const {
  return itemid_;
}
inline void ResourceBuildingSpeedUpRequest::set_itemid(::google::protobuf::int32 value) {
  set_has_itemid();
  itemid_ = value;
}

// -------------------------------------------------------------------

// ResourceBuildingSpeedUpResponse

// required int32 buildingId = 1;
inline bool ResourceBuildingSpeedUpResponse::has_buildingid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ResourceBuildingSpeedUpResponse::set_has_buildingid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ResourceBuildingSpeedUpResponse::clear_has_buildingid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ResourceBuildingSpeedUpResponse::clear_buildingid() {
  buildingid_ = 0;
  clear_has_buildingid();
}
inline ::google::protobuf::int32 ResourceBuildingSpeedUpResponse::buildingid() const {
  return buildingid_;
}
inline void ResourceBuildingSpeedUpResponse::set_buildingid(::google::protobuf::int32 value) {
  set_has_buildingid();
  buildingid_ = value;
}

// -------------------------------------------------------------------

// ResourceBuildingListPush

// repeated .slg.protocol.ResourceBuildingDTO resourceBuildings = 1;
inline int ResourceBuildingListPush::resourcebuildings_size() const {
  return resourcebuildings_.size();
}
inline void ResourceBuildingListPush::clear_resourcebuildings() {
  resourcebuildings_.Clear();
}
inline const ::slg::protocol::ResourceBuildingDTO& ResourceBuildingListPush::resourcebuildings(int index) const {
  return resourcebuildings_.Get(index);
}
inline ::slg::protocol::ResourceBuildingDTO* ResourceBuildingListPush::mutable_resourcebuildings(int index) {
  return resourcebuildings_.Mutable(index);
}
inline ::slg::protocol::ResourceBuildingDTO* ResourceBuildingListPush::add_resourcebuildings() {
  return resourcebuildings_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::slg::protocol::ResourceBuildingDTO >&
ResourceBuildingListPush::resourcebuildings() const {
  return resourcebuildings_;
}
inline ::google::protobuf::RepeatedPtrField< ::slg::protocol::ResourceBuildingDTO >*
ResourceBuildingListPush::mutable_resourcebuildings() {
  return &resourcebuildings_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace protocol
}  // namespace slg

#ifndef SWIG
namespace google {
namespace protobuf {

template <>
inline const EnumDescriptor* GetEnumDescriptor< ::slg::protocol::ResourceBuildingCommandId>() {
  return ::slg::protocol::ResourceBuildingCommandId_descriptor();
}
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::slg::protocol::ResourceBuildingPushId>() {
  return ::slg::protocol::ResourceBuildingPushId_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_resource_5fbuilding_2eproto__INCLUDED