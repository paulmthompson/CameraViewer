#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <filesystem>
#include <memory>
#include <numeric>
#include <ostream>
#include <stdlib.h>
#include <vector>

#include <QDir>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>

MainWindow::MainWindow(QWidget * parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow) {
    ui->setupUi(this);

    _addCallbacks();

    _on_button_scene = new QGraphicsScene(this);

    QPainterPath on_button_path;
    on_button_path.addEllipse(0.0, 0.0, 25.0, 25.0);
    on_button_path.setFillRule(Qt::WindingFill);

    _on_button_scene->addPath(on_button_path, QPen(Qt::red), QBrush(Qt::red));
    ui->graphicsView_2->setScene(_on_button_scene);
    ui->graphicsView_2->show();


    _camera_view_scene = new QGraphicsScene(this);

    QImage myimage = QImage(ui->graphicsView->width(), ui->graphicsView->height(), QImage::Format_Grayscale8);
    myimage.fill(Qt::black);
    _camera_view_pixmap = _camera_view_scene->addPixmap(QPixmap::fromImage(myimage));
    ui->graphicsView->setScene(_camera_view_scene);
    ui->graphicsView->show();

    _camManager = std::make_unique<CameraManager>();
    //cams = std::vector<std::unique_ptr<Camera>>(0);

    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &MainWindow::_acquisitionLoop);
    _recordMode = false;

    _viewActive = true;
    _softwareTrigger = false;

    _img_to_display = std::vector<uint8_t>(480 * 640);
    _cam_to_display = 0;//Camera to display in the viewer

    _loop_time = 40;

    _elapsed_times = std::vector<int>(_loop_time);
    _elapsed_times_i = 0;

    _timer->start(_loop_time);

    _updateSaveName(QDir::currentPath().toStdString() + "/" + "test.mp4");

    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setColumnWidth(0, 20);
    ui->tableWidget->setColumnWidth(1, 100);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::_addCallbacks() {
    connect(ui->add_virtual_button, &QPushButton::clicked, this, &MainWindow::_addVirtualCamera);
    connect(ui->connect_button, &QPushButton::clicked, this, &MainWindow::_connectCamera);
    connect(ui->start_cam_button, &QPushButton::clicked, this, &MainWindow::_triggerCamButton);
    connect(ui->tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(_selectCameraInTable(int, int)));
    connect(ui->record_button, &QPushButton::clicked, this, &MainWindow::_recordButton);
    connect(ui->view_button, &QPushButton::clicked, this, &MainWindow::_viewButton);
    connect(ui->change_save_button, &QPushButton::clicked, this, &MainWindow::_savePathButton);
    connect(ui->rescan_button, &QPushButton::clicked, this, &MainWindow::_scanForCameras);
    connect(ui->actionLoad_Configuration, &QAction::triggered, this, &MainWindow::_loadConfiguration);
}

void MainWindow::_drawConnected(Connected_Button_Color color) {


    _on_button_scene->clear();

    QPainterPath on_button_path;
    on_button_path.addEllipse(0.0, 0.0, 25.0, 25.0);
    on_button_path.setFillRule(Qt::WindingFill);

    switch (color) {
        case green:
            _on_button_scene->addPath(on_button_path, QPen(Qt::green), QBrush(Qt::green));
            break;
        case red:
            _on_button_scene->addPath(on_button_path, QPen(Qt::red), QBrush(Qt::red));
            break;
    }
}

void MainWindow::_connectCamera() {

    QItemSelectionModel * select = ui->tableWidget->selectionModel();
    if (select->hasSelection()) {
        int cam_num = select->selectedRows()[0].data().toInt();

        if (_camManager->connectCamera(cam_num)) {
            _drawConnected(green);
            ui->tableWidget->setItem(cam_num, 2, new QTableWidgetItem(QString::fromStdString("Yes")));

            //Our display frame needs to be able to receive the biggest frame possible.
            if (_camManager->getCanvasSize(cam_num) > _img_to_display.size()) {
                _img_to_display.resize(_camManager->getCanvasSize(cam_num));
            }
        } else {
            ui->tableWidget->setItem(cam_num, 2, new QTableWidgetItem(QString::fromStdString("No")));
        }
    }
}

