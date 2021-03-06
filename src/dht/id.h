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
 * @file dht/id.h
 * @brief DHT Id representing a DHT key and positions in the hash space
 */
#ifndef EPYX_DHT_ID_H
#define EPYX_DHT_ID_H

#include <iostream>
#include <string>
#include <stdint.h>
#include "../crypto/sha2.h"

namespace Epyx
{
namespace DHT
{

    class Distance;

    /**
     * @class Id
     * @brief represents positions in the DHT hash space
     */
    class Id
    {
    public:
        static const int LENGTH = 256;
        static const int STORAGE_SIZE = 256/8;
        uint8_t data[STORAGE_SIZE];

        // Initialisation method
        enum Initialisation {
            INIT_ZERO,
            INIT_RANDOM
        };

        /**
         * @brief build a zeroed ID
         */
        Id();

        Id(Initialisation init);

        // Copy
        Id(const Id& id);
        Id& operator=(const Id& id);

        /**
         * @brief Randomize ID data
         */
        void randomize();

        /**
         * @brief Reset an ID to zero
         */
        void reset();
    };


    std::ostream& operator<<(std::ostream& os, const Id& id);
    std::istream& operator>>(std::istream& in, Id& id);

    void idForString(Id& id, const std::string& toCipher);

    /**
     * @class Distance
     * @brief a distance is just an Id but we want different types to add methods
     */
    class Distance: public Id
    {
    public:
        /**
         * @brief the constructor of a Distance from 2 Ids
         * @param a one of the Ids
         * @param b the other
         */
        Distance(const Id* a, const Id* b);
        /**
         * @brief the constructor of a Distance from 2 Ids
         * @param a one of the Ids
         * @param b the other
         */
        Distance(const Id& a, const Id& b);

        /**
         * @brief returns the index first non null bit
         * @return log2(distance) kind of
         */
        int firstActiveBit() const;

    private:
        int firstActive;

        void initSelf(const Id* a, const Id* b);
    };

    std::ostream& operator<<(std::ostream& os, const Distance& d);
    bool operator<(const Id &a, const Id &b);
    bool operator==(const Id &a, const Id &b);
}
}

#endif /* EPYX_DHT_ID_H */
