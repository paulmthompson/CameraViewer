#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>
#include <memory>

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QTimer>

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
    void updateModelandSerial(int cam_num);

    QTimer* timer;

    void acquisitionLoop();
    void initiateStopSequence();

    // In recordMode, cameras will save frames that they acquire
    bool recordMode;
    int record_countdown;


    bool viewActive;
    bool softwareTrigger;
    void turnOnTrigger();
    void turnOffTrigger();

    void updateCanvas(QImage& img);
    std::vector<uint8_t> img_to_display;
    int cam_to_display;

    std::vector<int> elapsed_times;
    int loop_time;
    int elapsed_times_i;

    bool areCamerasConnected();

    std::string save_file_path;
    void changeFileNames();
    void changeFileNames(std::unique_ptr<Camera>& cam);

private slots:
    void addVirtualCamera();
    void connectCamera();
    void recordButton();
    void selectCameraInTable(int row, int column);
    void viewButton();
    void triggerCamButton();
    void savePathButton();
    void scanForCameras();

};



#endif // MAINWINDOW_H
