/**
 * @file n2np/node.h
 * @brief N2NP node implementation
 */

#ifndef EPYX_N2NP_NODE_H
#define EPYX_N2NP_NODE_H

#include "packet.h"
#include "module.h"
#include "../net/netselectsocket.h"
#include "../parser/gttparser.h"
#include "../core/atom/counter.h"
#include "../core/atom/map.h"
#include "../core/timeout.h"

namespace Epyx
{
    namespace N2NP
    {
        class Module; //It does exist, it tell you.

        class Node;
        /**
         * @class Node
         * @brief N2NP node
         */
        class Node : public NetSelectSocket
        {
        public:
            /**
             * @brief Create a new node
             * @param addr Relay Address (IP:port)
             */
            Node(const Address& addr);

            /**
             * @brief Send an N2NP packet to another node
             * @param to destination Node ID
             * @param method
             * @param data
             * @param size
             * @return true on success
             */
            bool send(const NodeId& to, const std::string& method,
                    const char *data, unsigned long size, bool store = true);

            /**
             * @brief Send an ancapsulated GTT packet packet to another node
             *
             * First build the GTT packet, then add N2NP headers, and send the
             * whole through network
             *
             * @param to destination Node ID
             * @param method
             * @param pkt holy packet
             * @return true on success
             */
            bool send(const NodeId& to, const std::string& method,
                    const GTTPacket& pkt);

            /**
             * @brief Register a new Module
             * @param method Which method to use
             * @param m The module to call
             */
            void addModule(const std::string& method, Module *m);

            /**
             * @brief Offer a new direct connection to use
             * @param recipient The other end of the link, as a nodeId
             * @param socket The socket to be given
             */
            void offerDirectConn(const NodeId& recipient, Socket *socket);

            /**
             * @brief Get node ID
             * @return this->nodeid
             */
            const NodeId& getId() const;

            /**
             * @brief Test if node is ready
             * @return true if it is
             */
            bool isReady() const;

            /**
             * @brief Wait for a node to be ready
             * @param timeout
             * @return true is the node is ready, false is timeout has expired
             */
            bool waitReady(const Timeout& timeout) const;

            /**
             * @brief Get the node address, from the point of view of the relay
             * @return nodeAddressFromRelay
             */
            const Address& getNodeAddress() const;

        protected:
            /**
             * @brief Eat data and treat each N2NP packet it contains
             */
            void eat(const char *data, long size);

        private:
            /**
             * @brief Treat a N2NP packet
             */
            void treat(Packet *pkt);
            void sendAck(Packet *pkt);
            void sendErr(Packet *pkt);

            // This ID
            NodeId nodeid;
            bool hasId;

            // Callbacks for Recv
            atom::Map<std::string, Module*> modules;

            // GTT parser
            GTTParser gttparser;

            // Most recently used node IDs.
            // Each node has a number which corresponds to a weight.
            atom::Map<NodeId, unsigned int> mruNodeIds;

            // Disable copy
            Node(const Node&);
            const Node& operator=(const Node&);

            atom::Counter curId;
            atom::Map<unsigned long, Packet*> sentMap;

            // Stockage for sockets
            atom::Map<NodeId, Socket*> directSockets;

            // Node address, from the relay point of view
            Address nodeAddressFromRelay;
        };
    }
}

#endif /* EPYX_N2NP_NODE_H */

