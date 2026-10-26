//
// Created by 李玉辉 on 2026/10/26.
//

#ifndef RASPI_MAINWINDOW_H
#define RASPI_MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
};


#endif //RASPI_MAINWINDOW_H
