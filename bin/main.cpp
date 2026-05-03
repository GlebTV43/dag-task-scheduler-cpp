#include "../src/controller/Controller.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

int main()
{   
    #ifdef _WIN32
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
    #endif
    try
    {
        GeoRequester geo_requester;
        WeatherRequester weather_requester;
        PlaceRequester places_requester;

        GeoRepository geo_repository(geo_requester);
        WeatherRepository weather_repository(weather_requester);
        PlaceRepository places_repository(places_requester);

        RecommendationService service(
            geo_repository,
            weather_repository,
            places_repository
        );

        Controller controller(service);

        controller.ShowRecommendation();
    }
    catch (const std::exception& error)
    {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
    catch (const MySTD::Exception& error)
    {
        std::cerr << "Scheduler error: " << error.What() << '\n';
        return 1;
    }

    return 0;
}