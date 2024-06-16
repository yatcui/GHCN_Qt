/*
    Functions with template definitions should be in header files, because they must be 
    available to the compiler when specializations are instantiated. 
    Otherwise they may not be available in the object code => linker error.
    
    See: https://stackoverflow.com/questions/64544744/g-skipping-a-function-when-compiling-to-object-file
    
    
    "A function template by itself is not a type, or a function, or any other entity. 
    No code is generated from a source file that contains only template definitions. 
    In order for any code to appear, a template must be instantiated: the template arguments 
    must be determined so that the compiler can generate an actual function (or class, from a class template)."
    
    Here "getYearlyAverages()" is not available in object code, if it is located in a separate
    compilation unit (the other functions are). May be due to "auto" in function definition?

*/

#include <iostream>
#include <numeric>
#include <ranges>
#include <string>
#include <regex>
#include <memory>
#include <map>
#include <algorithm>
#include <iterator>
#include <span>
#include <fstream>
#include <filesystem>

#include "measurement.hpp"
#include "station.hpp"


/*
IV. FORMAT OF "ghcnd-stations.txt"

------------------------------
Variable   Columns   Type
------------------------------
ID            1-11   Character
LATITUDE     13-20   Real
LONGITUDE    22-30   Real
ELEVATION    32-37   Real
STATE        39-40   Character
NAME         42-71   Character
GSN FLAG     73-75   Character
HCN/CRN FLAG 77-79   Character
WMO ID       81-85   Character
------------------------------
*/
std::unique_ptr<std::vector<Station>>
readStations(std::ifstream& inStream)
{
    auto stations = std::make_unique<std::vector<Station>>();
    std::string line;
    while (std::getline(inStream, line)) {
        std::string id = line.substr(0, 11);
        double latitude = std::stod(line.substr(12, 8));
        double longitude = std::stod(line.substr(21, 9));
        double elevation = std::stod(line.substr(31, 6));
        std::string name = line.substr(41, 30);
        // Remove trailing whitespace
        size_t endpos = name.find_last_not_of(" \t\n\r");
        if (std::string::npos != endpos)
            name.erase(endpos + 1);
        stations->push_back(Station(id, latitude, longitude, elevation, name));
    }
    return stations;
}

std::unique_ptr<std::vector<Measurement>>
readMeasurementsForStation(std::ifstream& inStream)
{
    auto measurements = std::make_unique<std::vector<Measurement>>();
    std::string line;
    std::smatch match;
    // Matches at least one subexpression "everything but comma" to the left.
    std::regex item{"[^,]+"};  
    while (std::getline(inStream, line)) {
        // Station ID (skipped)
        std::regex_search(line, match, item);
        line = match.suffix();  // Rest of string not yet searched.
        // Date
        std::regex_search(line, match, item);
        std::string date{match.str()};
        line = match.suffix();
        // Element 
        std::regex_search(line, match, item);
        std::string element{match.str()};
        line = match.suffix();
        // Value
        std::regex_search(line, match, item);
        int value = stoi(match.str());
        measurements->push_back(Measurement(date, value, element));
    }
    return measurements;
}


std::span<const Measurement>
getMeasurementsForYearSpan(const std::unique_ptr<std::vector<Measurement>>& measurements, int startYear, int endYear)
{
    const std::vector<Measurement>& data = *measurements;
    
    // Determines iterator to start year.
    auto startIter = std::ranges::find_if(data, [startYear](auto m) {return m.getYear() == startYear;});
    if (startIter == data.end()) {
        // End year not found => return empty span.
        return std::span<Measurement>();
    }

    // Determines iterator to end year.
    auto it = std::ranges::find_if(data | std::ranges::views::reverse, [endYear](auto m) {return m.getYear() == endYear;});
    auto endIter = data.end();  // Reverse search from end.
    if (it != data.rend()) { 
        endIter = std::next(it).base();  // Get forward iterator to end of span.
    }
    
    if (startIter != data.end() && endIter != data.end()) {
        return std::span(startIter, endIter);
    }
    else {
        // End year not found => return empty span.
        return std::span<Measurement>();
    }
}


