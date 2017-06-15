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
#include "Filehandler/filehandler.h"
#include "videoproject.h"
#include "video.h"
#include "Filehandler/json_item.h"
#include "Project/Analysis/analysis.h"
#include "projectmanager.h"

typedef int ID;
class FileHandler;
class ProjectManager;
/**
 * @brief The Project class
 * incomplete class, will be added on
 * along with parser functionality
 */
class Project : SaveableNode{
    ProjectManager* project_manager;
    bool m_changes_made = true;
    std::map<ID,VideoProject*> videos;
    std::vector<Report*> reports;
public:
    std::string name;
    ID id;
    ID video_id;
    std::string dir = "";
    std::string dir_bookmarks = "";
    std::string dir_videos = "";

public:
    Project(ProjectManager* projet_manager);
    Project(ProjectManager* projet_manager, ID id, std::string name);
    ~Project();
    virtual int add_child(JsonItem* item);
//    void add_report(Report* report);
//    ID add_video(Video *vid);
//    ID add_video_project(VideoProject* vid_proj);
//    void add_report(std::string file_path);

    void delete_artifacts();
    void remove_video_project(ID id);
    // read and write operator for Projects
    void read(const QJsonObject& json);
    void write(QJsonObject& json);
    ID add_analysis(ID video_id, Analysis analysis);
    ID add_bookmark(ID video_id, Bookmark *bookmark);
    void add_analysis(Analysis an);
    bool is_saved();
    void save_project();
    void load_project(string file_path);
    std::map<ID, VideoProject *>& get_videos();
    VideoProject* get_video(ID id);
    bool proj_equals(Project& other);
// TODO

//    void add_drawing();      
};


#endif // PROJECT_H