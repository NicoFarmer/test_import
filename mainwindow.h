#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCameraInfo>
#include "video_thread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QList<QCameraInfo> m_cameras;
    void refresh_camera_list();

    QThread m_video_worker_thread;
    VideoWorker *m_video_worker;
    void video_worker_init(QString video_source);
signals:
    void operate(tVideoInput param);
    void stopWork();

public slots:
    void videoHandleResults(tVideoResult result);
    void videoThreadStopped();
    void videoWorkStarted();
    void videoWorkFinished();

public slots :
    void startVideoWork();
    void stopVideoWork();

    void initVideoThread();
    void killVideoThread();
};

#endif // MAINWINDOW_H
