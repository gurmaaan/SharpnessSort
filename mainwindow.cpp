#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupWidgets();

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
            _images << img;
            _originalImages << img;
            QStandardItem *imgItem = new QStandardItem;
            QStandardItem *nameItem = new QStandardItem(_imgNames.at(i));
            QStandardItem *numItem = new QStandardItem(QString::number(i+1));
            numItem->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
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

    setupModelRow(_model, numRow, 0, QString(HEADER_NUM));
    setupModelRow(_model, imgRow, 1, QString(HEADER_IMG));
    setupModelRow(_model, nameRow, 2, QString(HEADER_NAME));
    setupModelRow(_model, sharpRow, 3, QString(HEADER_SHARP));

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
    ui->scale_sb->setValue(1.0);
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

    _plot = ui->plot_gv;
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
    ui->view_gv->fitInView(pixmapItem);
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
    connect(viewArea, &GraphicsViewRectItem::posChanged, this, &MainWindow::setVisibleRectCorners);
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
    setVisibleAreaRect(visible);
    setSB(ui->tlx_sb, static_cast<int>( visible.topLeft().x()     ) );
    setSB(ui->tly_sb, static_cast<int>( visible.topLeft().y()     ) );
    setSB(ui->brx_sb, static_cast<int>( visible.bottomRight().x() ) );
    setSB(ui->bry_sb, static_cast<int>( visible.bottomRight().y() ) );
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

double MainWindow::randomSHarp(double min, double max)
{
    double rand = static_cast<double>(qrand()) / static_cast<double>(RAND_MAX);
    return min + rand * (max - min);
}

