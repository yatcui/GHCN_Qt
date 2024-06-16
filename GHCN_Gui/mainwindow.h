#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "measurement.hpp"
#include <QMainWindow>

#include "qcustomplot.h"

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

    void on_btn_startsearch_clicked();
    void on_cmb_stations_textActivated(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QCustomPlot *customPlot;
    QCPItemTracer *yearTracer;
    void loadChart(const QString &stationId, int startYear, int endYear, MeasurementType type, const QString &graphName);
};

#endif // MAINWINDOW_H
