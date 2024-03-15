#include "include/Forecast.h"
#include <iostream>
#include <string>
#include "cpr/cpr.h"
#include "json/json.h"

std::string ReplaceSpacesForRequest(std::string str) {
    std::string result;
    for (char ch : str) {
        if (ch == ' ') {
            result += "%20";
        } else {
            result += ch;
        }
    }
    return result;
}

class ApiNinjas {
private:
    static std::string api_key_;
public:
    static Json::Value GetCityInformationJson(const std::string city_name) {
        cpr::Response r = cpr::Get(cpr::Url{"https://api.api-ninjas.com/v1/city?name=" + ReplaceSpacesForRequest(city_name)}, cpr::Header{{"X-Api-Key", api_key_}});
        if (r.status_code == 200) {
            Json::Value city_information;
            Json::Reader reader;
            if (reader.parse(r.text, city_information)) {
                return city_information[0];
            } else {
                throw std::runtime_error("400");
            }
        } else {
            throw std::runtime_error(std::to_string(r.status_code));
        }
    }
};

std::string ApiNinjas::api_key_ = "kVM1hqiDLiCF0AxNobdecQ==HEEOpqJBaAWpURsx";

class OpenMeteoApi {
public:
    static Json::Value GetForecastJson(const Json::Value city_information) {
        cpr::Response r = cpr::Get(cpr::Url{"https://api.open-meteo.com/v1/forecast?latitude=" + city_information["latitude"].asString() + "&longitude=" + city_information["longitude"].asString() + "&current=temperature_2m,apparent_temperature,precipitation,weather_code,wind_speed_10m,wind_direction_10m&hourly=temperature_2m,apparent_temperature,precipitation,weather_code,wind_speed_10m,wind_direction_10m&forecast_days=16"});
        if (r.status_code == 200) {
            Json::Value weather_forecast;
            Json::Reader reader;
            if (reader.parse(r.text, weather_forecast)) {
                return weather_forecast;
            } else {
                throw std::runtime_error("400");
            }
        } else {
            throw std::runtime_error(std::to_string(r.status_code));
        }
    }
};

