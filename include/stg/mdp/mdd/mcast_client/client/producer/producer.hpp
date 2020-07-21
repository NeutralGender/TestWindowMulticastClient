#pragma once

#include <string>

#include <stg/ai/xring/xring.hpp>

namespace stg::mdp::mdd::mcast_client::client::producer
{
using stg::ai::xring::xring_t;
using stg::ai::xring::xring_conf;
using stg::ai::xring::xslot_t;

/**
 * @brief Write message in xring
 */
class producer_t
{
public:
    producer_t(xring_t<>& rng_):
               rng_(rng_)
    {}

    ~producer_t()
    {}

    /**
     * @brief Write message in xring
     * 
     * @param message received message from server 
     */
    void xring_write(const char* message)
    {
        auto instance = rng_.alloc<char*>( sizeof(message) );
        auto slot = xslot_t::slot(instance);
        slot->commit();
    }

protected:


private:
    /**
     * @brief ring instance
     * 
     */
    xring_t<>& rng_;
};

}