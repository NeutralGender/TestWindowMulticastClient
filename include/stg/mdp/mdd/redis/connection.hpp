#pragma once

#include <iostream>
#include <string>
#include <exception>
#include <stdexcept>
#include <optional>

#include <hiredis/hiredis.h>
#include <hiredis/hiredis_ssl.h>

namespace stg::mdp::mdd::redis
{

class connection
{
public:
    connection(const std::string& hostname, const std::size_t& port):
            hostname(hostname), 
            port(port),
            conn(nullptr)
    {}

    ~connection() 
    { redisFree(conn); }

    std::optional<redisContext*> get_connection()
    { return ( conn == nullptr ) ? nullptr : conn; }

    void connect()
    {
        try
        {
            conn = redisConnect(hostname.c_str(), port);

            if( conn == NULL || conn->err )
                throw std::runtime_error("Redis connect() fault\n");
        }  
        catch(const std::runtime_error& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    void ssl_connect(const std::string& CA_crt,
                     const std::string& client_cert,
                     const std::string& private_key,
                     const std::string& to_request )
    {
        try
        {
            if(redisSecureConnection(conn,CA_crt.c_str(), 
                                     client_cert.c_str(), 
                                     private_key.c_str(),
                                     to_request.c_str()) != REDIS_OK)
            throw std::runtime_error("Redis ssl_connect() fault\n");
        }
        catch(const std::runtime_error& e)
        {
            std::cerr << e.what() << '\n';
        }
        
    }

    void auth(const std::string& passwd)
    { 
        const char* auth_format = "AUTH %s";

        try
        {
            redisReply* reply = static_cast<redisReply*> (redisCommand(conn, auth_format, passwd.c_str()));
                
            if( reply->type == REDIS_REPLY_ERROR )
                throw std::runtime_error("Redis auth() fault\n");

            freeReplyObject(reply);
        }
        catch(const std::runtime_error& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

protected:

private:
    const std::string hostname;
    const std::size_t port;
    redisContext* conn;
};

}
