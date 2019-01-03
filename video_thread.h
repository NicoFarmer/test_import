#ifndef VIDEO_THREAD_H
#define VIDEO_THREAD_H

#include <QObject>
#include <QThread>

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

public slots:
    void doWork(tVideoInput parameter);

signals:
    void resultReady(tVideoResult result);
};



#endif // VIDEO_THREAD_H
