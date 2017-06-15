#include "project.h"
/**
 * @brief Project::Project
 * @param file_handler
 * @param id
 * @param name
 */
Project::Project(ProjectManager *project_manager, ID id, std::string name){
    this->m_project_manager = project_manager;
    this->m_name = name;
    this->m_file_name = name;
    this->m_directory = "";
    this->m_id = id;
    this->m_vid_count = 0;
    this->m_videos.clear();
    this->m_changes_made = true;
}
/**
 * @brief Project::Project
 */
Project::Project(ProjectManager *project_manager){
    this->m_project_manager = project_manager;
    this->m_name = "";
    this->m_file_name = "";
    this->m_videos.clear();
    this->m_vid_count = 0;
    // Setting ids to default values, -1 indicating invalid value.
    this->m_id = -1;

}

/**
 * @brief Project::~Project
 * Clears contents of video map
 */
Project::~Project(){
}

string Project::get_type_info()
{

}

ID Project::add_video(Video *vid)
{
    VideoProject* vid_proj = new VideoProject(vid);
    m_videos.push_back(vid_proj);
    add_child(vid_proj);
    return m_vid_count++;
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
    QString q_dir = QString::fromStdString(this->m_dir);
    QString q_dir_bookmarks = QString::fromStdString(this->m_dir_bookmarks);
    // Delete main project file
    delete_node();
    // Delete directories
    directory.rmdir(q_dir_bookmarks);
    directory.rmdir(q_dir);
}

void Project::remove_video_project(ID v_id)
{
    VideoProject* temp = this->m_videos.at(v_id);
    delete temp;
    m_videos.erase(m_videos.begin() + v_id);
    this->m_changes_made = true;
}

/**
 * @brief Project::read
 * @param json
 * Read project parameters from json object.
 */
void Project::read(const QJsonObject& json){
    this->m_name = json["name"].toString().toStdString();
    this->m_dir = json["root_dir"].toString().toStdString();
    this->m_dir_bookmarks = json["bookmark_dir"].toString().toStdString();
    this->m_dir_videos = json["video_dir"].toString().toStdString();
    this->m_file_name = this->m_name;
}

/**
 * @brief Project::write
 * @param json
 * Write project parameters to json object.
 */
void Project::write(QJsonObject& json){
    json["name"] = QString::fromStdString(this->m_name);
    json["root_dir"] =  QString::fromStdString(this->m_dir);
    json["bookmark_dir"] = QString::fromStdString(this->m_dir_bookmarks);
    json["video_dir"] = QString::fromStdString(this->m_dir_videos);
}

/**
 * @brief add_child
 * @param item
 */
ID Project::add_child(JsonNode* item){
    m_changes_made = true;
    return SaveableNode::add_child(item);
}

/**
 * @brief Project::add_analysis
 * @param v_id id of video to add analysis to
 * @param analysis
 */
ID Project::add_analysis(ID v_id, Analysis analysis){
    this->m_changes_made = true;
    return m_videos.at(v_id)->add_analysis(analysis);
}

/**
 * @brief Project::add_bookmark
 * @param v_id the id of the video
 * @param bookmark
 * Add new bookmark to Videoproj corresponding to id.
 */
ID Project::add_bookmark(ID v_id, Bookmark *bookmark){
    VideoProject* v = this->m_videos.at(v_id);
    this->m_changes_made = true;
    return v->add_bookmark(bookmark);
}

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
    directory.mkpath(QString::fromStdString(this->m_dir));
    directory.mkpath(QString::fromStdString(this->m_dir_bookmarks));
    save_node(this->m_name, this->m_dir);
    this->m_changes_made = false;
}

void Project::load_project(const std::string& file_path){
    load_node(file_path);
}

/**
 * @brief Project::get_videos
 * @return videos&
 */
std::vector<VideoProject*> &Project::get_videos(){
    return this->m_videos;
}

/**
 * @brief Project::get_video
 * @param id
 * @return Returns the video with the specified id.
 */
VideoProject* Project::get_video(ID v_id) {
    return this->m_videos[v_id];
}
