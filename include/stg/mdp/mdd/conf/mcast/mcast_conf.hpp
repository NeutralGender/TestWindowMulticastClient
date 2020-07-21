#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <exception>

#include <stg/ai/ai.hpp>
#include <stg/mdp/mdd/conf/mcast/types.hpp>

namespace stg::mdp::mdd::mcast::conf{

class mcast_conf
{
public:
    mcast_conf(const json& j):
               mcast_sender( j["multicast"]["server"] ),
               asm_recv( j["multicast"]["client"]["ASM"] ),
               ssm_recv( j["multicast"]["client"]["SSM"] ),
               mcast_receiver( j["multicast"]["client"] )
    {}

    const struct multicast_conf::server mcast_sender;
    const struct multicast_conf::client::ASM asm_recv;
    const struct multicast_conf::client::SSM ssm_recv;
    const struct multicast_conf::client mcast_receiver;

};


} // namespace stg::mdp::mdd::mcast::conf
