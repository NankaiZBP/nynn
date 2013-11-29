/**
 * Autogenerated by Thrift Compiler (0.9.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef DataXceiver_H
#define DataXceiver_H

#include <thrift/TDispatchProcessor.h>
#include "nynn_mm_types.h"

namespace nynn { namespace mm {

class DataXceiverIf {
 public:
  virtual ~DataXceiverIf() {}
  virtual void fetch(std::vector<std::vector<int8_t> > & _return, const int64_t vtxno, const int32_t blkno) = 0;
  virtual bool post(const std::vector<std::vector<int8_t> > & data) = 0;
};

class DataXceiverIfFactory {
 public:
  typedef DataXceiverIf Handler;

  virtual ~DataXceiverIfFactory() {}

  virtual DataXceiverIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(DataXceiverIf* /* handler */) = 0;
};

class DataXceiverIfSingletonFactory : virtual public DataXceiverIfFactory {
 public:
  DataXceiverIfSingletonFactory(const boost::shared_ptr<DataXceiverIf>& iface) : iface_(iface) {}
  virtual ~DataXceiverIfSingletonFactory() {}

  virtual DataXceiverIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler(DataXceiverIf* /* handler */) {}

 protected:
  boost::shared_ptr<DataXceiverIf> iface_;
};

class DataXceiverNull : virtual public DataXceiverIf {
 public:
  virtual ~DataXceiverNull() {}
  void fetch(std::vector<std::vector<int8_t> > & /* _return */, const int64_t /* vtxno */, const int32_t /* blkno */) {
    return;
  }
  bool post(const std::vector<std::vector<int8_t> > & /* data */) {
    bool _return = false;
    return _return;
  }
};

typedef struct _DataXceiver_fetch_args__isset {
  _DataXceiver_fetch_args__isset() : vtxno(false), blkno(false) {}
  bool vtxno;
  bool blkno;
} _DataXceiver_fetch_args__isset;

class DataXceiver_fetch_args {
 public:

  DataXceiver_fetch_args() : vtxno(0), blkno(0) {
  }

  virtual ~DataXceiver_fetch_args() throw() {}

  int64_t vtxno;
  int32_t blkno;

  _DataXceiver_fetch_args__isset __isset;

  void __set_vtxno(const int64_t val) {
    vtxno = val;
  }

  void __set_blkno(const int32_t val) {
    blkno = val;
  }

