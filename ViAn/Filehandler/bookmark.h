#ifndef BOOKMARK_H
#define BOOKMARK_H
#include <QJsonObject>
#include <QString>
#include <QImage>
#include <QImageWriter>
#include <QFile>
#include <QDir>
#include <string>
#include <iostream>
#include "saveable.h"
/**
 * @brief The Bookmark class
 * Bookmark class is used for storing bookmarks, i.e. user
 * marked points in a video and an associated frame.
 */
class Bookmark : Saveable{
public:
    Bookmark(QString dir_path, QString file_name, QString description, int frame_nbr, QImage frame);
    Bookmark();
    int get_frame_number();
    QImage get_frame();
    QString get_description();
    void read(const QJsonObject& json);
    void write(QJsonObject& json);
    void export_frame();
    void create_file_path();
    void remove_exported_image();
private:
    QImage frame;           // Frame of the bookmark
    int frame_number;       // Frame at which the bookmark was taken
    QString dir_path;       // Path to the directory for the bookmarks
    QString description;    // Description for the bookmark, given by user

    // Note that this variable can be altered when the bookmark is exported.
    QString file_name;      // File path to the frame image associated with the bookmark
};

#endif // BOOKMARK_H
