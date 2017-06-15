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
#include "Filehandler/jsonnode.h"
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
class Project : public SaveableNode{
    ProjectManager* m_project_manager;
    bool m_changes_made = true;
    std::vector<VideoProject*> m_videos;
    std::vector<Report*> m_reports;
protected:

public:
    std::string m_name;
    ID m_id;
    ID m_vid_count;
    std::string m_dir = "";
    std::string m_dir_bookmarks = "";
    std::string m_dir_videos = "";

public:
    Project(ProjectManager* m_project_manager);
    Project(ProjectManager* m_project_manager, ID id, std::string m_name);
    ~Project();

    virtual int add_child(JsonNode* item);
    virtual Project* clone() const;
    virtual std::string get_type_info();


    void add_report(Report* report);
    ID add_video(Video *vid);
    ID add_video_project(VideoProject* vid_proj);
    void add_report(std::string file_path);
    void delete_artifacts();
    void remove_video_project(ID v_id);
    // read and write operator for Projects
    void read(const QJsonObject& json);
    void write(QJsonObject& json);
    ID add_analysis(ID v_id, Analysis analysis);
    ID add_bookmark(ID v_id, Bookmark *bookmark);
    void add_analysis(Analysis an);
    bool is_saved();
    void save_project();
    void load_project(const string& file_path);
    std::vector<VideoProject *> &get_videos();
    VideoProject* get_video(ID v_id);
    bool proj_equals(Project& other);
// TODO

//    void add_drawing();      
};


#endif // PROJECT_H
