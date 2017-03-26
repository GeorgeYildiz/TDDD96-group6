#ifndef VIDEO_PRODUCER_H
#define VIDEO_PRODUCER_H

#include <QThread>
#include "video_player.h"

class video_producer : public QThread { Q_OBJECT
public:
    video_producer(video_player* player);

signals:
    void fetched_frame(QImage frame);

public slots:

protected:
    void run() override;

private:
    video_player* mPlayer;
    cv::VideoCapture capture;
};

#endif // VIDEO_PRODUCER_H
