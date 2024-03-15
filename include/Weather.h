#ifndef LABWORK7_YURYS0_WEATHER_H
#define LABWORK7_YURYS0_WEATHER_H

#include <string>
#include <map>

static std::map<int, std::string> WEATHER_CODES = {
        {0, "Clear"},
        {1, "Cloudy"},
        {2, "Partly Cloudy"},
        {3, "Scattered Clouds"},
        {45, "Foggy"},
        {48, "Foggy"},
        {51, "Light Drizzle"},
        {53, "Moderate Drizzle"},
        {55, "Heavy Drizzle"},
        {56, "Light Freezing Drizzle"},
        {57, "Heavy Freezing Drizzle"},
        {61, "Light Rain"},
        {63, "Moderate Rain"},
        {65, "Heavy Rain"},
        {66, "Light Freezing Rain"},
        {67, "Heavy Freezing Rain"},
        {71, "Light Snow"},
        {73, "Moderate Snow"},
        {75, "Heavy Snow"},
        {77, "Snow Grains"},
        {80, "Light Rain Showers"},
        {81, "Moderate Rain Showers"},
        {82, "Violent Rain Showers"},
        {85, "Light Snow Showers"},
        {86, "Heavy Snow Showers"},
        {95, "Slight Thunderstorm"},
        {96, "Thunderstorm with Hail"},
        {99, "Heavy Thunderstorm with Hail"}
};

static std::map<int, std::string> WIND_DIRECTIONS = {
        {0, "North"}, {45, "North-east"}, {90, "East"}, {135, "South-east"},
        {180, "South"}, {225, "South-west"}, {270, "West"}, {315, "North-west"}
};

struct Weather {
    int temperature;
    int apparent_temperature;
    int weather_code;
    std::string weather_state;
    int wind_speed;
    int wind_direction_degrees;
    std::string wind_direction;
    int precipitation;
};

#endif //LABWORK7_YURYS0_WEATHER_H
