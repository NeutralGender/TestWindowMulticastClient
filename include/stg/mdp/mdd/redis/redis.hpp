#pragma once

#include <string>

#include <stg/mdp/mdd/redis/connection.hpp>

namespace stg::mdp::mdd::redis
{

/**
 * @brief Structure for stored credentials paths
 * Direct constructor was deleted
 * Developer must use ssl_auth::builder for init and construct ssl_auth structure
 * 
 * Usage: 
 * ssl_auth auth = ssl_auth::builder{}.ca_cert("ca") .cert("cert") .key("key") .request("req")
                                      .build();
 */
struct ssl_auth
{
private:
    using self = ssl_auth;
public:
    ssl_auth() = default;

    /**
     * @brief ssl_auth Builder
     */
    class builder;

    /**
     * @brief Print credentials paths
     */
    void print()
    {
        std::cout << CA_rt << std::endl
                  << client_crt << std::endl
                  << client_priv_key << std::endl
                  << serv_to_request << std::endl;
    }

private:
    /**
     * @brief Private constructor for a new ssl auth object
     * Use builder for create new object;
     */
    ssl_auth(std::string& CA_rt, 
             std::string& client_crt, 
             std::string& client_priv_key, 
             std::string& serv_to_request):
            CA_rt(CA_rt),
            client_crt(client_crt),
            client_priv_key(client_priv_key),
            serv_to_request(serv_to_request)
    {}

    std::string CA_rt;
    std::string client_crt;
    std::string client_priv_key;
    std::string serv_to_request;
};

/**
 * @brief Class for ssl_auth build
 */
class ssl_auth::builder
{
private:
    /**
     * @brief Alias for class name
     */
    using self = builder;

public:
    /**
     * @brief Create ssl_auth instance with credentials paths initialized in advance 
     * 
     * @return ssl_auth 
     */
    ssl_auth build()
    { return ssl_auth(ca_crt_, cli_crt_, priv_key_, req_); }

    /**
     * @brief Set CA_cart
     * 
     * @param ca_cert CA_cert path
     * @return self& FluentAPI
     */
    self& ca_cert(const std::string& ca_cert)
    {
        ca_crt_ = ca_cert;
        return *this;
    }

    /**
     * @brief Set client cert path
     * 
     * @param cli_crt Client cert path
     * @return self& FluentAPI
     */
    self& cert(const std::string& cli_crt)
    {
        cli_crt_ = cli_crt;
        return *this;
    }

    /**
     * @brief Set client private key path
     * 
     * @param priv_key Client private key path
     * @return self& FluentAPI
     */
    self& key(const std::string& priv_key)
    {
        priv_key_ = priv_key;
        return *this;
    }

    /**
     * @brief Set server to request
     * 
     * @param req Server to request
     * @return self& FluentAPI
     */
    self& request(const std::string& req)
    {
        req_ = req;
        return *this;
    }


private:
    std::string ca_crt_;
    std::string cli_crt_;
    std::string priv_key_;
    std::string req_;
};

/**
 * @brief Redis instance depends on data type
 * 
 * @tparam T Redis data type for processing
 */
template<typename T>
class redis_t
{
public:
    /**
     * @brief Construct a new redis t object
     * 
     * @param hostname Redis server host ipv4 address
     * @param port Redis server port number
     * @param redis_inst Redis data type instance
     */
    redis_t(const std::string& hostname, const std::size_t& port, T& redis_inst):
            redis_inst(redis_inst),
            conn( hostname, port )
    {}

    ~redis_t() {}

    /**
     * @brief Connect to redis server
     */
    void connect()
    { conn.connect(); }

    /**
     * @brief Authenticate bye ssl certificate
     * 
     * @param auth Structure with stored credentials paths
     */
    void ssl_connection(ssl_auth auth)
    { conn.ssl_connect(auth.CA_rt, auth.client_crt, auth.client_priv_key, auth.serv_to_request); }

    /**
     * @brief Password authentication on redis server
     * 
     * @param passwd Redis server password
     */
    void auth(const std::string& passwd)
    { conn.auth("passwd"); }

    /**
     * @brief Write 
     * 
     * @tparam Args Variadic template std::string
     * @param args Redis command parameters for writing
     * @return auto Vector with redis records
     */
    template<typename ... Args>
    auto write(Args ... args)
    { return redis_inst.write(conn, args...); }

    /**
     * @brief Read all data from redis server
     * Depends on redis_t template parameter
     * 
     * @return auto std::vector with redis server records
     */
    auto read()
    { redis_inst.read(conn); }

protected:

private:
    T& redis_inst;
    connection conn;
};

}
