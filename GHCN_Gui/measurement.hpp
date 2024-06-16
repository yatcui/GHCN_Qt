#ifndef MEASUREMENT_HPP
#define MEASUREMENT_HPP

#include <string>
#include <map>

/*
       Core elements of measurement according to GHCN readme.txt, retrieved from
       
       https://www1.ncdc.noaa.gov/pub/data/ghcn/daily/readme.txt
       
       at April 7, 2024:
       
       PRCP = Precipitation (tenths of mm)
   	   SNOW = Snowfall (mm)
	   SNWD = Snow depth (mm)
       TMAX = Maximum temperature (tenths of degrees C)
       TMIN = Minimum temperature (tenths of degrees C)
*/
enum class MeasurementType
{
    PRCP, 
    SNOW,
    SNWD,
    TMAX,
    TMIN,
    UNKNOWN
};


class Measurement
{
    public:
        Measurement(const std::string& date, const int& value, const std::string& element);

        const int& getYear() const;
        
        const int& getMonth() const;
        
        const int& getDay() const;
        
        const int& getValue() const;
        
        const MeasurementType& getType() const;
        
        static const float& getScalingForType(const MeasurementType& type);
        
        static std::map<std::string, MeasurementType> s_mapStringMeasurementType;

        static std::map<MeasurementType, float> s_mapMeasurementScaling;
        
    protected:
        int m_year;
        int m_month;
        int m_day;
        int m_value;
        MeasurementType m_type;
};

#endif // MEASUREMENT_HPP

