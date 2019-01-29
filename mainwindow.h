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

    QStringList getImgNames() const;
    void setImgNames(const QStringList &imgNames);

    QImage getActiveImage() const;
    void setActiveImage(const QImage &activeImage);

signals:
    void resultCalced(QImage result);

public slots:
    void setImgDiff(QImage result);
    void setBaseIndex(int baseIndex);

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

    void on_diffK_H_sldr_valueChanged(int value);

    void on_calckSharp_btn_clicked();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *_model;
    QStringList _imgNames;
    ClickableGS *_viewScene;
    QGraphicsScene *_diffScene;
    QList<QImage> _images;
    QImage _baseImage;
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
    QVector< QVector<int> > genSharpMask(int w = 2, int h = 2, int type = 0);
    void printSharpMask(QVector< QVector<int> > mask);
    int sumOfPosMaskKoeff(QVector< QVector<int> > mask);
    int sharpKoeff(QVector< QVector<int> > mask, QImage img);
};

#endif // MAINWINDOW_H
