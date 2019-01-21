#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupWidgets();
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
        qDebug() << imgNames.at(i) << " : " << img.width() << " x " << img.height();
    }
    _model->appendRow(imgRow);
    _model->appendRow(nameRow);
    setActiveImg(0);
}

void MainWindow::setupWidgets()
{
    _model = new QStandardItemModel;
    ui->tableView->setModel(_model);

    _scene = new ClickableGS;
    ui->vie_gv->setScene(_scene);
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
        _scene->clear();
        QPixmap pixmap(QPixmap::fromImage(_images.at(index)));
        QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(pixmap);
        _scene->addItem(pixmapItem);
    }
}

void MainWindow::scaleImage(double k)
{

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
