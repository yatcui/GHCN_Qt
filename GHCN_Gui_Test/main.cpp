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
    std::string filename{"../../data/ghcnd-stations_test.txt"};
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

BOOST_AUTO_TEST_SUITE_END()
