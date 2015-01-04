#ifndef ZMQ_REACTOR_H
#define ZMQ_REACTOR_H

#include "zmq.hpp" // from module cppzmq
#include "foo/zmq_util.h" // ZmqRouterRequest
#include <string>
#include <thread>
#include <memory>

class ZmqReactor;

// ZmqReactorRequest hides from the Reactor client the fact if you can send
// a message directly to the tcpSOcket or if you have to go thru the 
// inprocSocket (decicion depends on sending thread).
class ZmqReactorRequest {
    public:

        std::shared_ptr<zmq::message_t> getPayload() const {
            return request.getPayload();
        }
        
        // you may call this method from any thread, the response will be
        // routed to the original sender of the request.
        void sendResponse(const zmq::message_t& response) const;
    
    private: 
        // c'tor should only be used by ZmqReactor
        // @param request is the original client's request the message handler
        //        may or may not send a response to.
        // @param inprocSocketAddr is the relaying inproc socket if the message
        //        handler happens to want to send a response from a worker thread.
        ZmqReactorRequest(
            ZmqRouterRequest&& request, 
            zmq::context_t& context,
            zmq::socket_t& tcpSocket,
            const std::string& inprocSocketAddr);
        friend ZmqReactor;
        
        ZmqRouterRequest request;
        zmq::context_t& context;
        zmq::socket_t& tcpSocket;
        std::string inprocSocketAddr;
        std::thread::id tcpSocketThreadId;
};

// listens to a (usually tcp) zmq socket and calls the given onMessage
// handler for each incoming message.
class ZmqReactor {
    public:
        // @param tcpSocket must be a bound ZMQ_ROUTER (typically bound to a
        //        TCP addr)
        // @param onMessage is a func that may send 0..n responses to the 
        //        request. This func must not do any sync IO, since otherwise
        //        the reactor won't scale: instead it should spawn a 
        //        workerthread for slow IO, or better use async IO and send
        //        responses as futures resove.
        //        onMessage returns true if the message implies the server 
        //        should quit.
        ZmqReactor(
            zmq::context_t& context,
            zmq::socket_t& tcpSocket,
            const std::function<bool(const ZmqReactorRequest& req)>& onMessage);
        void run();
    private:
        zmq::context_t& context;
        zmq::socket_t& tcpSocket;
        const std::function<bool(const ZmqReactorRequest& req)>& onMessage;
};

#endif