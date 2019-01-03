#include <qdebug.h>

#include "video_thread.h"


// ======================================================
// ======================================================

void VideoWorker::setVideoName(QString video_name)
{
    m_video_name = video_name;
    //!TODO : open video port
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

void VideoWorker::doWork(tVideoInput parameter) {
    tVideoResult result;
    /* actions ... */

// AJOUTE PAR LAGUICHE --------------------------
    cv::Mat frame; //image du buffer video
    cv::Mat frameCloned;

    //capture d'une image du flux video
    capture->grab();

    //récupération de l'image
    bool captureOK=capture->retrieve(frame,0);

    //l'image a-t-elle bien été récupérée
    if (captureOK)
    {
        //clone de l'image pour la persistence des données
        frameCloned=frame.clone();

        //analyse de l'image

        cv::Mat inputImage=frameCloned.clone();

        std::vector<int> markerIds;
        std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
        cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
        cv::aruco::detectMarkers(inputImage, dictionary, markerCorners, markerIds);
        if (markerIds.size() > 0)
        cv::aruco::drawDetectedMarkers(frameCloned, markerCorners, markerIds);

        //on affiche l'image traitée
        cv::imshow("capture",frameCloned);
        QThread::msleep(5);
        inputImage.release();

        //opencv ne profite pas du garbage collector de Qt
        frame.release();
        frameCloned.release();
    }
    else
    {
        //opencv ne profite pas du garbage collector de Qt
        frame.release();
        frameCloned.release();
    }
// ----------------------------------------------

   /* QString str;
    qDebug() << str.sprintf("Thread start on %s / with   %f   %f   %f", m_video_name.toStdString().c_str(), parameter.data1, parameter.data2, parameter.data3);;
    int i=0;
    const int total_count = 5;
    while(i++<total_count)
    {
        QThread::sleep(1);
        result.result1 = parameter.data1;
        result.result2 += parameter.data3 + 0.1 ;
        qDebug() << "Thread is still running" << ((float)i/total_count)*100. << "%";
    }*/
    emit resultReady(result);
}




