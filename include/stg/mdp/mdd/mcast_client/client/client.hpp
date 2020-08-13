#pragma once

#include <vector>
#include <thread>
#include <mutex>

#include <stg/mdp/mdd/mcast_client/client/poll/poll.hpp>
#include <stg/mdp/mdd/mcast_client/client/dissemination/dissemination.hpp>
#include <stg/mdp/mdd/mcast_client/thread/thread_channel.hpp>
#include <stg/mdp/mdd/net/multicast/types.hpp>
#include <stg/mdp/mdd/common/logger/logger.hpp>

namespace stg::mdp::mdd::client
{

using stg::mdp::mdd::mcast_client::client::poll::WSApoll;
using stg::mdp::mdd::mcast_client::thread::thread_channel;
using stg::mdp::mdd::mcast_client::thread::destruct_method;
using stg::mdp::mdd::mcast_client::client::dissemination::dissm_receiver;
using stg::mdp::mdd::net::multicast::mcast_type;
using stg::mdp::mdd::common::logger::logger_t;


class client_t
{
public:
    client_t(const std::size_t addresses):
             epolls_(addresses), 
             theads_(addresses),
             groups(addresses)
    {
        for( std::size_t i = 0; i < epolls_.size(); ++i )
            epolls_[i] = std::make_unique<WSApoll>();
    }

    client_t(const std::size_t cores, 
             const std::size_t threads):
             cores(cores), 
             threads(threads)
    {
        epolls_.resize( cores * threads );
        for( std::size_t i = 0; i < epolls_.size(); ++i )
            epolls_[i] = std::make_unique<WSApoll>();
    }

    ~client_t()
    {}

    template <mcast_type T>
    void add_channels_epoll(dissm_receiver<T>& A, dissm_receiver<T>& B);

    template <mcast_type T>
    void add_channels_epoll(dissm_receiver<T>& A);

    static void wait(std::unique_ptr<WSApoll>& epoll_, std::mutex& mtx, std::size_t& count);

    void create_thread(std::size_t& count);
    void create_thread1(const std::size_t core, const std::size_t thread, std::size_t& count);

protected:

private:
    template <mcast_type T>
    void add_channel(dissm_receiver<T>& feedA, dissm_receiver<T>& feedB)
    {
        for( std::size_t i = 0; i < feedA.channels.size(); ++i )
        {
            for( std::size_t j = 0; j < feedA.channels[i].size(); ++j )
            {
                epolls_[j]->add_fd( feedA.channels[i][j]->get_sock(),
                                    feedB.channels[i][j]->get_sock() );
            }
        }
    }

    std::vector< std::unique_ptr<WSApoll> > epolls_;
    std::vector< std::unique_ptr<thread_channel> > theads_;
    std::mutex mtx;

    std::size_t groups;
    std::size_t cores; 
    std::size_t threads;
};

template <mcast_type T>
void client_t::add_channels_epoll(dissm_receiver<T>& A, dissm_receiver<T>& B)
{
    add_channel(A, B);
    //std::cin.get();
}

template <mcast_type T>
void client_t::add_channels_epoll(dissm_receiver<T>& feedA)
{
    for( std::size_t i = 0; i < feedA.channels.size(); ++i )
    {
        for( std::size_t j = 0; j < feedA.channels[i].size(); ++j )
        {
            epolls_[j]->add_fd( feedA.channels[i][j]->get_sock() );
        }
    }
}

void client_t::wait(std::unique_ptr<WSApoll>& epoll_, std::mutex& mtx, std::size_t& count)
{
    while(1)
    {
        auto nsfd  = epoll_->wait();
    
        int n = 0;
        std::unique_lock<std::mutex> lock(mtx);
        if( char buf[1500] = "\0"; !( ( n = recv(nsfd, buf, sizeof(buf), 0) ) < 0 ) )
        {
            count++;
            //std::cout << "Receive: " << n << " : " << nsfd << "\n";
            //std::cout << "Log: " << logger_t::hexdump_string((uint8_t*)buf, strlen(buf)) << std::endl;
        }
    }
}

void client_t::create_thread(std::size_t& count)
{
    for( std::size_t i = 0; i < theads_.size(); ++i  )
    {
        theads_[i] = std::make_unique<thread_channel>( std::thread(&client_t::wait, 
                                                                     std::ref(epolls_[i]),
                                                                     std::ref(mtx),
                                                                     std::ref(count) ), 
                                                        destruct_method::join );

    }
}

void client_t::create_thread1(const std::size_t cores,
                     const std::size_t threads, 
                     std::size_t& count)
{
    std::size_t groups_in_thread = ( ( groups ) / ( cores * threads ) );
    theads_.resize( cores * threads );

    for( std::size_t core = 0; core < cores; ++core )
    {
        for( std::size_t thread = 0; thread < threads; ++thread )
        {
             if( ( groups - ( (core + 1 ) * ( thread + 1 ) * groups_in_thread ) ) >= groups_in_thread )
             {
                 std::vector< std::unique_ptr<WSApoll> > sub(std::move_iterator(epolls_))
             }
             else
             {

             }
        }
    }


    for( std::size_t i = 0; i < theads_.size(); ++i  )
    {
        theads_[i] = std::make_unique<thread_channel>( std::thread(&client_t::wait, 
                                                                     std::ref(epolls_[i]),
                                                                     std::ref(mtx),
                                                                     std::ref(count) ), 
                                                        destruct_method::join );

    }
}

}