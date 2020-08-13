#pragma once

#include <string>

namespace stg::mdp::mdd::common::logger
{

class logger_t
{
public:
    logger_t() = default;

    ~logger_t();

    static std::string hexdump_string(const uint8_t* data, uint32_t size)
    {
        constexpr std::size_t PRINT_BYTES_PER_LINE { 16 };
        constexpr char fmt[] = "%02x";

        char hexpair[3] = {0};
        std::string out;

        std::size_t offset = 0;
        std::size_t nlines = size / PRINT_BYTES_PER_LINE;
        if(nlines * PRINT_BYTES_PER_LINE < size)
            nlines++;

        for(std::size_t line = 0; line < nlines; line++)
        {
            for(std::size_t j = 0; j < PRINT_BYTES_PER_LINE; j++)
            {
                if(offset + j >= size)
                    continue;
                else
                    snprintf(hexpair, sizeof(hexpair), fmt, data[offset + j]);

                out += hexpair;
            }

            offset += PRINT_BYTES_PER_LINE;

        }

        return (out);
    }

protected:

private:

};

}
