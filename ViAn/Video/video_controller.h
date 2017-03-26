#ifndef VIDEO_CONTROLLER_H
#define VIDEO_CONTROLLER_H

#include <QObject>

class video_controller : public QObject { Q_OBJECT
public:
    explicit video_controller(QObject *parent = 0);

signals:
    void play();
    void pause();
    void stop();

public slots:
};

#endif // VIDEO_CONTROLLER_H
