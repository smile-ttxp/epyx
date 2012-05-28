#include "listener.h"

namespace Epyx {
    namespace DirectConnection {
        
        Listener::Listener(TCPServer *srv)
            :srv(srv)
        {
        }
        void Listener::run(){
            struct sockaddr_storage clientAddr;
            socklen_t clientAddrLen;
            struct sockaddr_storage localAddr;
            socklen_t localAddrLen;
            
            clientAddrLen = sizeof clientAddr;
            localAddrLen = sizeof localAddr;
            
            int newfd = srv->getFd();
            
            ::accept(newfd,(struct sockaddr*) &clientAddr, &clientAddrLen);
            sock = new TCPSocket(newfd, Address((struct sockaddr*) &clientAddr));
            ::getsockname(newfd,(struct sockaddr*) &localAddr,&localAddrLen);
            sock->setLocalAddress(Address((struct sockaddr*) &localAddr));
        }
        Address Listener::getAddress(){
            if (sock != null)
                return sock->getAddress();
            else
                return Address();
        }
        Address Listener::getLocalAddress(){
            struct sockaddr_storage localAddr;
            socklen_t localAddrLen;
            localAddrLen = sizeof localAddr;
            
            ::getsockname(newfd,(struct sockaddr*) &localAddr,&localAddrLen);
            return Address ( (struct sockaddr*) &localAddr);
        }
        TCPSocket * Listener::getSocket(){
            return sock;
        }
    } // namespace DirectConnection
} // namespace Epyx
