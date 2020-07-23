#pragma once

// standart
#include <string>
#include <map>
#include <cstring>

#include <hiredis/hiredis.h>

#include <stg/mdp/mdd/redis/connection.hpp>

namespace stg::mdp::mdd::redis::type::set
{

using stg::mdp::mdd::redis::connection;

class set_t
{
public:
    set_t() = default;
    ~set_t() {}

    void write(connection& conn, const std::string& key, const std::string& value)
    {
        if( conn.get_connection().has_value() )
        {
            redisReply* reply = static_cast<redisReply*>(redisCommand(conn.get_connection().value(),
                                                                       set_fmt,
                                                                       key.data(),(size_t) strlen (key.data()),
                                                                       value.data(), (size_t) value.size()
                                                                      )
                                    );

            freeReplyObject(reply);
        }
    }

    std::map<std::string, std::string> read(connection& conn)
    {
        std::map<std::string, std::string> record;

        redisReply* reply = nullptr;

        if( conn.get_connection().has_value() )
        {
            // Get All keys
            reply = static_cast<redisReply*>(redisCommand(conn.get_connection().value(), 
                                                          key_fmt));

            redisReply *temp = nullptr;
            for( size_t i = 0; i < reply->elements; ++i )
            {
                // Get element by key
                temp = static_cast<redisReply*>(redisCommand(conn.get_connection().value(),
                                                             get_fmt, &reply->element[i]->str[0],
                                                             reply->element[i]->len)
                                                );

                record.try_emplace( std::string( reply->element[i]->str, reply->element[i]->len ), 
                                    std::string( temp->str, temp->len ) );                

                freeReplyObject(temp);
            }
        }

        freeReplyObject(reply);

        return (record);
    }

protected:

private:
    static constexpr char set_fmt[] = "SET %b %b";
    static constexpr char get_fmt[] = "GET %b";
    static constexpr char key_fmt[] = "KEYS *";
};

}

