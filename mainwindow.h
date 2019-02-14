#ifndef MAINWINDOW_H3
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
#include "mask.h"
#include "widgets/graphicsviewrectitem.h"
#include "widgets/qcustomplot.h"

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
    int getActiveIndex() const;
    QStringList getImgNames() const;
    QImage getActiveImage() const;
    QImage getDiffImg() const;
    QRectF getVisibleAreaRect() const;

    void setBaseImage(const QImage &value);
    void setActiveIndex(int activeIndex);
    void setImgNames(const QStringList &imgNames);
    void setActiveImage(const QImage &activeImage);
    void setDiffImg(const QImage &diffImg);
    void setVisibleAreaRect(const QRectF &visibleAreaRect);
    double getDelta() const;
    void setDelta(double delta);

public slots:
    void setImgDiff(QImage result);
    void setBaseIndex(int baseIndex);
    void setVisibleRectCorners(QRectF visible);

private slots:
    void on_action_openDir_triggered();
    void on_action_exit_triggered();
    void on_action_next_triggered();
    void on_action_previous_triggered();
    void on_action_first_triggered();
    void on_action_last_triggered();
    void on_baseImg_cb_currentIndexChanged(int index);
    void on_tableView_clicked(const QModelIndex &index);
    void on_calckSharp_btn_clicked();
    void on_areaSetup_btn_clicked(bool checked);
    void on_areaW_sb_valueChanged(int arg1);
    void on_areaH_sb_valueChanged(int arg1);
    void on_plotCurrent_sldr_sliderMoved(int position);

    void on_plotCurrent_sb_valueChanged(double arg1);

    void on_move_btn_clicked();

    void on_cancelButton_clicked();

    void on_moveAreaTop_btn_clicked();

    void on_areaMoveRight_btn_clicked();

    void on_areaMoveLeft_btn_clicked();

    void on_moveAreaBottom_btn_clicked();

    void on_actionSaveToFile_triggered();

private:
    Ui::MainWindow *ui;

    QStandardItemModel *_model;
    QGraphicsScene *_viewScene;
    QGraphicsScene *_diffScene;
    QCustomPlot *_plot;

    QStringList _imgNames;
    QList<QImage> _originalImages;
    QList<QImage> _images;
    QList<QImage> _croppedImages;
    QRectF _visibleAreaRect;
    QImage _baseImage;
    QImage _diffImg;
    QImage _activeImage;
    int _baseIndex;
    int _activeIndex;
    int _sharpIndex;
    double _delta;

    QVector<double> _sharpK;
    //
    void setupWidgets();
    void setSB(QSpinBox *sb, int value);
    void setActiveImg(int index);
    void setActiveImg(QImage img);
    void setupModelRow(QStandardItemModel *model, QList<QStandardItem*> row, int rowNum = 0, QString headerName = "Header");
    //
    void paintViewRect(double tlx, double tly, int w, int h);
    //----------------------------------------------
    QImage diffImages(QImage base, QImage current);
    QColor validColor(int r = 0, int g = 0, int b = 0);
    int validComponent(int c = 0);
    //---------------------------------------------
    double sharpKoeff(Mask mask, QImage img);
    double randomSHarp(double min, double max);
    int avBr(QImage gray);
    QImage grayScaleImg(QImage img);
    void buildPlot(QVector<double> sharpK);
};

#endif // MAINWINDOW_H
