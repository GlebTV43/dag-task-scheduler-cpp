#include "PlaceRequester.hpp"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

void PlaceRequester::ReadKey()
{
    std::ifstream file("../src/data/geoapify_apikey.txt");

    if (!file.is_open())
    {
        throw std::runtime_error("Can`t find geoapify_apikey.txt");
    }

    std::getline(file, apikey_);
    file.close();
}

using json = nlohmann::json;

std::vector<Place> PlaceRequester::ParseJsonToPlace(const std::string& text)
{
    json data = json::parse(text);

    if (!data.contains("features"))
    {
        throw std::runtime_error("Places response does not contain features");
    }

    std::vector<Place> places;

    for (const auto& features : data["features"])
    {

        if (!features.contains("properties"))
        {
            throw std::runtime_error("Places response does not contain properties");
        }

        const json& prop = features["properties"];

        Place place;

        place.name = prop.value("name", "Unknown place");
        place.address = prop.value("formatted", "");
        place.distance = prop.value("distance", 0.0);

        if (prop.contains("categories") && !prop["categories"].empty())
        {
            place.category = prop["categories"][0].get<std::string>();
        }

        place.latitude = prop.value("lat", 0.0);
        place.longitude = prop.value("lon", 0.0);

        places.push_back(place);
    }

    return places;
}

std::vector<Place> PlaceRequester::FindNearbyPlaces(double longitude, double latitude, const std::string& category, 
                                                    int radius_meters, int limit) 
{
    cpr::Response response = cpr::Get(
        cpr::Url{kPlaceUrl},
        cpr::Parameters
        {
            {"categories", category},
            {"filter", "circle:" + std::to_string(longitude) + "," + std::to_string(latitude) + "," + std::to_string(radius_meters)},
            {"bias", "proximity:" + std::to_string(longitude) + "," + std::to_string(latitude)},
            {"limit", std::to_string(limit)},
            {"apiKey", apikey_}
        }
    );

    if (response.error.code != cpr::ErrorCode::OK)
    {   
        throw std::runtime_error("Place request error: " + response.error.message);
    } 

    if (response.status_code != 200)
    {
        throw std::runtime_error(
            "Place bad status code: " + std::to_string(response.status_code)
        );
    }

    return ParseJsonToPlace(response.text);
}