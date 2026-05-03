#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>

#include "../domain/Place.hpp"

class PlaceRequester
{
    static constexpr const char* kPlaceUrl = "https://api.geoapify.com/v2/places"; 

    std::string apikey_;

    void ReadKey();

    std::vector<Place> ParseJsonToPlace(const std::string& text); 

public:

    PlaceRequester()
    {
        ReadKey();
    }

    std::vector<Place> FindNearbyPlaces(double longitude, double latitude, const std::string& category, int radius = 3000, int limit = 10);
};