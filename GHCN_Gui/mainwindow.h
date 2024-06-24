#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

    void on_cmb_stations_textActivated(const QString& selection);
    void on_btn_update_clicked();

private:
    Ui::MainWindow *ui;
    QCustomPlot *customPlot;
    QCPItemTracer *yearTracer;

    DataProvider m_dataProvider;  // Data model for wheather data

    void addGraph(MeasurementType mType, Season season, const QString& graphName);
    void hideGraph(const QString& graphName);
    void updateGraphs();
    void onStationSelectionChanged();
    void onStationSearchTriggered();
};

#endif // MAINWINDOW_H
