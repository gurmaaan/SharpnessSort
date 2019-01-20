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
//    ui->tableView->setMinimumHeight(icons * 2);

    QList<QStandardItem*> imgRow;
    QList<QStandardItem*> nameRow;
    for(int i = 0; i < imgNames.length(); i++)
    {
        QString imgPath = QString(TEST_DIR) + QDir::separator() + imgNames.at(i);
        QImage img(imgPath);
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
    _model->appendRow(imgRow);
    _model->appendRow(nameRow);
}

void MainWindow::setupWidgets()
{
    _model = new QStandardItemModel;
    ui->tableView->setModel(_model);

    ui->tableView->horizontalHeader()->setVisible(false);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setShowGrid(false);
}

void MainWindow::on_action_exit_triggered()
{
    QGuiApplication::exit();
}
