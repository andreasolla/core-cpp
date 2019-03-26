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

class ISource;

typedef struct _ISource__isset {
  _ISource__isset() : name(false), bytes(false), _args(true) {}
  bool name :1;
  bool bytes :1;
  bool _args :1;
} _ISource__isset;

class ISource : public virtual ::apache::thrift::TBase {
 public:

  ISource(const ISource&);
  ISource& operator=(const ISource&);
  ISource() : name(), bytes() {

  }

  virtual ~ISource() throw();
  std::string name;
  std::string bytes;
  std::map<std::string, std::string>  _args;

  _ISource__isset __isset;

  void __set_name(const std::string& val);

  void __set_bytes(const std::string& val);

  void __set__args(const std::map<std::string, std::string> & val);

  bool operator == (const ISource & rhs) const
  {
    if (!(name == rhs.name))
      return false;
    if (!(bytes == rhs.bytes))
      return false;
    if (!(_args == rhs._args))
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
