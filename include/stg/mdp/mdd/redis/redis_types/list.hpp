#pragma once

#include <string>
#include <list>
#include <map>
#include <cstring>
#include <stdarg.h>

#include <hiredis/hiredis.h>

#include <stg/mdp/mdd/redis/connection.hpp>

namespace stg::mdp::mdd::redis::type::list
{

using stg::mdp::mdd::redis::connection;

/**
 * @brief Redis list data type
 * 
 */
class list_t
{
public:
    list_t() {}
    ~list_t() {}

    /**
     * @brief 
     * 
     * @tparam L List key name
     * @tparam Args Variadic template for list`s elements
     * @param conn Hiredis redisContext pointer for connection 
     * @param key List`s key name
     * @param args Args Variadic template for list`s elements
     */
    template<typename L, typename ... Args>
    void write(connection& conn, L key, Args ... args)
    {
        for(const auto& n : {args...})
            push( conn, list_t::lpush_fmt, key, n );
    }

    /**
     * @brief 
     * 
     * @tparam L List key name
     * @param conn Hiredis redisContext pointer for connection
     * @param fmt L/R push format
     * @param key List`s key name
     * @param value Value for pushing into redis list
     */
    template<typename L>
    inline void push(connection& conn, const char fmt[], L key, L value)
    {
        if( conn.get_connection().has_value() )
        {
            redisReply* reply = static_cast<redisReply*>((redisCommand(conn.get_connection().value(),
                                                                       fmt,
                                                                       key,(size_t) strlen (key),
                                                                       value,(size_t) strlen (value)
                                             )
            ));

            freeReplyObject(reply);
        }
    }

    /**
     * @brief Reading all lists from redis server
     * 
     * @param conn Hiredis redisContext pointer for connection
     * @return std::map< std::string, std::list<std::string> > 
     */
    std::map< std::string, std::list<std::string> > read(connection& conn)
    {
        std::map< std::string, std::list<std::string> > record;

        redisReply* reply = nullptr;

        if( conn.get_connection().has_value() )
        {
            // Get All keys
            reply = static_cast<redisReply*>(redisCommand(conn.get_connection().value(), 
                                                          key_fmt));

            redisReply *temp = nullptr;
            for( size_t i = 0; i < reply->elements; ++i )
            {
                // Get all elements by key
                temp = static_cast<redisReply*>(redisCommand(conn.get_connection().value(),
                                                             lrange_fmt, &reply->element[i]->str[0],
                                                             (size_t) strlen(reply->element[i]->str))
                                                );

                std::list<std::string> list; 
                for( size_t j = 0; j < temp->elements; ++j )
                {
                    // Access All elements by key
                    list.push_back(std::string{temp->element[j]->str, strlen(temp->element[j]->str)});
                }

                record.try_emplace(std::string{reply->element[i]->str, strlen(reply->element[i]->str)},
                                    list);

                freeReplyObject(temp);
            }
        }

        freeReplyObject(reply);

        return std::move(record);
    }

protected:

private:
    static constexpr char lpush_fmt[] = "LPUSH %b %b";
    static constexpr char rpush_fmt[] = "RPUSH %b %b";
    static constexpr char key_fmt[] = "KEYS *";
    static constexpr char lrange_fmt[] = "LRANGE %b 0 -1";
};

}
