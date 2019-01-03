#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_video_worker(NULL)
{
    ui->setupUi(this);
    refresh_camera_list();

    connect(ui->start_work, SIGNAL(released()), this, SLOT(startVideoWork()));
    connect(ui->stop_work, SIGNAL(released()), this, SLOT(stopVideoWork()));
    connect(ui->init_thread, SIGNAL(released()), this, SLOT(initVideoThread()));
    connect(ui->kill_thread, SIGNAL(released()), this, SLOT(killVideoThread()));
}

MainWindow::~MainWindow()
{
    killVideoThread();
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
    m_video_worker->init(video_source_name);
    m_video_worker->moveToThread(&m_video_worker_thread);
    connect(&m_video_worker_thread, &QThread::finished, m_video_worker, &QObject::deleteLater);
    connect(&m_video_worker_thread, &QThread::finished, this, &MainWindow::videoThreadStopped);
    connect(this, SIGNAL(operate(tVideoInput)), m_video_worker, SLOT(doWork(tVideoInput)));
    connect(m_video_worker, SIGNAL(resultReady(tVideoResult)), this, SLOT(videoHandleResults(tVideoResult)));
    connect(m_video_worker, SIGNAL(workStarted()), this, SLOT(videoWorkStarted()));
    connect(m_video_worker, SIGNAL(workFinished()), this, SLOT(videoWorkFinished()));
    m_video_worker_thread.start();
}

// ======================================================================
// Video Worker events
// ======================================================================
void MainWindow::videoHandleResults(tVideoResult result)
{
    qDebug() << "Video result available:";
    qDebug() << "   result1:" << result.result1;
    qDebug() << "   result2:" << result.result2;
    ui->out_data1->setValue(result.result1);
    ui->out_data2->setValue(result.result2);
}

void MainWindow::videoWorkStarted()
{
    ui->work_status->setValue(1);
    ui->start_work->setEnabled(false);
    ui->list_algo->setEnabled(false);
    ui->stop_work->setEnabled(true);

    qDebug() << "Video work is started";
}

void MainWindow::videoWorkFinished()
{
    ui->work_status->setValue(0);
    ui->start_work->setEnabled(true);
    ui->list_algo->setEnabled(true);
    ui->stop_work->setEnabled(false);

    qDebug() << "Video work is finished";
}


void MainWindow::videoThreadStopped()
{
    ui->work_status->setValue(0);
    qDebug() << "Thread is stopped";
}

// ======================================================================
void MainWindow::initVideoThread()
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

    if (m_video_worker != NULL) {
        bool state = true;
        ui->kill_thread->setEnabled(state);
        ui->start_work->setEnabled(state);
        ui->list_algo->setEnabled(state);
    }
}

void MainWindow::killVideoThread()
{
    if (m_video_worker == NULL) return;

    qDebug() << "MainWindow::killVideoThread Disconnect all";
    disconnect(&m_video_worker_thread, &QThread::finished, m_video_worker, &QObject::deleteLater);
    disconnect(this, SIGNAL(operate(tVideoInput)), m_video_worker, SLOT(doWork(tVideoInput)));
    disconnect(m_video_worker, SIGNAL(resultReady(tVideoResult)), this, SLOT(videoHandleResults(tVideoResult)));
    disconnect(m_video_worker, SIGNAL(workStarted()), this, SLOT(videoWorkStarted()));
    disconnect(m_video_worker, SIGNAL(workFinished()), this, SLOT(videoWorkFinished()));

    m_video_worker_thread.quit();
    m_video_worker_thread.wait();

    delete m_video_worker;
    m_video_worker = NULL;

    bool state = false;
    ui->kill_thread->setEnabled(state);
    ui->start_work->setEnabled(state);
    ui->stop_work->setEnabled(state);
    ui->list_algo->setEnabled(state);

}



void MainWindow::startVideoWork()
{
    tVideoInput param;
    param.video_process_algo = (tVideoProcessAlgoType)ui->list_algo->currentIndex();
    param.data1 = ui->in_data1->value();
    param.data2 = ui->in_data2->value();
    param.data3 = ui->in_data3->value();
    emit operate(param);
}

void MainWindow::stopVideoWork()
{
    qDebug() << "UI Stop work";
    if (m_video_worker) m_video_worker->stopWork();
}
