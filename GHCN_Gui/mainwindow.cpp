#include <cmath>
#include <format>

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
    this->ui->setupUi(this);  // constructs the widget hierarchy (ui_mainwindow.h)

    // Experimental
    // m_checkBoxFunc.emplace(this->ui->chk_tmax_year, [this](){qDebug() << this->ui->chk_tmax_year->objectName();});

    this->customPlot = this->ui->plt_yearspan;  // "rename" to commonly used identifier
    this->customPlot->setInteractions(QCP::iSelectPlottables);
    // this->customPlot->legend->setVisible(true);

    connect(this->customPlot, SIGNAL(plottableClick(QCPAbstractPlottable*, int, QMouseEvent*)),
            this, SLOT(onPlottableClick(QCPAbstractPlottable*, int, QMouseEvent*)));

    connect(this->customPlot, SIGNAL(plottableDoubleClick(QCPAbstractPlottable*, int, QMouseEvent*)),
            this, SLOT(onPlottableDoubleClick(QCPAbstractPlottable*, int, QMouseEvent*)));

    connect(this->customPlot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(showPointValue(QMouseEvent*)));

    connect(this->customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(onSelectionChangedByUser()));

    // Set up the year tracer which sticks to the graph data:
    this->yearTracer = new QCPItemTracer(this->customPlot);
    this->yearTracer->setInterpolating(false);
    this->yearTracer->setStyle(QCPItemTracer::tsCrosshair);  // tsCircle: warning messages for NaN y values.

    // TODO: Read set up values from config file.
    // Set up colors for graphs.
    m_seasonGraphConfig.emplace(Season::SPRING, GraphConfig("#99FF99", "#4C9900"));
    m_seasonGraphConfig.emplace(Season::SUMMER, GraphConfig("#FFB266", "#CC0000"));
    m_seasonGraphConfig.emplace(Season::AUTUMN, GraphConfig("#CCCC00", "#994C00"));
    m_seasonGraphConfig.emplace(Season::WINTER, GraphConfig("#99CCFF", "#0000FF"));
    m_seasonGraphConfig.emplace(Season::YEAR, GraphConfig("#A0A0A0", "#000000"));

    // Set up width for normal and selected graphs.
    m_graphWidth = 1;
    m_selectedGraphWidth = 1.5;

    this->customPlot->hide();   
}


MainWindow::~MainWindow()
{
    delete this->ui;
    // Following line crashes application if uncommented. Seems that QCustomPlot takes ownership.
    //delete this->yearTracer;
    delete this->customPlot;
}


void MainWindow::onSelectionChangedByUser()
{
    if (this->customPlot->selectedGraphs().isEmpty()) {
        this->yearTracer->setGraph(nullptr);  // De-register graph (if any) from tracer
        this->yearTracer->setVisible(false);
        this->customPlot->replot();
    }
}


