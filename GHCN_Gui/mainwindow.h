#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Experimental: function objects in map (see below)
// #include <map>
// #include <functional>

#include <memory>

#include <QMainWindow>

#include "qcustomplot.h"

#include "dataprovider.hpp"
#include "measurement.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    // Enables Qt’s meta-object system, which is required for signals and slots to work.
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showPointValue(QMouseEvent*);
    void onPlottableClick(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event);
    void onPlottableDoubleClick(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event);
    void onSelectionChangedByUser();

    // Slots for UI elements are connected by name given in Designer.

    void on_chk_tmax_spring_stateChanged(int state);
    void on_chk_tmin_spring_stateChanged(int state);
    void on_chk_tmax_summer_stateChanged(int state);
    void on_chk_tmin_summer_stateChanged(int state);
    void on_chk_tmax_autumn_stateChanged(int state);
    void on_chk_tmin_autumn_stateChanged(int state);
    void on_chk_tmax_winter_stateChanged(int state);
    void on_chk_tmin_winter_stateChanged(int state);
    void on_chk_tmax_year_stateChanged(int state);
    void on_chk_tmin_year_stateChanged(int state);

    void on_cmb_stations_currentTextChanged(const QString& selection);
    void on_btn_update_clicked();

    void on_spb_latitude_valueChanged(double value);
    void on_spb_longitude_valueChanged(double value);
    void on_spb_radius_valueChanged(int value);
    void on_spb_top_valueChanged(int value);
    void on_spb_startyear_valueChanged(int value);
    void on_spb_endyear_valueChanged(int value);

private:
    class StationSearchParameters
    {
    public:
        StationSearchParameters(double latitude, double longitude, int radius, int top, int startYear, int endYear)
            : m_latitude(latitude),
            m_longitude(longitude),
            m_radius(radius),
            m_top(top),
            m_startYear(startYear),
            m_endYear(endYear)
            {};

        StationSearchParameters(const StationSearchParameters& other)
        {
            m_latitude = other.m_latitude;
            m_longitude = other.m_longitude;
            m_radius = other.m_radius;
            m_top = other.m_top;
            m_startYear = other.m_startYear;
            m_endYear = other.m_endYear;
        };

        StationSearchParameters& operator=(const StationSearchParameters& other)
        {
            m_latitude = other.m_latitude;
            m_longitude = other.m_longitude;
            m_radius = other.m_radius;
            m_top = other.m_top;
            m_startYear = other.m_startYear;
            m_endYear = other.m_endYear;
            return *this;
        };

        bool operator==(const StationSearchParameters& other) const {
            return (m_latitude == other.m_latitude &&
                    m_longitude == other.m_longitude &&
                    m_radius == other.m_radius &&
                    m_top == other.m_top &&
                    m_startYear == other.m_startYear &&
                    m_endYear == other.m_endYear);
        };

        std::string toString() const
        {
            return std::format("lat={:.5f}; lng={:.5f}; radius={}; top={}; start={}; end={}",
                               m_latitude, m_longitude, m_radius, m_top, m_startYear, m_endYear);
        }

        double latitude() const {return m_latitude;};
        double longitude() const {return m_longitude;};
        int radius() const {return m_radius;};
        int top() const {return m_top;};
        int startYear() const {return m_startYear;};
        int endYear() const {return m_endYear;};

        void setLatitude(double latitude) {m_latitude = latitude;};
        void setLongitude(double longitude) {m_longitude = longitude;};
        void setRadius(int radius) {m_radius = radius;};
        void setTop(int top) {m_top = top;};
        void setStartYear(int startYear) {m_startYear = startYear;};
        void setEndYear(int endYear) {m_endYear = endYear;};

    private:
        double m_latitude;
        double m_longitude;
        int m_radius;
        int m_top;
        int m_startYear;
        int m_endYear;
    };


    class GraphConfig
    {
    public:
        GraphConfig(const std::string& maxColor, const std::string& minColor)
            : m_maxColor(maxColor), m_minColor(minColor)
            {};

        const std::string& maxColor() const {
            return m_maxColor;
        };

        const std::string& minColor() const {
            return m_minColor;
        };

    private:
        std::string m_maxColor;
        std::string m_minColor;
    };

private:
    Ui::MainWindow *ui;
    QCustomPlot *customPlot;
    QCPItemTracer *yearTracer;

    DataProvider m_dataProvider;  // Data model for wheather data

    std::unique_ptr<StationSearchParameters> m_previousSearchParameters;
    std::unique_ptr<StationSearchParameters> m_currentSearchParameters;

    std::map<Season, GraphConfig> m_seasonGraphConfig;  // Specific graph configs for seasons
    double m_graphWidth;  // General graph line width
    double m_selectedGraphWidth;  // Line width for selected graphs

    // Experimental: Register function object with component.
    // std::map<QCheckBox*, std::function<void()>> m_checkBoxFunc;

    void addGraph(MeasurementType mType, Season season, const QString& graphName, const QColor& color);
    void hideGraph(const QString& graphName);
    void updateGraphs();
    void onStationSelectionChanged();
    void onStationSearchTriggered();
};

#endif // MAINWINDOW_H
