#pragma once

#include <vector>
#include <mutex>

#include <stg/mdp/mdd/mcast_client/client/epoll/epoll.hpp>
#include <stg/mdp/mdd/mcast_client/client/dissemination/dissemination.hpp>
#include <stg/mdp/mdd/mcast_client/thread/thread_channel.hpp>
#include <stg/mdp/mdd/mcast_client/client/arbitrator/arbitrator.hpp>
#include <stg/mdp/mdd/mcast_client/client/producer/producer.hpp>
#include <stg/mdp/mdd/mcast_client/cpu_affinity/cpu_affinity.hpp>
#include <stg/mdp/mdd/net/multicast/types.hpp>
#include <stg/ai/xring/xring.hpp>

namespace stg::mdp::mdd::client
{

using stg::mdp::mdd::mcast_client::client::epoll::epoll_t;
using stg::mdp::mdd::mcast_client::thread::thread_channel;
using stg::mdp::mdd::mcast_client::thread::destruct_method;
using stg::mdp::mdd::mcast_client::client::dissemination::dissm_receiver;
using stg::mdp::mdd::mcast_client::client::arbitrator::arbitrator_t;
using stg::mdp::mdd::mcast_client::client::arbitrator::drop_type;
using stg::mdp::mdd::mcast_client::client::producer::producer_t;
using stg::mdp::mdd::mcast_client::cpu_affinity::cpu_affinity_t;
using stg::mdp::mdd::net::multicast::mcast_type;
using stg::ai::xring::xring_t;


class client_t
{
public:
    client_t(std::size_t queue_len, xring_t<>& rng):
             epolls_(queue_len), 
             theads_(queue_len),
             prod_(rng)
    {
        for( std::size_t i = 0; i < epolls_.size(); ++i )
            epolls_[i] = std::make_unique<epoll_t>(2);
    }

    ~client_t()
    {}

    /**
     * @brief Create an epoll_ fd object
     * 
     */
    void create_epoll_fd();

    template <mcast_type T>
    void add_channels_epoll(dissm_receiver<T>& A, dissm_receiver<T>& B);

    static void wait(std::unique_ptr<epoll_t>& epoll_, 
                     arbitrator_t& arbitrator_, 
                     producer_t& prod_,
                     std::mutex& mtx);

    void create_thread();

protected:

private:
    template <mcast_type T>
    void add_channel(dissm_receiver<T>& feed)
    {
        for( std::size_t i = 0; i < feed.channels.size(); ++i )
        {
            for( std::size_t j = 0; j < feed.channels[i].size(); ++j )
            {
                epolls_[j]->add_fd( feed.channels[i][j]->get_sock() );
            }
        }
    }

    arbitrator_t arbitrator_;
    std::vector< std::unique_ptr<epoll_t> > epolls_;
    std::vector< std::unique_ptr<thread_channel> > theads_;
    producer_t prod_;
    cpu_affinity_t cpu_affinity_;
    std::mutex mtx;
};

void client_t::create_epoll_fd()
{
    for( std::size_t i = 0; i < epolls_.size(); ++i )
        epolls_[i]->create();
}

template <mcast_type T>
void client_t::add_channels_epoll(dissm_receiver<T>& A, dissm_receiver<T>& B)
{
    add_channel(A);
    add_channel(B);
}

void client_t::wait(std::unique_ptr<epoll_t>& epoll_,
                    arbitrator_t& arbitrator_,
                    producer_t& prod_,
                    std::mutex& mtx)
{
    while(true)
    {
        auto [nsfd, events]  = epoll_->wait(-1);

        for( std::size_t i = 0; i < nsfd; ++i )
        {
            std::unique_lock<std::mutex> lock(mtx);
            // TODO: Only for test
            if( char buf[50] = "\0"; not ( recv(events[i].data.fd, buf, 50, 0) < 0 ) )
                arbitrator_.write(buf, prod_);
        }
    }
}

void client_t::create_thread()
{
    for( std::size_t i = 0; i < theads_.size(); ++i  )
    {
        theads_[i] = std::make_unique<thread_channel>( std::thread(&client_t::wait, 
                                                                     std::ref(epolls_[i]), 
                                                                     std::ref(arbitrator_),
                                                                     std::ref(prod_),
                                                                     std::ref(mtx) ), 
                                                        destruct_method::join );

        cpu_affinity_.set_affinity( i, theads_[i]->get().native_handle() );
    }
}

}