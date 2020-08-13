#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <exception>

#include <js.hpp>

namespace stg::mdp::mdd::conf::dissemination
{

using json = nlohmann::json;

struct channel_t 
{
    std::string name;
    std::string tag;
    std::string group;
    std::size_t port;
};

struct site_t 
{
    std::string source;
    std::string country;
    std::string city;
    std::vector<channel_t> channel;
};

struct feed_t 
{
    std::vector<site_t> site;
};

void from_json(const json& j, feed_t& prod)
{
    for(auto& elem : j)
        prod.site.emplace_back(elem);
}

void from_json(const json& j, site_t& site)
{
    for( auto& elem : j["channel"] )
        site.channel.emplace_back(elem);

    site.source = j["source"];
    site.country = j["country"];
    site.city = j["city"];
}


void from_json(const json& j, channel_t& ch)
{
    ch.name = j["name"];
    ch.tag = j["tag"];
    ch.group = j["group"];
    ch.port = j["port"];
}

class diss_quota_conf
{
public:
    diss_quota_conf(const json& j):
                    quota( j["quotation"]["A"] )
    {}

    ~diss_quota_conf()
    {}

    const struct feed_t quota;
};


}