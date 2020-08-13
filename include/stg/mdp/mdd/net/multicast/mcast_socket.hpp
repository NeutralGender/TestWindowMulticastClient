#pragma once

#include <string>

/* WINSOCK
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
*/

#include <stg/mdp/mdd/net/socket.hpp>

namespace stg::mdp::mdd::net::multicast
{

class mcast_socket:
      public socket_t
{
public:
    mcast_socket(const std::size_t port_):
                               port(port_) 
    {}

    virtual ~mcast_socket() 
    {}

    virtual bool socket_settings() = 0;

protected:
    const std::size_t port;
};

} // namespace stg::mdp::mdd::net::multicast
