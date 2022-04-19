#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <vector>
#include <memory>
#include <stdlib.h>
#include <numeric>

#include "virtual_camera.h"

#include <QGraphicsPixmapItem>
#include <QPainterPath>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QElapsedTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    addCallbacks();

    on_button_scene = new QGraphicsScene(this);

    QPainterPath on_button_path;
    on_button_path.addEllipse(0.0,0.0,25.0,25.0);
    on_button_path.setFillRule(Qt::WindingFill);

    on_button_scene->addPath(on_button_path,QPen(Qt::red),QBrush(Qt::red));
    ui->graphicsView_2->setScene(on_button_scene);
    ui->graphicsView_2->show();


    camera_view_scene = new QGraphicsScene(this);

    QImage myimage = QImage(ui->graphicsView->width(),ui->graphicsView->height(),QImage::Format_Grayscale8);
    myimage.fill(Qt::black);
    camera_view_pixmap = camera_view_scene->addPixmap(QPixmap::fromImage(myimage));
    ui->graphicsView->setScene(camera_view_scene);
    ui->graphicsView->show();

    cams = std::vector<std::unique_ptr<Camera>>(0);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::acquisitionLoop);
    acquisitionActive = false;
    img_to_display = std::vector<uint8_t>(480*640);
    cam_to_display = 0;

    loop_time = 40;

    elapsed_times = std::vector<int>(loop_time);
    elapsed_times_i = 0;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addCallbacks() {
    connect(ui->add_virtual_button,SIGNAL(clicked()),this,SLOT(addVirtualCamera()));
    connect(ui->connect_button,SIGNAL(clicked()),this,SLOT(connectCamera()));
    connect(ui->play_button,SIGNAL(clicked()),this,SLOT(playButton()));
}

void MainWindow::drawConnected(Connected_Button_Color color) {


    this->on_button_scene->clear();

    QPainterPath on_button_path;
    on_button_path.addEllipse(0.0,0.0,25.0,25.0);
    on_button_path.setFillRule(Qt::WindingFill);

    switch (color) {
        case green:
            this->on_button_scene->addPath(on_button_path,QPen(Qt::green),QBrush(Qt::green));
            break;
        case red:
            this->on_button_scene->addPath(on_button_path,QPen(Qt::red),QBrush(Qt::red));
            break;
    }

}

void MainWindow::connectCamera() {

    QItemSelectionModel *select = ui->tableWidget->selectionModel();
    if (select->hasSelection()) {
        int cam_num =  select->selectedRows()[0].data().toInt();

        drawConnected(green);

        updateModelandSerial(cam_num);
        cams[cam_num]->initializeVideoEncoder();

        cams[cam_num]->connectCamera();

        if (cams[cam_num]->getAttached()) {
            ui->tableWidget->setItem(cam_num,2,new QTableWidgetItem(QString::fromStdString("Yes")));
        } else {
            ui->tableWidget->setItem(cam_num,2,new QTableWidgetItem(QString::fromStdString("No")));
        }
    }
}

void MainWindow::updateModelandSerial(int cam_num) {
    std::string mymodel = this->cams[cam_num].get()->getModel();
    std::string myserial = this->cams[cam_num].get()->getSerial();

    ui->CameraModel->setText(QString::fromStdString(mymodel));
    ui->CameraSerial->setText(QString::fromStdString(myserial));
}

void MainWindow::addVirtualCamera() {

    VirtualCamera v;
    cams.push_back(std::unique_ptr<Camera>(v.copy_class()));

    updateCameraTable();
}

void MainWindow::updateCameraTable() {

    ui->tableWidget->setRowCount(0);
    int current_row = 0;
    for (auto& cam : this->cams) {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->setItem(current_row,0,new QTableWidgetItem(QString::number(current_row)));
        ui->tableWidget->setItem(current_row,1,new QTableWidgetItem(QString::fromStdString(cam->getModel())));
        if (cam->getAttached()) {
            ui->tableWidget->setItem(current_row,2,new QTableWidgetItem(QString::fromStdString("Yes")));
        } else {
            ui->tableWidget->setItem(current_row,2,new QTableWidgetItem(QString::fromStdString("No")));
        }
        current_row++;
    }

    //ui->tableWidget->selectRow(0);
}

void MainWindow::playButton() {

    if (this->acquisitionActive) {

        for (auto& cam : this->cams) {
            cam->stopAcquisition();
        }

        //I should still be alerted if trailing frames come in right?

        timer->stop();
        ui->play_button->setText(QString("Play"));
        acquisitionActive = false;

    } else {

        for (auto& cam : this->cams) {
            cam->startAcquisition();
        }
        ui->play_button->setText(QString("Pause"));
        timer->start(this->loop_time);
        acquisitionActive = true;
    }
}

void MainWindow::acquisitionLoop() {

    QElapsedTimer timer2;
    timer2.start();

    for (auto& cam : this->cams) {
        cam->get_data(this->img_to_display);
    }

    elapsed_times[elapsed_times_i++] = timer2.elapsed();

    if (elapsed_times_i >= elapsed_times.size())
    {
        elapsed_times_i = 0;
        float mean_time = accumulate(elapsed_times.begin(),elapsed_times.end(),0) / elapsed_times.size();
        qDebug() << "The loop took" << mean_time << "on average with a loop time of " << this->loop_time;
    }

    // Display
    QImage img = QImage(&this->img_to_display[0],this->cams[this->cam_to_display]->getWidth(), this->cams[this->cam_to_display]->getHeight(), QImage::Format_Grayscale8);
    updateCanvas(img);
}

void MainWindow::updateCanvas(QImage& img)
{
    camera_view_pixmap->setPixmap(QPixmap::fromImage(img).scaled(ui->graphicsView->width(),ui->graphicsView->height()));
}
