#include "station.hpp"

Station::Station(const std::string &id, const double &latitude, const double &longitude, const double &elevation, const std::string &name) :
    m_id(id),
    m_latitude(latitude),
    m_longitude(longitude),
    m_elevation(elevation),
    m_name(name)
{}

const std::string& Station::getId() const
{
    return m_id;
}

const double& Station::getLatitude() const
{
    return m_latitude;
}

const double& Station::getLongitude() const
{
    return m_longitude;
}

const double& Station::getElevation() const
{
    return m_elevation;
}

const std::string& Station::getName() const
{
    return m_name;
}
