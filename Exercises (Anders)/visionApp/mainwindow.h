#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <QLabel>
#include <QDebug>
#include <QMouseEvent>
#include <QTextEdit>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

protected:
    void mouseMoveEvent(QMouseEvent *event);

private:
    Ui::MainWindow *ui;

    QString imagePath;
    cv::Mat inputImage;
};

#endif // MAINWINDOW_H