void WriteData(Forecast& city, Json::Value weather_forecast_data_json) {
    city.loading_time_ = time(NULL);

    // current weather
    city.current_weather_.temperature = weather_forecast_data_json["current"]["temperature_2m"].asInt();
    city.current_weather_.apparent_temperature = weather_forecast_data_json["current"]["apparent_temperature"].asInt();
    city.current_weather_.precipitation = weather_forecast_data_json["current"]["precipitation"].asInt();
    city.current_weather_.weather_code = weather_forecast_data_json["current"]["weather_code"].asInt();
    city.current_weather_.weather_state = WEATHER_CODES[city.current_weather_.weather_code];
    city.current_weather_.wind_speed = weather_forecast_data_json["current"]["wind_speed_10m"].asInt();
    city.current_weather_.wind_direction_degrees = weather_forecast_data_json["current"]["wind_direction_10m"].asInt();
    city.current_weather_.wind_direction = WIND_DIRECTIONS[city.current_weather_.wind_direction_degrees / 45 * 45];

    // forecast
    for (int i = 0; i < 16; ++i) {
        city.forecast_[i] = Day(weather_forecast_data_json["hourly"]["time"][i * 24].asString());

        // morning
        int morning_time = i * 24 + 7;
        city.forecast_[i].morning.temperature = weather_forecast_data_json["hourly"]["temperature_2m"][morning_time].asInt();
        city.forecast_[i].morning.apparent_temperature = weather_forecast_data_json["hourly"]["apparent_temperature"][morning_time].asInt();
        city.forecast_[i].morning.precipitation = weather_forecast_data_json["hourly"]["precipitation"][morning_time].asInt();
        city.forecast_[i].morning.weather_code = weather_forecast_data_json["hourly"]["weather_code"][morning_time].asInt();
        city.forecast_[i].morning.weather_state = WEATHER_CODES[city.forecast_[i].morning.weather_code];
        city.forecast_[i].morning.wind_speed = weather_forecast_data_json["hourly"]["wind_speed_10m"][morning_time].asInt();
        city.forecast_[i].morning.wind_direction_degrees = weather_forecast_data_json["hourly"]["wind_direction_10m"][morning_time].asInt();
        city.forecast_[i].morning.wind_direction = WIND_DIRECTIONS[city.forecast_[i].morning.wind_direction_degrees / 45 * 45];

        // afternoon
        int afternoon_time = i * 24 + 14;
        city.forecast_[i].afternoon.temperature = weather_forecast_data_json["hourly"]["temperature_2m"][afternoon_time].asInt();
        city.forecast_[i].afternoon.apparent_temperature = weather_forecast_data_json["hourly"]["apparent_temperature"][afternoon_time].asInt();
        city.forecast_[i].afternoon.precipitation = weather_forecast_data_json["hourly"]["precipitation"][afternoon_time].asInt();
        city.forecast_[i].afternoon.weather_code = weather_forecast_data_json["hourly"]["weather_code"][afternoon_time].asInt();
        city.forecast_[i].afternoon.weather_state = WEATHER_CODES[city.forecast_[i].afternoon.weather_code];
        city.forecast_[i].afternoon.wind_speed = weather_forecast_data_json["hourly"]["wind_speed_10m"][afternoon_time].asInt();
        city.forecast_[i].afternoon.wind_direction_degrees = weather_forecast_data_json["hourly"]["wind_direction_10m"][afternoon_time].asInt();
        city.forecast_[i].afternoon.wind_direction = WIND_DIRECTIONS[city.forecast_[i].afternoon.wind_direction_degrees / 45 * 45];

        // evening
        int evening_time = i * 24 + 19;
        city.forecast_[i].evening.temperature = weather_forecast_data_json["hourly"]["temperature_2m"][evening_time].asInt();
        city.forecast_[i].evening.apparent_temperature = weather_forecast_data_json["hourly"]["apparent_temperature"][evening_time].asInt();
        city.forecast_[i].evening.precipitation = weather_forecast_data_json["hourly"]["precipitation"][evening_time].asInt();
        city.forecast_[i].evening.weather_code = weather_forecast_data_json["hourly"]["weather_code"][evening_time].asInt();
        city.forecast_[i].evening.weather_state = WEATHER_CODES[city.forecast_[i].evening.weather_code];
        city.forecast_[i].evening.wind_speed = weather_forecast_data_json["hourly"]["wind_speed_10m"][evening_time].asInt();
        city.forecast_[i].evening.wind_direction_degrees = weather_forecast_data_json["hourly"]["wind_direction_10m"][evening_time].asInt();
        city.forecast_[i].evening.wind_direction = WIND_DIRECTIONS[city.forecast_[i].evening.wind_direction_degrees / 45 * 45];

        // night
        int night_time = i * 24 + 23;
        city.forecast_[i].night.temperature = weather_forecast_data_json["hourly"]["temperature_2m"][night_time].asInt();
        city.forecast_[i].night.apparent_temperature = weather_forecast_data_json["hourly"]["apparent_temperature"][night_time].asInt();
        city.forecast_[i].night.precipitation = weather_forecast_data_json["hourly"]["precipitation"][night_time].asInt();
        city.forecast_[i].night.weather_code = weather_forecast_data_json["hourly"]["weather_code"][night_time].asInt();
        city.forecast_[i].night.weather_state = WEATHER_CODES[city.forecast_[i].night.weather_code];
        city.forecast_[i].night.wind_speed = weather_forecast_data_json["hourly"]["wind_speed_10m"][night_time].asInt();
        city.forecast_[i].night.wind_direction_degrees = weather_forecast_data_json["hourly"]["wind_direction_10m"][night_time].asInt();
        city.forecast_[i].night.wind_direction = WIND_DIRECTIONS[city.forecast_[i].night.wind_direction_degrees / 45 * 45];
    }
}

Forecast GetForecast(const std::string city_name) {
    Forecast city(city_name);
    WriteData(city, OpenMeteoApi::GetForecastJson(ApiNinjas::GetCityInformationJson(city_name)));
    return city;
}