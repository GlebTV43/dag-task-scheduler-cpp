#pragma once

#include <string>

struct Geo
{
    std::string ip;

    std::string country_code;
    std::string country;
    std::string region;
    std::string city;

    double latitude = 0.0;
    double longitude = 0.0;
};
