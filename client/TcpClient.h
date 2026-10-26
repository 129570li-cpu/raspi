//
// Created by 李玉辉 on 2026/10/26.
//

#ifndef RASPI_TCPCLIENT_H
#define RASPI_TCPCLIENT_H
#include <QTcpSocket>
#include <QByteArray>
#include <QJsonObject>

class TcpClient : public QObject{
    Q_OBJECT

    public:
    explicit TcpClient(QObject* parent = nullptr);
    void sendRequest(const QJsonObject& request);
    void connectToServer(const QString& host, quint16 port);

    signals:
        void connected();
        void disconnected();
        void errorOccurred(const QString& message);
        void responseReceived(const QJsonObject& response);

private:
    QTcpSocket socket_;
    void onReadyRead();

    QByteArray receiveBuffer_;
};



#endif //RASPI_TCPCLIENT_H
