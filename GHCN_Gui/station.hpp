#ifndef STATION_HPP
#define STATION_HPP

#include <string>

/*
       Station data according to GHCN readme.txt, retrieved from

       https://www1.ncdc.noaa.gov/pub/data/ghcn/daily/readme.txt

       at June 14, 2024:

    ID          Station identification code.
                - First two chars: FIPS country code
                - Third char: network code that identifies the station numbering system
                - Remaining eight chars: actual station ID
    LATITUDE     latitude (in decimal degrees).
    LONGITUDE    longitude (in decimal degrees).
    ELEVATION    elevation (in meters, missing = -999.9).
    NAME         Name of station
*/


class Station
{
public:
    Station(const std::string& id, const double& latitude, const double& longitude, const double& elevation, const std::string& name);

    const std::string& getId() const;

    const double& getLatitude() const;

    const double& getLongitude() const;

    const double& getElevation() const;

    const std::string& getName() const;

protected:
    std::string m_id;
    double m_latitude;
    double m_longitude;
    double m_elevation;
    std::string m_name;

};

#endif // STATION_HPP
