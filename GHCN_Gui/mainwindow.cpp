#include <algorithm>
#include <cmath>
#include <format>
#include <cctype>  // toupper

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include "mainwindow.h"
#include "qcustomplot.h"
#include "ui_mainwindow.h"

#include "measurement.hpp"
#include "dataprovider.hpp"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_dataProvider("../../data/", "ghcnd-stations_gm.txt", ".csv")
{
    ui->setupUi(this);

    ui->chk_tmax_year->setChecked(true);
    ui->chk_tmin_year->setChecked(true);

    this->customPlot = ui->plt_yearspan;
    this->customPlot->setInteractions(QCP::iSelectPlottables);
    // this->customPlot->legend->setVisible(true);

    connect(this->customPlot, SIGNAL(plottableClick(QCPAbstractPlottable*, int, QMouseEvent*)),
            this, SLOT(onPlottableClick(QCPAbstractPlottable*, int, QMouseEvent*)));

    connect(this->customPlot, SIGNAL(plottableDoubleClick(QCPAbstractPlottable*, int, QMouseEvent*)),
            this, SLOT(onPlottableDoubleClick(QCPAbstractPlottable*, int, QMouseEvent*)));

    connect(this->customPlot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(showPointValue(QMouseEvent*)));

    connect(this->customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(onSelectionChangedByUser()));

    // Add the year tracer (red circle) which sticks to the graph data:
    this->yearTracer = new QCPItemTracer(this->customPlot);
    this->yearTracer->setInterpolating(false);
    this->yearTracer->setStyle(QCPItemTracer::tsCircle);
    this->yearTracer->setPen(QPen(Qt::red));
    this->yearTracer->setBrush(Qt::red);
    this->yearTracer->setSize(10);

    // TODO: How to show initially empty QCustomPlot widget? Make pens for all items transparent?
    this->customPlot->hide();
}


MainWindow::~MainWindow()
{
    delete this->ui;
    // Following line crashes application if uncommented. Seems that QCustomPlot takes ownership.
    //delete this->yearTracer;
    delete this->customPlot;
}


void MainWindow::onPlottableClick(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event)
{
    if (typeid(*plottable) == typeid(QCPGraph)) {
        qDebug() << "Start tracer for graph" << plottable->name();
        this->showPointValue(event);  // Start tracer
    }
}


void MainWindow::onSelectionChangedByUser()
{
    if (this->customPlot->selectedGraphs().isEmpty()) {
        this->yearTracer->setGraph(nullptr);  // De-register graph (if any) from tracer
        this->yearTracer->setVisible(false);
        this->customPlot->replot();
    }
}

void MainWindow::onPlottableDoubleClick(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event)
{
    double x = plottable->interface1D()->dataMainKey(dataIndex);
    double y = plottable->interface1D()->dataMainValue(dataIndex);
    qDebug() << "Nearest measurement point at (" << x << ", " << y << ")";
}


void MainWindow::showPointValue(QMouseEvent* event)
{
    // Inspired by:
    // QCustomPlot support forum at https://www.qcustomplot.com/index.php/support/forum
    // Answer to: "How to get the point under the mouse in a graph"
    // December 22, 2017, 09:08 by santa

    // Get selected graph
    QList selectedGraphs = this->customPlot->selectedGraphs();
    if (selectedGraphs.size() != 1) {  // Make sure only single graph can be traced
        return;
    }
    QCPGraph * graph = selectedGraphs.first();

     // Setup the item tracer
    this->yearTracer->setGraph(graph);
    this->yearTracer->setGraphKey(this->customPlot->xAxis->pixelToCoord(event->pos().x()));
    this->yearTracer->setVisible(true);
    this->customPlot->replot();

    // Get the values from the tracer's coords
    QPointF temp = this->yearTracer->position->coords();

    // Show a tooltip which tells the values
    QToolTip::showText(event->globalPosition().toPoint(),
                       tr("<h4>%L1</h4>"
                          "<table>"
                          "<tr>"
                          "<td><h5>X: %L2</h5></td>" "<td>  ,  </td>" "<td><h5>Y: %L3</h5></td>"
                          "</tr>"
                          "</table>").arg(graph->name(), QString::number(temp.x(), 'f', 0), QString::number(temp.y(), 'f', 1)),
                       this->customPlot, this->customPlot->rect());
}


