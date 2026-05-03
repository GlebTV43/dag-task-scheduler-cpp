#include "WeatherRequester.hpp"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Weather WeatherRequester::ParseJsonToWeather(const std::string& text, double latitude, double longitude)
{
    json data = json::parse(text);

    if (!data.contains("current"))
    {
        throw std::runtime_error("Weather response does not contain current field");
    }

    Weather weather;

    const json& current = data["current"];

    weather.latitude = latitude;
    weather.longitude = longitude;

    weather.time = current.value("time", "");

    weather.temperature = current.value("temperature_2m", 0.0);
    weather.apparent_temperature = current.value("apparent_temperature", 0.0);
    weather.humidity = current.value("relative_humidity_2m", 0.0);

    weather.precipitation = current.value("precipitation", 0.0);
    weather.rain = current.value("rain", 0.0);
    weather.snowfall = current.value("snowfall", 0.0);

    weather.weather_code = current.value("weather_code", 0);
    weather.wind_speed = current.value("wind_speed_10m", 0.0);
    weather.cloud_cover = current.value("cloud_cover", 0.0);

    weather.is_day = current.value("is_day", 0) == 1;

    return weather;
}

Weather WeatherRequester::GetCurrentWeather(double latitude, double longitude)
{
    cpr::Response response = cpr::Get
    (
        cpr::Url{kWeatherUrl},
        cpr::Parameters
            {
                {"latitude", std::to_string(latitude)},
                {"longitude", std::to_string(longitude)},
                {"current", "temperature_2m,apparent_temperature,relative_humidity_2m,precipitation,rain,snowfall,weather_code,wind_speed_10m,cloud_cover,is_day"},
                {"timezone", "auto"}
            }
    );

    if (response.error.code != cpr::ErrorCode::OK)
    {
        throw std::runtime_error("Weather request error: " + response.error.message);
    }

    if (response.status_code != 200)
    {
        throw std::runtime_error("Weather bad status code: " + std::to_string(response.status_code));
    }

    return ParseJsonToWeather(response.text, latitude, longitude);
}