#include <format>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <map>
#include <memory>

#include "ghcn_dataprovider.hpp"

using namespace std;

int main_(int argc, char* argv[]) {
   
    if (argc != 5) {
        cout << format("Usage: {} filename startYear endYear month\n", argv[0]);
        exit(EXIT_FAILURE);
        }
    string filename = argv[1];
    int startYear = stoi(argv[2]);
    int endYear = stoi(argv[3]);
    int month = stoi(argv[4]);
    cout << format("Reading from {}\n", filename);
    
    if (ifstream inStream{filename, ios::in}) {
        unique_ptr<vector<Measurement>> measurements = readMeasurementsForStation(inStream);
        inStream.close();
        cout << format("{} measurements\n", measurements->size());
        cout << format("Size of measurement object: {} bytes\n", sizeof measurements->at(0));
        span<const Measurement> interval = getMeasurementsForYearSpan(measurements, startYear, endYear);
        cout << format("{} measurements from {} to {}\n", interval.size(), startYear, endYear);
        
        // Filter for measurement type.
        auto type{MeasurementType::TMAX};
        auto filtered_interval{interval | views::filter([type](auto m) {return m.getType() == type;})};
        int count = distance(filtered_interval.begin(), filtered_interval.end());
        cout << format("{} TMAX measurements from {} to {}\n", count, startYear, endYear);
        
        // Get yearly averages of measurements for given type.
        float scaling = Measurement::getScalingForType(type);
        unique_ptr<map<int, float>> yearlyAverages = getYearlyAverages(filtered_interval, scaling);
        for (const auto& pair : *yearlyAverages) {
            cout << format("Average of TMAX in {} was {:>+5.1f} °C\n", pair.first, pair.second);
            }
        // Get monthly averages of measurements for given type in given year.
        unique_ptr<map<int, float>> monthlyAverages = getMontlyAverages(filtered_interval, scaling, endYear);
        for (const auto& pair : *monthlyAverages) {
            cout << format("Average of TMAX in {}-{:02} was {:>+5.1f} °C\n", endYear, pair.first, pair.second);
            }
        // Get measurement values for given type in given month.
        auto dailyValues = getDailyValues(filtered_interval, scaling, endYear, month);
        for (const auto& pair : *dailyValues) {
            cout << format("TMAX on {}-{:02}-{:02} was {:>+5.1f} °C\n", endYear, month, pair.first, pair.second);
            }
        }
    else {
        cout << format("Opening {} failed\n", filename);
        exit(EXIT_FAILURE);
        }
    }
 
