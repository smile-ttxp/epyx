#include "static-actors.h"
#include "node.h"
#include "internal-node.h"
#include "finder-actor.h"
#include <algorithm>
#include "consts.h"

namespace Epyx
{
namespace DHT
{

    void GetCallback::onError() {
    }

    GetCallback::~GetCallback() {
    }

    GetterSearchCallback::GetterSearchCallback(ActorId<GetterActor> parent)
    :parent(parent) {
    }

    void GetterSearchCallback::onFound(ClosestQueue& result) {
        std::vector<Peer::SPtr>* res = new std::vector<Peer::SPtr>();
        ClosestQueue::iterator it;
        for(it = result.begin(); it != result.end(); it ++)  {
            res->push_back((*it).second);
        }
        parent.post(EPYX_AQ("find success"), res);
    }

    void GetterSearchCallback::onError() {
        parent.post(EPYX_AQ("find failure"));
    }

    GetterActor::GetterActor(InternalNode& n, const std::string& key, GetCallback* cb)
    :ProcessActor(n), callback(cb), key(key), found(false) {
    }

    void GetterActor::start() {
        Id id;
        idForString(id, key);
        //Start the FIND process
        n.findClosest(new GetterSearchCallback(Actor::getId(this)), GET_REDUNDANCY, id);
    }

    void GetterActor::treat(EPYX_AQA("find success"), std::vector<Peer::SPtr>* peers) {
        for(auto it = peers->begin(); it != peers->end(); it ++) {
            ask(**it);
        }
        pendingRequests = peers->size();

        found = true;
        delete peers;
    }

    void GetterActor::treat(EPYX_AQA("find failure")) {
        timeout();
    }

    void GetterActor::onNewAnswer(Peer* peer, Packet* pkt) {
        if (pkt->method == M_GOT && pkt->status == 0) {
            callback->onGot(pkt->value);
            delete callback;
            kill();
        } else {
            onGetResponse();
        }

        delete peer;
        delete pkt;
    }

    void GetterActor::onAnswerTimeout(long id) {
        //Acknowledge that we received the result of a query
        onGetResponse();
    }

    void GetterActor::onGetResponse() {
        pendingRequests --;
        if(pendingRequests == 0) {
            timeout();
        }
    }

    void GetterActor::ask(Peer& p) {
        Packet pkt;
        pkt.method = M_GET;
        pkt.key = key;

        //HACK!!!
        Peer::SPtr peer(new Peer(p));

        this->sendQuery(peer, pkt, SINGLE_REQUEST_TIMEOUT);
    }

    void GetterActor::timeout() {
        callback->onError();
        delete callback;
        kill();
    }
}
}
