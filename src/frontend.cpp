#include <fstream>
#include <ncurses.h>
#include "src/backend.cpp"
#include <vector>

std::string day_of_week(int year, int month, int day) {
    const std::string days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

    const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    for (int i = 0; i < month - 1; ++i) {
        day += days_in_month[i];
    }

    // дополнительный день для високосных годов, если текущий год високосный и месяц больше февраля
    if (month > 2 && (year % 4 == 0 && year % 100 != 0 || year % 400 == 0)) {
        ++day;
    }

    return days[day % 7];
}

void PrintError(std::exception& e) {
    clear();

    mvprintw(LINES / 2 - 2, (COLS - strlen("Error: ") - strlen(e.what())) / 2, "Error: %s", e.what());
    mvprintw(LINES / 2, (COLS - strlen("Please try again")) / 2, "Please try again");
    mvprintw(LINES / 2 + 2, (COLS - strlen("Press any button to continue or ESC to exit")) / 2,
             "Press any button to continue or ESC to exit");

    refresh();

    if (getch() == 27) {
        endwin();
        exit(0);
    }
}

void PrintError(std::exception& e, std::string user_input) {
    clear();

    mvprintw(LINES / 2 - 3, (COLS - strlen("Error: The city was not found") - strlen(e.what())) / 2, "Error: The city was not found");
    mvprintw(LINES / 2 - 1, (COLS - strlen("Your input: ") - strlen(user_input.c_str())) / 2, "Your input: %s", user_input.c_str());
    mvprintw(LINES / 2 + 1, (COLS - strlen("Please try again")) / 2, "Please try again");
    mvprintw(LINES / 2 + 3, (COLS - strlen("Press any button to continue or ESC to exit")) / 2,
             "Press any button to continue or ESC to exit");

    refresh();

    if (getch() == 27) {
        endwin();
        exit(0);
    }
}

void PrintConfigError(std::exception& e, bool& use_default_config) {
    clear();

    mvprintw(LINES / 2 - 4, (COLS - strlen("Error: Error openning the config file")) / 2, "Error: Error openning the config file");
    mvprintw(LINES / 2 - 2, (COLS - strlen("You can continue with default configuration data")) / 2, "You can continue with default configuration data");
    mvprintw(LINES / 2, (COLS - strlen("Press D for use default config")) / 2, "Press D for use default config");
    mvprintw(LINES / 2 + 2, (COLS - strlen("Press any button to try to load the config again")) / 2, "Press any button to try to load the config again");
    mvprintw(LINES / 2 + 4, (COLS - strlen("Press ESC to exit")) / 2, "Press ESC to exit");

    refresh();

    switch (getch()) {
        case 27:
            endwin();
            exit(0);
        case 'd':
            use_default_config = true;
            break;
        default:
            break;
    }
}

Json::Value ImportConfig() {
    std::ifstream config_file("../config.json");

    bool use_default_config = false;
    while (!config_file.is_open() && !use_default_config) {
        std::runtime_error e = std::runtime_error("Error openning the config file");
        PrintConfigError(e, use_default_config);
    }

    Json::Value config;
    Json::Reader reader;

    if (use_default_config) {
        reader.parse("{ \"data_update_frequency\" : 60, \"days_for_forecast\": 3 }", config);
        return config;
    }

    std::string json_string_config((std::istreambuf_iterator<char>(config_file)), std::istreambuf_iterator<char>());

    config_file.close();

    while (!reader.parse(json_string_config, config)) {
        std::runtime_error e = std::runtime_error("Config file parsing error");
        PrintError(e);
    }

    return config;
}

void AddCity(std::vector<Forecast>& cities) {
    clear();

    nocbreak();
    echo();
    notimeout(stdscr, true);

    mvprintw(LINES / 2, (COLS - strlen("Enter the name of the city: ")) / 2 - 5, "Enter the name of the city: ");
    refresh();

    char bufer[100];
    getstr(bufer);
    std::string city_name(bufer);

    cbreak();
    noecho();

    try {
        bool is_city_exists = false;
        for (int i = 0; i < cities.size(); ++i) {
            if (cities[i].GetName() == city_name) {
                is_city_exists = true;
                break;
            }
        }
        if (!is_city_exists) {
            cities.push_back(GetForecast(city_name));
        }
    } catch (std::exception& e) {
        if (!strcmp(e.what(), "400")) {
            PrintError(e, city_name);
        } else {
            PrintError(e);
        }

        AddCity(cities);
    }
}

