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
#include "mask.h"
#include "widgets/graphicsviewrectitem.h"

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

    void setBaseImage(const QImage &value);
    void setActiveIndex(int activeIndex);
    void setImgNames(const QStringList &imgNames);
    void setActiveImage(const QImage &activeImage);

    QImage getDiffImg() const;
    void setDiffImg(const QImage &diffImg);

signals:
    void resultCalced(QImage result);

public slots:
    void setImgDiff(QImage result);
    void setBaseIndex(int baseIndex);
    void updateCorners(int scrollBarPos);
    void updateDiffCorners(int scrollBarPos);
    void updateCurosor(QCursor cursor);

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

private:
    Ui::MainWindow *ui;
    QStandardItemModel *_model;
    QStringList _imgNames;
    QGraphicsScene *_viewScene;
    QGraphicsScene *_diffScene;
    QList<QImage> _images;
    QImage _baseImage;
    QImage _diffImg;
    int _baseIndex;
    QImage _activeImage;
    int _activeIndex;

    int _sharpIndex;
    //
    void setupWidgets();
    void connectAll();
    void setSB(QSpinBox *sb, int value);
    void setActiveImg(int index);
    void setActiveImg(QImage img);
    void scaleImage(double k);
    void setupModelRow(QStandardItemModel *model, QList<QStandardItem*> row, int rowNum = 0, QString headerName = "Header");
    //----------------------------------------------
    QImage diffImages(QImage base, QImage current);
    void setVisibleRectCorners(QRectF visible);
    QColor validColor(int r = 0, int g = 0, int b = 0);
    int validComponent(int c = 0);
    //---------------------------------------------
    int sharpKoeff(Mask mask, QImage img);
    QImage grayScaleImg(QImage img);
};

#endif // MAINWINDOW_H
