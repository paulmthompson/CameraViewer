#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <vector>
#include <memory>

#include "virtual_camera.h"

#include <QGraphicsPixmapItem>
#include <QPainterPath>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

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

    QImage myimage = QImage(640,480,QImage::Format_Grayscale8);
    myimage.fill(Qt::black);
    camera_view_pixmap = camera_view_scene->addPixmap(QPixmap::fromImage(myimage));
    ui->graphicsView->setScene(camera_view_scene);
    ui->graphicsView->show();

    cams = std::vector<std::unique_ptr<Camera>>(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addCallbacks() {
    connect(ui->add_virtual_button,SIGNAL(clicked()),this,SLOT(addVirtualCamera()));
    connect(ui->connect_button,SIGNAL(clicked()),this,SLOT(connectCamera()));
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
    for (auto& cam : this->cams) {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    }

    ui->tableWidget->selectRow(0);
}
