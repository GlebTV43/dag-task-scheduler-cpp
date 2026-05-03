#pragma once

#include <string>
#include <vector>

#include "Geo.hpp"
#include "Weather.hpp"
#include "Place.hpp"

struct Recommendation
{
    Geo geo;
    Weather weather;
    std::string category;
    std::vector<Place> places;
};