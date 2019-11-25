#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->lineEditMyIP->setValidator(&ipValidator);
}

Widget::~Widget()
{
    delete ui;
}

void
Widget::on_btnSet_clicked()
{
    // откроем сокет для ответа на запрос
    auto context_reply = zmq_ctx_new();
    zmq_ctx_get(context_reply, ZMQ_MAX_SOCKETS);
    auto socket_reply = zmq_socket(context_reply, ZMQ_REP);

    // Почему то не работает с udp
    // zmq_bind(socket_reply, "tcp://*:5555"); //* - что это?

    QString addr = "tcp://" + ui->lineEditMyIP->text() + ":45936";
    // zmq_bind(socket_reply, "tcp://" + ui->lineEditIP->text() + ":45936"); //
    // нет
    if (zmq_bind(socket_reply, addr.toStdString().data()) == -1) // да
    {
        // Ошибка открытия сокета для ответа клиента
        std::cerr << "Ошибка открытия сокета для ответа клиента" << std::endl;
        return;
    }

    // теперь откроем сокет для запроса
    auto context_request = zmq_ctx_new();
    zmq_ctx_get(context_request, ZMQ_MAX_SOCKETS);
    auto socket_request = zmq_socket(context_request, ZMQ_REQ);

    addr = "tcp://" + ui->lineEditMyIP->text() + ":45936";

    if (zmq_connect(socket_request, addr.toStdString().data()) == -1)
    {
        // Ошибка открытия сокета для запроса клиенту
        std::cerr << "Ошибка открытия сокета для запроса клиенту" << std::endl;
        return;
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
            if (zmq_send(socket_request, message, message.size(), 0) == -1)
            {
                // Не отсылается на принимающий сокет
                std::cerr << "Не отсылается на принимающий сокет" << std::endl;
                return;
            }
            if (zmq_msg_close(&part))
            {
                // Ошибка удаления сообщения
                std::cerr << "Ошибка удаления сообщения" << std::endl;
                return;
            }
            continue; // Получается бесконечный цикл прослушки, который
                      // блокирует поток, в котором находится
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

    // закрываем сокеты
    zmq_close(socket_reply);
    zmq_close(socket_request);

    // завершаем работу с контекстами
    zmq_ctx_shutdown(context_reply);
    zmq_ctx_shutdown(context_request);

    // теперь можем уничтожить контексты сокетов
    zmq_ctx_destroy(context_reply);
    zmq_ctx_destroy(context_request);
}
