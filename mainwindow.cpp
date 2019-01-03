#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_video_worker(NULL)
{
    ui->setupUi(this);
    refresh_camera_list();

    connect(ui->start, SIGNAL(released()), this, SLOT(startVideoThread()));
    connect(ui->stop, SIGNAL(released()), this, SLOT(stopVideoThread()));
}

MainWindow::~MainWindow()
{
    stopVideoThread();
    delete ui;
}

// ______________________________________________________________________________
void MainWindow::refresh_camera_list()
{
    m_cameras = QCameraInfo::availableCameras();
    ui->video_devices_list->clear();
    int i=0;
    foreach (const QCameraInfo &cameraInfo, m_cameras) {
        ui->video_devices_list->insertItem(i++, cameraInfo.deviceName());
    }

    // insert a dummy camera (for test)
    ui->video_devices_list->insertItem(i++, "/dev/video_dummy1");
    ui->video_devices_list->insertItem(i++, "/dev/video_dummy2");
}

// ______________________________________________________________________________
void MainWindow::video_worker_init(QString video_source_name)
{
    qRegisterMetaType<tVideoInput>();
    qRegisterMetaType<tVideoResult>();

    m_video_worker = new VideoWorker;
    m_video_worker->setVideoName(video_source_name);
    m_video_worker->moveToThread(&m_video_worker_thread);
    connect(&m_video_worker_thread, &QThread::finished, m_video_worker, &QObject::deleteLater);
    connect(&m_video_worker_thread, &QThread::finished, this, &MainWindow::videoThreadStopped);
    connect(this, SIGNAL(operate(tVideoInput)), m_video_worker, SLOT(doWork(tVideoInput)));
    connect(m_video_worker, SIGNAL(resultReady(tVideoResult)), this, SLOT(videoHandleResults(tVideoResult)));
    m_video_worker_thread.start();
}

void MainWindow::videoHandleResults(tVideoResult result)
{
    ui->thread_status->setValue(0);
    qDebug() << "Thread finished. Result is :";
    qDebug() << "   result1:" << result.result1;
    qDebug() << "   result2:" << result.result2;
    ui->out_data1->setValue(result.result1);
    ui->out_data2->setValue(result.result2);
}

void MainWindow::videoThreadStopped()
{
    ui->thread_status->setValue(0);
    qDebug() << "Thread is stopped";
}

// ______________________________________________________________________________
void MainWindow::startVideoThread()
{
    if (m_video_worker == NULL) {
        // récupère le nom du port vidéo sélectionné
        QList<QListWidgetItem*> list_items = ui->video_devices_list->selectedItems();
        QString video_source_name = "";
        if (list_items.count() != 0) {
            video_source_name = list_items.at(0)->text();
        }
        qDebug() << video_source_name;

        video_worker_init(video_source_name);
    }

    tVideoInput param;
    param.data1 = ui->in_data1->value();
    param.data2 = ui->in_data2->value();
    param.data3 = ui->in_data3->value();
    emit operate(param);
    ui->thread_status->setValue(1);
}

void MainWindow::stopVideoThread()
{
    if (m_video_worker == NULL) return;

    disconnect(&m_video_worker_thread, &QThread::finished, m_video_worker, &QObject::deleteLater);
    disconnect(this, SIGNAL(operate(tVideoInput)), m_video_worker, SLOT(doWork(tVideoInput)));
    disconnect(m_video_worker, SIGNAL(resultReady(tVideoResult)), this, SLOT(videoHandleResults(tVideoResult)));

    m_video_worker_thread.quit();
    m_video_worker_thread.wait();

    delete m_video_worker;
    m_video_worker = NULL;
}
