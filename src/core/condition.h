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
 * @file condition.h
 * @brief Thread condition management astraction layer
 *
 * It is not exposing the full pthread capabilities on purpose to make it
 * simpler even if it might create small overheads
 */
#ifndef EPYX_CONDITION_H
#define EPYX_CONDITION_H

#include <boost/noncopyable.hpp>
#include <pthread.h>
#include "mutex.h"

namespace Epyx
{
    /**
     * @class Condition
     *
     * @brief a condition synchronization primitive
     *
     * A higher level condition primitive: threads can wait on the condition
     * until they are notified. A condition is always associated with a Mutex
     * and closely associated with it: the calls to lock/unlock cannot be put
     * in the Condition as you want to use it to access to shared variables
     * when notified and exactly when you are notified. For those not convinced,
     * try to see how you could make a blocking queue without this mechanism.
     */
    class Condition : private boost::noncopyable
    {
    public:

        /**
         * @brief The Condition constructor
         */
        Condition();

        /**
         * @brief The Condition destructor
         */
        ~Condition();

        /**
         * @brief Locks the Mutex associated with the Condition
         */
        void lock();

        /**
         * @brief Tries to lock the Mutex associated with the Condition
         * @return true if it locked, false otherwise
         */
        bool tryLock();

        /**
         * @brief Unlocks the Mutex associated with the Condition
         */
        void unlock();


        /**
         * @brief The call blocks until the thread is notified.
         *
         * Before calling wait you must call lock() on the condition, after the call
         * the current thread will hold the lock so you must unlock() it.
         */
        void wait();

        /**
         * @brief The call blocks until the thread is notified or the time is elapsed
         *
         * Before calling timedWait you must call lock() on the condition, after the call
         * the current thread will hold the lock so you must unlock() it.
         *
         * @param seconds the number of seconds to wait
         * @param nanoseconds the number of nanoseconds to wait
         * @return true if the wait timed out, false otherwise
         */
        bool timedWait(int seconds, int nanoseconds);

        /**
         * @brief The call blocks until the thread is notified or the time is elapsed
         *
         * Before calling timedWait you must call lock() on the condition, after the call
         * the current thread will hold the lock so you must unlock() it.
         *
         * @param ms the number of milliseconds to wait
         * @return true if the wait timed out, false otherwise
         */
        bool timedWait(int ms);

        /**
         * @brief Notify one thread waiting on the queue
         *
         * Before calling notify you must call lock() on the condition and after the call
         * you must unlock() it.
         */
        void notify();

        /**
         * @brief Notify all the threads waiting on the queue
         *
         * Before calling notify you must call lock() on the condition and after the call
         * you must unlock() it.
         */
        void notifyAll();

        /**
         * @brief An accessor for the mutex associated with the condition
         * @return the Mutex associated with the condition
         */
        Mutex& getMutex();

    private:
        Mutex mutex;
        pthread_cond_t cond;
    };
}

#endif /* EPYX_CONDITION_H */
