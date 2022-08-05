#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <vector>
#include <memory>
#include <stdlib.h>
#include <numeric>
#include <filesystem>
#include <ostream>

#include <QGraphicsPixmapItem>
#include <QPainterPath>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QElapsedTimer>
#include <QDir>
#include <QFileDialog>

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

    camManager = std::make_unique<CameraManager>();
    //cams = std::vector<std::unique_ptr<Camera>>(0);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::acquisitionLoop);
    recordMode = false;

    viewActive = true;
    softwareTrigger = false;

    img_to_display = std::vector<uint8_t>(480*640);
    cam_to_display = 0; //Camera to display in the viewer

    loop_time = 40;

    elapsed_times = std::vector<int>(loop_time);
    elapsed_times_i = 0;

    timer->start(this->loop_time);

    save_file_path = QDir::currentPath().toStdString() + "/" + "test.mp4";
    ui->save_path_label->setText(QString::fromStdString(save_file_path.string()));

    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setColumnWidth(0,20);
    ui->tableWidget->setColumnWidth(1,100);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addCallbacks() {
    connect(ui->add_virtual_button,SIGNAL(clicked()),this,SLOT(addVirtualCamera()));
    connect(ui->connect_button,SIGNAL(clicked()),this,SLOT(connectCamera()));
    connect(ui->start_cam_button,SIGNAL(clicked()),this,SLOT(triggerCamButton()));
    connect(ui->tableWidget,SIGNAL(cellClicked(int,int)),this,SLOT(selectCameraInTable(int,int)));
    connect(ui->record_button,SIGNAL(clicked()),this,SLOT(recordButton()));
    connect(ui->view_button,SIGNAL(clicked()),this,SLOT(viewButton()));
    connect(ui->change_save_button,SIGNAL(clicked()),this,SLOT(savePathButton()));
    connect(ui->rescan_button,SIGNAL(clicked()),this,SLOT(scanForCameras()));
    connect(ui->actionLoad_Configuration,SIGNAL(triggered()),this,SLOT(loadConfiguration()));
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

        if (camManager->connectCamera(cam_num)) {
            drawConnected(green);
            ui->tableWidget->setItem(cam_num,2,new QTableWidgetItem(QString::fromStdString("Yes")));

            //Our display frame needs to be able to receive the biggest frame possible.
            if (camManager->getCanvasSize(cam_num) > this->img_to_display.size()) {
                this->img_to_display.resize(camManager->getCanvasSize(cam_num));
            }
        } else {
            ui->tableWidget->setItem(cam_num,2,new QTableWidgetItem(QString::fromStdString("No")));
        }
    }
}

void MainWindow::updateModelandSerial(int cam_num) {

    ui->CameraModel->setText(QString::fromStdString(this->camManager->getModel(cam_num)));
    ui->CameraSerial->setText(QString::fromStdString(this->camManager->getSerial(cam_num)));
}

void MainWindow::updateCameraTable() {

    ui->tableWidget->setRowCount(0);
    int current_row = 0;
    for (int i = 0; i < camManager->numberOfCameras(); i++) {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->setItem(current_row,0,new QTableWidgetItem(QString::number(current_row)));
        ui->tableWidget->setItem(current_row,1,new QTableWidgetItem(QString::fromStdString(camManager->getSerial(current_row))));
        if (camManager->getAttached(current_row)) {
            ui->tableWidget->setItem(current_row,2,new QTableWidgetItem(QString::fromStdString("Yes")));
        } else {
            ui->tableWidget->setItem(current_row,2,new QTableWidgetItem(QString::fromStdString("No")));
        }
        current_row++;
    }

    //ui->tableWidget->selectRow(0);
}

void MainWindow::viewButton() {
    if (viewActive) {
        viewActive = false;
    } else {
        viewActive = true;
    }
}

//Our cameras should first stop collecting frames
void MainWindow::initiateStopSequence() {


}

void MainWindow::recordButton() {

    if (this->recordMode) {
        this->recordMode = false;
        camManager->setRecordCountdown();//Number of acquisition loops before turning off saving and closing mp4 file. This helps if there are some lingering frames to acquire
        if (this->softwareTrigger) {
            turnOffTrigger();
        }
        ui->view_record_label->setText(QString::fromStdString("View Mode"));
        ui->graphicsView->setStyleSheet("#graphicsView{border: 5px solid black}");
    } else {

        this->recordMode = true;
        this->camManager->setRecord(true);

        ui->view_record_label->setText(QString::fromStdString("Record Mode"));
        ui->graphicsView->setStyleSheet("#graphicsView{border: 5px solid red}");
    }
}


