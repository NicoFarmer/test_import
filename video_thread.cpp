#include <qdebug.h>

#include "video_thread.h"

// ======================================================
// ======================================================
void VideoWorker::init(QString video_name)
{
    m_video_name = video_name;
    //!TODO : transformer le nom du port vidéo en index pour OpenCV
    capture= new cv::VideoCapture(0);
    if(capture->isOpened())
     {
         qDebug()<<"La caméra est opérationnelle.";
         //infos de debug
         qDebug() << endl <<"camera choisie :" << video_name;
         int FourCC=capture->get(CV_CAP_PROP_FOURCC);
         qDebug("FOURCC code: %c%c%c%c", FourCC, FourCC>>8, FourCC>>16, FourCC>>24);
         qDebug() << "Has to be converted to RGB :" << capture->get(CV_CAP_PROP_CONVERT_RGB); //((capture->get(CV_CAP_PROP_CONVERT_RGB)==1) ? "YES":"NO");
         qDebug() << "Set height to 240 :" << ((capture->set(CV_CAP_PROP_FRAME_HEIGHT,240)) ? "OK" : "NOK");
         qDebug() << "Set width to 320 :" << ((capture->set(CV_CAP_PROP_FRAME_WIDTH,320)) ? "OK" : "NOK") << endl;
         cv::namedWindow( "capture", cv::WINDOW_AUTOSIZE );// Create a window for display.
     }
     else
         qDebug() << endl << "Caméra inopérante :-(" << endl;
}

void VideoWorker::stopWork()
{
    m_stop_work_request = true;
    qDebug() << "VideoWorker::stopWork Stop requested !";
}


// ======================================================
// ======================================================
void VideoWorker::doWork(tVideoInput parameter) {
    m_stop_work_request = false;

    emit workStarted();
    // ----------------------------------------------
    while (!m_stop_work_request)
    {
        switch(parameter.video_process_algo)
        {
            case VIDEO_PROCESS_ALGO1 :
                _video_process_algo1(parameter);
            break;

            case VIDEO_PROCESS_DUMMY :
                _video_process_dummy(parameter);
            break;

            // ...

            default :
                // nothing to do
                QThread::msleep(50);
            break;
        }
        QThread::msleep(5);
    }
    // ----------------------------------------------
    emit workFinished();
}


// ========================================================
// _________________________________________________________________
void VideoWorker::_video_process_algo1(tVideoInput parameter)
{
    tVideoResult result;

    //capture d'une image du flux video
    capture->grab();

    //récupération de l'image
    bool captureOK=capture->retrieve(m_frame,0);

    //l'image a-t-elle bien été récupérée
    if (captureOK)
    {
        //clone de l'image pour la persistence des données
        m_frameCloned=m_frame.clone();

        //analyse de l'image

        cv::Mat inputImage=m_frameCloned.clone();

        std::vector<int> markerIds;
        std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
        cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
        cv::aruco::detectMarkers(inputImage, dictionary, markerCorners, markerIds);
        if (markerIds.size() > 0)
            cv::aruco::drawDetectedMarkers(m_frameCloned, markerCorners, markerIds);

        //on affiche l'image traitée
        cv::imshow("capture",m_frameCloned);
        QThread::msleep(5);
        inputImage.release();

        if (markerIds.size() != 0) {
            result.markers_detected = markerIds;
            emit resultReady(result);
        }

        //opencv ne profite pas du garbage collector de Qt
        m_frame.release();
        m_frameCloned.release();
    }
    else
    {
        //opencv ne profite pas du garbage collector de Qt
        m_frame.release();
        m_frameCloned.release();
    }
}


// _________________________________________________________________
void VideoWorker::_video_process_dummy(tVideoInput parameter)
{
    QString str;
    tVideoResult result;
    qDebug() << str.sprintf("Thread start on %s / with   %f   %f   %f", m_video_name.toStdString().c_str(), parameter.data1, parameter.data2, parameter.data3);;
    int i=0;
    const int total_count = 5;
    while(i++<total_count)
    {
        QThread::sleep(1);
        result.result1 = parameter.data1;
        result.result2 += parameter.data3 + 0.1 ;
        emit resultReady(result);
        qDebug() << "Thread is still running" << ((float)i/total_count)*100. << "%";
    }
}
