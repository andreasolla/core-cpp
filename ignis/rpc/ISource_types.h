/**
 * Autogenerated by Thrift Compiler (0.11.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef ISource_TYPES_H
#define ISource_TYPES_H

#include <iosfwd>

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/TBase.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>

#include <thrift/stdcxx.h>


namespace ignis { namespace rpc {

class IEncoded;

class ISource;

typedef struct _IEncoded__isset {
  _IEncoded__isset() : name(false), bytes(false) {}
  bool name :1;
  bool bytes :1;
} _IEncoded__isset;

class IEncoded : public virtual ::apache::thrift::TBase {
 public:

  IEncoded(const IEncoded&);
  IEncoded& operator=(const IEncoded&);
  IEncoded() : name(), bytes() {
  }

  virtual ~IEncoded() throw();
  std::string name;
  std::string bytes;

  _IEncoded__isset __isset;

  void __set_name(const std::string& val);

  void __set_bytes(const std::string& val);

  bool operator == (const IEncoded & rhs) const
  {
    if (__isset.name != rhs.__isset.name)
      return false;
    else if (__isset.name && !(name == rhs.name))
      return false;
    if (__isset.bytes != rhs.__isset.bytes)
      return false;
    else if (__isset.bytes && !(bytes == rhs.bytes))
      return false;
    return true;
  }
  bool operator != (const IEncoded &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const IEncoded & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(IEncoded &a, IEncoded &b);

std::ostream& operator<<(std::ostream& out, const IEncoded& obj);

typedef struct _ISource__isset {
  _ISource__isset() : params(true) {}
  bool params :1;
} _ISource__isset;

class ISource : public virtual ::apache::thrift::TBase {
 public:

  ISource(const ISource&);
  ISource& operator=(const ISource&);
  ISource() {

  }

  virtual ~ISource() throw();
  IEncoded obj;
  std::map<std::string, std::string>  params;

  _ISource__isset __isset;

  void __set_obj(const IEncoded& val);

  void __set_params(const std::map<std::string, std::string> & val);

  bool operator == (const ISource & rhs) const
  {
    if (!(obj == rhs.obj))
      return false;
    if (!(params == rhs.params))
      return false;
    return true;
  }
  bool operator != (const ISource &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ISource & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(ISource &a, ISource &b);

std::ostream& operator<<(std::ostream& out, const ISource& obj);

}} // namespace

#endif
