#pragma once

#include <string>
#include <vector>

#include "../repositories/GeoRepository.hpp"
#include "../repositories/WeatherRepository.hpp"
#include "../repositories/PlaceRepository.hpp"

#include "../domain/Recommendation.hpp"

#include "../lib/TTaskScheduler.hpp"

class RecommendationService
{
private:
    GeoRepository& geo_repository_;
    WeatherRepository& weather_repository_;
    PlaceRepository& places_repository_;

    std::string ChooseCategoryByWeather(const Weather& weather)
    {
        if (weather.HasRain())
        {
            return "catering.cafe";
        }

        if (weather.HasSnow())
        {
            return "entertainment.museum";
        }

        if (weather.IsCold())
        {
            return "catering.restaurant";
        }

        if (weather.IsWarm())
        {
            return "leisure.park";
        }

        if (weather.IsWindy())
        {
            return "commercial.shopping_mall";
        }

        return "catering.cafe";
    }

public:

    RecommendationService(GeoRepository& geo_repository, 
        WeatherRepository& weather_repository, 
        PlaceRepository& places_repository) : geo_repository_(geo_repository), weather_repository_(weather_repository),
                                              places_repository_(places_repository) {}
    
    Recommendation GetRecommendation()
    {   
        TTaskScheduler scheduler;

        auto geo_task = scheduler.Add
        (
            [this]()   // [this] - захватывает указатель на текущий объект, таким образом, я теперь могу пользоваться полями класса
            {
                return geo_repository_.GetGeo();
            }                                 
        );

        auto weather_task = scheduler.Add
        (
            [this](Geo geo)
            {
                return weather_repository_.GetWeather(geo.latitude, geo.longitude);
            }, 
            geo_task.GetFutureResult<Geo&>()
        );

        auto category_task = scheduler.Add(
            [this](Weather weather)
            {
                return ChooseCategoryByWeather(weather);
            },
            weather_task.GetFutureResult<Weather&>()
        );

        auto place_task = scheduler.Add
        (
            [this](Geo geo, const std::string& category)
            {
                return places_repository_.GetPlaces(geo.longitude, geo.latitude, category);
            }, 
            geo_task.GetFutureResult<Geo&>(), 
            category_task.GetFutureResult<std::string&>()
        );

        scheduler.ExecuteAll();

        Recommendation result;

        result.geo = geo_task.GetResultSync<Geo>();
        result.weather = weather_task.GetResultSync<Weather>();
        result.category = category_task.GetResultSync<std::string>();
        result.places = place_task.GetResultSync<std::vector<Place>>();

        return result;
    }  
};