void MainWindow::_updateModelandSerial(int cam_num) {

    ui->CameraModel->setText(QString::fromStdString(_camManager->getModel(cam_num)));
    ui->CameraSerial->setText(QString::fromStdString(_camManager->getSerial(cam_num)));
}

void MainWindow::_updateCameraTable() {

    ui->tableWidget->setRowCount(0);
    int current_row = 0;
    for (int i = 0; i < _camManager->numberOfCameras(); i++) {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->setItem(current_row, 0, new QTableWidgetItem(QString::number(current_row)));
        ui->tableWidget->setItem(current_row, 1, new QTableWidgetItem(QString::fromStdString(_camManager->getSerial(current_row))));
        if (_camManager->getAttached(current_row)) {
            ui->tableWidget->setItem(current_row, 2, new QTableWidgetItem(QString::fromStdString("Yes")));
        } else {
            ui->tableWidget->setItem(current_row, 2, new QTableWidgetItem(QString::fromStdString("No")));
        }
        current_row++;
    }

    //ui->tableWidget->selectRow(0);
}

void MainWindow::_viewButton() {
    if (_viewActive) {
        _viewActive = false;
    } else {
        _viewActive = true;
    }
}

//Our cameras should first stop collecting frames
void MainWindow::_initiateStopSequence() {
}

void MainWindow::_recordButton() {

    if (_recordMode) {
        // Stopping recording
        _recordMode = false;
        _camManager->setRecord(false);//Number of acquisition loops before turning off saving and closing mp4 file. This helps if there are some lingering frames to acquire
        if (_softwareTrigger) {
            _turnOffTrigger();
        }
        ui->view_record_label->setText(QString::fromStdString("View Mode"));
        ui->graphicsView->setStyleSheet("#graphicsView{border: 5px solid black}");

        _saved_file_paths.push_back(_save_file_path);
    } else {

        if (_checkForOverwrite()) {

            _recordMode = true;
            _camManager->setRecord(true);

            ui->view_record_label->setText(QString::fromStdString("Record Mode"));
            ui->graphicsView->setStyleSheet("#graphicsView{border: 5px solid red}");
        }
    }
}

bool MainWindow::_checkForOverwrite() {
    // Check if current save path is in our list of saved files
    auto it = std::find(_saved_file_paths.begin(), _saved_file_paths.end(), _save_file_path);
    if (it != _saved_file_paths.end()) {
        // Found a match - show warning dialog
        QMessageBox overwriteBox;
        overwriteBox.setWindowTitle("Warning: File Exists");
        overwriteBox.setText("The file '" + QString::fromStdString(_save_file_path.filename().string()) +
                             "' has already been used in this session.\n\nDo you want to overwrite it?");
        overwriteBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        overwriteBox.setDefaultButton(QMessageBox::No);
        overwriteBox.setIcon(QMessageBox::Warning);

        int response = overwriteBox.exec();
        return (response == QMessageBox::Yes);
    }

    // No match found, no overwrite will occur
    return true;
}

// This is the software trigger method
void MainWindow::_triggerCamButton() {

    if (_camManager->areCamerasConnected()) {

        if (_softwareTrigger) {
            _turnOffTrigger();
        } else {
            _turnOnTrigger();
        }
    } else {
        std::cout << "No cameras are connected. Cannot acquire frames!" << std::endl;
    }
}

void MainWindow::_turnOnTrigger() {
    //Send software trigger signal to camera
    ui->start_cam_button->setText(QString("Stop Camera"));
    _softwareTrigger = true;
    _camManager->trigger(true);
}

void MainWindow::_turnOffTrigger() {
    //If they are running, we should stop them, but if we are in record mode, we should make sure to also stop the recording in such a way as to not lose frames
    ui->start_cam_button->setText(QString("Trigger Camera"));
    _softwareTrigger = false;
    _camManager->trigger(false);
}

