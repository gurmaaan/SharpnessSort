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
    int tvh = ui->tableView->height();
    int tvw = ui->tableView->width();
    int tvcnt = tvw / tvh;
    int iconw = tvw / tvcnt;
    int icons = std::min(tvh, iconw);
    ui->tableView->setIconSize(QSize(icons, icons));
    ui->tableView->verticalHeader()->setDefaultSectionSize(icons);
    ui->tableView->horizontalHeader()->setDefaultSectionSize(icons);

    QList<QStandardItem*> imgRow;
    for(int i = 0; i < imgNames.length(); i++)
    {
        QString imgPath = QString(TEST_DIR) + QDir::separator() + imgNames.at(i);
        QImage img(imgPath);
        _images.append(img);
        QStandardItem *imgItem = new QStandardItem;
        QIcon icon(QPixmap::fromImage(img));
        imgItem->setIcon(icon);
        imgRow << imgItem;
        ui->dir_progress->setValue(i+1);
    }
    _model->appendRow(imgRow);
}

void MainWindow::setupWidgets()
{
    _model = new QStandardItemModel;
    ui->tableView->setModel(_model);

    ui->tableView->horizontalHeader()->setVisible(false);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setShowGrid(false);
}
