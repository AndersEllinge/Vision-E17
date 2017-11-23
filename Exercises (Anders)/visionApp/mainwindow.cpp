#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    imagePath = QFileDialog::getOpenFileName(this, "Select Image", "/home", "Image (*.png *.xpm *.jpg)");
    std::string path = imagePath.toStdString();
    inputImage = cv::imread(path,1);
    cvtColor(inputImage, inputImage, CV_BGR2RGB);
    ui->imageLabel->setPixmap(QPixmap::fromImage(QImage(inputImage.data,inputImage.cols,inputImage.rows,inputImage.step,QImage::Format_RGB888)));
}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
    ui->coordinateTextBox->setPlainText(QString::number(event->pos().x()));
}