void PrintWeather(Forecast& city, int days_for_forecast, int data_update_frequency) {
    if (difftime(city.loading_time_, time(NULL)) > data_update_frequency) {
        city = GetForecast(city.city_name_);
    }
    clear();
    printw("\n");
    std::string head = "Weather forecast for " + city.city_name_;
    mvprintw(getcury(stdscr), (COLS - strlen(head.c_str())) / 2, head.c_str());
    printw("\n%s", city.current_weather_.weather_state.c_str());
    printw("\n%d°C (%d)", city.current_weather_.temperature, city.current_weather_.apparent_temperature);
    printw("\n\n\n\n");
    for (int i = 0; i < days_for_forecast; ++i) {
        std::string date =
                day_of_week(city.forecast_[i].date.year, city.forecast_[i].date.month, city.forecast_[i].date.day) +
                ". " + std::to_string(city.forecast_[i].date.day) + ' ' + MONTHS_NAMES[city.forecast_[i].date.month] +
                ' ' + std::to_string(city.forecast_[i].date.year);
        mvprintw(getcury(stdscr), (COLS - strlen(std::string(strlen(date.c_str()) + 6, '-').c_str())) / 2 - 1,
                 std::string(strlen(date.c_str()) + 6, '-').c_str());
        printw("\n");

        std::string half_column = std::string((COLS - strlen("MORNINGAFTERNOONEVENINGNIGHT||||")) / 8, '-');
        std::string remain_column = std::string(strlen(half_column.c_str()) - (strlen(date.c_str()) / 2 + 3), '-');
        int column1 = strlen(half_column.c_str()) * 2 + strlen("MORNING");
        int column2 = strlen(half_column.c_str()) * 2 + strlen("AFTERNOON");
        int column3 = strlen(half_column.c_str()) * 2 + strlen("EVENING");
        int column4 = strlen(half_column.c_str()) * 2 + strlen("NIGHT");

        mvprintw(getcury(stdscr), 0, "%sMORNING%s|%sAFTERNOON%s|  %s  |%sEVENING%s|%sNIGHT%s",
                 half_column.c_str(),
                 half_column.c_str(),
                 half_column.c_str(),
                 remain_column.c_str(),
                 date.c_str(),
                 remain_column.c_str(),
                 half_column.c_str(),
                 half_column.c_str(),
                 half_column.c_str());
        printw("\n%s|%s%s|%s%s|%s", std::string(column1, ' ').c_str(),
               std::string(strlen(half_column.c_str()) + strlen("AFTERNOON") + strlen(remain_column.c_str()), ' ').c_str(),
               std::string(strlen(date.c_str()) / 2 + 2, '-').c_str(), std::string(strlen(date.c_str()) / 2 + 3, '-').c_str(), std::string(
                        strlen(remain_column.c_str()) + strlen("EVENING") + strlen(half_column.c_str()), ' ').c_str(),
               std::string(strlen(half_column.c_str()) * 2 + strlen("NIGHT"), ' ').c_str());
        for (int y = 0; y < 4; ++y) {
            printw("\n");

            if (y == 0) {
                int morning_offset = (column1 - strlen(city.forecast_[i].morning.weather_state.c_str())) / 2;
                int afternoon_offset = (column2 - strlen(city.forecast_[i].afternoon.weather_state.c_str())) / 2 + column1 + 1;
                int evening_offset = (column3 - strlen(city.forecast_[i].evening.weather_state.c_str())) / 2 + column1 + column2 + 2;
                int night_offset = (column4 - strlen(city.forecast_[i].night.weather_state.c_str())) / 2 + column1 + column2 + column3 + 3;

                mvprintw(getcury(stdscr), morning_offset, city.forecast_[i].morning.weather_state.c_str());
                mvprintw(getcury(stdscr), column1, "|");
                mvprintw(getcury(stdscr), afternoon_offset, city.forecast_[i].afternoon.weather_state.c_str());
                mvprintw(getcury(stdscr), column1 + column2, "|");
                mvprintw(getcury(stdscr), evening_offset, city.forecast_[i].evening.weather_state.c_str());
                mvprintw(getcury(stdscr), column1 + column2 + column3 + 1, "|");
                mvprintw(getcury(stdscr), night_offset, city.forecast_[i].night.weather_state.c_str());

            } else if (y == 1) {
                std::string morning_temperature = std::to_string(city.forecast_[i].morning.temperature) + "°C (" + std::to_string(city.forecast_[i].morning.apparent_temperature) + ')';
                std::string afternoon_temperature = std::to_string(city.forecast_[i].afternoon.temperature) + "°C (" + std::to_string(city.forecast_[i].afternoon.apparent_temperature) + ')';
                std::string evening_temperature = std::to_string(city.forecast_[i].evening.temperature) + "°C (" + std::to_string(city.forecast_[i].evening.apparent_temperature) + ')';
                std::string night_temperature = std::to_string(city.forecast_[i].night.temperature) + "°C (" + std::to_string(city.forecast_[i].night.apparent_temperature) + ')';

                mvprintw(getcury(stdscr), (column1 - strlen(morning_temperature.c_str())) / 2, morning_temperature.c_str());
                mvprintw(getcury(stdscr), column1, "|");
                mvprintw(getcury(stdscr), (column2 - strlen(afternoon_temperature.c_str())) / 2 + column1 + 1, afternoon_temperature.c_str());
                mvprintw(getcury(stdscr), column1 + column2, "|");
                mvprintw(getcury(stdscr), (column3 - strlen(evening_temperature.c_str())) / 2 + column1 + column2 + 2, evening_temperature.c_str());
                mvprintw(getcury(stdscr), column1 + column2 + column3 + 1, "|");
                mvprintw(getcury(stdscr), (column4 - strlen(night_temperature.c_str())) / 2 + column1 + column2 +column3 + 3, night_temperature.c_str());

            } else {
                printw("%s|%s|%s|%s", std::string(column1, ' ').c_str(), std::string(column2 - 1, ' ').c_str(), std::string(column3, ' ').c_str(), std::string(column4, ' ').c_str());
            }
        }
        mvprintw(getcury(stdscr), 0, "%s|%s|%s|%s", std::string(column1, '-').c_str(), std::string(column2 - 1, '-').c_str(), std::string(column3, '-').c_str(), std::string(column4, '-').c_str());
        printw("\n\n\n\n\n\n\n");
        refresh();
    }
}

