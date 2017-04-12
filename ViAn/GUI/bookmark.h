#ifndef BOOKMARK_H
#define BOOKMARK_H

#include <QListWidgetItem>

class Bookmark : public QListWidgetItem {
public:
    Bookmark(std::string vid_name, int frame_nbr, QImage img, QString string, QListWidget* view);
    int get_frame_number();
    std::string get_video_name();
private:
    std::string video_name;
    int frame_number;
};

#endif // BOOKMARK_H
