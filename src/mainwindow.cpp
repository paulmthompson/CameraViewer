#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QGraphicsPixmapItem>
#include <QPainterPath>
#include <QPainter>
#include <QGraphicsScene>

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

    //ui->Left_Scroll_area->verticalScrollBar
}
