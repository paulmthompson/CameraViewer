#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>
#include <memory>

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

#include "camera.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum Connected_Button_Color {red, green};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void addCallbacks();

    void drawConnected(Connected_Button_Color color);
    QGraphicsScene* on_button_scene;

    QGraphicsScene* camera_view_scene;
    QGraphicsPixmapItem* camera_view_pixmap;

    std::vector<std::unique_ptr<Camera>> cams;

    void updateCameraTable();

private slots:
    void addVirtualCamera();
    void connectCamera();

};



#endif // MAINWINDOW_H
