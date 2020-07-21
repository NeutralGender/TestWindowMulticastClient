#pragma once

#include <string>

#include <stg/mdp/mdd/redis/connection.hpp>

namespace stg::mdp::mdd::redis
{

template<typename T>
class redis_t
{
public:
    redis_t(const std::string& hostname, const std::size_t& port, T& writer):
            redis_inst(writer),
            conn( hostname, port )
    {}

    ~redis_t() {}

    void connect()
    { conn.connect(); }

    void ssl_connection(const std::string& CA_rt)
    { conn.ssl_connect(NULL,NULL,NULL,NULL); }

    void auth(const std::string& passwd)
    { conn.auth("passwd"); }

    template<typename ... Args>
    auto write(Args ... args)
    { return redis_inst.write(conn, args...); }

    //template <typename R>
    auto read()
    { redis_inst.read(conn); }

protected:

private:
    T& redis_inst;
    connection conn;
};

}