  bool operator == (const DataXceiver_fetch_args & rhs) const
  {
    if (!(vtxno == rhs.vtxno))
      return false;
    if (!(blkno == rhs.blkno))
      return false;
    return true;
  }
  bool operator != (const DataXceiver_fetch_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const DataXceiver_fetch_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class DataXceiver_fetch_pargs {
 public:


  virtual ~DataXceiver_fetch_pargs() throw() {}

  const int64_t* vtxno;
  const int32_t* blkno;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _DataXceiver_fetch_result__isset {
  _DataXceiver_fetch_result__isset() : success(false) {}
  bool success;
} _DataXceiver_fetch_result__isset;

class DataXceiver_fetch_result {
 public:

  DataXceiver_fetch_result() {
  }

  virtual ~DataXceiver_fetch_result() throw() {}

  std::vector<std::vector<int8_t> >  success;

  _DataXceiver_fetch_result__isset __isset;

  void __set_success(const std::vector<std::vector<int8_t> > & val) {
    success = val;
  }

  bool operator == (const DataXceiver_fetch_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const DataXceiver_fetch_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const DataXceiver_fetch_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _DataXceiver_fetch_presult__isset {
  _DataXceiver_fetch_presult__isset() : success(false) {}
  bool success;
} _DataXceiver_fetch_presult__isset;

class DataXceiver_fetch_presult {
 public:


  virtual ~DataXceiver_fetch_presult() throw() {}

  std::vector<std::vector<int8_t> > * success;

  _DataXceiver_fetch_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _DataXceiver_post_args__isset {
  _DataXceiver_post_args__isset() : data(false) {}
  bool data;
} _DataXceiver_post_args__isset;

class DataXceiver_post_args {
 public:

  DataXceiver_post_args() {
  }

  virtual ~DataXceiver_post_args() throw() {}

  std::vector<std::vector<int8_t> >  data;

  _DataXceiver_post_args__isset __isset;

  void __set_data(const std::vector<std::vector<int8_t> > & val) {
    data = val;
  }

  bool operator == (const DataXceiver_post_args & rhs) const
  {
    if (!(data == rhs.data))
      return false;
    return true;
  }
  bool operator != (const DataXceiver_post_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const DataXceiver_post_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class DataXceiver_post_pargs {
 public:


  virtual ~DataXceiver_post_pargs() throw() {}

  const std::vector<std::vector<int8_t> > * data;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _DataXceiver_post_result__isset {
  _DataXceiver_post_result__isset() : success(false) {}
  bool success;
} _DataXceiver_post_result__isset;

class DataXceiver_post_result {
 public:

  DataXceiver_post_result() : success(0) {
  }

  virtual ~DataXceiver_post_result() throw() {}

  bool success;

  _DataXceiver_post_result__isset __isset;

  void __set_success(const bool val) {
    success = val;
  }

  bool operator == (const DataXceiver_post_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const DataXceiver_post_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const DataXceiver_post_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _DataXceiver_post_presult__isset {
  _DataXceiver_post_presult__isset() : success(false) {}
  bool success;
} _DataXceiver_post_presult__isset;

class DataXceiver_post_presult {
 public:


  virtual ~DataXceiver_post_presult() throw() {}

  bool* success;

  _DataXceiver_post_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

class DataXceiverClient : virtual public DataXceiverIf {
 public:
  DataXceiverClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) :
    piprot_(prot),
    poprot_(prot) {
    iprot_ = prot.get();
    oprot_ = prot.get();
  }
  DataXceiverClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) :
    piprot_(iprot),
    poprot_(oprot) {
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void fetch(std::vector<std::vector<int8_t> > & _return, const int64_t vtxno, const int32_t blkno);
  void send_fetch(const int64_t vtxno, const int32_t blkno);
  void recv_fetch(std::vector<std::vector<int8_t> > & _return);
  bool post(const std::vector<std::vector<int8_t> > & data);
  void send_post(const std::vector<std::vector<int8_t> > & data);
  bool recv_post();
 protected:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class DataXceiverProcessor : public ::apache::thrift::TDispatchProcessor {
 protected:
  boost::shared_ptr<DataXceiverIf> iface_;
  virtual bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext);
 private:
  typedef  void (DataXceiverProcessor::*ProcessFunction)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  typedef std::map<std::string, ProcessFunction> ProcessMap;
  ProcessMap processMap_;
  void process_fetch(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_post(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  DataXceiverProcessor(boost::shared_ptr<DataXceiverIf> iface) :
    iface_(iface) {
    processMap_["fetch"] = &DataXceiverProcessor::process_fetch;
    processMap_["post"] = &DataXceiverProcessor::process_post;
  }

  virtual ~DataXceiverProcessor() {}
};

class DataXceiverProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  DataXceiverProcessorFactory(const ::boost::shared_ptr< DataXceiverIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::boost::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::boost::shared_ptr< DataXceiverIfFactory > handlerFactory_;
};

class DataXceiverMultiface : virtual public DataXceiverIf {
 public:
  DataXceiverMultiface(std::vector<boost::shared_ptr<DataXceiverIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~DataXceiverMultiface() {}
 protected:
  std::vector<boost::shared_ptr<DataXceiverIf> > ifaces_;
  DataXceiverMultiface() {}
  void add(boost::shared_ptr<DataXceiverIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  void fetch(std::vector<std::vector<int8_t> > & _return, const int64_t vtxno, const int32_t blkno) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->fetch(_return, vtxno, blkno);
    }
    ifaces_[i]->fetch(_return, vtxno, blkno);
    return;
  }

  bool post(const std::vector<std::vector<int8_t> > & data) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->post(data);
    }
    return ifaces_[i]->post(data);
  }

};

}} // namespace

#endif