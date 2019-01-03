#include <qdebug.h>

#include "video_thread.h"


// ======================================================
// ======================================================

void VideoWorker::setVideoName(QString video_name)
{
    m_video_name = video_name;
    //!TODO : open video port
}

void VideoWorker::doWork(tVideoInput parameter) {
    tVideoResult result;
    /* actions ... */
    QString str;
    qDebug() << str.sprintf("Thread start on %s / with   %f   %f   %f", m_video_name.toStdString().c_str(), parameter.data1, parameter.data2, parameter.data3);;
    int i=0;
    const int total_count = 5;
    while(i++<total_count)
    {
        QThread::sleep(1);
        result.result1 = parameter.data1;
        result.result2 += parameter.data3 + 0.1 ;
        qDebug() << "Thread is still running" << ((float)i/total_count)*100. << "%";
    }
    emit resultReady(result);
}




