#ifndef LABWORK7_FORECAST_H
#define LABWORK7_FORECAST_H

#include "include/Day.h"
#include "json/json.h"
#include <ctime>

class Forecast {
private:
    std::string city_name_;
    Day forecast_[16];
    Weather current_weather_;
    time_t loading_time_;

    friend void WriteData(Forecast& city, Json::Value weather_forecast_data_json);
    friend void PrintWeather(Forecast& city, int days_for_forecast, int data_ipdate_frequency);
public:
    std::string GetName() {
        return city_name_;
    }
    Forecast(const std::string city_name) : city_name_(city_name) {};
};

#endif //LABWORK7_FORECAST_H