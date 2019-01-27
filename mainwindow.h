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

    int getActiveIndex() const;
    void setActiveIndex(int activeIndex);

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
    void on_tableView_clicked(const QModelIndex &index);
    void on_MainWindow_customContextMenuRequested(const QPoint &pos);
    void on_scale_sldr_sliderMoved(int position);
    void on_vie_gv_rubberBandChanged(const QRect &viewportRect, const QPointF &fromScenePoint, const QPointF &toScenePoint);

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
    void setActiveImg(QImage img);
    void setBaseImgPreview(QImage img);
    void scaleImage(double k);
    void diffImages(QImage base, QImage current);
    void setVisibleRectCorners(QRectF visible);
    QColor validColor(int r = 0, int g = 0, int b = 0);
    int validComponent(int c = 0);
};

#endif // MAINWINDOW_H