void MainWindow::onPlottableClick(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event)
{
    if (typeid(*plottable) == typeid(QCPGraph)) {
        this->showPointValue(event);  // Show tracer here
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

    // Get the values from the tracer's coords
    QPointF temp = this->yearTracer->position->coords();

    // Setup the item tracer
    this->yearTracer->setGraph(graph);
    this->yearTracer->setPen(graph->pen());  // Same color as graph.
    this->yearTracer->setGraphKey(this->customPlot->xAxis->pixelToCoord(event->pos().x()));
    this->yearTracer->setVisible(true);
    this->customPlot->replot();

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


void MainWindow::updateGraphs()
{
    this->customPlot->hide();
    this->yearTracer->setGraph(nullptr);
    this->yearTracer->setVisible(false);
    this->statusBar()->clearMessage();

    if (this->ui->chk_tmax_spring->isChecked()) {
        this->addGraph(MeasurementType::TMAX, Season::SPRING, "TMAX Spring",
                       QColor(m_seasonGraphConfig.at(Season::SPRING).maxColor().c_str()));
        /* Note:
         * Access to map by operator [] does not compile, as it tries to
         * construct a GraphConfig object via the default constructor.
         * Compiler does not know that an entry for Season::Spring exists.
         * A call to operator [] for an non-exisiting key inserts the default for value
         * (which is default-constructed for objects).
         * On the contrary, .at() performs bound checking an thows std::out_of_range.
         */

    } else {
        this->hideGraph("TMAX Spring");
    }

    if (this->ui->chk_tmin_spring->isChecked()) {
        this->addGraph(MeasurementType::TMIN, Season::SPRING, "TMIN Spring",
                       QColor(m_seasonGraphConfig.at(Season::SPRING).minColor().c_str()));
    } else {
        this->hideGraph("TMIN Spring");
    }

    if (this->ui->chk_tmax_summer->isChecked()) {
        this->addGraph(MeasurementType::TMAX, Season::SUMMER, "TMAX Summer",
                       QColor(m_seasonGraphConfig.at(Season::SUMMER).maxColor().c_str()));
    } else {
        this->hideGraph("TMAX Summer");
    }

    if (this->ui->chk_tmin_summer->isChecked()) {
        this->addGraph(MeasurementType::TMIN, Season::SUMMER, "TMIN Summer",
                       QColor(m_seasonGraphConfig.at(Season::SUMMER).minColor().c_str()));
    } else {
        this->hideGraph("TMIN Summer");
    }

    if (this->ui->chk_tmax_autumn->isChecked()) {
        this->addGraph(MeasurementType::TMAX, Season::AUTUMN, "TMAX Autumn",
                       QColor(m_seasonGraphConfig.at(Season::AUTUMN).maxColor().c_str()));
    } else {
        this->hideGraph("TMAX Autumn");
    }

    if (this->ui->chk_tmin_autumn->isChecked()) {
        this->addGraph(MeasurementType::TMIN, Season::AUTUMN, "TMIN Autumn",
                       QColor(m_seasonGraphConfig.at(Season::AUTUMN).minColor().c_str()));
    } else {
        this->hideGraph("TMIN Autumn");
    }

    if (this->ui->chk_tmax_winter->isChecked()) {
        this->addGraph(MeasurementType::TMAX, Season::WINTER, "TMAX Winter",
                       QColor(m_seasonGraphConfig.at(Season::WINTER).maxColor().c_str()));
    } else {
        this->hideGraph("TMAX Winter");
    }

    if (this->ui->chk_tmin_winter->isChecked()) {
        this->addGraph(MeasurementType::TMIN, Season::WINTER, "TMIN Winter",
                       QColor(m_seasonGraphConfig.at(Season::WINTER).minColor().c_str()));
    } else {
        this->hideGraph("TMIN Winter");
    }

    if (this->ui->chk_tmax_year->isChecked()) {
        this->addGraph(MeasurementType::TMAX, Season::YEAR, "TMAX Year",
                       QColor(m_seasonGraphConfig.at(Season::YEAR).maxColor().c_str()));
    } else {
        this->hideGraph("TMAX Year");
    }

    if (this->ui->chk_tmin_year->isChecked()) {
        this->addGraph(MeasurementType::TMIN, Season::YEAR, "TMIN Year",
                       QColor(m_seasonGraphConfig.at(Season::YEAR).minColor().c_str()));
    } else {
        this->hideGraph("TMIN Year");
    }

    int startYear = this->ui->spb_startyear->value();
    int endYear = this->ui->spb_endyear->value();

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


void MainWindow::addGraph(MeasurementType mType, Season season, const QString& graphName, const QColor& color)
{
    const std::string stationId = this->ui->cmb_stations->currentText().toStdString();
    int startYear = this->ui->spb_startyear->value();
    int endYear = this->ui->spb_endyear->value();
    double longitude = this->ui->spb_longitude->value();
    int startMonth = -1;
    int endMonth = -1;


    if (season == Season::SPRING) {
        if (longitude > 0) {
            startMonth = 3;
            endMonth = 5;
        } else {
            startMonth = 9;
            endMonth = 11;
        }
    } else if (season == Season::SUMMER) {
        if (longitude > 0) {
            startMonth = 6;
            endMonth = 8;
        } else {
            startMonth = 12;
            endMonth = 2;
        }
    } else if (season == Season::AUTUMN) {
        if (longitude > 0) {
            startMonth = 9;
            endMonth = 11;
        } else {
            startMonth = 3;
            endMonth = 5;
        }
    } else if (season == Season::WINTER) {
        if (longitude > 0) {
            startMonth = 12;
            endMonth = 2;
        } else {
            startMonth = 6;
            endMonth = 8;
        }
    } else {  // Full year
        startMonth = 1;
        endMonth = 12;
    }

    QCPGraph * graph = nullptr;
    for (int i = 0; i < this->customPlot->graphCount(); ++i) {
        if (this->customPlot->graph(i)->name() == graphName) {
            graph = this->customPlot->graph(i);
            graph->setVisible(true);
            break;
        }
    }
    if (graph == nullptr) {
        this->customPlot->addGraph();
        graph = this->customPlot->graph();
    } else {
        double firstKey = graph->data()->at(0)->key;
        double lastKey = graph->data()->at(graph->data()->size() - 1)->key;
        // Check if graph already exists with required parameter values.
        if (firstKey == startYear && lastKey == endYear) {
            // qDebug() << "Graph" << graphName << "in range" << lastKey << firstKey << "made visible";
            return;
        }
    }

    // TODO: Load in background thread if data needs to be downloaded. Ask data providerin advance.
    //       GUI elements should be disabled for longer loading times.
    auto yearlyAverages = m_dataProvider.getAveragesForMonthRange(stationId, startYear, endYear, startMonth, endMonth, mType);

    if (yearlyAverages->empty()) {
        this->statusBar()->showMessage(std::format("No data for selected station {} available", stationId).c_str());
        // this->customPlot->show();  // Show previous plot.
        return;
    }
    QVector<double> x;
    QVector<double> y;
    for (int i = startYear; i <= endYear; ++i) {
        x.append(i);
        if (yearlyAverages->contains(i)) {
            y.append(yearlyAverages->at(i));
        } else {  // Indicate missing values. Prevents QCP from interpolating over the gap.
            y.append(qQNaN());
            // Message when the tracer hits this value (only with style tsCircle):
            // "QPainterPath::arcTo: Adding arc where a parameter is NaN, results are undefined"
            // TODO: How to prevent that?
            // Anyway, tracer works fine with tsCircle, too.
        }
    }

    graph->setData(x, y, true);
    graph->setName(graphName);

    QPen pen = graph->pen();
    pen.setColor(color);
    pen.setWidthF(m_graphWidth);  // Default: zero (0), which makes for a 1 pt width graph.
    graph->setPen(pen);

    // Pen to indicate selected graph.
    QPen selPen = QPen(pen);
    selPen.setWidthF(m_selectedGraphWidth);
    graph->selectionDecorator()->setPen(selPen);

    // Data points as filled circles.
    graph->setScatterStyle(QCPScatterStyle::ssDisc);
}


void MainWindow::hideGraph(const QString& graphName) {

    for (int i = 0; i < this->customPlot->graphCount(); ++i) {
        if (this->customPlot->graph(i)->name() == graphName) {
            this->customPlot->graph(i)->setVisible(false);
            break;
        }
    }
}


void MainWindow::onStationSelectionChanged()
{
    if (this->customPlot->selectedGraphs().isEmpty()) {
        this->yearTracer->setGraph(nullptr);  // De-register graph (if any) from tracer
        this->yearTracer->setVisible(false);
        this->customPlot->replot();
    }
}


void MainWindow::on_btn_update_clicked()
{

}


void MainWindow::on_cmb_stations_currentTextChanged(const QString& selection)
{
    this->customPlot->clearGraphs();
    this->updateGraphs();
}


void MainWindow::on_chk_tmin_spring_stateChanged(int state)
{
    this->updateGraphs();
}

void MainWindow::on_chk_tmax_spring_stateChanged(int state)
{
    this->updateGraphs();
}

void MainWindow::on_chk_tmin_summer_stateChanged(int state)
{
    this->updateGraphs();
}

void MainWindow::on_chk_tmax_summer_stateChanged(int state)
{
    this->updateGraphs();
}

void MainWindow::on_chk_tmin_autumn_stateChanged(int state)
{
    this->updateGraphs();
}

void MainWindow::on_chk_tmax_autumn_stateChanged(int state)
{
    this->updateGraphs();
}

void MainWindow::on_chk_tmin_winter_stateChanged(int state)
{
    this->updateGraphs();
}

void MainWindow::on_chk_tmax_winter_stateChanged(int state)
{
    this->updateGraphs();
}

void MainWindow::on_chk_tmin_year_stateChanged(int state)
{
    this->updateGraphs();
}

void MainWindow::on_chk_tmax_year_stateChanged(int state)
{
    // Experimental (see mainwindow.h)
    // m_checkBoxFunc[this->ui->chk_tmax_year]();
    this->updateGraphs();
}
