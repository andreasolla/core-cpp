/**
 * Autogenerated by Thrift Compiler (0.14.1)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef IBackendService_H
#define IBackendService_H

#include <thrift/TDispatchProcessor.h>
#include <thrift/async/TConcurrentClientSyncInfo.h>
#include <memory>
#include "IBackendService_types.h"

namespace ignis { namespace rpc { namespace driver {

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning (disable : 4250 ) //inheriting methods via dominance 
#endif

class IBackendServiceIf {
 public:
  virtual ~IBackendServiceIf() {}
  virtual void stop() = 0;
};

class IBackendServiceIfFactory {
 public:
  typedef IBackendServiceIf Handler;

  virtual ~IBackendServiceIfFactory() {}

  virtual IBackendServiceIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(IBackendServiceIf* /* handler */) = 0;
};

class IBackendServiceIfSingletonFactory : virtual public IBackendServiceIfFactory {
 public:
  IBackendServiceIfSingletonFactory(const ::std::shared_ptr<IBackendServiceIf>& iface) : iface_(iface) {}
  virtual ~IBackendServiceIfSingletonFactory() {}

  virtual IBackendServiceIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler(IBackendServiceIf* /* handler */) {}

 protected:
  ::std::shared_ptr<IBackendServiceIf> iface_;
};

class IBackendServiceNull : virtual public IBackendServiceIf {
 public:
  virtual ~IBackendServiceNull() {}
  void stop() {
    return;
  }
};


class IBackendService_stop_args {
 public:

  IBackendService_stop_args(const IBackendService_stop_args&);
  IBackendService_stop_args& operator=(const IBackendService_stop_args&);
  IBackendService_stop_args() {
  }

  virtual ~IBackendService_stop_args() noexcept;

  bool operator == (const IBackendService_stop_args & /* rhs */) const
  {
    return true;
  }
  bool operator != (const IBackendService_stop_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const IBackendService_stop_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class IBackendService_stop_pargs {
 public:


  virtual ~IBackendService_stop_pargs() noexcept;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

class IBackendServiceClient : virtual public IBackendServiceIf {
 public:
  IBackendServiceClient(std::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
    setProtocol(prot);
  }
  IBackendServiceClient(std::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, std::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    setProtocol(iprot,oprot);
  }
 private:
  void setProtocol(std::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
  setProtocol(prot,prot);
  }
  void setProtocol(std::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, std::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    piprot_=iprot;
    poprot_=oprot;
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
 public:
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void stop();
  void send_stop();
 protected:
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class IBackendServiceProcessor : public ::apache::thrift::TDispatchProcessor {
 protected:
  ::std::shared_ptr<IBackendServiceIf> iface_;
  virtual bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext);
 private:
  typedef  void (IBackendServiceProcessor::*ProcessFunction)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  typedef std::map<std::string, ProcessFunction> ProcessMap;
  ProcessMap processMap_;
  void process_stop(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  IBackendServiceProcessor(::std::shared_ptr<IBackendServiceIf> iface) :
    iface_(iface) {
    processMap_["stop"] = &IBackendServiceProcessor::process_stop;
  }

  virtual ~IBackendServiceProcessor() {}
};

class IBackendServiceProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  IBackendServiceProcessorFactory(const ::std::shared_ptr< IBackendServiceIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::std::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::std::shared_ptr< IBackendServiceIfFactory > handlerFactory_;
};

class IBackendServiceMultiface : virtual public IBackendServiceIf {
 public:
  IBackendServiceMultiface(std::vector<std::shared_ptr<IBackendServiceIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~IBackendServiceMultiface() {}
 protected:
  std::vector<std::shared_ptr<IBackendServiceIf> > ifaces_;
  IBackendServiceMultiface() {}
  void add(::std::shared_ptr<IBackendServiceIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  void stop() {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->stop();
    }
    ifaces_[i]->stop();
  }

};

// The 'concurrent' client is a thread safe client that correctly handles
// out of order responses.  It is slower than the regular client, so should
// only be used when you need to share a connection among multiple threads
class IBackendServiceConcurrentClient : virtual public IBackendServiceIf {
 public:
  IBackendServiceConcurrentClient(std::shared_ptr< ::apache::thrift::protocol::TProtocol> prot, std::shared_ptr<::apache::thrift::async::TConcurrentClientSyncInfo> sync) : sync_(sync)
{
    setProtocol(prot);
  }
  IBackendServiceConcurrentClient(std::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, std::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot, std::shared_ptr<::apache::thrift::async::TConcurrentClientSyncInfo> sync) : sync_(sync)
{
    setProtocol(iprot,oprot);
  }
 private:
  void setProtocol(std::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
  setProtocol(prot,prot);
  }
  void setProtocol(std::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, std::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    piprot_=iprot;
    poprot_=oprot;
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
 public:
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void stop();
  void send_stop();
 protected:
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
  std::shared_ptr<::apache::thrift::async::TConcurrentClientSyncInfo> sync_;
};

#ifdef _MSC_VER
  #pragma warning( pop )
#endif

}}} // namespace

#endif
