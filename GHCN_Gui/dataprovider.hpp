#ifndef DATAPROVIDER_HPP
#define DATAPROVIDER_HPP

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <span>
#include <utility>

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

VII. FORMAT OF "ghcnd-inventory.txt"

    ------------------------------
    Variable   Columns   Type
    ------------------------------
    ID            1-11   Character
        LATITUDE     13-20   Real
        LONGITUDE    22-30   Real
        ELEMENT      32-35   Character
        FIRSTYEAR    37-40   Integer
        LASTYEAR     42-45   Integer
    ------------------------------


Meterological seasons:

Northern | Southern | Start | End
-----------------------------------------------------
Winter   | Summer   | 1 Dec | 28 Feb (29 if leap year)
Spring   | Autumn   | 1 Mar | 31 May
Summer   | Winter   | 1 Jun | 31 Aug
Autumn   | Spring   | 1 Sep | 30 Nov
*/

enum class Season
{
    WINTER,
    SPRING,
    SUMMER,
    AUTUMN,
    YEAR
};


class DataProvider
{
public:

    DataProvider(const std::string& dataDirName, const std::string& stationFileName, const std::string& inventoryFileName, const std::string& csvExt);

    std::unique_ptr<std::map<int, float>>
    getYearlyAverages(const std::string& stationId, int startYear, int endYear, const MeasurementType& type);

    std::unique_ptr<std::map<int, float>>
    getAveragesForMonthRange(const std::string& stationId, int startYear, int endYear, int startMonth, int endMonth, const MeasurementType& type);

    std::unique_ptr<std::map<int, float>>
    getMonthlyAverages(const std::string& stationId, int year, const MeasurementType& type);

    std::unique_ptr<std::map<int, float>>
    getDailyValues(const std::string& stationId, int year, int month, const MeasurementType& type);

    std::unique_ptr<std::vector<std::pair<std::string, double>>>
    getNearestStations(double latitude, double longitude, int radius);

    bool
    hasMeasurementsForYearRange(const std::string& stationId, int startYear, int endYear, MeasurementType type);

private:
    class InventoryEntry
    {
    public:
        InventoryEntry(const std::string& stationId, MeasurementType type, int startYear, int endYear)
            : m_stationId(stationId), m_type(type), m_startYear(startYear), m_endYear(endYear)
            {};

        const std::string& stationId() const {return m_stationId;};
        MeasurementType type() const {return m_type;};
        int startYear() const {return m_startYear;};
        int endYear() const {return m_endYear;};

    private:
        std::string m_stationId;
        MeasurementType m_type;
        int m_startYear;
        int m_endYear;
    };

private:

    const std::string m_dataDirName;
    const std::string m_stationFileName;
    const std::string m_inventoryFileName;
    const std::string m_csvExt;

    std::unique_ptr<std::vector<Station>> m_StationsCache;  // all available stations

    std::unique_ptr<std::vector<InventoryEntry>> m_stationInventory;

    // Measurements for previously accessed stations. TODO: LRU cache.
    std::map<std::string, std::unique_ptr<std::vector<Measurement>>> m_MeasurementsCache;

    bool readStations();
    bool readInventory();

    double haversine(double lat1,  double lat2, double lng1, double lng2);

    std::unique_ptr<std::vector<std::pair<int, double>>>
    calcNearestStations(double latitude, double longitude, int radius);

    const std::string csvFilenameFromStationId(const std::string& station_id);

    bool readMeasurementsForStation(const std::string& stationId);

    std::span<const Measurement>
    calcMeasurementSpanForYearRange(const std::unique_ptr<std::vector<Measurement>>& measurements, int startYear, int endYear);
};

#endif // DATAPROVIDER_HPP
