#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupWidgets();
    connectAll();
    showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_openDir_triggered()
{
    QDir imgDir(TEST_DIR);
    ui->dirPath_le->setText(QFileDialog::getExistingDirectory(nullptr, "Выберите папку", imgDir.absolutePath()));
    QStringList imgNames = imgDir.entryList(QStringList() << "*.bmp" << "*.bmp",QDir::Files);

    ui->dir_progress->setMaximum(imgNames.count());
    ui->tableView->setIconSize(QSize(100, 100));
    ui->tableView->setRowHeight(0, 100);

    QList<QStandardItem*> imgRow;
    QList<QStandardItem*> nameRow;
    for(int i = 0; i < imgNames.length(); i++)
    {
        setSB(ui->imgCnt_sb, i);

        QString imgPath = QString(TEST_DIR) + QDir::separator() + imgNames.at(i);
        QImage img(imgPath);

        setSB(ui->imgSizeH_sb, img.height());
        setSB(ui->imgSizeW_sb, img.width());
        _images.append(img);
        QStandardItem *imgItem = new QStandardItem;
        QStandardItem *nameItem = new QStandardItem(imgNames.at(i));
        nameItem->setData(Qt::AlignCenter, Qt::TextAlignmentRole);

        imgItem->setSizeHint(QSize(100, 100));
        imgItem->setIcon(QPixmap::fromImage(img));

        imgRow << imgItem;
        nameRow << nameItem;

        ui->dir_progress->setValue(i+1);
    }

    ui->baseImg_cb->addItems(imgNames);
    _model->appendRow(imgRow);
    _model->appendRow(nameRow);
    setActiveImg(0);
    setBaseImgPreview(_images.first());
}

void MainWindow::setupWidgets()
{
    _model = new QStandardItemModel;
    ui->tableView->setModel(_model);

    _viewScene = new ClickableGS;
    ui->vie_gv->setScene(_viewScene);

    _diffScene = new QGraphicsScene;
    ui->diff_gv->setScene(_diffScene);
}

void MainWindow::connectAll()
{
    connect(this, &MainWindow::resultCalced, 
            this, &MainWindow::setImgDiff);
}

void MainWindow::setSB(QSpinBox *sb, int value)
{
    sb->setMaximum(value);
    sb->setMinimum(0);
    sb->setValue(value);
}

void MainWindow::setActiveImg(int index)
{
    if( (index >= 0) && (index < _images.count()) )
    {
        setVisibleRectCorners(ui->vie_gv->mapToScene(ui->vie_gv->viewport()->geometry()).boundingRect());
        setActiveIndex(index);
        ui->tableView->selectColumn(index);

        QImage activeImg = _images.at(index);
        setActiveImg(activeImg);
        diffImages(_baseImage, activeImg);
    }
}

void MainWindow::setActiveImg(QImage img)
{
    _viewScene->clear();
    QPixmap pixmap( QPixmap::fromImage(img) );
    QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(pixmap);
    _viewScene->addItem(pixmapItem);
}

void MainWindow::setBaseImgPreview(QImage img)
{
    QPixmap pixMap = QPixmap::fromImage(img);
    ui->baseImagPrevie_lbl->setPixmap(pixMap.scaled(250, 150));
}

void MainWindow::scaleImage(double k)
{
    Q_UNUSED(k);
    //TODO : image scaling
}

void MainWindow::diffImages(QImage base, QImage current)
{   
    int k = ui->diffK_sb->value();
    if(base.size() == current.size())
    {
        QImage resImg(base.size(), base.format());
        for(int j = 0; j < base.height(); j++)
        {
            for(int i = 0; i < base.width(); i++)
            {
                int r = (( base.pixelColor(i,j).red() - current.pixelColor(i,j).red() ) * k) + 128;
                int g = (( base.pixelColor(i,j).green() - current.pixelColor(i,j).green() ) * k) + 128;
                int b = (( base.pixelColor(i,j).blue() - current.pixelColor(i,j).blue() ) * k) + 128;

                QColor resClr = validColor(r, g, b);
                resImg.setPixelColor(i, j, resClr);
            }
        }
        setImgDiff(resImg);
    }
}

