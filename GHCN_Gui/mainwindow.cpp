#include <string>
#include <algorithm>
#include <cmath>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include "mainwindow.h"
#include "qcustomplot.h"
#include "ui_mainwindow.h"

#include "measurement.hpp"
#include "ghcn_dataprovider.hpp"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->customPlot = ui->plt_yearspan;
    this->customPlot->setInteractions(QCP::iSelectPlottables);

    connect(this->customPlot, SIGNAL(plottableClick(QCPAbstractPlottable*, int, QMouseEvent*)),
            this, SLOT(onPlottableClick(QCPAbstractPlottable*, int, QMouseEvent*)));

    connect(this->customPlot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(showPointValue(QMouseEvent*)));

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
    double x = plottable->interface1D()->dataMainKey(dataIndex);
    double y = plottable->interface1D()->dataMainValue(dataIndex);
    qDebug() << "Nearest measurement point at (" << x << ", " << y << ")";
}


void MainWindow::showPointValue( QMouseEvent* event )
{
    // Taken from QCustomPlot support forum at https://www.qcustomplot.com/index.php/support/forum
    // Answer to: "How to get the point under the mouse in a graph"
    // December 22, 2017, 09:08 by santa

    if (this->customPlot->graphCount() == 0) {
        return;
    }

    QCPGraph *graph = this->customPlot->graph(0);

    // Get selected graph (in my case selected means the plot is selected from the legend)
    /*
    for (int i=0; i<this->this->customPlot->graphCount(); ++i)
    {
        if( this->this->customPlot->legend->itemWithPlottable(this->customPlot->graph(i))->selected() )
        {
            graph = this->this->customPlot->graph(i);
            break;
        }
    }
    */
    // Setup the item tracer
    this->yearTracer->setGraph(graph);
    this->yearTracer->setGraphKey(this->customPlot->xAxis->pixelToCoord(event->pos().x()));
    this->customPlot->replot();

    // **********Get the values from the item tracer's coords***********
    QPointF temp = this->yearTracer->position->coords();

    // Show a tooltip which tells the values
    QToolTip::showText(event->globalPosition().toPoint(),
                       tr("<h4>%L1</h4>"
                          "<table>"
                          "<tr>"
                          "<td><h5>X: %L2</h5></td>" "<td>  ,  </td>" "<td><h5>Y: %L3</h5></td>"
                          "</tr>"
                          "</table>").arg(graph->name(), QString::number( temp.x(), 'f', 0 ), QString::number( temp.y(), 'f', 1 )),
                       this->customPlot, this->customPlot->rect());
}


void MainWindow::on_btn_startsearch_clicked()
{

}


void MainWindow::on_cmb_stations_textActivated(const QString &stationId)
{
    // Retrieve arguments for loadChart from GUI
    int startYear = ui->spb_startyear->value();
    int endYear = ui->spb_endyear->value();

    this->customPlot->hide();

    QString graphName = stationId;
    graphName.prepend("TMAX ");
    loadChart(stationId, startYear, endYear, MeasurementType::TMAX, graphName);
}


void MainWindow::loadChart(const QString &stationId, int startYear, int endYear, MeasurementType type, const QString &graphName)
{
    auto yearlyAverages = getYearlyAveragesForSpan(stationId.toStdString(), startYear, endYear, type);
    QVector<double> x;
    QVector<double> y;
    for (const auto& pair : *yearlyAverages) {
        x.append(pair.first);
        y.append(pair.second);
    }
    double y_max = *std::max_element(y.begin(), y.end());
    double y_min = *std::min_element(y.begin(), y.end());

    this->customPlot->addGraph();
    this->customPlot->graph(0)->setData(x, y, true);
    this->customPlot->graph(0)->setName(graphName);

    // Do *not* indicate selection of graph by different pen.
    QPen pen = this->customPlot->graph(0)->pen();
    this->customPlot->graph(0)->selectionDecorator()->setPen(pen);

    this->customPlot->xAxis->setLabel("year");
    this->customPlot->yAxis->setLabel("°C");
    this->customPlot->xAxis->setRange(startYear - 1, endYear + 1);
    this->customPlot->yAxis->setRange(std::floor(y_min - 1), std::floor(y_max + 1));

    // Data points as filled circles.
    this->customPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);

    this->customPlot->replot();
    this->customPlot->show();
}
