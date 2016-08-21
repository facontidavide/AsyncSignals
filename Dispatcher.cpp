#include "Dispatcher.h"

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>


Dispatcher::Dispatcher():
  strand_(io_),
  work_(io_),
  thread_( boost::bind(&boost::asio::io_service::run, &io_) )
{

}

Dispatcher::~Dispatcher() {
  io_.stop();
  thread_.join();
}
