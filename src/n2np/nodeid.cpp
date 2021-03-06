#include "nodeid.h"
#include "../core/common.h"

namespace Epyx
{
    namespace N2NP
    {

        NodeId::NodeId()
        :name(), relay() {
        }

        NodeId::NodeId(const SockAddress& relay)
        :name(), relay(relay) {
        }

        NodeId::NodeId(const std::string& name, const SockAddress& relay)
        :name(name), relay(relay) {
        }

        NodeId::NodeId(const std::string& str) {
            size_t pos = str.find('@');
            if (pos == std::string::npos) {
                log::error << "N2NP: Arobas is missing in " << str << log::endl;
                throw ParserException("N2np::NodeId", "No @ in ID");
            }
            name = str.substr(0, pos);
            relay = SockAddress(str.substr(pos + 1));
        }

        std::ostream& operator<<(std::ostream& os, const NodeId& nodeid) {
            return os << nodeid.name << '@' << nodeid.relay;
        }

        std::string NodeId::toString() const {
            std::ostringstream stream;
            stream << *this;
            return stream.str();
        }

        int NodeId::compare(const NodeId& nodeid) const {
            int c = relay.compare(nodeid.relay);
            if (c != 0) return c;
            return name.compare(nodeid.name);
        }

        bool operator==(const NodeId& nodeid1, const NodeId& nodeid2) {
            return nodeid1.compare(nodeid2) == 0;
        }

        bool operator!=(const NodeId& nodeid1, const NodeId& nodeid2) {
            return nodeid1.compare(nodeid2) != 0;
        }

        bool operator<(const NodeId& nodeid1, const NodeId& nodeid2) {
            return nodeid1.compare(nodeid2) < 0;
        }

        const std::string& NodeId::getName() const {
            return name;
        }

        const SockAddress& NodeId::getRelay() const {
            return relay;
        }

        bool NodeId::empty() const {
            return name.empty() && relay.empty();
        }
    }
}
