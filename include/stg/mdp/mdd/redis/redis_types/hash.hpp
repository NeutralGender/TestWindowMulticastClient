#pragma once

#include <string>
#include <vector>
#include <cstring>

#include <hiredis/hiredis.h>

#include <stg/mdp/mdd/redis/connection.hpp>

namespace stg::mdp::mdd::redis::type::hash
{

using stg::mdp::mdd::redis::connection;

/**
 * @brief Redis Hash record type
 */
struct record_t
{
    record_t(const std::string& key, const std::string& field, const std::string& value):
        key(key),
        field(field),
        value(value)
    {}

    // IF WE WILL KNOW RECORD SIZE, MAY CHANGE TO fixed char array
    std::string key;
    std::string field;
    std::string value;
};

/**
 * @brief Redis hash data type
 */
class hash_t
{
public:
    hash_t() = default;
    ~hash_t() {}

    /**
     * @brief Redis HSET function
     * 
     * @param conn Hiredis redisContext pointer for connection 
     * @param key Record`s key
     * @param field Record`s field
     * @param value Record`s value
     */
    void write(connection& conn, const std::string& key, const std::string& field, const std::string& value)
    {
        if( conn.get_connection().has_value() )
        {
            redisReply* reply = static_cast<redisReply*>((redisCommand(conn.get_connection().value(),
                                                                       hset_fmt,
                                                                       key.data(),(size_t) strlen (key.data()),
                                                                       field.data(),(size_t) strlen (field.data()),
                                                                       value.data(),(size_t) strlen (value.data())
                                             )
            ));

            freeReplyObject(reply);
        }
    }

    /**
     * @brief Redis HGET function
     * 
     * @param conn Hiredis redisContext pointer for connection
     * @return std::vector<record_t> 
     */
    std::vector<record_t> read(connection& conn)
    {
        std::vector<record_t> record;
        record.reserve(100);

        redisReply* reply = nullptr;

        if( conn.get_connection().has_value() )
        {
            reply = static_cast<redisReply*>(redisCommand(conn.get_connection().value(), 
                                                          key_fmt));

            redisReply *temp = nullptr;
            for( size_t i = 0; i < reply->elements; i++ )
            {
                temp = static_cast<redisReply*>(redisCommand(conn.get_connection().value(),
                                                             hget_fmt, &reply->element[i]->str[0],
                                                             reply->element[i]->len)
                                                );

                for(size_t j = 0; j < temp->elements; j = j+2)
                {
                    record.push_back( record_t( std::string{reply->element[i]->str, reply->element[i]->len},
                                                std::string{temp->element[j]->str, temp->element[j]->len},
                                                std::string{temp->element[j+1]->str, temp->element[j+1]->len}
                                            ) );
                }


                freeReplyObject(temp);
            }
        }

        freeReplyObject(reply);

        return record;
    }

protected:

private:
    static constexpr char hset_fmt[] = "HSET %b %b %b";
    static constexpr char hget_fmt[] = "HGETALL %b";
    static constexpr char key_fmt[] = "KEYS *";
};

}
