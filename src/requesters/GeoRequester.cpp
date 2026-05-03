#include "GeoRequester.hpp"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void GeoRequester::ReadKey()
{
    // std::cout << "Current path: " << std::filesystem::current_path() << '\n';

    std::ifstream file("../src/data/2ip_apikey.txt");

    if (!file.is_open())
    {
        throw std::runtime_error("Can`t find 2ip_apikey.txt");
    }

    std::getline(file, apikey_);
    file.close();
}

Geo GeoRequester::ParseJsonToGeo(const std::string& text)
{
    // std::cout << text << std::endl;
    json data = json::parse(text);

    Geo geo;

    geo.ip = data.value("ip", "");
    geo.country_code = data.value("code", "");
    geo.country = data.value("country", "");
    geo.region = data.value("region", "");
    geo.city = data.value("city", "");

    geo.latitude = std::stod(data.value("lat", "0"));
    geo.longitude = std::stod(data.value("lon", "0"));

    // std::cout << geo.latitude << " " << geo.longitude << "\n\n";

    return geo;
}

Geo GeoRequester::GetCurrentGeo()
{
    cpr::Response response = cpr::Get
    (
        cpr::Url{kGeoUrl},
        cpr::Parameters
        {
            {"token", apikey_}
        }
    );

    // std::cout << "Status: " << response.status_code << '\n';

    if (response.error.code != cpr::ErrorCode::OK)
    {   
        // std::cout << "CPR error: " << response.error.message << '\n';
        throw std::runtime_error("Geo request error: " + response.error.message);
    } 

    if (response.status_code != 200)
    {
        throw std::runtime_error(
            "Geo bad status code: " + std::to_string(response.status_code)
        );
    }

    // std::cout << "Body:\n" << response.text << '\n' << '\n';

    return ParseJsonToGeo(response.text);
}

