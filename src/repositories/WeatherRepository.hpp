#pragma once

#include "../requesters/WeatherRequester.hpp"

class WeatherRepository
{
private:
    WeatherRequester& weather_requester_;

public:
    explicit WeatherRepository(WeatherRequester& weather_requester) : weather_requester_(weather_requester) {}

    Weather GetWeather(double latitude, double longitude)
    {
        return weather_requester_.GetCurrentWeather(latitude, longitude);
    }
};