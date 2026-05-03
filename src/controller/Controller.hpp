#pragma once

#include <iostream>

#include "../servicies/RecommendationService.hpp"

class Controller
{
    RecommendationService& service_;
public:

    Controller(RecommendationService& service) : service_(service) {}

    void ShowRecommendation()
    {
        Recommendation rec = service_.GetRecommendation();

        std::cout << "Current city: " << rec.geo.city << '\n';
        std::cout << "Temperature: " << rec.weather.temperature << " C\n";
        std::cout << "Selected category: " << rec.category << '\n';

        std::cout << "\nRecommended places:\n";

        for (const Place& place : rec.places)
        {
            std::cout << "- " << place.name << '\n';

            if (!place.address.empty())
            {
                std::cout << "  Address: " << place.address << '\n';
            }

            std::cout << "  Distance: " << place.distance << " m\n\n";
        }
    }
};