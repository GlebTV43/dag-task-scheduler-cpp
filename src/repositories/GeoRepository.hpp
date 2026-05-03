#pragma once

#include "../requesters/GeoRequester.hpp"

class GeoRepository
{
    GeoRequester& geo_requester_;

public:

    explicit GeoRepository(GeoRequester& geo_requester) : geo_requester_(geo_requester) {} 

    Geo GetGeo()
    {
        return geo_requester_.GetCurrentGeo();
    }
};