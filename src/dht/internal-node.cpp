#include "internal-node.h"
#include "node.h"

namespace Epyx
{
namespace DHT
{

    InternalNode::InternalNode(const Id& id, Node& parent, const std::string& name)
    :actors(5, name + "Actors"), id(id), parent(parent), kbucket(id) {
        pingActor = actors.add(new PingActor(*this));
        getActor = actors.add(new GetActor(*this));
        storeActor = actors.add(new StoreActor(*this));
        findActor = actors.add(new FindActor(*this));
    }

    InternalNode::~InternalNode() {
    }

    void InternalNode::processPacket(Packet& pkt, Target& target) {

        this->kbucket.seenPeer(target.id, target.n2npId);

        switch(pkt.method){
            case M_PING:
                this->pingActor.post(*(new StaticActorData(target, pkt)));
                break;

            case M_PONG:
                delete &pkt;
                delete &target;
                break;

            case M_GET:
                this->getActor.post(*(new StaticActorData(target, pkt)));
                break;

            case M_STORE:
                this->storeActor.post(*(new StaticActorData(target, pkt)));
                break;

            case M_FIND:
                this->findActor.post(*(new StaticActorData(target, pkt)));
                break;

            case M_GOT:
            case M_STORED:
            case M_FOUND:
                this->dispatchToProcessActor(pkt, target);
                break;

            default:
                log::error << "The DHT received a packet with an unknown method" << log::endl;
                delete &pkt;
                delete &target;
                break;
        }
    }

    void InternalNode::send(Packet& pkt, const Target& target) {
        this->parent.send(pkt, target);
    }

    void InternalNode::sendPing(Target& t){
        Packet pkt;
        pkt.method = M_PING;
        send(pkt, t);
        //delete t ?
    }

    void InternalNode::sendPong(Target& target) {
        Packet pkt;
        pkt.method = M_PONG;
        send(pkt, target);
        delete &target;
    }

    long InternalNode::registerProcessActor(Actor<ProcessActorData>& actor, int timeout) {
        //TODO delete all this when the node is destroyed
        ActorId<ProcessActorData>* a;
        if(timeout > 0){
            a = new ActorId<ProcessActorData>(actors.add(actor, timeout));
        }else{
            a = new ActorId<ProcessActorData>(actors.add(actor));
        }
        long n = processActorsCount.getIncrement();
        processActors.set(n, a);
        return n;
    }

    void InternalNode::dispatchToProcessActor(Packet& pkt, Target& target){
        ActorId<ProcessActorData>* id = processActors.getAndLock(pkt.connectionId, NULL);
        if(id == NULL){
            processActors.endUnlock();
            return;
        }
        ActorId<ProcessActorData> saved_id(*id);
        processActors.endUnlock();

        saved_id.post(*(new ProcessActorData(target, pkt)));
    }

    void InternalNode::unregisterProcessActor(long actorNumber){
        ActorId<ProcessActorData>* id = processActors.getAndLock(actorNumber, NULL);
        if(id == NULL){
            processActors.endUnlock();
            return;
        }
        ActorId<ProcessActorData> saved_id(*id);
        delete id;
        processActors.endUnlock();

        saved_id.kill();
        processActors.unset(actorNumber);
    }

}
}