#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->lineEditServerIP->setValidator(&ipValidator);

    curve.attach(ui->plot);
    curve.setStyle(QwtPlotCurve::Lines);
    curve.setPaintAttribute(QwtPlotCurve::FilterPoints, true);
    curve.setPaintAttribute(QwtPlotCurve::ClipPolygons, true);
    QPen pen(QColor(0, 0, 0));
    pen.setWidth(2);
    curve.setPen(pen);

    curveKalman.attach(ui->plot);
    curveKalman.setStyle(QwtPlotCurve::Lines);
    curveKalman.setPaintAttribute(QwtPlotCurve::FilterPoints, true);
    curveKalman.setPaintAttribute(QwtPlotCurve::ClipPolygons, true);
    QPen pen1(QColor(0, 0, 255));
    pen.setWidth(2);
    curveKalman.setPen(pen1);
}

Widget::~Widget()
{
    curve.detach();
    curveKalman.detach();
    delete ui;
    delete zoom;
    delete whlzmsvc;
    delete axzmsvc;
}

void
Widget::on_btnSend_clicked()
{
    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point end;

    QByteArray byteArray;

    // откроем сокет для ответа на запрос от сервера
    auto context_reply = zmq_ctx_new();
    zmq_ctx_get(context_reply, ZMQ_MAX_SOCKETS);
    auto socket_reply = zmq_socket(context_reply, ZMQ_REP);

    // Почему то не работает с udp
    // zmq_bind(socket_reply, "tcp://*:5555"); //* - что это?

    QString addr = "tcp://" + ui->lineEditServerIP->text() + ":45936";
    // zmq_bind(socket_reply, "tcp://" + ui->lineEditIP->text() + ":45936"); //
    // нет
    if (zmq_bind(socket_reply, addr.toStdString().data()) == -1) // да
    {
        // Ошибка открытия сокета для ответа сервера
        std::cerr << "Ошибка открытия сокета для ответа сервера" << std::endl;
        return;
    }

    // теперь откроем сокет для запроса
    auto context_request = zmq_ctx_new();
    zmq_ctx_get(context_request, ZMQ_MAX_SOCKETS);
    auto socket_request = zmq_socket(context_request, ZMQ_REQ);

    addr = "tcp://" + ui->lineEditServerIP->text() + ":45936";

    if (zmq_connect(socket_request, addr.toStdString().data()) == -1)
    {
        // Ошибка открытия сокета для запроса серверу
        std::cerr << "Ошибка открытия сокета для запроса серверу" << std::endl;
        return;
    }

    // Отправим ui->sboxCountSend.value() байт
    for (int i = 0; i < ui->sboxCountSend->value(); i++)
    {
        byteArray.append('a');
    }

    if (zmq_send(socket_request, byteArray, byteArray.size(), 0) == -1)
    {
        // Не отсылается на принимающий сокет
        std::cerr << "Не отсылается на принимающий сокет" << std::endl;
        return;
    }
    else
    {
        start = std::chrono::steady_clock::now();
    }

    // Входим в цикл чтения сообщений
    QByteArray message;

    int more         = 0;
    size_t more_size = sizeof(more);

    zmq_msg_t part;
    if (zmq_msg_init(&part))
    {
        // Ошибка инициализации сообщения
        std::cerr << "Ошибка инициализации сообщения" << std::endl;
        return;
    }

    do
    {
        // zmq_recvmsg сама удалит предыдущее сообщение в part
        int msg_size = zmq_recvmsg(socket_reply, &part, 0);
        if (msg_size > 0)
        {
            message = QByteArray((char*)zmq_msg_data(&part), msg_size);
            // по факту прочтения останавливаем таймер
            end = std::chrono::steady_clock::now();

            // Отчистим от уже ненужного сообщения
            if (zmq_msg_close(&part))
            {
                // Ошибка удаления сообщения
                std::cerr << "Ошибка удаления сообщения" << std::endl;
                return;
            }
            break; // Если даже что и пришло - статистику берём по первому
                   // пришедшему сообщению, которое первым и было отправлено
        }

        if (msg_size < 0)
        {
            if (errno
                == EAGAIN) // В общем-то при блокирующем read не возникнет.
                continue;

            // Ошибка чтения
            std::cerr << "Ошибка чтения" << std::endl;
            return;
        }

        // Есть ли продолжение посылки
        if (zmq_getsockopt(socket_reply, ZMQ_RCVMORE, &more, &more_size))
        {
            // Ошибка проверки есть ли ещё сообщения
            std::cerr << "Ошибка проверки есть ли ещё сообщения" << std::endl;
            return;
        }
    } while (more);

    // В итоге в message будет принятое сообщение, а в nanosec будет время
    // путешествия этого сообщения через свитчи
    long nanosec
        = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
              .count();

    ui->labelNS->setText(QString::number(nanosec));

    // закрываем сокеты
    zmq_close(socket_reply);
    zmq_close(socket_request);

    // завершаем работу с контекстами
    zmq_ctx_shutdown(context_reply);
    zmq_ctx_shutdown(context_request);

    // теперь можем уничтожить контексты сокетов
    zmq_ctx_destroy(context_reply);
    zmq_ctx_destroy(context_request);

    x.push_back(0);
    x[x.size() - 1] = x.size();
    y.push_back(nanosec);

    KalmanFilterSimple1D* kalman
        = new KalmanFilterSimple1D(1, 8, 1.005, 1); // задаем F, H, Q и R
    // Последний параметр клонит кривую Калмана книзу (от 1 до 10),
    // предпоследний клонит кривую Калмана кверху (от 1 до 10) второй - степень
    // сглаживания (высталяется через 1 до 10), первый - тоже степень
    // сглаживания (выставляется от 0 до 1)

    kalman->SetState(y[0],
                     0.1); // Задаем начальные значение State и Covariance

    kalman->Correct(y[y.size() - 1]); // Применяем алгоритм
    y_kalman.push_back(kalman->getState()); // Сохраняем текущее состояние
    x_kalman.push_back(x.size());

    replot(ui->plot);
}

void
Widget::replot(QwtPlot* plot)
{
    zoom = new QwtChartZoom(plot);
    zoom->setRubberBandColor(Qt::black);
    whlzmsvc = new QWheelZoomSvc();
    whlzmsvc->attach(zoom);
    axzmsvc = new QAxisZoomSvc();
    axzmsvc->attach(zoom);

    plot->setAxisScale(QwtPlot::yLeft, 0.0, 5000000.0);
    plot->setAxisScale(QwtPlot::xBottom, 0.0, 100.0);
    curve.setSamples(x, y);
    curveKalman.setSamples(x_kalman, y_kalman);
    plot->replot();
}
