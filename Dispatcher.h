#ifndef DISPATCHER_H
#define DISPATCHER_H


#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/signals2.hpp>
#include <boost/asio.hpp>
#include <boost/unordered_map.hpp>
#include <boost/any.hpp>
#include <boost/thread.hpp>
#include <boost/make_shared.hpp>
#include  <stdio.h>

class Dispatcher;

template <typename Signature> class SignalEmitter{

public:

  ~SignalEmitter() {}

  const std::string getName() const;

  // invoke the signal synchronously
  template <class... Args> void invokeSync( Args... args )
  {
    signal_(args...);
  }

  // invoke the signal asynchronously
  template <class... Args> void invokeAsync( Args... args )
  {
    strand_->post( boost::bind( boost::ref(signal_),args... ) );
  }

  void connect(const boost::function<Signature>& callback)
  {
    signal_.connect( callback );
  }

protected:

  friend class Dispatcher;

  SignalEmitter(const std::string& name, boost::asio::strand* strand): strand_(strand){}


  boost::signals2::signal<Signature> signal_;
  boost::asio::strand* strand_;


};


//--------------------------------------------------------------------------------

class Dispatcher : boost::noncopyable
{
public:
  Dispatcher::Dispatcher():  strand_(io_),  work_(io_),
  thread_( boost::bind(&boost::asio::io_service::run, &io_) )
  {  }

  ~Dispatcher() 
  {
    io_.stop();
    thread_.join();
  }


  // FACTORY. Create an emitter object
  template <typename Signature> boost::shared_ptr< SignalEmitter<Signature> >
  createEmitter(const std::string& name)
  {
    auto emitter_ptr = new SignalEmitter<Signature> ( name, &strand_ );
    auto emitter = boost::shared_ptr< SignalEmitter<Signature> >( emitter_ptr );
    signals_[name] = emitter;
    return emitter;
  }

  template <typename Signature> boost::shared_ptr< SignalEmitter<Signature> >
  createEmitter(const char* name)
  {
    return createEmitter<Signature>( std::string(name));
  }

  template <typename Signature> bool connect(const std::string& name, const boost::function<Signature>& callback)
  {
    auto signal = boost::any_cast<boost::shared_ptr< SignalEmitter<Signature> > >( signals_[name] );
    signal->connect( callback );
  }

  // disconnect by name
  template <typename Signature> bool disconnect(const std::string& name, boost::function<Signature> );

private:
  boost::asio::io_service io_;
  boost::asio::strand strand_;
  boost::unordered_map<std::string, boost::any > signals_;
  boost::asio::io_service::work work_;
  boost::thread thread_;
};

inline Dispatcher::Dispatcher():
  strand_(io_),
  work_(io_),
  thread_( boost::bind(&boost::asio::io_service::run, &io_) )
{

}





#endif // DISPATCHER_H
