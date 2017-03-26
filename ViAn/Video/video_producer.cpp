#include "video_producer.h"
#include <iostream>
video_producer::video_producer(video_player* player) {
    mPlayer = player;
}

void video_producer::run() {
    QObject::connect(this, SIGNAL(fetched_frame(QImage)), mPlayer, SLOT(enqueueFrame(QImage)));
    std::cout << "Starting producer thread" << std::endl;

    capture.open("seq_01.mp4");
    if (!capture.isOpened()) {
        std::cout << "Could not open file" << std::endl;
    }

    cv::Mat frame, RGBframe;
    QImage img;
    while(capture.read(frame)) {
        if (frame.channels() == 3) {
            cv::cvtColor(frame, RGBframe, CV_BGR2RGB);
            img = QImage((const unsigned char*)(frame.data), RGBframe.cols, RGBframe.rows,QImage::Format_RGB888);
            emit fetched_frame(img);

        }
    }
    std::cout << "Exiting capture loop" << std::endl;
}
