#pragma once

#define _WINSOCKAPI_

#include <stg/mdp/mdd/net/multicast/mcast_socket.hpp>
#include <stg/mdp/mdd/net/multicast/types.hpp>

#include <iostream>
#include <string.h>
#include <exception>

//#pragma comment (lib,"Ws2_32.lib")
#include <winsock2.h>
#include <winsock.h>
#include <Ws2tcpip.h>
#include <mswsock.h>

/* WINSOCK
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
*/

namespace stg::mdp::mdd::net::multicast
{

#define LOG_TAG "[stg.ai.test.xring] "

template< mcast_type MCast_Provider = mcast_type::ANY >
class mcast_receiver:
  public mcast_socket
{
public:
    mcast_receiver(const std::size_t port):
                         mcast_socket(port)
    {}

    ~mcast_receiver() 
    {}


    virtual void init_socket(const std::string& group_addr)// override
    {
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = INADDR_ANY;
    }

    [[nodiscard]]
    virtual bool socket_settings() override
    { return ( set_reuse_addr() && bind_socket() ) == false ?  false : true; }
    
    int receive(std::string& receive_data)
    {
      int read_count = 0;

      receive_data.reserve( 1500 );
      if( ( read_count = recv( sockfd, receive_data.data(), 1500, max_mtu ) ) < 0 ) 
      {
          std::cout << "multicast_socket receive: read_count return -1: " << GetLastError() << std::endl;
          return ( -1 );
      }

      receive_data.shrink_to_fit();

      return ( read_count );
    }

    /**
     * @brief join ( *, g ) group
     * 
     * @param local_addr local IPv4 address of the inter on which the multicast group should be joined
     * @param group_addr IPv4 address of multicast group
     */
    bool join_group( const std::string& local_addr, const std::string& group_addr );

    /**
     * @brief join ( s, g ) group
     * 
     * @param source_ip IPv4 address of multicast source
     * @param group_ip IPv4 address of multicast group
     * @param inter local IPv4 address of the inter on which the multicast group should be joined
     */
    bool join_group(const std::string& source_ip, 
                    const std::string& group_ip, 
                    const std::string& inter);

private:
    struct sockaddr_in local; // local inter
    struct ip_mreq group; // kernel join multicast
    constexpr static std::size_t max_mtu = 1500;

private:
    [[nodiscard]] bool bind_socket()
    {
        if( ( bind(sockfd, (struct sockaddr*)&local, sizeof(local) ) )  < 0 )
        {
            std::cout << "Cannot Bind Socket: " << GetLastError() << std::endl;
            closesocket(sockfd);
            return false;
        }

        return true;
    }

    [[nodiscard]] bool set_reuse_addr()
    {
        int reuse = 1;
        if( ( setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)(&reuse), sizeof(reuse) )  ) < 0 )
        {
            std::cout << "Cannot set REUSEADDR setsockopt(): " << GetLastError() << std::endl;
            closesocket(sockfd);
            return false;
        }

        return true;
    }

};

template<>
[[nodiscard]] bool mcast_receiver<mcast_type::ANY>::join_group(const std::string& local_addr, 
                                                 const std::string& group_addr )
{
    group.imr_multiaddr.s_addr = inet_addr(group_addr.c_str());
    group.imr_interface.s_addr = inet_addr(local_addr.c_str());

    if( ( setsockopt( sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
                      (char*)(&group), sizeof(group) ) ) < 0 )
    {
        std::cout << "join_group setsockopt(): " << GetLastError() << std::endl;
        closesocket(sockfd);
        return false;
    }

    return true;
}

template<>
[[nodiscard]] bool mcast_receiver<mcast_type::SSM>::join_group(const std::string& source_ip, 
                                                 const std::string& group_ip, 
                                                 const std::string& local )
{
    struct ip_mreq_source group_source_req; 

    memset( &group_source_req, 0, sizeof( group_source_req ) );

    group_source_req.imr_multiaddr.s_addr = inet_addr(group_ip.c_str());
    group_source_req.imr_sourceaddr.s_addr = inet_addr(source_ip.c_str());
    group_source_req.imr_interface.s_addr = inet_addr(local.c_str());

    /* Join GROUP( S, G ) */
    if( setsockopt( sockfd, IPPROTO_IP, IP_ADD_SOURCE_MEMBERSHIP, 
                    (char*)(&group_source_req), sizeof(group_source_req) ) < 0 )
    {
        std::cout << "join_group setsockopt(): " << GetLastError() << std::endl;
        closesocket(sockfd);
        return false;
    }

    return true;
}


} // namespace stg::mdp::mdd::net::multicast