void Run() {
    initscr();
    scrollok(stdscr, TRUE);
    idlok(stdscr, TRUE);
    cbreak();
    noecho();

    std::vector<Forecast> cities;
    int index_selected = 0;


    Json::Value config = ImportConfig();

    int days_for_forecast = config["days_for_forecast"].asInt();
    if (days_for_forecast < 0) days_for_forecast = 0;
    if (days_for_forecast > 16) days_for_forecast = 16;

    int data_update_frequency = config["data_update_frequency"].asInt();

    AddCity(cities);
    PrintWeather(cities.back(), days_for_forecast, data_update_frequency);

    while(true) {
        timeout(data_update_frequency * 1000);

        switch(getch()) {
            case 27:
                endwin();
                return;
            case ERR:
                PrintWeather(cities[index_selected], days_for_forecast, data_update_frequency);
                break;
            case 'n':
                if (index_selected == cities.size() - 1) {
                    AddCity(cities);
                    PrintWeather(cities.back(), days_for_forecast, data_update_frequency);
                    ++index_selected;
                } else {
                    PrintWeather(cities[++index_selected], days_for_forecast, data_update_frequency);
                }
                break;
            case 'p':
                if (index_selected == 0) {
                    PrintWeather(cities[cities.size() - 1], days_for_forecast, data_update_frequency);
                    index_selected = cities.size() - 1;
                } else {
                    PrintWeather(cities[--index_selected], days_for_forecast, data_update_frequency);
                }
                break;
            case '+':
                if (days_for_forecast < 16) {
                    PrintWeather(cities[index_selected], ++days_for_forecast, data_update_frequency);
                }
                break;
            case '-':
                if (days_for_forecast > 0) {
                    PrintWeather(cities[index_selected], --days_for_forecast, data_update_frequency);
                }
                break;
            default:
                PrintWeather(cities[index_selected], days_for_forecast, data_update_frequency);
                break;
        }
    }
}