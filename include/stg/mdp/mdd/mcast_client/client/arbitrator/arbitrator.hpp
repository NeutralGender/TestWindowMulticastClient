#pragma once

#include <iostream>
#include <cstddef>
#include <string>
#include <cstring>
#include <vector>

#include <stg/mdp/mdd/mcast_client/client/producer/producer.hpp>
#include <stg/ai/log.hpp>

namespace stg::mdp::mdd::mcast_client::client::arbitrator
{

using stg::mdp::mdd::mcast_client::client::producer::producer_t;

/**
 * @brief enum class with return value for arbitrator
 */
enum class drop_type : unsigned
{
    FALSE = 0,
    TRUE,
    OUT_ORDER
};

/**
 * @brief Multicast Client packages arbitrator
 */
class arbitrator_t
{
public:
    arbitrator_t()
    {
        rec_cache.reserve( 5 );
    }

    ~arbitrator_t()
    {}

    /**
     * @brief Drop same multicast packges and wite to ring
     * 
     * @param message received message
     * @return drop_type 
     */
    drop_type write( char* message, producer_t& m_prod )
    {
        #define LOG_TAG "[stg.ai.test.xring] "

        LOG_AUDIT(LOG_TAG "[%s]", log_t::hexdump_string((uint8_t*)message, strlen(message)).c_str());
        LOG_FLUSH;

        uint32_t seq = message[0] - '0';     //bytes_from_string(message, ";");

        if( seq > ( cur_seq + 1 ) )
        {            
            if( not is_consist(message) )
            {
                rec_cache.push_back( message );
            }

            return( drop_type::OUT_ORDER );
        }
        else if( seq == ( cur_seq + 1 ) ) 
        {
            ( rec_cache.empty() ? write_empty_ring(message, seq, m_prod) : 
                                    write_not_empty_ring(message, seq, m_prod) );

            return (drop_type::TRUE);
        }

        return (drop_type::FALSE);
        
        #undef LOG_TAG
    }

protected:

private:
    /**
     * @brief Check is message in rec_cache
     * 
     * @param message received by server
     * @return true if the vector consists message
     * @return false if the vector doesn`t consist message
     */
    bool is_consist(char* message)
    {
        if( std::find(rec_cache.begin(), rec_cache.end(), message ) 
                                                 != rec_cache.end() )
            return 1;

        return 0;

    }

    /**
     * @brief write message if vector empty
     * 
     * @param message received by server
     * @param seq message sequence
     * @param m_prod writer to ring
     */
    void write_empty_ring(char* message, uint32_t seq, producer_t& m_prod)
    {
        cur_seq++;
        m_prod.xring_write(message);
    }

    /**
     * @brief write message if vector not empty
     * 
     * @param message received by server
     * @param seq message sequence
     * @param m_prod writer to ring
     */
    void write_not_empty_ring(char* message, uint32_t seq, producer_t& m_prod)
    {
        cur_seq++;
        m_prod.xring_write(message);

        while( not rec_cache.empty() )
        {
            cur_seq++;
            m_prod.xring_write(rec_cache.back());
            rec_cache.pop_back();
        }
        
    }


    /**
     * @brief Get first 4 bytes from received message from multicast Server
     * 
     * @param message received message from server
     * @return uint32_t message sequence
     */
    //uint32_t bytes_from_string( std::string& message, const char* delim )
    uint32_t bytes_from_string( char* message, const char* delim )
    { return ( atoi( strtok( message, delim ) ) ); }

    /**
     * @brief Current message sequence( first 4 bytes )
     */
    std::size_t cur_seq = -1;

    /**
     * @brief Cache for rescheduling package
     * reserve 5 elem
     */
    std::vector< char* > rec_cache;
};

}