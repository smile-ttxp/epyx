/*
 *   Copyright 2012 Epyx Team
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */
/**
 * @file tcpserver.h
 * @brief TCP server implementation
 */

#ifndef EPYX_TCPSERVER_H
#define EPYX_TCPSERVER_H

#include "server.h"
#include "tcpsocket.h"
#include <memory>

namespace Epyx
{
    /**
     * @class TCPServer
     * @brief TCP Server implementation
     */
    class TCPServer : public Server
    {
    public:
        /**
         * @brief Create a new TCP server
         * @param addr IP address and TCP port to bind to
         * @param nbConn maximum connection number
         */
        TCPServer(const SockAddress& addr, unsigned int nbConn);

        /**
         * @brief Accept a connection
         * @return a shared_ptr to the new client socket
         * 
         * The returned shared_ptr is empty when the server is not binded
         */
        std::unique_ptr<TCPSocket> accept();
    };

}
#endif /* EPYX_TCPSERVER_H */
