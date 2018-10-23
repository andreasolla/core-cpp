/**
 * Autogenerated by Thrift Compiler (0.11.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "IKeysModule_types.h"

#include <algorithm>
#include <ostream>

#include <thrift/TToString.h>

namespace ignis { namespace rpc { namespace executor {


IExecutorKeys::~IExecutorKeys() throw() {
}


void IExecutorKeys::__set_msg_id(const int64_t val) {
  this->msg_id = val;
}

void IExecutorKeys::__set_addr(const std::string& val) {
  this->addr = val;
}

void IExecutorKeys::__set_keys(const std::vector<int64_t> & val) {
  this->keys = val;
}
std::ostream& operator<<(std::ostream& out, const IExecutorKeys& obj)
{
  obj.printTo(out);
  return out;
}


uint32_t IExecutorKeys::read(::apache::thrift::protocol::TProtocol* iprot) {

  ::apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;

  bool isset_msg_id = false;
  bool isset_addr = false;
  bool isset_keys = false;

  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_I64) {
          xfer += iprot->readI64(this->msg_id);
          isset_msg_id = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->addr);
          isset_addr = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_LIST) {
          {
            this->keys.clear();
            uint32_t _size0;
            ::apache::thrift::protocol::TType _etype3;
            xfer += iprot->readListBegin(_etype3, _size0);
            this->keys.resize(_size0);
            uint32_t _i4;
            for (_i4 = 0; _i4 < _size0; ++_i4)
            {
              xfer += iprot->readI64(this->keys[_i4]);
            }
            xfer += iprot->readListEnd();
          }
          isset_keys = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  if (!isset_msg_id)
    throw TProtocolException(TProtocolException::INVALID_DATA);
  if (!isset_addr)
    throw TProtocolException(TProtocolException::INVALID_DATA);
  if (!isset_keys)
    throw TProtocolException(TProtocolException::INVALID_DATA);
  return xfer;
}

uint32_t IExecutorKeys::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  ::apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);
  xfer += oprot->writeStructBegin("IExecutorKeys");

  xfer += oprot->writeFieldBegin("msg_id", ::apache::thrift::protocol::T_I64, 1);
  xfer += oprot->writeI64(this->msg_id);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("addr", ::apache::thrift::protocol::T_STRING, 2);
  xfer += oprot->writeString(this->addr);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("keys", ::apache::thrift::protocol::T_LIST, 3);
  {
    xfer += oprot->writeListBegin(::apache::thrift::protocol::T_I64, static_cast<uint32_t>(this->keys.size()));
    std::vector<int64_t> ::const_iterator _iter5;
    for (_iter5 = this->keys.begin(); _iter5 != this->keys.end(); ++_iter5)
    {
      xfer += oprot->writeI64((*_iter5));
    }
    xfer += oprot->writeListEnd();
  }
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(IExecutorKeys &a, IExecutorKeys &b) {
  using ::std::swap;
  swap(a.msg_id, b.msg_id);
  swap(a.addr, b.addr);
  swap(a.keys, b.keys);
}

IExecutorKeys::IExecutorKeys(const IExecutorKeys& other6) {
  msg_id = other6.msg_id;
  addr = other6.addr;
  keys = other6.keys;
}
IExecutorKeys& IExecutorKeys::operator=(const IExecutorKeys& other7) {
  msg_id = other7.msg_id;
  addr = other7.addr;
  keys = other7.keys;
  return *this;
}
void IExecutorKeys::printTo(std::ostream& out) const {
  using ::apache::thrift::to_string;
  out << "IExecutorKeys(";
  out << "msg_id=" << to_string(msg_id);
  out << ", " << "addr=" << to_string(addr);
  out << ", " << "keys=" << to_string(keys);
  out << ")";
}

}}} // namespace
