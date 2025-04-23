#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <filesystem>
#include <memory>
#include <vector>

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QMainWindow>
#include <QTimer>

#include "camera.hpp"
#include "cameramanager.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

enum Connected_Button_Color { red,
                              green };

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget * parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow * ui;
    void _addCallbacks();

    void _drawConnected(Connected_Button_Color color);
    QGraphicsScene * _on_button_scene;

    QGraphicsScene * _camera_view_scene;
    QGraphicsPixmapItem * _camera_view_pixmap;

    std::unique_ptr<CameraManager> _camManager;

    void _updateCameraTable();
    void _updateModelandSerial(int cam_num);

    QTimer * _timer;

    void _acquisitionLoop();
    void _initiateStopSequence();

    // In _recordMode, cameras will save frames that they acquire
    bool _recordMode;

    bool _viewActive;
    bool _softwareTrigger;
    void _turnOnTrigger();
    void _turnOffTrigger();

    void _updateCanvas(QImage & img);
    std::vector<uint8_t> _img_to_display;
    int _cam_to_display;

    std::vector<int> _elapsed_times;
    int _loop_time;
    int _elapsed_times_i;

    std::filesystem::path _save_file_path;
    std::filesystem::path _config_path;

    void _updateSaveName(std::filesystem::path path);

private slots:
    void _addVirtualCamera();
    void _connectCamera();
    void _recordButton();
    void _selectCameraInTable(int row, int column);
    void _viewButton();
    void _triggerCamButton();
    void _savePathButton();
    void _scanForCameras();
    void _loadConfiguration();
};


#endif// MAINWINDOW_H
