#ifndef PROJECT_H
#define PROJECT_H
#include <map>
#include <string>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <sstream>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>

#include "filehandler.h"
#include "videoproject.h"
#include "video.h"
#include "saveable.h"
typedef int ID;
class FileHandler;

/**
 * @brief The Project struct
 * incomplete struct, will be added on
 * along with parser functionality
 */
struct Project : Saveable{
    std::string name;
    FileHandler* file_handler;
public:    
    Project(FileHandler* file_handler);
    Project(FileHandler* file_handler, ID id, std::string name);
    Project(FileHandler* file_handler, std::string dir_path);
    ~Project();
    ID add_video(Video *vid);
    ID add_video_project(VideoProject* vid_proj);
    void add_bookmark(ID id, Bookmark *bookmark);
    void delete_artifacts();
    void remove_video_project(ID id);
    // read and write operator for Projects
    void read(const QJsonObject& json);
    void write(QJsonObject& json);


// TODO
//    void add_analysis();
//    void add_drawing();      
public:
    ID id;
    ID v_id;
    std::map<ID,VideoProject*> videos;
    ID dir;
    ID dir_bookmarks;
    ID dir_videos;
    bool saved;
};


#endif // PROJECT_H
