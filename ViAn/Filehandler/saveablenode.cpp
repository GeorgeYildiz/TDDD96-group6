#include "saveablenode.h"

SaveableNode::SaveableNode(const std::string& file_name, const std::string &dir_path)
{
    m_directory = dir_path;
    m_file_name = file_name;
}

SaveableNode::SaveableNode()
{

}
SaveableNode::~SaveableNode()
{

}

/**
 * @brief FileHandler::save_saveable
 * @param savable
 * @param dir_path
 * @param save_format
 * @return Saves a file to provided directory
 */
bool SaveableNode::save_node(const SAVE_FORMAT& save_format){
    QString dir_name = QString::fromStdString(m_directory);
    if (!dir.exists(dir_name))
        dir.mkpath(QString::fromStdString(m_directory));
    QFile save_file(save_format == JSON
                    ? QString::fromStdString(dir_path + "/" + m_file_name + ".json")
                    : QString::fromStdString(dir_path + "/" + m_file_name + ".dat"));

    if(!save_file.open(QIODevice::WriteOnly)){
        qWarning("Couldn't open save file.");
        return false;
    }
    QJsonObject json_saveable;
    for(JsonItem data : m_data) {
        m_data->write(json_saveable);
    }
    QJsonDocument save_doc(json_saveable);
    save_file.write(save_format == JSON
            ? save_doc.toJson()
            : save_doc.toBinaryData());
    this->save_name = save_file.fileName().toStdString();
    return true;
}

/**
 * @brief Saveable::load_savable
 * @param full savable path
 * @param save_format
 * @return loaded Savable
 * Loads saveable from binary or json file.
 */
bool SaveableNode::load_node(const std::string& full_path, const SAVE_FORMAT& save_format){
    QFile load_file(save_format == JSON
        ? QString::fromStdString(full_path)
        : QString::fromStdString(full_path));
    if (!load_file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open load file %s. ", load_file.fileName().toStdString().c_str());
        return false;
    }
    QByteArray save_data = load_file.readAll();
    QJsonDocument load_doc(save_format == JSON
        ? QJsonDocument::fromJson(save_data)
        : QJsonDocument::fromBinaryData(save_data));
    this->read(load_doc.object());
    this->save_name = load_file.fileName().toStdString();
    return true;
}

/**
 * @brief SaveableTree::delete_saveable
 * @return
 */
bool SaveableNode::delete_node()
{
    QFile file(QString::fromStdString(this->save_name));
    file.remove();
}