void MainWindow::on_btn_startsearch_clicked()
{

}


void MainWindow::on_cmb_stations_textActivated(const QString& selection)
{
    // Retrieve arguments for loadChart from GUI
    int startYear = ui->spb_startyear->value();
    int endYear = ui->spb_endyear->value();

    this->customPlot->hide();
    this->yearTracer->setGraph(nullptr);
    this->yearTracer->setVisible(false);
    this->customPlot->clearGraphs();
    this->statusBar()->clearMessage();

    for (QCheckBox* checkBox : this->ui->grp_plots->findChildren<QCheckBox*>()) {
        // qDebug() << checkBox->objectName();
        if (checkBox->isChecked()) {
            const std::string name{checkBox->objectName().toStdString()};
            std::string mtypeName{name.substr(4, 4)};   // Measurement type name encoded in component name, e. g. chk_tmax_year.
            std::transform(mtypeName.begin(), mtypeName.end(), mtypeName.begin(), ::toupper);  // Name must be in upper case.
            const std::string graphName = std::format("{} {}", mtypeName, selection.toStdString());
            // qDebug() << graphName << " " << type;
            MeasurementType mtype = Measurement::s_mapStringMeasurementType[mtypeName];
            addGraph(selection.toStdString(), startYear, endYear, mtype, graphName.c_str());
        }
    }
    this->customPlot->xAxis->setLabel("year");
    this->customPlot->yAxis->setLabel("°C");
    this->customPlot->xAxis->setRange(startYear - 1, endYear + 1);  // Some margin to left and right.
    this->customPlot->yAxis->rescale();
    auto yRange = this->customPlot->yAxis->range();
    // Rescale y-axis to have some margin on bottom and top.
    this->customPlot->yAxis->setRange(std::floor(yRange.lower - 1), std::floor(yRange.upper + 1));
    this->customPlot->replot();
    this->customPlot->show();
}


void MainWindow::addGraph(const std::string& stationId, int startYear, int endYear, MeasurementType type, const QString& graphName)
{
    auto yearlyAverages = m_dataProvider.getYearlyAverages(stationId, startYear, endYear, type);
    if (yearlyAverages->empty()) {
        this->statusBar()->showMessage(std::format("No data for selected station {} available", stationId).c_str());
        // this->customPlot->show();  // Show previous plot.
        return;
    }
    QVector<double> x;
    QVector<double> y;
    for (const auto& pair : *yearlyAverages) {
        x.append(pair.first);
        y.append(pair.second);
    }

    QCPGraph * graph = nullptr;
    for (int i = 0; i < this->customPlot->graphCount(); ++i) {
        if (this->customPlot->graph(i)->name() == graphName) {
            graph = this->customPlot->graph(i);
            graph->setVisible(true);
        }
    }
    if (graph == nullptr) {
        this->customPlot->addGraph();
        graph = this->customPlot->graph();
    }
    graph->setData(x, y, true);
    graph->setName(graphName);

    // Do *not* indicate selection of graph by different pen.
    // QPen pen = graph->pen();
    // graph->selectionDecorator()->setPen(pen);

    // Data points as filled circles.
    graph->setScatterStyle(QCPScatterStyle::ssDisc);
}

void MainWindow::on_chk_tmax_year_stateChanged(int state)
{
    qDebug() << std::format("Year max state: {}", state);
}

void MainWindow::on_chk_tmax_winter_stateChanged(int state)
{

}

