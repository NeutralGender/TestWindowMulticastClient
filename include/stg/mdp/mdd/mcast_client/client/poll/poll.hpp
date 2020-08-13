#pragma once

#include <iostream>
#include <cstddef>
#include <vector>
#include <array>
#include <exception>
#include <tuple>

#include <winsock2.h>

#define POLL_COUNT 1

namespace stg::mdp::mdd::mcast_client::client::poll
{

class WSApoll
{
public:
    WSApoll() {}
    ~WSApoll() { std::cout << "~Destructor" << std::endl; }

    void add_fd(std::size_t feedA, std::size_t feedB);
    void add_fd(std::size_t feedA);

    std::size_t wait();
protected:

private:
    std::array<pollfd, POLL_COUNT> wsa_poll;
};

void WSApoll::add_fd(std::size_t feedA, std::size_t feedB)
{
    //std::cout << "feedA: " << feedA << std::endl;
    wsa_poll[0].fd = feedA;
    //std::cout << "wsa_poll0 fd: " << wsa_poll[0].fd << std::endl;
    wsa_poll[0].events = POLLIN;

    //std::cout << "feedB: " << feedB << std::endl;
    wsa_poll[1].fd = feedB;
    //std::cout << "wsa_poll1 fd: " << wsa_poll[1].fd << std::endl;
    wsa_poll[1].events = POLLIN;
}

void WSApoll::add_fd(std::size_t feedA)
{
    //std::cout << "feedA: " << feedA << std::endl;
    wsa_poll[0].fd = feedA;
    wsa_poll[0].events = POLLIN;
}

std::size_t WSApoll::wait()
{
    int poll_cnt = WSAPoll((LPWSAPOLLFD)&wsa_poll, POLL_COUNT, -1);

    if( poll_cnt > 0 )
    {
        for( auto& fd : wsa_poll )
        {
            if(fd.revents & POLLIN) // Accept data
                return (fd.fd);
        }
    }
}

}