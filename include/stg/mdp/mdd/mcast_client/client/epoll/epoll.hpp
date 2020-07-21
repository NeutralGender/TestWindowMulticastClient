#pragma once

#include <iostream>
#include <cstddef>
#include <vector>
#include <exception>
#include <tuple>

#include <sys/epoll.h>
#include <unistd.h>


namespace stg::mdp::mdd::mcast_client::client::epoll
{

class epoll_t
{
public:
    epoll_t(std::size_t queue_len):
            queue_len(queue_len),
            events(queue_len)
    {}

    ~epoll_t()
    { close(epfd); }

    void create();
    void add_fd(std::size_t sockfd);
    void remove_fd(std::size_t sockfd, struct epoll_event& event);

    std::size_t get_fd()
    { return epfd; }

    /**
     * @brief Waiting for events on the file descriptor
     * 
     * @param timeout waits for timeout ms, 0 - return immediately; -1 - will mot return until an event is available
     * @return std::tuple<std::size_t, epoll_event*> 
     */
    std::tuple<std::size_t, epoll_event*> wait( int timeout );

protected:

private:
    void init_event(const int socket, struct epoll_event& event);
    void ctl_interface(std::size_t sockfd, struct epoll_event& event);

    int epfd = -1;
    const std::size_t queue_len;
    std::vector< epoll_event > events;
};

void epoll_t::create()
{
    try
    {
        if( ( epfd = epoll_create(queue_len) ) < 0 )
            throw std::exception();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

}

void epoll_t::add_fd(std::size_t sockfd)
{
    static struct epoll_event event {0};

    try
    {
        init_event(sockfd, event);
        ctl_interface(sockfd, event);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

}

void epoll_t::init_event( const int socket, struct epoll_event& event)
{
    event.events = EPOLLIN;// | EPOLLERR;
    event.data.fd = socket;
}

void epoll_t::ctl_interface(std::size_t sockfd, struct epoll_event& event)
{
    if( epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event) < 0 ) 
        throw std::exception();
}

void epoll_t::remove_fd(std::size_t sockfd, struct epoll_event& event)
{
    try
    {
        if( epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, &event) < 0 )
            throw std::exception();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

std::tuple<std::size_t, epoll_event*> epoll_t::wait( int timeout )
{
    int nfds = 0;

    try
    {
        if( ( nfds = epoll_wait(epfd, &*events.begin(), events.size(), timeout ) ) < 0 )
            throw std::runtime_error("wait error");
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << '\n';
    }

    return ( std::make_tuple(nfds, &*events.begin()) );
}

}