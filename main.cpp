#include <iostream>
#include "Dispatcher.h"

using namespace std;

typedef struct{
  float x,y,theta;
}Position2D;

void bumpReceived()
{
   cout << "Bump! From thread " << boost::this_thread::get_id() << endl;
}

void sensorReceived(float value)
{
  cout << "Sensor value " << value << " from thread " << boost::this_thread::get_id() << endl;
}

void positionReceived(const Position2D& pos)
{
  cout << "Robot position " <<  pos.x
       << "/" << pos.y
       << "/" << pos.theta
       << " from thread " << boost::this_thread::get_id() << endl;
}


int main(int argc, char *argv[])
{
  cout << "Hello World! my thread is " << boost::this_thread::get_id() << endl;

  Dispatcher dispatcher;

  auto bump_emitter   = dispatcher.createEmitter<void()>("bump");
  auto sensor_emitter = dispatcher.createEmitter<void(float)>("sensor");
  auto position_emitter = dispatcher.createEmitter<void(const Position2D&)>("position");

  boost::function<void()> bump_functor = boost::bind( &bumpReceived);
  dispatcher.connect( std::string("bump") , bump_functor  );

  boost::function<void(float)> sensor_functor = boost::bind( &sensorReceived, _1);
  dispatcher.connect( std::string("sensor") , sensor_functor );

  boost::function<void(const Position2D&)> position_functor = boost::bind( &positionReceived, _1);
  dispatcher.connect( std::string("position") , position_functor );

  Position2D robot_position = {1,2,3};

  cout << "\n------ Synchronous -------" << endl;
  bump_emitter->invokeSync();
  sensor_emitter->invokeSync(42);
  position_emitter->invokeSync(robot_position);

  cout << "\n------ Asynchronous -------" << endl;
  bump_emitter->invokeSync();
  sensor_emitter->invokeAsync(42);
  position_emitter->invokeAsync(robot_position);


  boost::this_thread::sleep(boost::posix_time::milliseconds(100));

  cout << "\n-------------" << endl;
  cout << "Done from thread is " << boost::this_thread::get_id() << endl;

  return 0;
}
