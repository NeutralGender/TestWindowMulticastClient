#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <exception>

#include <js.hpp>

namespace stg::mdp::mdd::conf::mcast{

using json = nlohmann::json;

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

class mcast_conf
{
public:
    mcast_conf(const json& j):
               mcast_sender( j["multicast"]["server"] ),
               asm_recv( j["multicast"]["client"]["ASM"] ),
               ssm_recv( j["multicast"]["client"]["SSM"] ),
               mcast_receiver( j["multicast"]["client"] ),
               json_addr( j["address"] ),
               port( j["port"] ),
               core( j["core"] ),
               thread( j["thread"] ),
               use_addr( j["use_addr"] ),
               ip_addr1( j["ip_addr1"] ),
               ip_addr2( j["ip_addr2"] )
    {}

    const struct multicast_conf::server mcast_sender;
    const struct multicast_conf::client::ASM asm_recv;
    const struct multicast_conf::client::SSM ssm_recv;
    const struct multicast_conf::client mcast_receiver;
    const std::size_t json_addr;
    const std::size_t port;

    const std::size_t core;
    const std::size_t thread;

    const std::size_t use_addr;
    const std::string ip_addr1;
    const std::string ip_addr2;
};


} // namespace stg::mdp::mdd::mcast::conf
