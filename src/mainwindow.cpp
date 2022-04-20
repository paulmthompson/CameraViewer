#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <vector>
#include <memory>
#include <stdlib.h>
#include <numeric>

#include "virtual_camera.h"
#include "basler_camera.h"

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

    cams = std::vector<std::unique_ptr<Camera>>(0);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::acquisitionLoop);
    recordMode = false;
    record_countdown = 0;

    viewActive = true;
    softwareTrigger = false;

    img_to_display = std::vector<uint8_t>(480*640);
    cam_to_display = 0; //Camera to display in the viewer

    loop_time = 40;

    elapsed_times = std::vector<int>(loop_time);
    elapsed_times_i = 0;

    timer->start(this->loop_time);

    save_file_path = QDir::currentPath().toStdString() + "/" + "test.mp4";
    ui->save_path_label->setText(QString::fromStdString(save_file_path));
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
        changeFileNames(cams[cam_num]);

        cams[cam_num]->connectCamera();

        if (cams[cam_num]->getAttached()) {
            ui->tableWidget->setItem(cam_num,2,new QTableWidgetItem(QString::fromStdString("Yes")));
            cams[cam_num]->startAcquisition();

            //Our display frame needs to be able to receive the biggest frame possible.
            if (cams[cam_num]->getWidth() * cams[cam_num]->getHeight() > this->img_to_display.size()) {
                this->img_to_display.resize(cams[cam_num]->getWidth() * cams[cam_num]->getHeight());
            }

        } else {
            std::cout << "Camera could not be connected" << std::endl;
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

    cams[cams.size()-1]->assignID(cams.size()-1);

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
        this->record_countdown = 5; //Number of acquisition loops before turning off saving and closing mp4 file. This helps if there are some lingering frames to acquire
        if (this->softwareTrigger) {
            turnOffTrigger();
        }
        ui->view_record_label->setText(QString::fromStdString("View Mode"));
        ui->graphicsView->setStyleSheet("#graphicsView{border: 5px solid black}");
    } else {
        this->recordMode = true;
        for (auto& cam : this->cams) {
            if (cam->getAttached()) {
                cam->setRecord(true);
            }
        }
        ui->view_record_label->setText(QString::fromStdString("Record Mode"));
        ui->graphicsView->setStyleSheet("#graphicsView{border: 5px solid red}");
    }
}


// This is the software trigger method
void MainWindow::triggerCamButton() {

    if (this->areCamerasConnected()) {

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
   for (auto& cam : this->cams) {
       if (cam->getAttached() && cam->getAquisitionState()) {
           cam->startTrigger();
       }
   }
}

void MainWindow::turnOffTrigger() {
    //If they are running, we should stop them, but if we are in record mode, we should make sure to also stop the recording in such a way as to not lose frames
    ui->start_cam_button->setText(QString("Trigger Camera"));
    this->softwareTrigger = false;
    for (auto& cam : this->cams) {
        if (cam->getAttached() && cam->getAquisitionState()) {
            cam->stopTrigger();
        }
    }
}

void MainWindow::acquisitionLoop() {

    if (this->areCamerasConnected()) {

        QElapsedTimer timer2;
        timer2.start();

        int num_frames_acquired = 0;

        //Cameras in the "active" state will return frames if they have them.
        for (auto& cam : this->cams) {
            if (cam->getAttached() && cam->getAquisitionState()) {
                num_frames_acquired += cam->get_data(this->img_to_display);
            }
        }

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

        ui->frame_count_save_label->setText(QString::fromStdString(std::to_string(cams[this->cam_to_display]->getTotalFramesSaved())));
        ui->frame_count_label->setText(QString::fromStdString(std::to_string(cams[this->cam_to_display]->getTotalFrames())));


        // Display
        if (this->viewActive) {
            if (num_frames_acquired > 0) {
                QImage img = QImage(&this->img_to_display[0],this->cams[this->cam_to_display]->getWidth(), this->cams[this->cam_to_display]->getHeight(), QImage::Format_Grayscale8);
                updateCanvas(img);
            }
        }

        // If the cameras are no longer triggered and we were saving, or we were told to stop saving (but still have a trigger), we should close the file
        if (record_countdown > 0) {
            if (record_countdown == 1) {
                for (auto& cam : this->cams) {
                    if (cam->getAttached()) {
                        cam->setRecord(false);
                    }
                }
            }
            record_countdown--;
        }

    }
}

void MainWindow::updateCanvas(QImage& img)
{
    camera_view_pixmap->setPixmap(QPixmap::fromImage(img).scaled(ui->graphicsView->width(),ui->graphicsView->height()));
}

bool MainWindow::areCamerasConnected() {

    bool output = false;
    for (auto& cam : this->cams) {
        output |= cam->getAttached();
    }

    return output;
}

void MainWindow::selectCameraInTable(int row, int column) {

    updateModelandSerial(row);

    this->cam_to_display = row;

    if (cams[row]->getAttached()) {
         drawConnected(green);
    } else {
        drawConnected(red);
    }
}

void MainWindow::savePathButton() {

    if (! this->recordMode) {
        QString vid_name =  QFileDialog::getSaveFileName(
                    this,
                    "Save File Name",
                    QDir::currentPath(),
                    "MP4 (*.mp4)");
        if (!vid_name.isEmpty()) {

            this->save_file_path = vid_name.toStdString();

            changeFileNames();

            ui->save_path_label->setText(vid_name);
        }
    } else {
        std::cout << "Can't change filename while you are already saving!" << std::endl;
    }
}

void MainWindow::changeFileNames() {

    std::filesystem::path p = this->save_file_path;
    std::string dir_path = p.parent_path().string() +"/";
    auto name_without_suffix = p.filename().replace_extension().string();

    for (auto& cam : this->cams) {

        cam->setSave(dir_path, name_without_suffix + std::to_string(cam->getID()) + ".mp4");
        cam->initializeVideoEncoder();
    }
}

void MainWindow::changeFileNames(std::unique_ptr<Camera>& cam) {

    std::filesystem::path p = this->save_file_path;
    std::string dir_path = p.parent_path().string() +"/";
    auto name_without_suffix = p.filename().replace_extension().string();

    cam->setSave(dir_path, name_without_suffix + std::to_string(cam->getID()) + ".mp4");
    cam->initializeVideoEncoder();
}

void MainWindow::scanForCameras() {

    auto b = BaslerCamera();

    auto connected_camera_strings = b.scan();

    for (auto& serial_num : connected_camera_strings) { //This should return a pair of the model name and serial number I think so that both can be put in the table
        cams.push_back(std::unique_ptr<Camera>(b.copy_class()));
        cams[cams.size()-1]->assignID(cams.size()-1);
        cams[cams.size()-1]->assignSerial(serial_num);
        updateCameraTable();
    }
}



