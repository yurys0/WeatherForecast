#ifndef LABWORK7_DAY_H
#define LABWORK7_DAY_H

#include "include/Weather.h"
#include "json/json.h"

static std::map<int, std::string> MONTHS_NAMES {
        {1, "Jan"},
        {2, "Feb"},
        {3, "Mar"},
        {4, "Apr"},
        {5, "May"},
        {6, "Jun"},
        {7, "Jul"},
        {8, "Aug"},
        {9, "Sep"},
        {10, "Oct"},
        {11, "Nov"},
        {12, "Dec"}
};

struct Date {
    int year;
    int month;
    int day;

    Date(const std::string datetime) {
        size_t delimiter1 = datetime.find('-');
        size_t delimiter2 = datetime.find('-', delimiter1 + 1);
        size_t delimiter3 = datetime.find('T');

        year = std::stoi(datetime.substr(0, delimiter1));
        month = std::stoi(datetime.substr(delimiter1 + 1, delimiter2 - delimiter1 - 1));
        day = std::stoi(datetime.substr(delimiter2 + 1, delimiter3 - delimiter2 - 1));
    };

    Date() {}
};

class Day {
public:
    Date date;
    Weather morning, afternoon, evening, night;

    Day(const std::string datetime) : date(datetime) {};
    Day() {}
};

#endif //LABWORK7_DAY_H
