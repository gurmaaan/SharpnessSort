#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QStandardItemModel>
#include <QImage>
#include <QColor>
#include <QGuiApplication>
#include <QGraphicsView>
#include <QSpinBox>
#include <QGraphicsItem>
//
#include <algorithm>
//
#include "static.h"
#include "widgets/clickablegs.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QImage getBaseImage() const;
    void setBaseImage(const QImage &value);

signals:
    void resultCalced(QImage result);

public slots:
    void setImgDiff(QImage result);

private slots:
    void on_action_openDir_triggered();
    void on_action_exit_triggered();
    void on_action_next_triggered();
    void on_action_previous_triggered();
    void on_action_first_triggered();
    void on_action_last_triggered();

    void on_baseImg_cb_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    QStandardItemModel *_model;
    ClickableGS *_viewScene;
    QGraphicsScene *_diffScene;
    QList<QImage> _images;
    QImage _baseImage;
    int _activeIndex;
    //
    void setupWidgets();
    void connectAll();
    void setSB(QSpinBox *sb, int value);
    void setActiveImg(int index);
    void scaleImage(double k);
    void diffImages(QImage base, QImage current);
};

#endif // MAINWINDOW_H