void MainWindow::setVisibleRectCorners(QRectF visible)
{
    setSB(ui->tlx_sb, static_cast<int>(visible.topLeft().x()));
    setSB(ui->tly_sb, static_cast<int>(visible.topLeft().y()));
    setSB(ui->brx_sb, static_cast<int>(visible.bottomRight().x()));
    setSB(ui->brx_sb, static_cast<int>(visible.bottomRight().y()));
}

QColor MainWindow::validColor(int r, int g, int b)
{
    int newR = 0, newG = 0, newB = 0;
    newR = validComponent(r);
    newG = validComponent(g);
    newB = validComponent(b);
    QColor outClr(newR, newG, newB);
    return outClr;
}

int MainWindow::validComponent(int c)
{
    int outC = 0;
    if(c < 0)
        outC = 0;
    else if(c > 255)
        outC = 255;
    else
        outC = c;
    return outC;
}

void MainWindow::on_action_exit_triggered()
{
    QGuiApplication::exit();
}

void MainWindow::on_action_next_triggered()
{
    setActiveImg(getActiveIndex() + 1);
}

void MainWindow::on_action_previous_triggered()
{
    setActiveImg(getActiveIndex() - 1);
}

void MainWindow::on_action_first_triggered()
{
    setActiveImg(0);
}

void MainWindow::on_action_last_triggered()
{
    setActiveImg(_images.length() - 1);
}

void MainWindow::on_baseImg_cb_currentIndexChanged(int index)
{
    if(index >= 0 && index < _images.count())
    {
        QImage baseImg = _images.at(index);
        setBaseImage(baseImg);
        setBaseImgPreview(baseImg);
        for(int i = 0; i < _model->columnCount(); i++)
           _model->setData(_model->index(1,i), QColor(Qt::white), Qt::BackgroundColorRole);
        _model->setData(_model->index(1,index), QColor(Qt::green), Qt::BackgroundColorRole);
    }
}

QImage MainWindow::getBaseImage() const
{
    return _baseImage;
}

void MainWindow::setBaseImage(const QImage &value)
{
    _baseImage = value;
}

void MainWindow::setImgDiff(QImage result)
{
    _diffScene->clear();
    QPixmap pm(QPixmap::fromImage(result));
    QGraphicsPixmapItem *pmItem = new QGraphicsPixmapItem(pm);
    _diffScene->addItem(pmItem);
}

void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
   setActiveImg(index.column());
}

void MainWindow::on_MainWindow_customContextMenuRequested(const QPoint &pos)
{
    //TODO : доделать меню
   QMenu *menu = new QMenu(this);
   QAction *setActiveAction = new QAction("Установить опорным", this);
   menu->addAction(setActiveAction);
   menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

void MainWindow::on_scale_sldr_sliderMoved(int position)
{
    double posD = static_cast<double>(position) / 10.0;
    ui->scale_sb->setValue(posD);
    if(_viewScene->items().count() > 0)
    {
        QImage img = _images.at(getActiveIndex());
        if(position <= 5)
        {
//            QImage scaled = img.scaledToWidth(img.height() * position);
//            QPixmap pixmap = QPixmap::fromImage(scaled);
//            _viewScene->clear();

//            QGraphicsPixmapItem *scaledItem = new QGraphicsPixmapItem(pixmap);
//            _viewScene->addItem(scaledItem);
        }
        //QPixmap pm(QPixmap::fromImage(img.scaled(img.width() * position, img.height() * position)));
    }
}

int MainWindow::getActiveIndex() const
{
    return _activeIndex;
}

void MainWindow::setActiveIndex(int activeIndex)
{
    _activeIndex = activeIndex;
}

void MainWindow::on_vie_gv_rubberBandChanged(const QRect &viewportRect, const QPointF &fromScenePoint, const QPointF &toScenePoint)
{
    qDebug() << viewportRect;
}
