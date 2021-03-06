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
 * @file dht/kbucket.h
 * @brief defines the KBucket : the DHT routing table
 */

#ifndef EPYX_DHT_KBUCKET_H
#define EPYX_DHT_KBUCKET_H

#include <list>
#include <vector>
#include <map>
#include <mutex>
#include "../n2np/nodeid.h"
#include "id.h"
#include "peer.h"
#include "../core/common.h"

namespace Epyx
{
namespace DHT
{
    class Bucket;

    /**
     * @class KBucket
     * @brief A KBucket is a kind of routing table for the DHT
     */
    class KBucket
    {
    public:
        /**
         * @brief Constructs the KBucket given the node's Id
         * @param self the Id of our node
         */
        KBucket(const Id& self);

        /**
         * @brief tells the KBucket we've seen someone (to update the routing table)
         * @param peerId the DHT Id of this peer
         * @param n2npId the N2NP Id of this peer
         */
        Peer::SPtr seenPeer(const Id& peerId, const N2NP::NodeId& n2npId);

        /**
         * @brief finds the nodes closest to an Id
         * @param id the Id we wat to getto
         * @param nearest where the results will be stored (maybe make it a result instead of an argument)
         * @param n the number of peers we want to get
         */
        void findNearestNodes(const Id& id, std::vector<Peer::SPtr> &nearest, int n);

        static const unsigned int NODES_PER_BUCKET = 20;
        static const unsigned int MAX_INACTIVE_PERIOD = 15000;

    private:
        std::mutex lock;
        std::vector<Bucket> buckets;
        Id ownId;
    };

    //Used for the internal representation of the KBucket
    class Bucket
    {
        public:
            std::list<Peer::SPtr> peers;
    };
}
}

#endif /* EPYX_DHT_KBUCKET_H */