int MainWindow::avBr(QImage gray)
{
    long int cnt = gray.height() * gray.width();
    long int sum = 0;
    for(int j = 0; j < gray.height(); j++)
    {
        for(int i = 0; i < gray.width(); i++)
        {
            sum += qGray(gray.pixel(i, j));
        }
    }
    return sum / cnt;
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

void MainWindow::buildPlot(QVector<double> sharpK)
{
    ui->tabWidget->setCurrentIndex(2);
    _plot->clearGraphs();

    _plot->legend->setVisible(false);
    _plot->legend->setFont(QFont("Helvetica", 8));

    QCPAxis *xAxis = _plot->xAxis;
    QCPAxis *yAxis = _plot->yAxis;

    double maxY = *std::max_element(sharpK.constBegin(), sharpK.constEnd());
    double minY = *std::min_element(sharpK.constBegin(), sharpK.constEnd());

    ui->plotMaxMin_min_sb->setMaximum(minY);
    ui->plotMaxMin_min_sb->setValue(minY);
    ui->plotMaxMin_max_sb->setMaximum(maxY);
    ui->plotMaxMin_max_sb->setValue(maxY);

    ui->plotCurrent_sb->setValue(minY);

    double delta = maxY - minY;
    double d = delta / 100.0;
    setDelta(d);

    double maxX = 0.0;
    double minX = static_cast<double>(sharpK.length());

    xAxis->setLabel("Номер изображения");
    yAxis->setLabel("Коэффициент резкости");

    QString fFamily = xAxis->tickLabelFont().family();
    QFont::StyleHint fStyleHint = xAxis->tickLabelFont().styleHint();
    int fPointSize = fPointSize = xAxis->tickLabelFont().pointSize();
    QFont font(fFamily, fPointSize * 2);
    font.setBold(true);
    font.setStyleHint(fStyleHint);

    xAxis->setTickLabelFont(font);
    yAxis->setTickLabelFont(font);
    xAxis->setLabelFont(font);
    yAxis->setLabelFont(font);

    xAxis->setRange(minX, maxX);
    yAxis->setRange(10, 20);

    QVector<double> dNums;
    for(int i = 0; i < sharpK.length(); i++)
        dNums << static_cast<double>(i);

    _plot->addGraph();
    int gn = _plot->graphCount() - 1;
    _plot->graph(gn)->setPen(QPen(Qt::black));
    _plot->graph(gn)->setLineStyle(QCPGraph::lsLine);
    _plot->graph(gn)->setScatterStyle( QCPScatterStyle(QCPScatterStyle::ssDisc, 4) );
    _plot->graph(gn)->setData(dNums, sharpK);
    _plot->graph(gn)->setName("График зависимости коэффициента резкости от номера изображения в серии");

    _plot->axisRect()->setupFullAxesBox();
    _plot->replot();
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
    ui->diff_gv->fitInView(pmItem);
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
    ui->sharp_progress->setMaximum(_images.length());
    for(int i = 0; i < _images.count(); i++)
    {
        double k = sharpKoeff(mask, _images.at(i));
        _sharpK << k;
        _model->item(3,i)->setData(k, Qt::DisplayRole);
        ui->sharp_progress->setValue(i+1);
    }
    int maxIndex = 0;
    double max = *std::max_element(_sharpK.constBegin(), _sharpK.constEnd());

    qDebug() << max;
    for(int i = 0; i < _sharpK.length(); i++)
    {
        if(_sharpK.at(i) == max)
            maxIndex = i;
        _sharpIndex = i;
    }
    QString itemStr = QString(SHARP_IMG_STR) + _imgNames.at(maxIndex);
    _model->item(2, maxIndex)->setData(itemStr, Qt::DisplayRole);
    _model->item(2, maxIndex)->setData(QColor(Qt::blue), Qt::BackgroundColorRole);
    buildPlot(_sharpK);
}

double MainWindow::sharpKoeff(Mask mask, QImage img)
{
    int maskW = mask.size().width();
    int maskH = mask.size().height();
    int posSum = (maskW * maskH) / 2;

    QImage grayImg = grayScaleImg(img);
    QVector<double> kVals;
    for(int j = 0; j < img.height() - maskH; j++)
    {
        double coeff = 0;
        for(int i = 0; i < img.width() - maskW; i++)
        {
            QVector<QVector<int>> newMaskVector;
            //считаю новую маску
            for(int ii = 0; ii < maskW; ii++)
            {
                QVector<int> newMaskRow;
                for(int jj = 0; jj < maskH; jj++)
                {
                    int mk = mask.maskAt(ii, jj) * grayImg.pixelColor(i+ii, j+jj).red();
                    newMaskRow.append(mk);
                }
                newMaskVector << newMaskRow;
                newMaskRow.clear();
            }

            int sum = 0;

            //нахожу сумму маски и сумму ее положительных элементов
            for(int jj = 0; jj < newMaskVector.length(); jj++)
            {
                QVector<int> newMaskRow = newMaskVector.at(jj);
                for(int ii = 0; ii < newMaskRow.length(); ii++)
                    sum += newMaskRow.at(ii);

            }
            if(sum < 0)
                sum = sum * (-1);
            coeff = static_cast<double>(sum) / static_cast<double>(posSum);
            kVals.append(coeff);
        }
    }
    double max = *std::max_element(kVals.begin(), kVals.end());

    return  max;
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
            tlx = _viewScene->items().first()->pos().x();
            tly = _viewScene->items().first()->pos().y();
        }
        paintViewRect(tlx, tly, ui->areaW_sb->value(), ui->areaH_sb->value());
    }
    else
    {
        if(_viewScene->items().count() > 1)
            _viewScene->removeItem(_viewScene->items().first());

        double imgH = static_cast<double>(getActiveImage().height());
        double rectH = getVisibleAreaRect().height();
        ui->scale_sb->setValue(rectH / imgH);
        for(int i = 0 ; i < _images.length(); i++)
        {
            QImage cropped = _images.at(i).copy(getVisibleAreaRect().toRect());
            _croppedImages << cropped;
            QPixmap pixmap = QPixmap::fromImage(cropped);
            _model->item(1, i)->setIcon(pixmap);
        }
        _images.clear();

        for(QImage img : _croppedImages)
            _images << img;
        setActiveImg(0);
        setBaseImage(_images.at(0));
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

void MainWindow::on_plotCurrent_sldr_sliderMoved(int position)
{
    double current = static_cast<double>(position) * getDelta() + ui->plotMaxMin_min_sb->value();
    ui->plotCurrent_sb->setValue(current);
}

double MainWindow::getDelta() const
{
    return _delta;
}

void MainWindow::setDelta(double delta)
{
    _delta = delta;
}

QRectF MainWindow::getVisibleAreaRect() const
{
    return _visibleAreaRect;
}

void MainWindow::setVisibleAreaRect(const QRectF &visibleAreaRect)
{
    _visibleAreaRect = visibleAreaRect;
}

void MainWindow::on_plotCurrent_sb_valueChanged(double arg1)
{
//    int grNum = _plot->graphCount() - 1;
//    if(grNum > 1)
//    {
//        _plot->removeGraph(grNum);
//        _plot->legend->removeItem(grNum);
//    }
//    double x1 = 0.0;
//    double x2 = static_cast<double>(_images.length());
//    QVector<double> xVals, yVals;
//    xVals << x1 << x2;
//    yVals << arg1 << arg1;

//    _plot->addGraph();
//    _plot->graph(_plot->graphCount() - 1)->setPen(QPen(Qt::blue, 2));
//    _plot->graph(_plot->graphCount() - 1)->setLineStyle(QCPGraph::lsLine);
//    _plot->graph(_plot->graphCount() - 1)->setData(xVals, yVals);
//    _plot->graph(_plot->graphCount() - 1)->setName("Разделяющая прямая");

//    _plot->replot();

//    int aceptedCnt = 0;
//    for(double k : _sharpK)
//    {
//        if(k >= arg1)
//            aceptedCnt++;
//    }
//    int all = _sharpK.length();
//    double aceptedPerc = (static_cast<double>(aceptedCnt) / static_cast<double>(all)) * 100;
//    double declinedPerc = (static_cast<double>(all - aceptedCnt) / static_cast<double>(all)) * 100;
//    ui->plotMore_val_sb->setMaximum(aceptedCnt);
//    ui->plotMore_val_sb->setValue(aceptedCnt);
//    ui->plotLess_val_sb->setMaximum(all - aceptedCnt);
//    ui->plotLess_val_sb->setValue(all - aceptedCnt);

//    ui->plotMore_per_sb->setValue(aceptedPerc);
//    ui->plotLess_per_sb->setValue(declinedPerc);
}

void MainWindow::on_move_btn_clicked()
{
    ui->cancelButton->setEnabled(true);
    QVector<int> indexToMove;
    double porog = ui->plotCurrent_sb->value();
    for(int i = 0; i < _sharpK.length(); i++)
    {
        if(_sharpK.at(i) <= porog)
            indexToMove << i;
    }

    QString folder = ui->dirPath_le->text();
    QDir dir(folder);
    if(!dir.entryList().contains(UNSHARP_DIR))
        dir.mkdir(UNSHARP_DIR);

    for(int index : indexToMove)
    {
        QString oldName = folder + QDir::separator() + _imgNames.at(index);
        QFile imgFile(oldName);
        QString newName = folder + QDir::separator() + QString(UNSHARP_DIR) + QDir::separator() + _imgNames.at(index);
        imgFile.rename(newName);
    }
}

void MainWindow::on_cancelButton_clicked()
{
    QString unsharpFolder = ui->dirPath_le->text() + QDir::separator() + QString(UNSHARP_DIR);
    QDir dir(unsharpFolder);
    QStringList imgNames = dir.entryList(QStringList() << IMG_FORMAT << IMG_FORMAT,QDir::Files);
    for(QString imgName : imgNames)
    {
        QString oldName = unsharpFolder + QDir::separator() + imgName;
        QFile imgFile(oldName);
        QString newName = ui->dirPath_le->text() + QDir::separator() + imgName;
        imgFile.rename(newName);
    }
}

void MainWindow::on_moveAreaTop_btn_clicked()
{
    double tlxd = static_cast<double>(ui->tlx_sb->value());
    double tlyd = static_cast<double>(ui->tly_sb->value() + 1);
    QPointF newPos(tlxd, tlyd);
    ui->view_gv->items().first()->setPos(newPos);
}

void MainWindow::on_areaMoveRight_btn_clicked()
{
    double tlxd = static_cast<double>(ui->tlx_sb->value() + 1);
    double tlyd = static_cast<double>(ui->tly_sb->value());
    QPointF newPos(tlxd, tlyd);
    ui->view_gv->items().first()->setPos(newPos);
}

void MainWindow::on_areaMoveLeft_btn_clicked()
{
    double tlxd = static_cast<double>(ui->tlx_sb->value() - 1);
    double tlyd = static_cast<double>(ui->tly_sb->value());
    QPointF newPos(tlxd, tlyd);
    ui->view_gv->items().first()->setPos(newPos);
}

void MainWindow::on_moveAreaBottom_btn_clicked()
{
    double tlxd = static_cast<double>(ui->tlx_sb->value());
    double tlyd = static_cast<double>(ui->tly_sb->value() + -1);
    QPointF newPos(tlxd, tlyd);
    ui->view_gv->items().first()->setPos(newPos);
}

void MainWindow::on_actionSaveToFile_triggered()
{
    QString downLoads = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Сохранение график", downLoads, "*.png");
    _plot->savePng(fileName, 1920, 1080);
}
