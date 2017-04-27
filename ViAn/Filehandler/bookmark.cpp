#include "bookmark.h"

/**
 * @brief Bookmark::Bookmark
 * @param frame_nbr Frame number associated with the bookmark.
 * @param frame Frame associated with the bookmark.
 * @param dir_path Path to the directory to store image in.
 * @param text Text description of the bookmark.
 */
Bookmark::Bookmark(QString dir_path, QString file_name, QString description, int frame_nbr, QImage frame) {
    this->frame_number = frame_nbr;
    this->frame = frame;
    this->file_name = file_name;
    this->description = description;

    // There's no file path yet, since the frame has not been exported
    this->dir_path = dir_path;
}

/**
 * @brief Bookmark::Bookmark
 * Null initializing constructor.
 */
Bookmark::Bookmark() {
    frame_number = 0;
    frame = QImage();
    dir_path = QString();
    file_name = QString();
    description = QString();
}

/**
 * @brief Bookmark::get_frame_number
 * @return Returns the frame number that the bookmark points to.
 */
int Bookmark::get_frame_number() {
    return frame_number;
}

/**
 * @brief Bookmark::get_frame
 * @return Returns the frame of the bookmark.
 */
QImage Bookmark::get_frame() {
    return frame;
}

/**
 * @brief Bookmark::get_description
 * @return Returns the description associated with the bookmark.
 */
QString Bookmark::get_description() {
    return description;
}

/**
 * @brief Bookmark::read
 * @param json
 * Reads a bookmark from a Json object.
 */
void Bookmark::read(const QJsonObject& json){
    this->frame_number = json["frame"].toInt();
    this->dir_path = json["dir"].toString();
    this->file_name = json["path"].toString();
    this->description = json["note"].toString();    
    frame.load(file_name);
}

/**
 * @brief Bookmark::write
 * @param json
 * Writes a bookmark to a Json object, and exports the frame.
 */
void Bookmark::write(QJsonObject& json){
    // Exports the frame and updates file_path.
    export_frame();

    json["frame"] = this->frame_number;
    json["dir"] = this->dir_path;
    json["path"] = this->file_name;
    json["note"] = this->description;
}

/**
 * @brief Bookmark::export_frame
 * Export the frame of the bookmark to a tiff-file in the project folder.
 */
void Bookmark::export_frame() {
    // Update file path in case there's already a file with this file name
    std::cout << dir_path.toStdString() << std::endl;
    std::cout << QDir(dir_path).absoluteFilePath(file_name+ ".tiff").toStdString() << std::endl;
    QString bm_fname = QString::fromStdString(file_name.toStdString().substr(0,file_name.toStdString().find_last_of(".")) + "_" +  std::to_string(frame_number) +".tiff");
    QImageWriter writer(QDir(dir_path).absoluteFilePath(QString::fromStdString("aids.tiff")), "tiff");
    writer.write(frame);
}

/**
 * @brief Bookmark::create_file_path
 * Creates and updates the file path to export the bookmark frame to.
 */
void Bookmark::create_file_path() {

    // Append FRAMENR.tiff to the directory path
    QString path = QString(dir_path);
    path.append("/");
    path.append(QString::number(frame_number));
    path.append(".tiff");

    int counter = 1;
    while (QFile::exists(path)) {
        // If file exists, try FRAMENR(X).tiff
        path = QString(dir_path);
        path.append("/");
        path.append(QString::number(frame_number));
        path.append("(");
        path.append(QString::number(counter));
        path.append(").tiff");
        counter++;
    }

    // Update file path variable
    file_name = path;
}

/**
 * @brief Bookmark::remove_exported_image
 * Removes the exported image, if there is one.
 */
void Bookmark::remove_exported_image() {
    // If the file path is empty, then the frame has not been exported so there's nothing to remove.
    if (!file_name.isEmpty()) {
        QFile file(file_name);
        file.remove();
    }
}
