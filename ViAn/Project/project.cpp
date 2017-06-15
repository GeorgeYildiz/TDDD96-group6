#include "project.h"
/**
 * @brief Project::Project
 * @param file_handler
 * @param id
 * @param name
 */
Project::Project(ProjectManager *projet_manager, ID id, std::string name){
    this->project_manager = projet_manager;
    this->name = name;
    this->m_file_name = name;
    this->m_directory = "";
    this->id = id;
    this->video_id = 0;
    this->videos.clear();
    this->m_changes_made = true;
}
/**
 * @brief Project::Project
 */
Project::Project(ProjectManager *projet_manager){
    this->project_manager = projet_manager;
    this->name = "";
    this->m_file_name = "";
    this->videos.clear();
    this->video_id = 0;
    // Setting ids to default values, -1 indicating invalid value.
    this->id = -1;

}

/**
 * @brief Project::~Project
 * Clears contents of video map
 */
Project::~Project(){
    for (auto vid_it = this->videos.begin(); vid_it != this->videos.end(); ++vid_it) {
        delete vid_it->second;
    }
    for (auto rep_it = this->reports.begin(); rep_it != this->reports.end(); ++rep_it) {
        delete *rep_it;
    }
}

/**
 * @brief Project::delete_artifacts
 * Delete all projects files.
 */
void Project::delete_artifacts(){    
//    // Delete files in all videoprojects
//    for(auto it = videos.begin(); it != videos.end(); it++){
//        VideoProject* vp = it->second;
//        vp->delete_artifacts();
//    }
//    // Delete all reports.
//    for(auto it = reports.begin(); it != reports.end(); it++){
//        Report* temp = *it;
//        QFile file (QString::fromStdString(temp->get_file_path()));
//        file.remove();
//    }
//    // Delete top level project artifacts
    QDir directory;
    QString q_dir = QString::fromStdString(this->dir);
    QString q_dir_bookmarks = QString::fromStdString(this->dir_bookmarks);
    // Delete main project file
    delete_node();
    // Delete directories
    directory.rmdir(q_dir_bookmarks);
    directory.rmdir(q_dir);
}

/**
 * @brief Project::read
 * @param json
 * Read project parameters from json object.
 */
void Project::read(const QJsonObject& json){
    this->name = json["name"].toString().toStdString();
    this->dir = json["root_dir"].toString().toStdString();
    this->dir_bookmarks = json["bookmark_dir"].toString().toStdString();
    this->dir_videos = json["video_dir"].toString().toStdString();
    this->m_file_name = this->name;
}

/**
 * @brief Project::write
 * @param json
 * Write project parameters to json object.
 */
void Project::write(QJsonObject& json){
    json["name"] = QString::fromStdString(this->name);
    json["root_dir"] =  QString::fromStdString(this->dir);
    json["bookmark_dir"] = QString::fromStdString(this->dir_bookmarks);
    json["video_dir"] = QString::fromStdString(this->dir_videos);
}

/**
 * @brief add_child
 * @param item
 */
ID Project::add_child(JsonItem* item){
    m_changes_made = true;
    return SaveableNode::add_child(item);
}

///**
// * @brief Project::add_analysis
// * @param v_id id of video to add analysis to
// * @param analysis
// */
//ID Project::add_analysis(ID v_id, Analysis analysis){
//    this->m_changes_made = true;
//    return this->m_json_children->add_child(analysis);
//}

///**
// * @brief Project::add_bookmark
// * @param v_id the id of the video
// * @param bookmark
// * Add new bookmark to Videoproj corresponding to id.
// */
//ID Project::add_bookmark(ID v_id, Bookmark *bookmark){
//    VideoProject* v = this->videos.at(v_id);
//    this->m_changes_made = true;
//    return v->add_child(bookmark);
//}

/**
 * @brief Project::is_saved
 * @return true if saved
 */
bool Project::is_saved(){
    return !this->m_changes_made;
}

/**
 * @brief Project::save_project
 * @return sets saved =true
 */
void Project::save_project(){    
    QDir directory;
    directory.mkpath(QString::fromStdString(this->dir));
    directory.mkpath(QString::fromStdString(this->dir_bookmarks));
    save_node(this->dir, this->dir+ "/" + this->name);
    this->m_changes_made = false;
}

void Project::load_project(const std::string& file_path){
    load_node(file_path);
}

/**
 * @brief Project::get_videos
 * @return videos&
 */
std::map<ID, VideoProject* > &Project::get_videos(){
    return this->videos;
}

/**
 * @brief Project::get_video
 * @param id
 * @return Returns the video with the specified id.
 */
VideoProject* Project::get_video(ID id) {
    return this->videos[id];
}
