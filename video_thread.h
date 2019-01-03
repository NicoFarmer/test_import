#ifndef VIDEO_THREAD_H
#define VIDEO_THREAD_H

#include <QObject>
#include <QThread>
// AJOUTE PAR LAGUICHE --------------------------
#include <QDebug>
#include "opencv2/opencv.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
// ----------------------------------------------

typedef struct
{
    float data1;
    float data2;
    float data3;
}tVideoInput;

typedef struct
{
    float result1;
    float result2;
}tVideoResult;

Q_DECLARE_METATYPE(tVideoInput)
Q_DECLARE_METATYPE(tVideoResult)

// ======================================================
// ======================================================
class VideoWorker : public QObject
{
    Q_OBJECT
public :
    void setVideoName(QString video_name);

private :
    QString m_video_name;
// AJOUTE PAR LAGUICHE --------------------------
    cv::VideoCapture * capture;
// ----------------------------------------------

public slots:
    void doWork(tVideoInput parameter);

signals:
    void resultReady(tVideoResult result);
};



#endif // VIDEO_THREAD_H
