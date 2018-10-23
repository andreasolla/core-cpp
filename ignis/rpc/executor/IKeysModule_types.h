/**
 * Autogenerated by Thrift Compiler (0.11.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef IKeysModule_TYPES_H
#define IKeysModule_TYPES_H

#include <iosfwd>

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/TBase.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>

#include <thrift/stdcxx.h>
#include "../IRemoteException_types.h"
#include "../ISource_types.h"

#include <unordered_map>

namespace ignis { namespace rpc { namespace executor {

class IExecutorKeys;


class IExecutorKeys : public virtual ::apache::thrift::TBase {
 public:

  IExecutorKeys(const IExecutorKeys&);
  IExecutorKeys& operator=(const IExecutorKeys&);
  IExecutorKeys() : msg_id(0), addr() {
  }

  virtual ~IExecutorKeys() throw();
  int64_t msg_id;
  std::string addr;
  std::vector<int64_t>  keys;

  void __set_msg_id(const int64_t val);

  void __set_addr(const std::string& val);

  void __set_keys(const std::vector<int64_t> & val);

  bool operator == (const IExecutorKeys & rhs) const
  {
    if (!(msg_id == rhs.msg_id))
      return false;
    if (!(addr == rhs.addr))
      return false;
    if (!(keys == rhs.keys))
      return false;
    return true;
  }
  bool operator != (const IExecutorKeys &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const IExecutorKeys & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(IExecutorKeys &a, IExecutorKeys &b);

std::ostream& operator<<(std::ostream& out, const IExecutorKeys& obj);

}}} // namespace

#endif
