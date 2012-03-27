/**
 * @file test-n2np.cpp
 * @brief This program tests Node to Node Protocol
 */

#include <iostream>
#include <sstream>
#include <string.h>
#include "core/common.h"
#include "localn2np/local-node.h"

/**
 * @brief Command-line interface to send packets
 */
void test_command(Epyx::LocalNode& node, const Epyx::Address& addr) {
    unsigned int id;
    std::string msg;
    Epyx::N2npPacketType type("test");

    // Waiting for other threads
    usleep(10);

    Epyx::log::debug << "[Cli:motd] Command-line interface" << Epyx::log::endl;
    Epyx::log::debug << "[Cli:motd] You are on " << node << Epyx::log::endl;
    Epyx::log::debug << "[Cli:motd] 2 Hello sends `Hello' to node 2" << Epyx::log::endl;
    Epyx::log::debug << "[Cli:motd] 0 quits." << Epyx::log::endl;

    while (true) {
        std::cin >> id;
        if (id <= 0) {
            Epyx::log::debug << "[Cli] Bye :)" << Epyx::log::endl;
            return;
        } else {
            std::cin >> msg;
            Epyx::log::debug << "[Cli] Sending " << msg << " to " << id << Epyx::log::endl;

            // Convert id to a node name
            std::ostringstream idStream;
            idStream << id;
            Epyx::N2npNodeId nodeTo(idStream.str().c_str(), addr);

            // Build a new packet
            Epyx::N2npPacket pkt(type, msg.size() + 1, msg.c_str());

            // Send !
            node.send(nodeTo, pkt);
        }
    }
}

/**
 * @biref Receive callback for every node
 */
bool nodeRecv(Epyx::LocalNode& node, const Epyx::N2npPacket& pkt, void* arg_) {
    Epyx::N2npPacketType pongType("pong");
    Epyx::log::debug << "[Node " << node << "] Recv from " << pkt.from << ": `"
        << pkt.data << "'" << Epyx::log::endl;
    // Send a pong with the same data
    // .. or not
    const char *data = pkt.data;
    int size = pkt.size;

    if (!strcasecmp(pkt.data, "o<"))
        data = "PAN !";
    else if (!strcasecmp(pkt.data, "Question?"))
        data = "The answer is 42.";
    if (data != pkt.data)
        size = strlen(data) + 1;

    Epyx::N2npPacket pongPkt(pongType, size, data);
    node.send(pkt.from, pongPkt);
    return true;
}

/**
 * @brief Receive callback for Pong messages
 */
bool nodeRecvPong(Epyx::LocalNode& node, const Epyx::N2npPacket& pkt, void* arg_) {
    Epyx::log::debug << "[Node " << node << "] Pong from " << pkt.from << ": `"
        << pkt.data << "'" << Epyx::log::endl;
    return true;
}

/**
 * @brief Test N2NP implementation
 */
void test_n2np() {
    const int nodeNum = 42;
    Epyx::LocalNode *nodes[nodeNum];
    Epyx::LocalRelay *relay = NULL;
    try {
        Epyx::Address addr("L0C4L", 0, 0);
        Epyx::N2npPacketType type("test");
        Epyx::N2npPacketType pongType("pong");

        // Create a relay
        relay = new Epyx::LocalRelay(addr);
        Epyx::log::debug << "Created relay " << *relay << Epyx::log::endl;

        // Create nodes
        for (int i = 0; i < nodeNum; i++) {
            std::ostringstream strid;
            strid << "node-" << (i + 1);
            nodes[i] = new Epyx::LocalNode(strid.str());
            nodes[i]->registerRecv(type, nodeRecv, NULL);
            nodes[i]->registerRecv(pongType, nodeRecvPong, NULL);
            nodes[i]->attach(relay);
        }
        Epyx::log::debug << "Created nodes 1.." << nodeNum << Epyx::log::endl;
        test_command(*(nodes[0]), addr);
    } catch (Epyx::Exception e) {
        Epyx::log::fatal << e << Epyx::log::endl;
    }
    if (relay) {
        relay->stop();
        delete relay;
    }
    for (int i = 0; i < nodeNum; i++) {
        if (nodes[i]) {
            nodes[i]->stop();
            delete nodes[i];
        }
    }
}

/**
 * @brief Setup environment and invoke test_n2np()
 */
int main() {
    try {
        Epyx::Thread::init();
        Epyx::log::init(Epyx::log::CONSOLE, "");
        test_n2np();
        Epyx::log::flushAndQuit();
    } catch (Epyx::Exception e) {
        std::cerr << e << "\n";
    }
    return 0;
}
