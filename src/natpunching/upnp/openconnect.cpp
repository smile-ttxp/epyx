#include "openconnect.h"
#include "../../net/udpsocket.h"
#include <sstream>

#define BUF_SIZE 4096

namespace Epyx {
    namespace UPNP {
        Natpunch::Natpunch() {
            this->endl = "\r\n";
        }

        void Natpunch::discover(){
            char data[BUF_SIZE];
            Epyx::UDPSocket sock(true);
            //UDPSocket is preparing to multicast on port 1900 For UPnP discover
            Address multicastAddr("239.255.255.250", 1900);
            sock.setAddress(multicastAddr);
            std::stringstream message;
            message << "M-SEARCH * HTTP/1.1" << endl
                    << "HOST: 239.255.255.250:1900" << endl
                    << "ST: urn:schemas-upnp-org:device:InternetGatewayDevice:1" << endl
                    << "MAN: \"ssdp:discover\"" << endl
                    << "MX: 2" << endl << endl;
            sock.write(message.str());

            sock.recv(data,BUF_SIZE);


        }
    }
}
