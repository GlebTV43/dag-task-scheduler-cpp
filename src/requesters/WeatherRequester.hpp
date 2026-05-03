#pragma once

#include <string>
#include <iostream>
#include <filesystem>
#include <stdexcept>

#include "../domain/Weather.hpp"

// public api

class WeatherRequester 
{
private:

    static constexpr const char* kWeatherUrl = "https://api.open-meteo.com/v1/forecast";

    Weather ParseJsonToWeather(const std::string& text, double latitude, double longitude);

public:

    WeatherRequester() {}

    Weather GetCurrentWeather(double latitude, double longitude);
};