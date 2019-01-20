#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QStandardItemModel>
#include <QImage>
#include <QGuiApplication>
//
#include <algorithm>
//
#include "static.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_openDir_triggered();

    void on_action_exit_triggered();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *_model;
    QList<QImage> _images;
    //
    void setupWidgets();
    void setActiveImg(int index);
};

#endif // MAINWINDOW_H
