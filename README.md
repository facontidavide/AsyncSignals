# AsyncSignals

Proof of concept of signals/slots pushed on a separate event loop.
This code was written for "didactic" purposes and you don't want to use it in production.

The idea is to use the __boost::signals2__ but to delegate the execution of the callbacks (slots) to a separate thread, 
not the callback that invoke the signal handler. 

To do this, we use boost::asio:io_service.
