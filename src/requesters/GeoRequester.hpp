#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <stdexcept>

#include "../domain/Geo.hpp"

class GeoRequester
{
private:
    static constexpr const char* kGeoUrl = "https://api.2ip.io";

    std::string apikey_;

    Geo ParseJsonToGeo(const std::string& text);

    void ReadKey();

public:

    GeoRequester() 
    {
        ReadKey();
    }

    Geo GetCurrentGeo();
};