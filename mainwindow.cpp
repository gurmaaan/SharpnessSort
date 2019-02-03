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
    QString pathStr = QFileDialog::getExistingDirectory(nullptr, TEST_DIR_TITTLE, TEST_DIR);
    QDir imgDir(pathStr);
    ui->dirPath_le->setText(pathStr);
    QStringList imgNames = imgDir.entryList(QStringList() << IMG_FORMAT << IMG_FORMAT,QDir::Files);
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

        QString imgPath = QString(pathStr) + QDir::separator() + _imgNames.at(i);
        QImage img(imgPath);

        if(!img.isNull())
        {
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
    }
    ui->baseImg_cb->addItems(_imgNames);

    setupModelRow(_model, numRow, 0, HEADER_NUM);
    setupModelRow(_model, imgRow, 1, HEADER_IMG);
    setupModelRow(_model, nameRow, 2, HEADER_NAME);
    setupModelRow(_model, sharpRow, 3, HEADER_SHARP);

    setActiveImg(0);
    int imgW = getActiveImage().width();
    int imgH = getActiveImage().height();
    setSB(ui->imgSizeH_sb, imgH);
    setSB(ui->imgSizeW_sb, imgW);
    ui->areaH_sb->setMaximum(imgH);
    ui->areaH_sldr->setMaximum(imgH);
    ui->areaW_sb->setMaximum(imgW);
    ui->areaW_sldr->setMaximum(imgW);
    ui->areaH_sldr->setValue(imgH / 10);
    ui->areaW_sldr->setValue(imgW / 10);
    setBaseIndex(0);

    ui->tableView->setRowHeight(1, TABLE_IC_SIZE);
}

void MainWindow::setupWidgets()
{
    _model = new QStandardItemModel;
    ui->tableView->setModel(_model);

    _viewScene = new QGraphicsScene;
    ui->view_gv->setScene(_viewScene);

    _diffScene = new QGraphicsScene;
    ui->diff_gv->setScene(_diffScene);
}

void MainWindow::connectAll()
{
    //connect(_viewScene->items()->first(), &GraphicsViewRectItem::posChanged,
        //    this, &MainWindow::receiveRect);
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
        setActiveIndex(index);
        ui->tableView->selectColumn(index);

        QImage activeImg = _images.at(index);
        QImage res = diffImages(_baseImage, activeImg);
        setActiveImg(activeImg);
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

void MainWindow::setupModelRow(QStandardItemModel *model, QList<QStandardItem *> row, int rowNum, QString headerName)
{
    QStandardItem *verticalHeader = new QStandardItem(headerName);
    model->appendRow(row);
    model->setVerticalHeaderItem(rowNum, verticalHeader);
}

void MainWindow::paintViewRect(double tlx, double tly, int w, int h)
{
    _viewScene->clear();
    QPixmap pm(QPixmap::fromImage(getActiveImage()));
    QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(pm);
    _viewScene->addItem(pixmapItem);
    QRectF rect(tlx, tly, w, h);
    GraphicsViewRectItem *viewArea = new GraphicsViewRectItem(rect);
    _viewScene->addItem(viewArea);
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
    setSB(ui->tlx_sb, static_cast<int>(visible.topLeft().x() / 10));
    setSB(ui->tly_sb, static_cast<int>(visible.topLeft().y() / 10));
    setSB(ui->brx_sb, static_cast<int>(visible.bottomRight().x() / 10));
    setSB(ui->bry_sb, static_cast<int>(visible.bottomRight().y() / 10));
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

int MainWindow::sharpKoeff(Mask mask, QImage img)
{
    int maskW = mask.size().width();
    int maskH = mask.size().height();

    QImage grayImg = grayScaleImg(img);
    QVector<int> kVals;
    for(int j = 0; j < img.height() - maskH; j++)
    {
        int coeff = 0;
        for(int i = 0; i < img.width() - maskW; i++)
        {
            QVector<QVector<int>> newMask;
            //считаю новую маску
            for(int ii = 0; ii < maskW; ii++)
            {
                QVector<int> newMaskRow;
                for(int jj = 0; jj < maskH; jj++)
                {
                    int mk = mask.maskAt(ii, jj) * grayImg.pixelColor(i+ii, j+jj).red();
                    newMaskRow.append(mk);
                }
                newMask << newMaskRow;
                newMaskRow.clear();
            }

            long int sum = 0;
            long int posSum = 0;

            //нахожу сумму маски и сумму ее положительных элементов
            for(int jj = 0; jj < newMask.length(); jj++)
            {
                QVector<int> newMaskRow = newMask.at(jj);
                for(int ii = 0; ii < newMaskRow.length(); ii++)
                {
                    int nmk = newMaskRow.at(ii);
                    sum += nmk;
                    if(nmk > 0)
                        posSum += nmk;
                }
            }
            coeff = posSum / sum;
            kVals.append(coeff);
        }
    }
    int max = *std::max_element(kVals.begin(), kVals.end());
    return  max;
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

void MainWindow::receiveRect(QRectF rect)
{
    qDebug() << rect;
}

void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
   setActiveImg(index.column());
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
    Mask mask(QSize(ui->sharpMask_width_sb->value(), ui->sharpMask_height_sb->value()), 
              ui->sharpMascType_cb->currentIndex());
    mask.print();
    for(QImage img : _images)
        qDebug() << sharpKoeff(mask, img);
}

QImage MainWindow::getDiffImg() const
{
    return _diffImg;
}

void MainWindow::setDiffImg(const QImage &diffImg)
{
    _diffImg = diffImg;
}

void MainWindow::on_areaSetup_btn_clicked(bool checked)
{
    ui->areaH_sb->setEnabled(checked);
    ui->areaH_lbl->setEnabled(checked);
    ui->areaW_lbl->setEnabled(checked);
    ui->areaW_sb->setEnabled(checked);
    ui->areaH_sldr->setEnabled(checked);
    ui->areaW_sldr->setEnabled(checked);
    if(checked)
    {
        double tlx = 0.0, tly = 0.0;
        if(_viewScene->items().count() > 1)
        {
            tlx = _viewScene->items().last()->pos().x();
            tly = _viewScene->items().last()->pos().y();
        }
        paintViewRect(tlx, tly, ui->areaW_sb->value(), ui->areaH_sb->value());
    }
}

void MainWindow::on_areaW_sb_valueChanged(int arg1)
{
    if(ui->areaSetup_btn->isChecked())
        paintViewRect(_viewScene->items().first()->pos().x(),
                      _viewScene->items().first()->pos().y(),
                      arg1,
                      static_cast<int>(_viewScene->items().first()->boundingRect().height()));
}

void MainWindow::on_areaH_sb_valueChanged(int arg1)
{
    if(ui->areaSetup_btn->isChecked())
        paintViewRect(_viewScene->items().first()->pos().x(),
                      _viewScene->items().first()->pos().y(),
                      static_cast<int>(_viewScene->items().first()->boundingRect().width()),
                      arg1);
}
