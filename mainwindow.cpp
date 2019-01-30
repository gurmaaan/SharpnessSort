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

    _baseIndex = 0;
    _sharpIndex = 0;
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
    setImgNames(imgNames);

    ui->dir_progress->setMaximum(imgNames.count());
    ui->tableView->setIconSize(QSize(TABLE_IC_SIZE,TABLE_IC_SIZE));

    QList<QStandardItem*> numRow;
    QList<QStandardItem*> imgRow;
    QList<QStandardItem*> nameRow;
    QList<QStandardItem*> sharpRow;
    for(int i = 0; i < _imgNames.length(); i++)
    {
        setSB(ui->imgCnt_sb, i);

        QString imgPath = QString(TEST_DIR) + QDir::separator() + _imgNames.at(i);
        QImage img(imgPath);

        setSB(ui->imgSizeH_sb, img.height());
        setSB(ui->imgSizeW_sb, img.width());
        _images.append(img);
        QStandardItem *imgItem = new QStandardItem;
        QStandardItem *nameItem = new QStandardItem(_imgNames.at(i));
        QStandardItem *numItem = new QStandardItem(QString::number(i+1));
        numItem->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
        //TODO: коэффициент резкости
        QStandardItem *sharpItem = new QStandardItem(QString::number(0));
        nameItem->setData(Qt::AlignCenter, Qt::TextAlignmentRole);

        imgItem->setIcon(QPixmap::fromImage(img));
        imgItem->setData(Qt::AlignCenter, Qt::TextAlignmentRole);

        numRow << numItem;
        imgRow << imgItem;
        nameRow << nameItem;
        sharpRow << sharpItem;

        ui->dir_progress->setValue(i+1);
    }
    ui->baseImg_cb->addItems(_imgNames);

    setupModelRow(_model, numRow, 0, "Номер");
    setupModelRow(_model, imgRow, 1, "Изображение");
    setupModelRow(_model, nameRow, 2, "Имя");
    setupModelRow(_model, sharpRow, 3, "Критерий резкости");

    setActiveImg(0);
    setBaseIndex(0);

    ui->tableView->setRowHeight(1, TABLE_IC_SIZE);

    setVisibleRectCorners(ui->view_gv->mapToScene(ui->view_gv->viewport()->geometry()).boundingRect());
}

void MainWindow::setupWidgets()
{

    _model = new QStandardItemModel;
    ui->tableView->setModel(_model);

    _viewScene = new ClickableGS;
    ui->view_gv->setScene(_viewScene);

    _diffScene = new QGraphicsScene;
    ui->diff_gv->setScene(_diffScene);
}

void MainWindow::connectAll()
{
    connect(this, &MainWindow::resultCalced, 
            this, &MainWindow::setImgDiff);
    //-------------------------------------
    connect(ui->view_gv->horizontalScrollBar(), &QScrollBar::sliderMoved,
            this, &MainWindow::updateCorners);
    connect(ui->view_gv->verticalScrollBar(), &QScrollBar::valueChanged,
            this, &MainWindow::updateCorners);
    connect(ui->diff_gv->verticalScrollBar(), &QScrollBar::valueChanged,
            this, &MainWindow::updateDiffCorners);
    connect(ui->view_gv->horizontalScrollBar(), &QScrollBar::valueChanged,
            this, &MainWindow::updateCorners);
    connect(ui->diff_gv->horizontalScrollBar(), &QScrollBar::valueChanged,
            this, &MainWindow::updateDiffCorners);
    connect(ui->view_gv->verticalScrollBar(), &QScrollBar::sliderMoved,
            this, &MainWindow::updateCorners);
    connect(ui->diff_gv->horizontalScrollBar(), &QScrollBar::sliderMoved,
            this, &MainWindow::updateDiffCorners);
    connect(ui->diff_gv->verticalScrollBar(), &QScrollBar::sliderMoved,
            this, &MainWindow::updateDiffCorners);
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
        setVisibleRectCorners(ui->view_gv->mapToScene(ui->view_gv->viewport()->geometry()).boundingRect());
        setActiveIndex(index);
        ui->tableView->selectColumn(index);

        QImage activeImg = _images.at(index);
        QImage res = diffImages(_baseImage, activeImg);

        double dWidth = static_cast<double>(activeImg.width());
        double dNewWidth = dWidth * ui->scale_sb->value();
        int newWidth = static_cast<int>(dNewWidth);

        activeImg = activeImg.scaledToWidth(newWidth);
        setActiveImg(activeImg);
        res = res.scaledToWidth(newWidth);
        setImgDiff(res);
    }
}

void MainWindow::setActiveImg(QImage img)
{
    _viewScene->clear();
    setActiveImage(img);
    QPixmap pixmap( QPixmap::fromImage(img) );
    QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(pixmap);
    _viewScene->addItem(pixmapItem);
}

void MainWindow::scaleImage(double k)
{
    Q_UNUSED(k);
    //TODO : image scaling
}

void MainWindow::setupModelRow(QStandardItemModel *model, QList<QStandardItem *> row, int rowNum, QString headerName)
{
    QStandardItem *verticalHeader = new QStandardItem(headerName);
    model->appendRow(row);
    model->setVerticalHeaderItem(rowNum, verticalHeader);
}

QImage MainWindow::diffImages(QImage base, QImage current)
{   
    QImage resOutputImg(base.size(), base.format());
    int k = ui->diffK_sb->value();
    if(base.size() == current.size())
    {
        for(int j = 0; j < base.height(); j++)
        {
            for(int i = 0; i < base.width(); i++)
            {
                int r = (( base.pixelColor(i,j).red() - current.pixelColor(i,j).red() ) * k) + 128;
                int g = (( base.pixelColor(i,j).green() - current.pixelColor(i,j).green() ) * k) + 128;
                int b = (( base.pixelColor(i,j).blue() - current.pixelColor(i,j).blue() ) * k) + 128;

                QColor resClr = validColor(r, g, b);
                resOutputImg.setPixelColor(i, j, resClr);
            }
        }
    }
    setDiffImg(resOutputImg);
    return resOutputImg;
}

