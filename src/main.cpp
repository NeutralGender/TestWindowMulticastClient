#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

#include <stg/mdp/mdd/conf/mcast/mcast_conf.hpp>
using stg::mdp::mdd::mcast::conf::mcast_conf;

#include <stg/mdp/mdd/conf/dissemination/production.hpp>
using stg::mdp::mdd::conf::dissemination::diss_prod_conf;

// xring
#include <stg/ai/xring/xring.hpp>
using stg::ai::xring::xring_t;
using stg::ai::xring::xring_conf;
using stg::ai::xring::xslot_t;
using stg::ai::xring::xring_fetch_t;
using stg::ai::xring::xring_err;

#include <stg/mdp/mdd/mcast_client/client/dissemination/dissemination.hpp>
using stg::mdp::mdd::mcast_client::client::dissemination::dissm_receiver;

#include <stg/mdp/mdd/net/multicast/types.hpp>
using stg::mdp::mdd::net::multicast::mcast_type;

#include <stg/mdp/mdd/mcast_client/client/client.hpp>
using stg::mdp::mdd::client::client_t;

#include <stg/mdp/mdd/common/reader/filesys/json_reader.hpp>
using stg::mdp::mdd::common::fs::json_reader;

#include <stg/mdp/mdd/common/reader/reader.hpp>
using stg::mdp::mdd::common::reader::reader_t;

#define LOG_TAG "[stg.ai.perf.xring] "
#include <stg/ai/log.hpp>
using stg::ai::path_t;

#include <stg/mdp/mdd/redis/redis.hpp>
using stg::mdp::mdd::redis::redis_t;
using stg::mdp::mdd::redis::ssl_auth;

#include <stg/mdp/mdd/redis/redis_types/hash.hpp>
using stg::mdp::mdd::redis::type::hash_t;

int main()
{
/*
    redis_t<hash_t> hash( "127.0.0.1", 6379, *(new hash_t) );
    hash.connect();
    hash.write("key", "field", "value");
    hash.read();
*/


    LOG_INSTANTIATE(path_t("log"), "stg.ai.perf.xring");
    LOG_CON(LOG_TAG "%s", "Running XRING perf test");

    LOG_FLUSH;
    LOG_AUDIT(LOG_TAG "%s", "Running XRING perf test" );
    LOG_FLUSH;
    LOG_CON_FORCE(LOG_TAG "%s", "Running XRING perf test");
    LOG_CON_FORCE(LOG_TAG "%s", CON_RED("Press Ctrl-Break to terminate..."));

    const fs::path workdir = fs::current_path();
    const fs::path prod_path = workdir / "clientMDP" / "etc" / "json" / "dissemination" / "prod.conf.json";
    
    const fs::path mcsat_path = workdir / "clientMDP" / "etc" / "json" / "mcast_conf.json";
    const fs::path pcap_path = workdir / "capture" / "multicast.pcapng";
    const fs::path flood = workdir / "capture" / "flooding.pcapng";

    reader_t<json_reader> prod_reader( *(new json_reader ) );
    if( not prod_reader.open(prod_path.c_str()) )
        return false;
    prod_reader.read();

    reader_t<json_reader> mcast_reader( *(new json_reader ) );

    if( not mcast_reader.open(mcsat_path.c_str()) )
      return false;

    mcast_reader.read();

    diss_prod_conf dprod( prod_reader.get_data() );
    mcast_conf mconf( mcast_reader.get_data() );

    xring_conf xconf;
    xconf.setup_capacity( 200 );

    xring_t<> rng ( xconf, "" );

    dissm_receiver<mcast_type::ANY> rA(dprod.prod_a.channels);
    if( not rA.init(dprod.prod_a.channels, mconf ) )
        return false;

    dissm_receiver<mcast_type::ANY> rB(dprod.prod_b.channels);
    if( not rB.init(dprod.prod_b.channels, mconf ) )
        return false;

    client_t client(6, rng);
    client.create_epoll_fd();
    client.add_channels_epoll( rA, rB );
    client.create_thread();

    return 0;
}

#undef LOG_TAG