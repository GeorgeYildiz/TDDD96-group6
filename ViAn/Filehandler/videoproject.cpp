#include "videoproject.h"

/**
 * @brief VideoProject::VideoProject
 * @param v
 * Set video.
 */
VideoProject::VideoProject(Video* v){
    this->video = v;
}

/**
 * @brief VideoProject::VideoProject
 * Create empty video.
 */
VideoProject::VideoProject(){
    this->video = new Video();
}

/**
 * @brief VideoProject::get_video
 * @return video
 * Return associated video.
 */
Video* VideoProject::get_video(){
    return this->video;
}

/**
 * @brief VideoProject::get_bookmarks
 * @return bookmarks
 * Return all bookmarks.
 */
std::vector<Bookmark *> VideoProject::get_bookmarks(){
    return this->bookmarks;
}

/**
 * @brief Video::read
 * @param json
 * Read videoproject parameters from json object.
 */
void VideoProject::read(const QJsonObject& json){
    this->video->read(json);
    QJsonArray json_bookmarks = json["bookmarks"].toArray();
    for(int i = 0; i != json_bookmarks.size(); i++){
        Bookmark* new_bookmark = new Bookmark();
        QJsonObject json_bookmark = json_bookmarks[i].toObject();
        new_bookmark->read(json_bookmark);
        this->bookmarks.push_back(new_bookmark);
    }
}

/**
 * @brief Video::write
 * @param json
 * Write videoproject parameters to json object.
 */
void VideoProject::write(QJsonObject& json){
    this->video->write(json);
    QJsonArray json_bookmarks;
    // Needs to delete bookmarks before exporting the new ones.
    delete_artifacts();
    for(auto it = bookmarks.begin(); it != bookmarks.end(); it++){
        QJsonObject json_bookmark;
        Bookmark* temp = *it;
        temp->write(json_bookmark);
        json_bookmarks.append(json_bookmark);
    }
    json["bookmarks"] = json_bookmarks;
    QJsonArray json_analyses;
    for(auto it2 = analyses.begin(); it2 != analyses.end(); it2++){
        QJsonObject json_analysis;
        Analysis an = it2->second;
        an.write(json_analysis);
        json_analyses.append(json_analysis);
    }

}

/**
 * @brief VideoProject::add_bookmark
 * @param bookmark
 * Add new bookmark.
 */
void VideoProject::add_bookmark(Bookmark *bookmark){
    this->bookmarks.push_back(bookmark);
}
/**
 * @brief VideoProject::add_analysis
 * @param analysis
 * @return ID for analysis
 * Adds analysis to video project.
 */
ID VideoProject::add_analysis(Analysis analysis){
    this->analyses.insert(std::make_pair(this->vid_id, analysis));
    return this->vid_id++;
}

/**
 * @brief VideoProject::delete_artifacts
 * Delete bookmark files.
 */
void VideoProject::delete_artifacts(){
    for(auto it = bookmarks.begin(); it != bookmarks.end(); it++){
        Bookmark* temp = *it;
        temp->remove_exported_image();
    }
}

