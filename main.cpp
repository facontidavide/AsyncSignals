#include <iostream>
#include "Dispatcher.h"

using namespace std;

void bumpReceived()
{
   cout << "Bump! From thread " << boost::this_thread::get_id() << endl;
}

void sensorReceived(float value)
{
  cout << "value " << value << " from thread " << boost::this_thread::get_id() << endl;
}


int main(int argc, char *argv[])
{
  cout << "Hello World! my thread is " << boost::this_thread::get_id() << endl;

  Dispatcher dispatcher;

  auto bump_emitter   = dispatcher.createEmitter<void()>("bump");
  auto sensor_emitter = dispatcher.createEmitter<void(float)>("sensor");

  boost::function<void()> bump_funtor = boost::bind( &bumpReceived);
  dispatcher.connect( std::string("bump") , bump_funtor  );

  boost::function<void(float)> sensor_funtor = boost::bind( &sensorReceived, _1);
  dispatcher.connect( std::string("sensor") , sensor_funtor );

  cout << "------ Synchronous -------" << endl;
  bump_emitter->invokeSync();
  sensor_emitter->invokeSync(42);

  cout << "------ Asynchronous -------" << endl;
  bump_emitter->invokeSync();
  sensor_emitter->invokeAsync(42);

  boost::this_thread::sleep(boost::posix_time::milliseconds(100));

  cout << "-------------" << endl;
  cout << "Done from thread is " << boost::this_thread::get_id() << endl;

  return 0;
}
