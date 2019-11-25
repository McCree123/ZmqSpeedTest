#ifndef WIDGET_H
#define WIDGET_H

#include <QRegExpValidator>
#include <QWidget>

#include "kalmanfiltersimple1d.hpp"
#include <chrono>
#include <iostream>
#include <zmq.h>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>

#include "qaxiszoomsvc.h"
#include "qwheelzoomsvc.h"
#include "qwtchartzoom.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget* parent = nullptr);
    ~Widget();

    void replot(QwtPlot* plot);

private slots:
    void on_btnSend_clicked();

private:
    Ui::Widget* ui;

    /* Создаем строку для регулярного выражения */
    QString ipRange{"(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])"};

    /* Создаем регулярное выражение с применением строки, как
     * повторяющегося элемента
     */
    QRegExp ipRegex{"^" + ipRange + "\\." + ipRange + "\\." + ipRange + "\\."
                    + ipRange + "$"};

    /* Создаем Валидатор регулярного выражения с применением
     * созданного регулярного выражения
     */
    QRegExpValidator ipValidator{ipRegex, this};

    QVector<double> x;
    QVector<double> y;

    QVector<double> x_kalman;
    QVector<double> y_kalman;

    QwtPlotCurve curve;
    QwtPlotCurve curveKalman;
    QwtChartZoom* zoom;
    QWheelZoomSvc* whlzmsvc;
    QAxisZoomSvc* axzmsvc;
};

#endif // WIDGET_H
