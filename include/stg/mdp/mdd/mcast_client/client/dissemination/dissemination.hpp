#pragma once

#include <iostream>
#include <vector>

#include <stg/mdp/mdd/net/multicast/rx.hpp>
#include <stg/mdp/mdd/net/multicast/types.hpp>
#include <stg/mdp/mdd/conf/mcast/mcast_conf.hpp>

namespace stg::mdp::mdd::mcast_client::client::dissemination
{

using stg::mdp::mdd::net::multicast::mcast_receiver;
using stg::mdp::mdd::net::multicast::mcast_type;
using stg::mdp::mdd::conf::dissemination::site_t;
using stg::mdp::mdd::conf::mcast::mcast_conf;

/**
 * @brief Feed channel class
 * 
 * @tparam mcast_type::SSM 
 */
template< mcast_type MCast_Provider = mcast_type::SSM >
class dissm_receiver
{
public:
    dissm_receiver(const std::vector<site_t>& site):
                   channels(site.size())
    {   
        for( std::size_t i = 0; i < site.size(); ++i )
        {
            channels[i].resize(site[i].channel.size());
            for( std::size_t j = 0; j < site[i].channel.size(); ++j )
            {
                channels[i][j] = std::make_unique<mcast_receiver<MCast_Provider>>(site[i].channel[j].port);
                //std::cout << site[i].channel[j].group << " : " << site[i].channel[j].port << std::endl;
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
    bool init(const std::vector<site_t>& dconf, const mcast_conf& mconf);
    
    /**
     * @brief container for multicast channels
     */
    std::vector< std::vector< std::unique_ptr< mcast_receiver<MCast_Provider> > > > channels;
};

template<>
[[nodiscard]] bool dissm_receiver<mcast_type::ANY>::init(const std::vector<site_t>& site, const mcast_conf& mconf)
{
    // Take each site
    for( std::size_t i = 0; i < site.size(); ++i )
    {
        // Take each site`s channel
        for( std::size_t j = 0; j < site[i].channel.size(); ++j )
        {
            if( ! channels[i][j]->create_socket(SOCK_DGRAM) )
            {
                std::cout << "Create Socket Err: " << GetLastError() << std::endl;
                return false;
            }

            channels[i][j]->init_socket(site[i].channel[j].group);
            
            if( ! channels[i][j]->socket_settings() )
            {
                std::cout << "init_socket Err: " << GetLastError() << std::endl;
                return false;
            }

            if( ! channels[i][j]->join_group(mconf.asm_recv.local, site[i].channel[j].group) )
            {
                std::cout << "join_group Err: " << GetLastError() << std::endl;
                return false;
            }
        }
    }

    return true;
}

template<>
[[nodiscard]] bool dissm_receiver<mcast_type::SSM>::init(const std::vector<site_t>& site, const mcast_conf& mconf)
{
    for( std::size_t i = 0; i < channels.size(); ++i )
    {
        for( std::size_t j = 0; j < channels[i].size(); ++i )
        {
            if( ! channels[i][j]->create_socket(SOCK_DGRAM) )
                return false;

            channels[i][j]->init_socket(site[i].channel[j].group);
            
            if( ! channels[i][j]->socket_settings() )
                return false;

            if( ! channels[i][j]->join_group(mconf.asm_recv.local, site[i].channel[j].group, site[i].source) )
                return false;
        }
    }

    return true;
}

}
