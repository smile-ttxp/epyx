#include "rootdesc.h"
#include "../../../include/tinyxml/tinyxml.h"
#include "../../net/tcpsocket.h"

namespace Epyx
{
    namespace UPNP
    {

        RootDesc::RootDesc(const URI& uri)
        :NetQuery(new TCPSocket(uri.getAddress())), uri(uri) {
        }

        bool RootDesc::query() {
            std::stringstream message;
            message << "GET " << uri.getPath() << " HTTP/1.1" << String::crlf
                << "Host: " << uri.getAddress() << String::crlf
                << "Connection: Close" << String::crlf
                << "User-Agent: Epyx Natpunching FTW" << String::crlf
                << String::crlf;
            return socket()->write(message.str());
        }

        std::unique_ptr<std::map<std::string, std::string> > RootDesc::eat(const byte_str& data) {
            std::unique_ptr<std::map<std::string, std::string> > services;
            std::string error;

            // Eat data with HTTP parser
            htpars.eat(data);
            std::unique_ptr<GTTPacket> pkt = htpars.getPacket();
            if (!pkt) {
                // Incomplete packet, there may be an error
                if (htpars.getError(error)) {
                    log::error << "HTTP Parser error during rootDesc parsing:\n"
                        << error << log::endl;
                }
                return services;
            }

            // Here, pkt is an HTTP packet.
            services.reset(new std::map<std::string, std::string>);

            TiXmlDocument domRootDesc;
            domRootDesc.Parse((const char*) pkt->body.c_str());
            TiXmlElement *node = domRootDesc.FirstChildElement()->FirstChildElement();
            this->parseRootDescFile(node, services.get());
            return services;
        }

        void RootDesc::parseRootDescFile(const TiXmlElement *node,
            std::map<std::string, std::string> *services) {
            EPYX_ASSERT(services != NULL);
            if (node == NULL)
                return;

            //If true, actualNode->Value() == "service"
            if (strcmp(node->Value(), "service") == 0) {
                const char *type = node->FirstChildElement("serviceType")->GetText();
                const char *ctrlUrl = node->FirstChildElement("controlURL")->GetText();
                (*services)[type] = ctrlUrl;
            } else if (strcmp(node->Value(), "serviceList") == 0) {
                this->parseRootDescFile(node->FirstChildElement(), services);
            } else if (strcmp(node->Value(), "deviceList") == 0) {
                this->parseRootDescFile(node->FirstChildElement(), services);
            } else if (strcmp(node->Value(), "device") == 0) {
                this->parseRootDescFile(node->FirstChildElement(), services);
            }

            if (node->NextSiblingElement() != NULL)
                this->parseRootDescFile(node->NextSiblingElement(), services);
        }
    }
}
