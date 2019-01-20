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
#include <QGraphicsView>
#include <QGraphicsItem>
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
    void on_action_next_triggered();
    void on_action_previous_triggered();
    void on_action_first_triggered();
    void on_action_last_triggered();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *_model;
    QGraphicsScene *_scene;
    QList<QImage> _images;
    int _activeIndex;
    //
    void setupWidgets();
    void setActiveImg(int index);
};

#endif // MAINWINDOW_H
