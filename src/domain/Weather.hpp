#pragma once

#include <string>

struct Weather
{
    double latitude = 0.0;
    double longitude = 0.0;

    std::string time;

    double temperature = 0.0;
    double apparent_temperature = 0.0;
    double humidity = 0.0;

    double precipitation = 0.0;
    double rain = 0.0;
    double snowfall = 0.0;

    double wind_speed = 0.0;
    double cloud_cover = 0.0;

    int weather_code = 0;
    bool is_day = false;

    bool HasPrecipitation() const
    {
        return precipitation > 0.0;
    }

    bool HasRain() const
    {
        return rain > 0.0;
    }

    bool HasSnow() const
    {
        return snowfall > 0.0;
    }

    bool IsCold() const
    {
        return temperature < 0.0;
    }

    bool IsWarm() const
    {
        return temperature >= 15.0 && temperature <= 28.0;
    }

    bool IsWindy() const
    {
        return wind_speed > 12.0;
    }
};