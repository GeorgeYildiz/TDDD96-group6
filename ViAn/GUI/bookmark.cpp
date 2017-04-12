#include "bookmark.h"

/**
 * @brief Bookmark::Bookmark
 * @param frame_nbr Frame number associated with the bookmark.
 * @param img Thumbnail image.
 * @param string Text description of the bookmark.
 * @param view Parent widget of the bookmark.
 */
Bookmark::Bookmark(std::string vid_name, int frame_nbr, QImage img, QString string, QListWidget* view) : QListWidgetItem(string, view) {
    video_name = vid_name;
    frame_number = frame_nbr;
    setData(Qt::DecorationRole, QPixmap::fromImage(img));
}

/**
 * @brief Bookmark::get_frame_number
 * @return Returns the frame number that the bookmark points to.
 */
int Bookmark::get_frame_number() {
    return frame_number;
}

/**
 * @brief Bookmark::get_video_name
 * @return Returns the name of the video that the bookmark is taken from
 */
std::string Bookmark::get_video_name() {
    return video_name;
}
