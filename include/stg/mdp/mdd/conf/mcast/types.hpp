#pragma once

#include <stg/ai/ai.hpp>

#include <cstddef>
#include <string>

namespace stg::mdp::mdd::mcast::conf
{


struct multicast_conf
{
    struct server
    {
        std::string local;
        std::string group;
        std::size_t port;
    };

    struct client
    {
        struct ASM
        {
            std::string local;
            std::string group;
        };

        struct SSM
        {
            std::string src;
            std::string group;
            std::string local;
        };

        std::size_t port;
    };
};


void from_json( const json& j, multicast_conf::server& srv )
{
    j.at("local").get_to(srv.local);
    j.at("group").get_to(srv.group);
    j.at("port").get_to(srv.port);
}

void from_json( const json& j, multicast_conf::client::ASM& asmcst )
{
    j.at("local").get_to(asmcst.local);
    j.at("group").get_to(asmcst.group);
}

void from_json( const json& j, multicast_conf::client::SSM& ssm )
{
    j.at("src").get_to(ssm.src);
    j.at("group").get_to(ssm.group);
    j.at("local").get_to(ssm.local);
}

void from_json( const json& j, multicast_conf::client& client )
{ j.at("port").get_to(client.port); }

} // namespace stg::mdp::mdd::conf

