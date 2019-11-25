#ifndef WIDGET_H
#define WIDGET_H

#include <QRegExpValidator>
#include <QWidget>
#include <iostream>
#include <zmq.h>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget* parent = nullptr);
    ~Widget();

private slots:
    void on_btnSet_clicked();

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
};

#endif // WIDGET_H
