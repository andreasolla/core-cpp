/**
 * Autogenerated by Thrift Compiler (0.11.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "ISortModule.h"

namespace ignis { namespace rpc { namespace executor {


ISortModule_localCustomSort_args::~ISortModule_localCustomSort_args() throw() {
}


uint32_t ISortModule_localCustomSort_args::read(::apache::thrift::protocol::TProtocol* iprot) {

  ::apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRUCT) {
          xfer += this->funct.read(iprot);
          this->__isset.funct = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_BOOL) {
          xfer += iprot->readBool(this->ascending);
          this->__isset.ascending = true;
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

  return xfer;
}

uint32_t ISortModule_localCustomSort_args::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  ::apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);
  xfer += oprot->writeStructBegin("ISortModule_localCustomSort_args");

  xfer += oprot->writeFieldBegin("funct", ::apache::thrift::protocol::T_STRUCT, 1);
  xfer += this->funct.write(oprot);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("ascending", ::apache::thrift::protocol::T_BOOL, 2);
  xfer += oprot->writeBool(this->ascending);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}


ISortModule_localCustomSort_pargs::~ISortModule_localCustomSort_pargs() throw() {
}


uint32_t ISortModule_localCustomSort_pargs::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  ::apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);
  xfer += oprot->writeStructBegin("ISortModule_localCustomSort_pargs");

  xfer += oprot->writeFieldBegin("funct", ::apache::thrift::protocol::T_STRUCT, 1);
  xfer += (*(this->funct)).write(oprot);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("ascending", ::apache::thrift::protocol::T_BOOL, 2);
  xfer += oprot->writeBool((*(this->ascending)));
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}


ISortModule_localCustomSort_result::~ISortModule_localCustomSort_result() throw() {
}


uint32_t ISortModule_localCustomSort_result::read(::apache::thrift::protocol::TProtocol* iprot) {

  ::apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRUCT) {
          xfer += this->ex.read(iprot);
          this->__isset.ex = true;
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

  return xfer;
}

uint32_t ISortModule_localCustomSort_result::write(::apache::thrift::protocol::TProtocol* oprot) const {

  uint32_t xfer = 0;

  xfer += oprot->writeStructBegin("ISortModule_localCustomSort_result");

  if (this->__isset.ex) {
    xfer += oprot->writeFieldBegin("ex", ::apache::thrift::protocol::T_STRUCT, 1);
    xfer += this->ex.write(oprot);
    xfer += oprot->writeFieldEnd();
  }
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}


ISortModule_localCustomSort_presult::~ISortModule_localCustomSort_presult() throw() {
}


uint32_t ISortModule_localCustomSort_presult::read(::apache::thrift::protocol::TProtocol* iprot) {

  ::apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRUCT) {
          xfer += this->ex.read(iprot);
          this->__isset.ex = true;
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

  return xfer;
}


ISortModule_localSort_args::~ISortModule_localSort_args() throw() {
}


uint32_t ISortModule_localSort_args::read(::apache::thrift::protocol::TProtocol* iprot) {

  ::apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_BOOL) {
          xfer += iprot->readBool(this->ascending);
          this->__isset.ascending = true;
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

  return xfer;
}

uint32_t ISortModule_localSort_args::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  ::apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);
  xfer += oprot->writeStructBegin("ISortModule_localSort_args");

  xfer += oprot->writeFieldBegin("ascending", ::apache::thrift::protocol::T_BOOL, 1);
  xfer += oprot->writeBool(this->ascending);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}


ISortModule_localSort_pargs::~ISortModule_localSort_pargs() throw() {
}


uint32_t ISortModule_localSort_pargs::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  ::apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);
  xfer += oprot->writeStructBegin("ISortModule_localSort_pargs");

  xfer += oprot->writeFieldBegin("ascending", ::apache::thrift::protocol::T_BOOL, 1);
  xfer += oprot->writeBool((*(this->ascending)));
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}


ISortModule_localSort_result::~ISortModule_localSort_result() throw() {
}


uint32_t ISortModule_localSort_result::read(::apache::thrift::protocol::TProtocol* iprot) {

  ::apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRUCT) {
          xfer += this->ex.read(iprot);
          this->__isset.ex = true;
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

  return xfer;
}

uint32_t ISortModule_localSort_result::write(::apache::thrift::protocol::TProtocol* oprot) const {

  uint32_t xfer = 0;

  xfer += oprot->writeStructBegin("ISortModule_localSort_result");

  if (this->__isset.ex) {
    xfer += oprot->writeFieldBegin("ex", ::apache::thrift::protocol::T_STRUCT, 1);
    xfer += this->ex.write(oprot);
    xfer += oprot->writeFieldEnd();
  }
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}


ISortModule_localSort_presult::~ISortModule_localSort_presult() throw() {
}


uint32_t ISortModule_localSort_presult::read(::apache::thrift::protocol::TProtocol* iprot) {

  ::apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRUCT) {
          xfer += this->ex.read(iprot);
          this->__isset.ex = true;
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

  return xfer;
}

void ISortModuleClient::localCustomSort(const  ::ignis::rpc::ISource& funct, const bool ascending)
{
  send_localCustomSort(funct, ascending);
  recv_localCustomSort();
}

void ISortModuleClient::send_localCustomSort(const  ::ignis::rpc::ISource& funct, const bool ascending)
{
  int32_t cseqid = 0;
  oprot_->writeMessageBegin("localCustomSort", ::apache::thrift::protocol::T_CALL, cseqid);

  ISortModule_localCustomSort_pargs args;
  args.funct = &funct;
  args.ascending = &ascending;
  args.write(oprot_);

  oprot_->writeMessageEnd();
  oprot_->getTransport()->writeEnd();
  oprot_->getTransport()->flush();
}

void ISortModuleClient::recv_localCustomSort()
{

  int32_t rseqid = 0;
  std::string fname;
  ::apache::thrift::protocol::TMessageType mtype;

  iprot_->readMessageBegin(fname, mtype, rseqid);
  if (mtype == ::apache::thrift::protocol::T_EXCEPTION) {
    ::apache::thrift::TApplicationException x;
    x.read(iprot_);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
    throw x;
  }
  if (mtype != ::apache::thrift::protocol::T_REPLY) {
    iprot_->skip(::apache::thrift::protocol::T_STRUCT);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
  }
  if (fname.compare("localCustomSort") != 0) {
    iprot_->skip(::apache::thrift::protocol::T_STRUCT);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
  }
  ISortModule_localCustomSort_presult result;
  result.read(iprot_);
  iprot_->readMessageEnd();
  iprot_->getTransport()->readEnd();

  if (result.__isset.ex) {
    throw result.ex;
  }
  return;
}

void ISortModuleClient::localSort(const bool ascending)
{
  send_localSort(ascending);
  recv_localSort();
}

void ISortModuleClient::send_localSort(const bool ascending)
{
  int32_t cseqid = 0;
  oprot_->writeMessageBegin("localSort", ::apache::thrift::protocol::T_CALL, cseqid);

  ISortModule_localSort_pargs args;
  args.ascending = &ascending;
  args.write(oprot_);

  oprot_->writeMessageEnd();
  oprot_->getTransport()->writeEnd();
  oprot_->getTransport()->flush();
}

void ISortModuleClient::recv_localSort()
{

  int32_t rseqid = 0;
  std::string fname;
  ::apache::thrift::protocol::TMessageType mtype;

  iprot_->readMessageBegin(fname, mtype, rseqid);
  if (mtype == ::apache::thrift::protocol::T_EXCEPTION) {
    ::apache::thrift::TApplicationException x;
    x.read(iprot_);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
    throw x;
  }
  if (mtype != ::apache::thrift::protocol::T_REPLY) {
    iprot_->skip(::apache::thrift::protocol::T_STRUCT);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
  }
  if (fname.compare("localSort") != 0) {
    iprot_->skip(::apache::thrift::protocol::T_STRUCT);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
  }
  ISortModule_localSort_presult result;
  result.read(iprot_);
  iprot_->readMessageEnd();
  iprot_->getTransport()->readEnd();

  if (result.__isset.ex) {
    throw result.ex;
  }
  return;
}

bool ISortModuleProcessor::dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext) {
  ProcessMap::iterator pfn;
  pfn = processMap_.find(fname);
  if (pfn == processMap_.end()) {
    iprot->skip(::apache::thrift::protocol::T_STRUCT);
    iprot->readMessageEnd();
    iprot->getTransport()->readEnd();
    ::apache::thrift::TApplicationException x(::apache::thrift::TApplicationException::UNKNOWN_METHOD, "Invalid method name: '"+fname+"'");
    oprot->writeMessageBegin(fname, ::apache::thrift::protocol::T_EXCEPTION, seqid);
    x.write(oprot);
    oprot->writeMessageEnd();
    oprot->getTransport()->writeEnd();
    oprot->getTransport()->flush();
    return true;
  }
  (this->*(pfn->second))(seqid, iprot, oprot, callContext);
  return true;
}

void ISortModuleProcessor::process_localCustomSort(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext)
{
  void* ctx = NULL;
  if (this->eventHandler_.get() != NULL) {
    ctx = this->eventHandler_->getContext("ISortModule.localCustomSort", callContext);
  }
  ::apache::thrift::TProcessorContextFreer freer(this->eventHandler_.get(), ctx, "ISortModule.localCustomSort");

  if (this->eventHandler_.get() != NULL) {
    this->eventHandler_->preRead(ctx, "ISortModule.localCustomSort");
  }

  ISortModule_localCustomSort_args args;
  args.read(iprot);
  iprot->readMessageEnd();
  uint32_t bytes = iprot->getTransport()->readEnd();

  if (this->eventHandler_.get() != NULL) {
    this->eventHandler_->postRead(ctx, "ISortModule.localCustomSort", bytes);
  }

  ISortModule_localCustomSort_result result;
  try {
    iface_->localCustomSort(args.funct, args.ascending);
  } catch ( ::ignis::rpc::IRemoteException &ex) {
    result.ex = ex;
    result.__isset.ex = true;
  } catch (const std::exception& e) {
    if (this->eventHandler_.get() != NULL) {
      this->eventHandler_->handlerError(ctx, "ISortModule.localCustomSort");
    }

    ::apache::thrift::TApplicationException x(e.what());
    oprot->writeMessageBegin("localCustomSort", ::apache::thrift::protocol::T_EXCEPTION, seqid);
    x.write(oprot);
    oprot->writeMessageEnd();
    oprot->getTransport()->writeEnd();
    oprot->getTransport()->flush();
    return;
  }

  if (this->eventHandler_.get() != NULL) {
    this->eventHandler_->preWrite(ctx, "ISortModule.localCustomSort");
  }

  oprot->writeMessageBegin("localCustomSort", ::apache::thrift::protocol::T_REPLY, seqid);
  result.write(oprot);
  oprot->writeMessageEnd();
  bytes = oprot->getTransport()->writeEnd();
  oprot->getTransport()->flush();

  if (this->eventHandler_.get() != NULL) {
    this->eventHandler_->postWrite(ctx, "ISortModule.localCustomSort", bytes);
  }
}

void ISortModuleProcessor::process_localSort(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext)
{
  void* ctx = NULL;
  if (this->eventHandler_.get() != NULL) {
    ctx = this->eventHandler_->getContext("ISortModule.localSort", callContext);
  }
  ::apache::thrift::TProcessorContextFreer freer(this->eventHandler_.get(), ctx, "ISortModule.localSort");

  if (this->eventHandler_.get() != NULL) {
    this->eventHandler_->preRead(ctx, "ISortModule.localSort");
  }

  ISortModule_localSort_args args;
  args.read(iprot);
  iprot->readMessageEnd();
  uint32_t bytes = iprot->getTransport()->readEnd();

  if (this->eventHandler_.get() != NULL) {
    this->eventHandler_->postRead(ctx, "ISortModule.localSort", bytes);
  }

  ISortModule_localSort_result result;
  try {
    iface_->localSort(args.ascending);
  } catch ( ::ignis::rpc::IRemoteException &ex) {
    result.ex = ex;
    result.__isset.ex = true;
  } catch (const std::exception& e) {
    if (this->eventHandler_.get() != NULL) {
      this->eventHandler_->handlerError(ctx, "ISortModule.localSort");
    }

    ::apache::thrift::TApplicationException x(e.what());
    oprot->writeMessageBegin("localSort", ::apache::thrift::protocol::T_EXCEPTION, seqid);
    x.write(oprot);
    oprot->writeMessageEnd();
    oprot->getTransport()->writeEnd();
    oprot->getTransport()->flush();
    return;
  }

  if (this->eventHandler_.get() != NULL) {
    this->eventHandler_->preWrite(ctx, "ISortModule.localSort");
  }

  oprot->writeMessageBegin("localSort", ::apache::thrift::protocol::T_REPLY, seqid);
  result.write(oprot);
  oprot->writeMessageEnd();
  bytes = oprot->getTransport()->writeEnd();
  oprot->getTransport()->flush();

  if (this->eventHandler_.get() != NULL) {
    this->eventHandler_->postWrite(ctx, "ISortModule.localSort", bytes);
  }
}

::apache::thrift::stdcxx::shared_ptr< ::apache::thrift::TProcessor > ISortModuleProcessorFactory::getProcessor(const ::apache::thrift::TConnectionInfo& connInfo) {
  ::apache::thrift::ReleaseHandler< ISortModuleIfFactory > cleanup(handlerFactory_);
  ::apache::thrift::stdcxx::shared_ptr< ISortModuleIf > handler(handlerFactory_->getHandler(connInfo), cleanup);
  ::apache::thrift::stdcxx::shared_ptr< ::apache::thrift::TProcessor > processor(new ISortModuleProcessor(handler));
  return processor;
}

void ISortModuleConcurrentClient::localCustomSort(const  ::ignis::rpc::ISource& funct, const bool ascending)
{
  int32_t seqid = send_localCustomSort(funct, ascending);
  recv_localCustomSort(seqid);
}

int32_t ISortModuleConcurrentClient::send_localCustomSort(const  ::ignis::rpc::ISource& funct, const bool ascending)
{
  int32_t cseqid = this->sync_.generateSeqId();
  ::apache::thrift::async::TConcurrentSendSentry sentry(&this->sync_);
  oprot_->writeMessageBegin("localCustomSort", ::apache::thrift::protocol::T_CALL, cseqid);

  ISortModule_localCustomSort_pargs args;
  args.funct = &funct;
  args.ascending = &ascending;
  args.write(oprot_);

  oprot_->writeMessageEnd();
  oprot_->getTransport()->writeEnd();
  oprot_->getTransport()->flush();

  sentry.commit();
  return cseqid;
}

void ISortModuleConcurrentClient::recv_localCustomSort(const int32_t seqid)
{

  int32_t rseqid = 0;
  std::string fname;
  ::apache::thrift::protocol::TMessageType mtype;

  // the read mutex gets dropped and reacquired as part of waitForWork()
  // The destructor of this sentry wakes up other clients
  ::apache::thrift::async::TConcurrentRecvSentry sentry(&this->sync_, seqid);

  while(true) {
    if(!this->sync_.getPending(fname, mtype, rseqid)) {
      iprot_->readMessageBegin(fname, mtype, rseqid);
    }
    if(seqid == rseqid) {
      if (mtype == ::apache::thrift::protocol::T_EXCEPTION) {
        ::apache::thrift::TApplicationException x;
        x.read(iprot_);
        iprot_->readMessageEnd();
        iprot_->getTransport()->readEnd();
        sentry.commit();
        throw x;
      }
      if (mtype != ::apache::thrift::protocol::T_REPLY) {
        iprot_->skip(::apache::thrift::protocol::T_STRUCT);
        iprot_->readMessageEnd();
        iprot_->getTransport()->readEnd();
      }
      if (fname.compare("localCustomSort") != 0) {
        iprot_->skip(::apache::thrift::protocol::T_STRUCT);
        iprot_->readMessageEnd();
        iprot_->getTransport()->readEnd();

        // in a bad state, don't commit
        using ::apache::thrift::protocol::TProtocolException;
        throw TProtocolException(TProtocolException::INVALID_DATA);
      }
      ISortModule_localCustomSort_presult result;
      result.read(iprot_);
      iprot_->readMessageEnd();
      iprot_->getTransport()->readEnd();

      if (result.__isset.ex) {
        sentry.commit();
        throw result.ex;
      }
      sentry.commit();
      return;
    }
    // seqid != rseqid
    this->sync_.updatePending(fname, mtype, rseqid);

    // this will temporarily unlock the readMutex, and let other clients get work done
    this->sync_.waitForWork(seqid);
  } // end while(true)
}

void ISortModuleConcurrentClient::localSort(const bool ascending)
{
  int32_t seqid = send_localSort(ascending);
  recv_localSort(seqid);
}

int32_t ISortModuleConcurrentClient::send_localSort(const bool ascending)
{
  int32_t cseqid = this->sync_.generateSeqId();
  ::apache::thrift::async::TConcurrentSendSentry sentry(&this->sync_);
  oprot_->writeMessageBegin("localSort", ::apache::thrift::protocol::T_CALL, cseqid);

  ISortModule_localSort_pargs args;
  args.ascending = &ascending;
  args.write(oprot_);

  oprot_->writeMessageEnd();
  oprot_->getTransport()->writeEnd();
  oprot_->getTransport()->flush();

  sentry.commit();
  return cseqid;
}

void ISortModuleConcurrentClient::recv_localSort(const int32_t seqid)
{

  int32_t rseqid = 0;
  std::string fname;
  ::apache::thrift::protocol::TMessageType mtype;

  // the read mutex gets dropped and reacquired as part of waitForWork()
  // The destructor of this sentry wakes up other clients
  ::apache::thrift::async::TConcurrentRecvSentry sentry(&this->sync_, seqid);

  while(true) {
    if(!this->sync_.getPending(fname, mtype, rseqid)) {
      iprot_->readMessageBegin(fname, mtype, rseqid);
    }
    if(seqid == rseqid) {
      if (mtype == ::apache::thrift::protocol::T_EXCEPTION) {
        ::apache::thrift::TApplicationException x;
        x.read(iprot_);
        iprot_->readMessageEnd();
        iprot_->getTransport()->readEnd();
        sentry.commit();
        throw x;
      }
      if (mtype != ::apache::thrift::protocol::T_REPLY) {
        iprot_->skip(::apache::thrift::protocol::T_STRUCT);
        iprot_->readMessageEnd();
        iprot_->getTransport()->readEnd();
      }
      if (fname.compare("localSort") != 0) {
        iprot_->skip(::apache::thrift::protocol::T_STRUCT);
        iprot_->readMessageEnd();
        iprot_->getTransport()->readEnd();

        // in a bad state, don't commit
        using ::apache::thrift::protocol::TProtocolException;
        throw TProtocolException(TProtocolException::INVALID_DATA);
      }
      ISortModule_localSort_presult result;
      result.read(iprot_);
      iprot_->readMessageEnd();
      iprot_->getTransport()->readEnd();

      if (result.__isset.ex) {
        sentry.commit();
        throw result.ex;
      }
      sentry.commit();
      return;
    }
    // seqid != rseqid
    this->sync_.updatePending(fname, mtype, rseqid);

    // this will temporarily unlock the readMutex, and let other clients get work done
    this->sync_.waitForWork(seqid);
  } // end while(true)
}

}}} // namespace

