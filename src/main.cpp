#include <iostream>

// MUST BEFORE winsock2.h
#define WINVER _WIN32_WINNT_WIN7
#define _WIN32_WINNT _WIN32_WINNT_WIN7
//#define _GLIBCXX_USE_NANOSLEEP

// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")

//#pragma comment (lib,"ws2_32.lib")

/*
#include <stg/mdp/mdd/common/reader/reader.hpp>
#include <stg/mdp/mdd/common/reader/filesys/json_reader.hpp>

using stg::mdp::mdd::common::reader::reader_t;
using stg::mdp::mdd::common::fs::json_reader;

int main()
{
    const char* prod_path = "etc/json/dissemination/prod.conf.json";
    
    const char* mcsat_path = "clientMDP/etc/json/mcast_conf.json";
    const char* pcap_path = "capture/multicast.pcapng";
    const char* flood = "capture/flooding.pcapng";

    reader_t<json_reader> prod_reader( *(new json_reader ) );
    if( ! prod_reader.open( prod_path ) )
    {
        std::cout << "Cannot open prod etc" << std::endl;
        return false;
    }
    prod_reader.read();

    std::cout << prod_reader.get_data() << std::endl;

    return 0;
}
*/


#include <stg/mdp/mdd/conf/mcast/mcast_conf.hpp>
using stg::mdp::mdd::conf::mcast::mcast_conf;

#include <stg/mdp/mdd/conf/dissemination/production.hpp>
using stg::mdp::mdd::conf::dissemination::diss_prod_conf;

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

#include <stg/mdp/mdd/common/json_gnr/writer/writer.hpp>
using stg::mdp::mdd::common::writer::writer_t;

#include <stg/mdp/mdd/common/json_gnr/json_generator/json_grt.hpp>
using stg::mdp::mdd::common::json_gnr::gnr::json_grt;


// Received package Counter 
std::size_t count = 0;

BOOL WINAPI consoleHandler(DWORD signal)
{
    if (signal == CTRL_C_EVENT)
    {
        printf("Ctrl-Break handled\n"); // do cleanup
        std::cout << count << std::endl;
        exit(0);
    }

    return TRUE;
}

int main()
{
    [[ maybe_unused ]] const char* prod_path = "etc/json/dissemination/prod.conf.json";
    [[ maybe_unused ]] const char* prod_addr = "etc/json/dissemination/prod_addr.json";
    
    [[ maybe_unused ]] const char* mcsat_path = "etc/json/mcast_conf.json";
    [[ maybe_unused ]] const char* pcap_path = "capture/multicast.pcapng";
    [[ maybe_unused ]] const char* flood = "capture/flooding.pcapng";

    reader_t<json_reader> mcast_reader( *(new json_reader ) );

    if( ! mcast_reader.open(mcsat_path) )
    {
        std::cout << "Cannot open mcast\n";
        return false;
    }
    mcast_reader.read();

    mcast_conf mconf( mcast_reader.get_data() );

    {
        writer_t w(prod_addr);

        std::string ip_add = ( mconf.use_addr == 1 ) ? mconf.ip_addr1 : mconf.ip_addr2;

        json_grt gnrt;
        gnrt.build_config(mconf.asm_recv.local, 
                          mconf.json_addr, 
                          mconf.port,
                          ip_add);
        gnrt.write();
    }

    reader_t<json_reader> prod_reader( *(new json_reader ) );
    if( ! prod_reader.open(prod_addr) )
    {
        std::cout << "Cannot open prod\n";
        return false;
    }
    prod_reader.read();

    WSADATA wsaData;

	if (WSAStartup(0x0101, &wsaData)) 
	{
		printf ("winsock not bi initialized !\n");
		WSACleanup();
	}
	else printf("Winsock initial OK !!!!\n");

//
    if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
        printf("\nERROR: Could not set control handler"); 
        return 1;
    }
//

    diss_prod_conf prod( prod_reader.get_data() );

    dissm_receiver<mcast_type::ANY> rA(prod.a.site);
    if( ! rA.init(prod.a.site, mconf ) )
        return false;

    client_t client(mconf.json_addr);
    client.add_channels_epoll( rA );
    client.create_thread(count);

/*
    if (WSACleanup())
		printf("Error Cleapir\n");
	else
		printf("Cleapir Good !!!!!\n");
*/

    return 0;
}
