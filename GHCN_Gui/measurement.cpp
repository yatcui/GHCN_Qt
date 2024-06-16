#include <string>
#include <map>

#include "measurement.hpp"

using namespace std;

Measurement::Measurement(const string& date, const int& value, const string& element)
    : m_value{value} {
    
        m_year = stoi(date.substr(0, 4));
        m_month = stoi(date.substr(4, 2));
        m_day = stoi(date.substr(6, 2));
        if (s_mapStringMeasurementType.find(element) == s_mapStringMeasurementType.end()) {
            m_type = MeasurementType::UNKNOWN;
            // cout << format("Unknown measurement type {}\n", element);
            } 
        else {
            m_type = s_mapStringMeasurementType[element];
            }
    }
    
const int& Measurement::getYear() const {
    return m_year;
    }

const int& Measurement::getMonth() const {
    return m_month;
    }

const int& Measurement::getDay() const {
    return m_day;
    }
    
const int& Measurement::getValue() const {
    return m_value;
    }

const MeasurementType& Measurement::getType() const {
    return m_type;
    }

const float& Measurement::getScalingForType(const MeasurementType& type) {
    return s_mapMeasurementScaling[type];    
    }
    
map<std::string, MeasurementType> Measurement::s_mapStringMeasurementType = {
    {"PRCP", MeasurementType::PRCP},
    {"SNOW", MeasurementType::SNOW},
    {"SNWD", MeasurementType::SNWD},
    {"TMIN", MeasurementType::TMIN},
    {"TMAX", MeasurementType::TMAX}
};
    
                                                                   
map<MeasurementType, float> Measurement::s_mapMeasurementScaling = {
    {MeasurementType::PRCP, 0.1f},
    {MeasurementType::SNOW, 0.1f},
    {MeasurementType::SNWD, 0.1f},
    {MeasurementType::TMIN, 0.1f},
    {MeasurementType::TMAX, 0.1f}
};

