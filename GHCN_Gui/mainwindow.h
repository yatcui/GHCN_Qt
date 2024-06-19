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

private:
    DataProvider m_dataProvider;  // Data model

private slots:
    void showPointValue(QMouseEvent*);
    void onPlottableClick(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event);

    // Slots for UI elements are connected by name given in Designer.
    void on_btn_startsearch_clicked();
    void on_cmb_stations_textActivated(const QString& selection);
    void on_chk_tmax_year_stateChanged(int);

private:
    Ui::MainWindow *ui;
    QCustomPlot *customPlot;
    QCPItemTracer *yearTracer;
    void addGraph(const std::string& stationId, int startYear, int endYear, MeasurementType type, const QString& graphName);
};

#endif // MAINWINDOW_H
