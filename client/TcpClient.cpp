//
// Created by 李玉辉 on 2026/10/26.
//

#include "TcpClient.h"
#include <QJsonDocument>
#include <QJsonParseError>
TcpClient::TcpClient(QObject* parent)
    : QObject(parent)
{
    connect(&socket_, &QTcpSocket::connected,
            this, &TcpClient::connected);

    connect(&socket_, &QTcpSocket::disconnected,
            this, &TcpClient::disconnected);

    connect(&socket_, &QTcpSocket::errorOccurred,
            this, [this](QAbstractSocket::SocketError) {
                emit errorOccurred(socket_.errorString());
            });
    connect(&socket_, &QTcpSocket::readyRead,
        this, &TcpClient::onReadyRead);
}

void TcpClient::connectToServer(const QString& host, quint16 port)
{
    socket_.connectToHost(host, port);
}



void TcpClient::sendRequest(const QJsonObject& request)
{
    if (socket_.state() != QAbstractSocket::ConnectedState)
        return;

    QByteArray body =
        QJsonDocument(request).toJson(QJsonDocument::Compact);

    if (body.size() > 512)
        return;

    // 4 字节 ASCII 长度头，与服务端 chat_message 协议一致
    QByteArray header =
        QByteArray::number(body.size()).rightJustified(4, ' ');

    socket_.write(header + body);
}



void TcpClient::onReadyRead()
{
    // 1. 读取新数据，追加到缓冲区
    receiveBuffer_.append(socket_.readAll());

    while (true)
    {
        // 2. 判断是否收到了完整的4字节消息头
        if (receiveBuffer_.size() < 4)
            return;

        // 3. 解析消息体长度
        QByteArray header = receiveBuffer_.left(4);

        bool ok = false;
        int bodyLength = header.trimmed().toInt(&ok);

        if (!ok || bodyLength < 0 || bodyLength > 512)
        {
            emit errorOccurred("Invalid message header");
            receiveBuffer_.clear();
            socket_.abort();
            return;
        }

        // 4. 判断消息体是否接收完整
        if (receiveBuffer_.size() < 4 + bodyLength)
            return;

        // 5. 取出完整的JSON消息体
        QByteArray body = receiveBuffer_.mid(4, bodyLength);

        // 6. 删除已经处理的数据
        receiveBuffer_.remove(0, 4 + bodyLength);

        // 7. 解析JSON
        QJsonParseError error;

        QJsonDocument document =
            QJsonDocument::fromJson(body, &error);

        if (error.error != QJsonParseError::NoError ||
            !document.isObject())
        {
            emit errorOccurred("Invalid JSON response");
            continue;
        }

        // 8. 通知界面层
        emit responseReceived(document.object());
    }
}