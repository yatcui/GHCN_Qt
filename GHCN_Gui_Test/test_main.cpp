#include <string>
#include <format>

#define BOOST_TEST_MODULE GHCN_Gui_Test
#include <boost/test/included/unit_test.hpp>

#include "dataprovider.hpp"

BOOST_AUTO_TEST_SUITE(public_api)

BOOST_AUTO_TEST_CASE(api_nearest_stations)
{
    // Fuerth
    double latitude = 49.47020;
    double longitude = 10.99019;
    int radius = 50;

    DataProvider dataProvider("../../data/", "ghcnd-stations_gm.txt", ".csv");
    auto nearestStations = dataProvider.getNearestStations(latitude, longitude, radius);

    BOOST_CHECK_EQUAL("GME00122614", nearestStations->at(0).first);
    BOOST_CHECK_EQUAL("2.2", std::format("{:.1f}", nearestStations->at(0).second));
    // for (std::pair<std::string, double> p : *nearestStations) {
    //     std::cout << std::format("{} at {:.1f} km\n", p.first, p.second);
    // }
}

BOOST_AUTO_TEST_CASE(api_yearly_averages)
{
    const std::string stationId{"GME00102380"};

    DataProvider dataProvider("../../data/", "ghcnd-stations_gm.txt", ".csv");
    auto yearlyAverages = dataProvider.getYearlyAverages(stationId, 1960, 2000, MeasurementType::TMAX);

    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*yearlyAverages)[1960]), "13.5");
    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*yearlyAverages)[2000]), "14.7");
    // for (auto p : *yearlyAverages) {
    //     std::cout << std::format("{} {:.1f}\n", p.first, p.second);
    // }

    auto yearlyAverages_1_12 = dataProvider.getAveragesForMonthRange(stationId, 1960, 2000, 1, 12, MeasurementType::TMAX);
    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*yearlyAverages)[1960]), "13.5");
    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*yearlyAverages)[2000]), "14.7");
}

BOOST_AUTO_TEST_CASE(api_yearly_averages_month_span)
{
    const std::string stationId{"GME00102380"};

    DataProvider dataProvider("../../data/", "ghcnd-stations_gm.txt", ".csv");

    auto yearlyAverages_12_2 = dataProvider.getAveragesForMonthRange(stationId, 1960, 2000, 12, 2, MeasurementType::TMAX);
    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*yearlyAverages_12_2)[1960]), "3.8");
    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*yearlyAverages_12_2)[1963]), "-2.2");
    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*yearlyAverages_12_2)[2000]), "4.8");

    auto yearlyAverages_3_5 = dataProvider.getAveragesForMonthRange(stationId, 1960, 2000, 3, 5, MeasurementType::TMAX);
    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*yearlyAverages_3_5)[1960]), "14.6");
    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*yearlyAverages_3_5)[2000]), "15.8");

    auto yearlyAverages_6_8 = dataProvider.getAveragesForMonthRange(stationId, 1960, 2000, 6, 8, MeasurementType::TMAX);
    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*yearlyAverages_6_8)[1960]), "22.3");
    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*yearlyAverages_6_8)[2000]), "23.2");

    auto yearlyAverages_9_11 = dataProvider.getAveragesForMonthRange(stationId, 1960, 2000, 9, 11, MeasurementType::TMAX);
    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*yearlyAverages_9_11)[1960]), "14.0");
    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*yearlyAverages_9_11)[2000]), "14.5");

}

BOOST_AUTO_TEST_CASE(api_montly_averages)
{
    const std::string stationId{"GME00102380"};

    DataProvider dataProvider("../../data/", "ghcnd-stations_gm.txt", ".csv");
    auto monthlyAverages = dataProvider.getMonthlyAverages(stationId, 2000, MeasurementType::TMAX);

    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*monthlyAverages)[1]), "2.7");
    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*monthlyAverages)[12]), "5.2");
    // for (auto p : *monthlyAverages) {
    //     std::cout << std::format("{} {:.1f}\n", p.first, p.second);
    // }
}

BOOST_AUTO_TEST_CASE(api_daily_values)
{
    const std::string stationId{"GME00102380"};

    DataProvider dataProvider("../../data/", "ghcnd-stations_gm.txt", ".csv");
    auto dailyValues = dataProvider.getDailyValues(stationId, 2000, 12, MeasurementType::TMAX);

    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*dailyValues)[1]), "6.6");
    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*dailyValues)[31]), "0.9");
    BOOST_CHECK_EQUAL(std::format("{:.1f}", (*dailyValues)[23]), "-4.3");
    // for (auto p : *dailyValues) {
    //     std::cout << std::format("{} {:.1f}\n", p.first, p.second);
    // }
}

BOOST_AUTO_TEST_SUITE_END()  // public_api
