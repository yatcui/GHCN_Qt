#include <string>
#include <format>
#include <fstream>
#include <iostream>

#define BOOST_TEST_MODULE GHCN_Gui_Test
#include <boost/test/included/unit_test.hpp>

#include "ghcn_dataprovider.hpp"

BOOST_AUTO_TEST_SUITE(ghcn_dataprovider)

BOOST_AUTO_TEST_CASE(read_stations)
{
    std::string filename{"../../data/ghcnd-stations_1st_ten.txt"};
    if (std::ifstream inStream{filename, std::ios::in}) {
        auto stations = readStations(inStream);

        // std::cout << "Number of stations: " << stations->size() << std::endl;
        BOOST_CHECK_EQUAL(10, stations->size());

        Station fst_station = stations->at(0);
        BOOST_CHECK_EQUAL("ACW00011604", fst_station.getId());
        BOOST_CHECK_EQUAL(17.1167, fst_station.getLatitude());
        BOOST_CHECK_EQUAL(-61.7833, fst_station.getLongitude());
        BOOST_CHECK_EQUAL(10.1, fst_station.getElevation());
        BOOST_CHECK_EQUAL("ST JOHNS COOLIDGE FLD", fst_station.getName());
    } else {
        std::string msg = std::format("Opening {} failed", filename);
        BOOST_FAIL(msg);
    }
}

BOOST_AUTO_TEST_CASE(csv_filename)
{
    const std::string stationId{"GME00102380"};
    std::string csv_filename = csvFilenameFromStationId(stationId);
    // std::cout << std::format("File for station {}: {}\n", stationId, csv_filename);
    BOOST_CHECK_EQUAL("../../data/GME00102380_2024-05-31.csv", csv_filename);
}

BOOST_AUTO_TEST_CASE(nearest_stations)
{
    // Fuerth
    constexpr double latitude = 49.47020;
    constexpr double longitude = 10.99019;

    std::string filename{"../../data/ghcnd-stations_gm.txt"};
    if (std::ifstream inStream{filename, std::ios::in}) {
        auto stations = readStations(inStream);
        BOOST_CHECK_EQUAL(1124, stations->size());
        // (index, distance) pairs for stations within radius of 50 km around Fuerth.
        auto nearestStations = calcNearestStations(stations, latitude, longitude, 50);
        std::pair<int, double> top = nearestStations->at(0);
        Station nearest = stations->at(top.first);
        BOOST_CHECK_EQUAL("GME00122614", nearest.getId());
        BOOST_CHECK_EQUAL("2.2", std::format("{:.1f}", top.second));
        // for (auto p : *nearestStations) {
        //     std::cout << std::format("{} at {:.1f} km\n", stations->at(p.first).getId(), p.second);
        // }
    } else {
        std::string msg = std::format("Opening {} failed", filename);
        BOOST_FAIL(msg);
    }
}

BOOST_AUTO_TEST_CASE(api_nearest_stations)
{
    // Fuerth
    double latitude = 49.47020;
    double longitude = 10.99019;
    int radius = 50;
    std::string station_file_name{STATION_FILE_NAME};
    STATION_FILE_NAME = "ghcnd-stations_gm.txt";
    auto nearestStations = getNearestStations(latitude, longitude, radius);
    BOOST_CHECK_EQUAL("GME00122614", nearestStations->at(0).first);
    BOOST_CHECK_EQUAL("2.2", std::format("{:.1f}", nearestStations->at(0).second));
    // for (std::pair<std::string, double> p : *nearestStations) {
    //     std::cout << std::format("{} at {:.1f} km\n", p.first, p.second);
    // }
    STATION_FILE_NAME = station_file_name;
}

BOOST_AUTO_TEST_CASE(api_yearly_averages)
{
    const std::string stationId{"GME00102380"};
    auto yearlyAverages = getYearlyAverages(stationId, 1960, 2000, MeasurementType::TMAX);
    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*yearlyAverages)[1960]), "13.5");
    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*yearlyAverages)[2000]), "14.7");
    // for (auto p : *yearlyAverages) {
    //     std::cout << std::format("{} {:.1f}\n", p.first, p.second);
    // }
}

BOOST_AUTO_TEST_CASE(api_montly_averages)
{
    const std::string stationId{"GME00102380"};
    auto monthlyAverages = getMonthlyAverages(stationId, 2000, MeasurementType::TMAX);
    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*monthlyAverages)[1]), "2.7");
    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*monthlyAverages)[12]), "5.2");
    // for (auto p : *monthlyAverages) {
    //     std::cout << std::format("{} {:.1f}\n", p.first, p.second);
    // }
}

BOOST_AUTO_TEST_CASE(api_daily_values)
{
    const std::string stationId{"GME00102380"};
    auto dailyValues = getDailyValues(stationId, 2000, 12, MeasurementType::TMAX);
    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*dailyValues)[1]), "6.6");
    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*dailyValues)[31]), "0.9");
    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*dailyValues)[23]), "-4.3");
    // for (auto p : *dailyValues) {
    //     std::cout << std::format("{} {:.1f}\n", p.first, p.second);
    // }
}

BOOST_AUTO_TEST_CASE(distance_on_earth)
{
    // Distance between Fuerth and Zuerich
    double dist1 = haversine(49.4739, 47.3831, 10.9982, 8.5667);
    double dist2 = haversine(47.3831, 49.4739, 8.5667, 10.9982);
    // std::cout << std::format("Distance Zuerich - Fuerth: {} km\n", dist2);

    BOOST_CHECK_EQUAL("293.966657", std::format("{:.6f}", dist1));
    BOOST_CHECK_EQUAL(dist1, dist2);
}

BOOST_AUTO_TEST_SUITE_END()
