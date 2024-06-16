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

        std::cout << "Number of stations: " << stations->size() << std::endl;
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
    std::cout << std::format("File for station {}: {}\n", stationId, csv_filename);
    BOOST_CHECK_EQUAL("../../data/GME00102380_2024-05-31.csv", csv_filename);

}

BOOST_AUTO_TEST_CASE(nearest_stations)
{
    const double latitude = 49.47020;
    const double longitude = 10.99019;

    std::string filename{"../../data/ghcnd-stations_gm.txt"};
    if (std::ifstream inStream{filename, std::ios::in}) {
        auto stations = readStations(inStream);
        BOOST_CHECK_EQUAL(1124, stations->size());
        //auto nearest_stations = getNearestStations(stations, latitude, longitude);
    } else {
        std::string msg = std::format("Opening {} failed", filename);
        BOOST_FAIL(msg);
    }
}

BOOST_AUTO_TEST_CASE(distance_on_earth)
{
    // Distance between Fuerth and Zuerich
    double dist1 = haversine(49.4739, 47.3831, 10.9982, 8.5667);
    double dist2 = haversine(47.3831, 49.4739, 8.5667, 10.9982);
    std::cout << std::format("Distance Zuerich - Fuerth: {} km\n", dist2);

    BOOST_CHECK_EQUAL("293.966657", std::format("{:.6f}", dist1));
    BOOST_CHECK_EQUAL(dist1, dist2);
}

BOOST_AUTO_TEST_SUITE_END()
