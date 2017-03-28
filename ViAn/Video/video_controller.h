#ifndef VIDEO_CONTROLLER_H
#define VIDEO_CONTROLLER_H

#include <QObject>

class VideoController : public QObject { Q_OBJECT
public:
    explicit VideoController(QObject *parent = 0);

signals:
    void play();
    void pause();
    void stop();

public slots:
};

#endif // VIDEO_CONTROLLER_H
