#pragma once

#include <iostream>
#include <vector>

#include <stg/mdp/mdd/net/multicast/rx.hpp>
#include <stg/mdp/mdd/net/multicast/types.hpp>
#include <stg/mdp/mdd/conf/dissemination/types.hpp>
#include <stg/mdp/mdd/conf/mcast/mcast_conf.hpp>

namespace stg::mdp::mdd::mcast_client::client::dissemination
{

using stg::mdp::mdd::net::multicast::mcast_receiver;
using stg::mdp::mdd::net::multicast::mcast_type;
using stg::mdp::mdd::conf::dissemination::channel_t;
using stg::mdp::mdd::mcast::conf::mcast_conf;

/**
 * @brief Feed channel class
 * 
 * @tparam mcast_type::SSM 
 */
template< mcast_type MCast_Provider = mcast_type::SSM >
class dissm_receiver
{
public:
    dissm_receiver(const std::vector<channel_t>& dconf):
                   channels(dconf.size())
    {         
        for( std::size_t i = 0; i < channels.size(); ++i )
        {
            channels[i].resize( dconf[i].broadcast.size() );
            for( std::size_t j = 0; j < channels[i].size(); ++j )
            {
                channels[i][j] = std::make_unique<mcast_receiver<MCast_Provider>>(dconf[i].broadcast[j].port);
            }
        }
    
    }

    ~dissm_receiver()
    {}

    /**
     * @brief initialize multicast channels
     * 
     * @param dconf Dissemination configuration("etc/json/dissemination")
     * @param mconf Multicast configuration( for local interface )
     */
    bool init(const std::vector<channel_t>& dconf, const mcast_conf& mconf);
    
    /**
     * @brief container for multicast channels
     */
    std::vector< std::vector< std::unique_ptr< mcast_receiver<MCast_Provider> > > > channels;

};

template<>
[[nodiscard]] bool dissm_receiver<mcast_type::ANY>::init(const std::vector<channel_t>& dconf, const mcast_conf& mconf)
{
    for( std::size_t i = 0; i < channels.size(); ++i )
    {
        for( std::size_t j = 0; j < channels[i].size(); ++j )
        {
            if( not channels[i][j]->create_socket(SOCK_DGRAM) )
                return false;

            channels[i][j]->init_socket(dconf[i].broadcast[j].group);
            
            if( not channels[i][j]->socket_settings() )
                return false;

            if( not channels[i][j]->join_group(mconf.asm_recv.local, dconf[i].broadcast[j].group) )
                return false;
        }
    }

    return true;
}

template<>
[[nodiscard]] bool dissm_receiver<mcast_type::SSM>::init(const std::vector<channel_t>& dconf, const mcast_conf& mconf)
{
    for( std::size_t i = 0; i < channels.size(); ++i )
    {
        for( std::size_t j = 0; j < channels[i].size(); ++i )
        {
            if( not channels[i][j]->create_socket(SOCK_DGRAM) )
                return false;

            channels[i][j]->init_socket(dconf[i].broadcast[j].group);
            
            if( not channels[i][j]->socket_settings() )
                return false;

            if( not channels[i][j]->join_group(mconf.asm_recv.local, dconf[i].broadcast[j].group, dconf[i].src) )
                return false;
        }
    }

    return true;
}

}
