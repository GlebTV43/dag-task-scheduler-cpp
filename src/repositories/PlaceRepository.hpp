#pragma once

#include "../requesters/PlaceRequester.hpp"

class PlaceRepository
{
    PlaceRequester& place_requester_;

public:

    explicit PlaceRepository(PlaceRequester& place_requester) : place_requester_(place_requester) {} 

    std::vector<Place> GetPlaces(double longitude, double latitude, const std::string& category)
    {
        return place_requester_.FindNearbyPlaces(longitude, latitude, category);
    }
};