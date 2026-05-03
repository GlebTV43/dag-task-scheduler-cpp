#pragma once

#include <string>

struct Place
{
    std::string name;
    std::string category;
    std::string address;

    double latitude = 0.0;
    double longitude = 0.0;

    double distance = 0.0;
};