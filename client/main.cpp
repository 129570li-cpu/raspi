//
// Created by 李玉辉 on 2026/10/13.
//
#include <QApplication>
#include <QWidget>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QWidget window;
  window.resize(800, 500);
  window.setWindowTitle("Raspberry Pi Device Manager");
  window.show();

  return app.exec();
}