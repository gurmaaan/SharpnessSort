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
    //TODO : норм размер иконок
    //ui->tableView->verticalHeader()->setDefaultSectionSize(100);
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

        QIcon icon(QPixmap::fromImage(img));
        imgItem->setIcon(icon);

        imgRow << imgItem;
        nameRow << nameItem;

        ui->dir_progress->setValue(i+1);
    }
    ui->baseImg_cb->addItems(imgNames);
    _model->appendRow(imgRow);
    _model->appendRow(nameRow);
    setActiveImg(0);
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
        _activeIndex = index;
        ui->tableView->selectColumn(index);
        _viewScene->clear();
        QImage activeImg = _images.at(index);
        QPixmap pixmap( QPixmap::fromImage(activeImg) );
        QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(pixmap);
        _viewScene->addItem(pixmapItem);
        //diffImages(_baseImage, activeImg);
    }
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
                int r = 0, g = 0, b = 0;
                r = (( base.pixelColor(i,j).red() - current.pixelColor(i,j).red() ) * k) + 128;
                g = (( base.pixelColor(i,j).green() - current.pixelColor(i,j).green() ) * k) + 128;
                b = (( base.pixelColor(i,j).blue() - current.pixelColor(i,j).blue() ) * k) + 128;
            }
        }

        setImgDiff(resImg);
    }
}

void MainWindow::on_action_exit_triggered()
{
    QGuiApplication::exit();
}

void MainWindow::on_action_next_triggered()
{
    setActiveImg(_activeIndex + 1);
}

void MainWindow::on_action_previous_triggered()
{
    setActiveImg(_activeIndex - 1);
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
    QImage baseImg = _images.at(index);
    setBaseImage(baseImg);
    QPixmap pixMap = QPixmap::fromImage(baseImg);
    ui->baseImagPrevie_lbl->setPixmap(pixMap.scaled(250, 150));
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