void MainWindow::_acquisitionLoop() {

    QElapsedTimer timer2;
    timer2.start();

    int num_frames_acquired = _camManager->acquisitionLoop();

    _elapsed_times[_elapsed_times_i++] = timer2.elapsed();

    if (_elapsed_times_i >= _elapsed_times.size()) {
        _elapsed_times_i = 0;
        float mean_time = accumulate(_elapsed_times.begin(), _elapsed_times.end(), 0) / _elapsed_times.size();
        qDebug() << "The loop took" << mean_time << "on average with a loop time of " << _loop_time;
        if (_recordMode) {
            //ui->graphicsView->setStyleSheet("#graphicsView{border: 5px solid black}");
        }
    }
    if (num_frames_acquired > 0) {
        _camManager->getImage(_img_to_display, _cam_to_display);
        ui->frame_count_save_label->setText(
                QString::fromStdString(std::to_string(_camManager->getTotalFramesSaved(_cam_to_display))));
        ui->frame_count_label->setText(
                QString::fromStdString(std::to_string(_camManager->getTotalFrames(_cam_to_display))));
    }

    // Display
    if (_viewActive) {
        if (num_frames_acquired > 0) {

            QImage img = QImage(&_img_to_display[0],
                                _camManager->getCanvasWidth(_cam_to_display),
                                _camManager->getCanvasHeight(_cam_to_display),
                                QImage::Format_Grayscale8);
            _updateCanvas(img);
        }
    }
}

void MainWindow::_updateCanvas(QImage & img) {
    _camera_view_pixmap->setPixmap(
            QPixmap::fromImage(img).scaled(
                    ui->graphicsView->width(),
                    ui->graphicsView->height(),
                    Qt::IgnoreAspectRatio,
                    Qt::SmoothTransformation));
}

void MainWindow::_selectCameraInTable(int row, int column) {

    _updateModelandSerial(row);

    _cam_to_display = row;

    if (_camManager->getAttached(row)) {
        _drawConnected(green);
    } else {
        _drawConnected(red);
    }
}

void MainWindow::_savePathButton() {

    if (!_recordMode) {
        auto dial = QFileDialog(this, "Save File Name", QDir::currentPath(), "MP4 (*.mp4)");

        QFile file(":/cameraviewer.qss");
        file.open(QFile::ReadOnly);
        QString styleSheet{QLatin1String(file.readAll())};
        //dial.setStyleSheet(styleSheet);
        dial.setStyleSheet("QPushButton { background-color: red }");

        QString vid_name = dial.getSaveFileName();

        //auto dial = QFileDialog::getSaveFileName(
        //                   this,
        //                  "Save File Name",
        //                 QDir::currentPath(),
        //                "MP4 (*.mp4)");
        if (!vid_name.isEmpty()) {
            _updateSaveName(vid_name.toStdString());
        }
    } else {
        std::cout << "Can't change filename while you are already saving!" << std::endl;
    }
}

void MainWindow::_updateSaveName(std::filesystem::path path) {

    _save_file_path = path;

    _camManager->changeFileNames(_save_file_path);
    ui->save_name_label->setText(QString::fromStdString(_save_file_path.filename().string()));
    ui->save_folder_label->setText(QString::fromStdString(_save_file_path.parent_path().string()));
}

void MainWindow::_scanForCameras() {

    _camManager->scanForCameras();

    _updateCameraTable();
}

void MainWindow::_addVirtualCamera() {

    _camManager->addVirtualCamera();

    _updateCameraTable();
}

void MainWindow::_loadConfiguration() {
    auto fileName = QFileDialog::getOpenFileName(this,
                                                 tr("Open Configuration File"), QDir::currentPath(), tr("JSON Files (*.json)"));

    std::cout << fileName.toStdString() << std::endl;

    if (!fileName.isEmpty()) {
        _config_path = fileName.toStdString();

        _camManager->loadConfigurationFile(_config_path);

        _updateCameraTable();
    }
}
