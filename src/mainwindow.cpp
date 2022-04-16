#include "mainwindow.h"
#include "./ui_mainwindow.h"

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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::drawConnected() {

    on_button_scene->clear();

    QPainterPath on_button_path;
    on_button_path.addEllipse(0.0,0.0,25.0,25.0);
    on_button_path.setFillRule(Qt::WindingFill);

    on_button_scene->addPath(on_button_path,QPen(Qt::green),QBrush(Qt::green));

}