void MainWindow::setVisibleRectCorners(QRectF visible)
{
    setSB(ui->tlx_sb, static_cast<int>(visible.topLeft().x()));
    setSB(ui->tly_sb, static_cast<int>(visible.topLeft().y()));
    setSB(ui->brx_sb, static_cast<int>(visible.bottomRight().x()));
    setSB(ui->bry_sb, static_cast<int>(visible.bottomRight().y()));
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

int MainWindow::sharpKoeff(QVector<QVector<int> > mask, QImage img)
{
    int maskW = mask.first().length();
    int maskH = mask.length();

    int coeff = 0;

    for(int j = 0; j < img.height() - maskH; j++)
    {
        for(int i = 0; i < img.width() - maskW; i++)
        {
            int gray = qGray(img.pixel(i, j));

        }
    }
    return  coeff;
}

QImage MainWindow::grayScaleImg(QImage img)
{
    QImage grayImg(img.size(), img.format());
    for (int j = 0; j < img.height(); j++)
    {
        for (int i = 0; i < img.width(); i++)
        {
            int gray = qGray(img.pixel(i,j));
            grayImg.setPixelColor(i, j, QColor(gray, gray, gray));
        }
    }
    return grayImg;
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
        setBaseIndex(index);
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
    setDiffImg(result);
    _diffScene->clear();
    QPixmap pm(QPixmap::fromImage(result));
    QGraphicsPixmapItem *pmItem = new QGraphicsPixmapItem(pm);
    _diffScene->addItem(pmItem);
}

void MainWindow::setBaseIndex(int baseIndex)
{
    for(int i = 0; i < _model->columnCount(); i++)
    {
       QModelIndex mIndex(_model->index(2, i));
       if(mIndex.data(Qt::DisplayRole).toString().startsWith(BASE_IMG_STR))
           _model->setData(mIndex, _imgNames.at(i), Qt::DisplayRole);
       _model->setData(mIndex, QColor(Qt::white), Qt::BackgroundColorRole);
    }

    QModelIndex mBaseIndex(_model->index(2, baseIndex));
    _model->setData(mBaseIndex, QColor(Qt::green), Qt::BackgroundColorRole);
    _model->setData(mBaseIndex, QString(BASE_IMG_STR) + _imgNames.at(baseIndex), Qt::DisplayRole);
}

void MainWindow::updateCorners(int scrollBarPos)
{
    Q_UNUSED(scrollBarPos);
    setVisibleRectCorners(ui->view_gv->mapToScene(ui->view_gv->viewport()->geometry()).boundingRect());
}

void MainWindow::updateDiffCorners(int scrollBarPos)
{
    Q_UNUSED(scrollBarPos);
    setVisibleRectCorners(ui->diff_gv->mapToScene(ui->diff_gv->viewport()->geometry()).boundingRect());
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

int MainWindow::getActiveIndex() const
{
    return _activeIndex;
}

void MainWindow::setActiveIndex(int activeIndex)
{
    _activeIndex = activeIndex;
}

QStringList MainWindow::getImgNames() const
{
    return _imgNames;
}

void MainWindow::setImgNames(const QStringList &imgNames)
{
    _imgNames = imgNames;
}

void MainWindow::on_diffK_H_sldr_valueChanged(int value)
{
    Q_UNUSED(value);
    //TODO: обновление разности при смене коэффициента
//    QImage res = diffImages(_baseImage, _activeImage);
}

QImage MainWindow::getActiveImage() const
{
    return _activeImage;
}

void MainWindow::setActiveImage(const QImage &activeImage)
{
    _activeImage = activeImage;
}

void MainWindow::on_calckSharp_btn_clicked()
{
    Mask mask(QSize(ui->sharpMask_width_sb->value(), ui->sharpMask_height_sb->value()), ui->sharpMascType_cb->currentIndex());
    mask.print();
    QImage gray = grayScaleImg(getActiveImage());
    setActiveImg(gray);
}

void MainWindow::on_scale_sb_valueChanged(double arg1)
{
    //BUG : при прорутке слайдера в положительную сторону все отлично, при прокрутке в отрицательную сторону какая-то залупа происходит и все виснет
    QImage activeImg = getActiveImage();
    QImage diffImage = getDiffImg();

    double dWidth = static_cast<double>(activeImg.width());
    double dNewWidth = dWidth * arg1;
    int newWidth = static_cast<int>(dNewWidth);
    activeImg = activeImg.scaledToWidth(newWidth);

    QPixmap activePixmap = QPixmap::fromImage(activeImg);
    QPixmap diffPixmap = QPixmap::fromImage(diffImage);
    QGraphicsPixmapItem *activePmIrtem = new QGraphicsPixmapItem(activePixmap);
    QGraphicsPixmapItem *diffPmIrtem = new QGraphicsPixmapItem(diffPixmap);

    _viewScene->clear();
    _viewScene->addItem(activePmIrtem);
    _diffScene->clear();
    _diffScene->addItem(diffPmIrtem);
}
void MainWindow::on_scale_sldr_sliderMoved(int position)
{
    double dPosition = static_cast<double>(position);
    ui->scale_sb->setValue(dPosition / 10.0);
}

QImage MainWindow::getDiffImg() const
{
    return _diffImg;
}

void MainWindow::setDiffImg(const QImage &diffImg)
{
    _diffImg = diffImg;
}
