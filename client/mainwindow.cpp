//
// Created by 李玉辉 on 2026/10/26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_MainWindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    connect(ui->connectButton, &QPushButton::clicked,
        this, [this]() {
            client_.connectToServer("pi.local", 9000);
        });
    connect(ui->GET_HOSTNAMEButton, &QPushButton::clicked,
    this, [this]() {
        QJsonObject request;
        request["type"] = "GET_HOSTNAME";
        request["request_id"] = 1;
        client_.sendRequest(request);
    });
    connect(ui->getSystemInfoButton, &QPushButton::clicked,
        this, [this]()
{
    QJsonObject request;
    request["type"] = "GET_SYSTEM_INFO";
    request["request_id"] = 2;

    client_.sendRequest(request);
});

    connect(ui->getCpuUsageButton, &QPushButton::clicked,
            this, [this]()
    {
        QJsonObject request;
        request["type"] = "GET_CPU_USAGE";
        request["request_id"] = 3;

        client_.sendRequest(request);
    });
    connect(&client_, &TcpClient::connected,
            this, [this]() {
                statusBar()->showMessage("树莓派连接成功");
            });

    connect(&client_, &TcpClient::disconnected,
            this, [this]() {
                statusBar()->showMessage("连接已断开");
            });

    connect(&client_, &TcpClient::errorOccurred,
            this, [this](const QString& error) {
                statusBar()->showMessage(error);
            });
    connect(&client_, &TcpClient::responseReceived,
        this, [this](const QJsonObject& response)
        {
            QByteArray data = QJsonDocument(response).toJson(
                QJsonDocument::Indented
            );

            ui->responseEdit->append(QString::fromUtf8(data));
        });
}

MainWindow::~MainWindow() {
    delete ui;
}
