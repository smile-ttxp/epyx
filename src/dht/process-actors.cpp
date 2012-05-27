#include "process-actors.h"
#include "internal-node.h"
#include "finder-actor.h"

namespace Epyx
{
namespace DHT
{

    ProcessActorData::ProcessActorData(Target& target, Packet* pkt)
    :target(target), pkt(pkt) {
    }

    ProcessActorData::ProcessActorData(Target& target, Packet& pkt)
    :target(target), pkt(&pkt) {
    }

    void ProcessActorData::freeData() {
        delete &target;
        delete pkt;
    }

    ProcessActor::ProcessActor(InternalNode& n, int timeout)
    :n(n) {
        processId = n.registerProcessActor(*this, timeout);
    }

    void ProcessActor::destroy() {
        n.unregisterProcessActor(processId);
    }

    SingularFindActor::SingularFindActor(InternalNode& n, ActorId<FinderActorData> p, Target& t, Id& requested)
    :ProcessActor(n, SINGLE_REQUEST_TIMEOUT), target(t), parent(p){
        Packet pkt;
        pkt.method = M_FIND;
        pkt.connectionId = processId;
        pkt.count = 5; //TODO make it a cst
        pkt.idToFind = requested;

        this->n.send(pkt, t);
    }

    void SingularFindActor::treat(ProcessActorData& msg) {
        if(msg.pkt->method == M_FOUND && msg.pkt->status == 0 && msg.pkt->count > 0) {
            parent.post(*(new FinderActorData(*(new Target(target)), msg.pkt->foundPeers, true)));
            msg.pkt->foundPeers = NULL;
            kill();
        } else {
            timeout();
        }
    }

    void SingularFindActor::timeout() {
        parent.post(*(new FinderActorData(*(new Target(target)), NULL, false)));
        kill();
    }
}
}