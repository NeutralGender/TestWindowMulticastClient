#pragma once

#include <exception>

#include <winsock2.h>
#include <winsock.h>
/** winsock
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
*/

namespace stg::mdp::mdd::net
{

/**
 * @brief Common interface for unix, multicast, etc sockets
 */
class socket_t
{
public:
    socket_t() 
    {}

    virtual ~socket_t() 
    { closesocket( sockfd ); }

    [[nodiscard]] bool create_socket(int socket_type)
    {
    #define LOG_TAG "[stg.ai.test.xring] "

        if( ( sockfd = socket( AF_INET, socket_type, 0 ) ) < 0 )
        {
            return false;
        }

        return true;

    #undef LOG_TAG
    }
    
    virtual void init_socket() {};

    const int& get_sock()
    { return (sockfd); }
    
protected:
    int sockfd = -1;

};

} // namespace stg::mdp::mdd::net
