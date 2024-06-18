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
*/

class DataProvider
{
public:

    DataProvider(const std::string& dataDirName, const std::string& stationFileName, const std::string& csvExt);

    std::unique_ptr<std::map<int, float>>
    getYearlyAverages(const std::string& station_id, int startYear, int endYear, const MeasurementType& type);

    std::unique_ptr<std::map<int, float>>
    getMonthlyAverages(const std::string& station_id, int year, const MeasurementType& type);

    std::unique_ptr<std::map<int, float>>
    getDailyValues(const std::string& station_id, int year, int month, const MeasurementType& type);

    std::unique_ptr<std::vector<std::pair<std::string, double>>>
    getNearestStations(double latitude, double longitude, int radius);

private:

    const std::string m_dataDirName;
    const std::string m_stationFileName;
    const std::string m_csvExt;

    class MeasurementsCacheEntry
    {
        MeasurementsCacheEntry(const std::string& stationId,
                               const std::string& date,
                               std::unique_ptr<std::vector<Measurement>>& measurements)
            : m_stationId(stationId), m_date(date)
        {
            m_measurements = std::move(measurements);
        };

    public:
        const std::string m_stationId;
        const std::string m_date;
        std::unique_ptr<std::vector<Measurement>> m_measurements;
    };

    std::map<std::string, MeasurementsCacheEntry> m_MeasurementsCache;

    std::unique_ptr<std::vector<Station>>
    readStations(std::ifstream& inStream);

    double
    haversine(double lat1,  double lat2, double lng1, double lng2);

    std::unique_ptr<std::vector<std::pair<int, double>>>
    calcNearestStations(const std::unique_ptr<std::vector<Station>>& stations, double latitude, double longitude, int radius);

    const std::string
    csvFilenameFromStationId(const std::string& station_id);

    std::unique_ptr<std::vector<Measurement>>
    readMeasurementsForStation(const std::string& stationId);

    std::span<const Measurement>
    calcMeasurementSpanForYearRange(const std::unique_ptr<std::vector<Measurement>>& measurements, int startYear, int endYear);
};

#endif // DATAPROVIDER_HPP