// This is the software trigger method
void MainWindow::triggerCamButton() {

    if (this->camManager->areCamerasConnected()) {

        if (this->softwareTrigger) {
            turnOffTrigger();
        } else {
            turnOnTrigger();
        }
    } else {
        std::cout << "No cameras are connected. Cannot acquire frames!" << std::endl;
    }
}

void MainWindow::turnOnTrigger() {
    //Send software trigger signal to camera
   ui->start_cam_button->setText(QString("Stop Camera"));
   this->softwareTrigger = true;
   this->camManager->startTrigger();
}

void MainWindow::turnOffTrigger() {
    //If they are running, we should stop them, but if we are in record mode, we should make sure to also stop the recording in such a way as to not lose frames
    ui->start_cam_button->setText(QString("Trigger Camera"));
    this->softwareTrigger = false;
    this->camManager->stopTrigger();
}

void MainWindow::acquisitionLoop() {

        QElapsedTimer timer2;
        timer2.start();

        int num_frames_acquired = camManager->acquisitionLoop();

        elapsed_times[elapsed_times_i++] = timer2.elapsed();

        if (elapsed_times_i >= elapsed_times.size())
        {
            elapsed_times_i = 0;
            float mean_time = accumulate(elapsed_times.begin(),elapsed_times.end(),0) / elapsed_times.size();
            qDebug() << "The loop took" << mean_time << "on average with a loop time of " << this->loop_time;
            if (this->recordMode) {
                //ui->graphicsView->setStyleSheet("#graphicsView{border: 5px solid black}");
            }
        }
        if (num_frames_acquired > 0 ) {
            camManager->getImage(this->img_to_display,this->cam_to_display);
            ui->frame_count_save_label->setText(
                        QString::fromStdString(std::to_string(camManager->getTotalFramesSaved(this->cam_to_display))));
            ui->frame_count_label->setText(
                        QString::fromStdString(std::to_string(camManager->getTotalFrames(this->cam_to_display))));
        }

        // Display
        if (this->viewActive) {
            if (num_frames_acquired > 0) {

                QImage img = QImage(&this->img_to_display[0],
                        camManager->getCanvasWidth(this->cam_to_display),
                        camManager->getCanvasHeight(this->cam_to_display),
                        QImage::Format_Grayscale8);
                updateCanvas(img);
            }
        }
}

void MainWindow::updateCanvas(QImage& img)
{
    camera_view_pixmap->setPixmap(QPixmap::fromImage(img).scaled(ui->graphicsView->width(),ui->graphicsView->height()));
}

void MainWindow::selectCameraInTable(int row, int column) {

    updateModelandSerial(row);

    this->cam_to_display = row;

    if (this->camManager->getAttached(row)) {
         drawConnected(green);
    } else {
        drawConnected(red);
    }
}

void MainWindow::savePathButton() {

    if (! this->recordMode) {
        auto dial = QFileDialog(this,"Save File Name", QDir::currentPath(),"MP4 (*.mp4)");

        QFile file(":/cameraviewer.qss");
        file.open(QFile::ReadOnly);
        QString styleSheet { QLatin1String(file.readAll()) };
        //dial.setStyleSheet(styleSheet);
        dial.setStyleSheet("QPushButton { background-color: red }");

        QString vid_name =  dial.getSaveFileName();

        //auto dial = QFileDialog::getSaveFileName(
         //                   this,
          //                  "Save File Name",
           //                 QDir::currentPath(),
            //                "MP4 (*.mp4)");
        if (!vid_name.isEmpty()) {

            this->save_file_path = vid_name.toStdString();

            camManager->changeFileNames(this->save_file_path);

            ui->save_path_label->setText(vid_name);
        }
    } else {
        std::cout << "Can't change filename while you are already saving!" << std::endl;
    }
}

void MainWindow::scanForCameras() {

    camManager->scanForCameras();

    updateCameraTable();

}

void MainWindow::addVirtualCamera() {

    camManager->addVirtualCamera();

    updateCameraTable();
}

void MainWindow::loadConfiguration() {
    auto fileName = QFileDialog::getOpenFileName(this,
        tr("Open Configuration File"), QDir::currentPath(), tr("JSON Files (*.json)"));

    std::cout << fileName.toStdString() << std::endl;

    this->config_path = fileName.toStdString();
}