std::unique_ptr<std::map<int, float>>
getYearlyAverages(auto filtered_interval, const float& scaling)
{
    // map sorts entries in ascending order based on key (which is the year here). 
    auto yearlyAverages = std::make_unique<std::map<int, float>>();
    auto it = filtered_interval.begin();
    while (it != filtered_interval.end()) {
        int year{it->getYear()};
        // Points after last measurement for current year.
        auto last = std::find_if(it, filtered_interval.end(), [year](const auto& m){return m.getYear() != year;});
        // Adds up measurement values and calculates average. Scaling before division to prevent rounding errors.
        float average = std::accumulate(it, last, 0, [](int sum, Measurement m){return sum + m.getValue();}) * scaling /
                        (std::ranges::distance(it, last));
        //std::cout << std::format("{} values in {}\n", std::ranges::distance(it, last), year);
        (*yearlyAverages)[year] = average; // O(1) for unordered_map, O(log n) for ordered map.
        it = last;
    }
    return yearlyAverages;
}


std::unique_ptr<std::map<int, float>>
getMontlyAverages(auto filtered_interval, const float& scaling, const int& year)
{
    auto monthlyAverages = std::make_unique<std::map<int, float>>();
    // Start: Iterator to year.
    auto it = std::ranges::find_if(filtered_interval, [year](auto m) {return m.getYear() == year;});
    if (it == filtered_interval.end()) {
        // Year not found => return empty map.
        return monthlyAverages;
    }
    auto last = std::find_if(it, filtered_interval.end(), [year](const auto& m){return m.getYear() != year;});        
    // Add up measurements for each month in year.
    while (it != last) {
        int month{it->getMonth()};
        auto last_day = std::find_if(it, filtered_interval.end(), [month](const auto& m){return m.getMonth() != month;});
        float average = std::accumulate(it, last_day, 0, [](int sum, Measurement m){return sum + m.getValue();}) * scaling /
                        (std::ranges::distance(it, last_day));
        //std::cout << std::format("{} values in {}\n", std::ranges::distance(it, last_day), month);
        (*monthlyAverages)[month] = average;
        it = last_day;
    }
    return monthlyAverages;
}


std::unique_ptr<std::map<int, float>>
getDailyValues(auto filtered_interval, const float& scaling, const int& year, const int& month)
{
    auto dailyValues = std::make_unique<std::map<int, float>>();
    // Determines iterator to year.
    auto it = std::ranges::find_if(filtered_interval, [year](auto m) {return m.getYear() == year;});
    if (it == filtered_interval.end()) {
        // year not found => return empty map.
        return dailyValues;
    }
    // Advances iterator to month.
    while (it != filtered_interval.end() && it->getMonth() != month) {
        ++it;
    }
    if (it == filtered_interval.end()) {
        // month not found => return empty map.
        return dailyValues;
    }
    while (it != filtered_interval.end() && it->getMonth() == month) {
        (*dailyValues)[it->getDay()] = it->getValue() * scaling;
        ++it;
    }
    return dailyValues;
}


const std::string
csvFilenameFromStationId(const std::string& station_id)
{
    const std::filesystem::path data_dir{"../../data/"};
    const std::string csv_ext{".csv"};
    std::vector<std::string> fileNames;
    for (auto const& entry : std::filesystem::directory_iterator{data_dir}) {
        if (!entry.is_directory()) {
            const std::string stem = entry.path().filename().stem().string();
            const std::string ext = entry.path().filename().extension().string();
            if (ext == csv_ext && stem.starts_with(station_id)) {
                fileNames.push_back(stem);
            }
        }
    }
    if (fileNames.size() == 0) {
        // station not found => return empty string.
        return std::string("");
    }
    std::sort(fileNames.begin(), fileNames.end());
    std::string filename = data_dir.string() + fileNames.back() + csv_ext;
    return filename;
}


std::unique_ptr<std::map<int, float>>
getYearlyAveragesForSpan(const std::string& station_id, int startYear, int endYear, const MeasurementType& type)
{
    std::string filename = csvFilenameFromStationId(station_id);
    if (filename.size() == 0) {
        return std::make_unique<std::map<int, float>>();
    }

    if (std::ifstream inStream{filename, std::ios::in}) {
        auto measurements = readMeasurementsForStation(inStream);
        inStream.close();
        auto interval = getMeasurementsForYearSpan(measurements, startYear, endYear);
        auto type{MeasurementType::TMAX};
        auto filtered_interval{interval | std::views::filter([type](auto m) {return m.getType() == type;})};
        float scaling = Measurement::getScalingForType(type);
        return getYearlyAverages(filtered_interval, scaling);
    } else {
        return std::make_unique<std::map<int, float>>();
    }
}
