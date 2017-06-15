#include "saveablenode.h"

SaveableNode::SaveableNode(const std::string file_name, const std::string dir_path)
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

ID SaveableNode::add_child(JsonItem *item)
{
    m_json_children.push_back(item);
    return m_json_children.size();
}

void SaveableNode::remove_child(ID id)
{
    JsonItem* child = m_json_children.at(id);
    m_json_children.erase(m_json_children.begin()+id);
    delete child;
}

/**
 * @brief FileHandler::save_saveable
 * @param savable
 * @param dir_path
 * @param save_format
 * @return Saves a file to provided directory
 */
bool SaveableNode::save_node(const std::string& file_name, const std::string& dir_path, const SAVE_FORMAT& save_format){
    QDir dir(dir_path);
    if (!dir.exists(dir_path))
        dir.mkpath(QString::fromStdString(dir_path));
    QFile save_file(save_format == JSON
                    ? QString::fromStdString(dir_path + "/" + file_name + ".json")
                    : QString::fromStdString(dir_path + "/" + file_name + ".dat"));

    if(!save_file.open(QIODevice::WriteOnly)){
        qWarning("Couldn't open save file.");
        return false;
    }
    QJsonObject document_data;
    QJsonArray children;
    for(auto it = m_json_children.begin(); it != m_json_children.end(); it++){
        QJsonObject child;
        (*it)->write(child);
        //factory.set( (it*)->get_type_info(), (*it) );
        children.append(child);
    }
    document_data["children"] = children;
    QJsonDocument save_doc(document_data);
    save_file.write(save_format == JSON
            ? save_doc.toJson()
            : save_doc.toBinaryData());
    m_file_name = save_file.fileName().toStdString();
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
    QJsonObject json = load_doc.object();
    QJsonArray json_children = json["children"].toArray();
    for (int i = 0; i < json_children.size(); ++i) {
        QJsonObject json_child = json_children[i].toObject();
        std::string type = json_child["typeid"].toString().toStdString();
        TypeFactory f;
        JsonItem* json_item = f.get(type);
        // How do we know type?
        json_item->read(json_child);
        add_child(json_item);
    }
    // Set m_dir here
    m_file_name = load_file.fileName().toStdString();
    return true;
}

/**
 * @brief SaveableTree::delete_saveable
 * @return
 */
bool SaveableNode::delete_node()
{
    QFile file(QString::fromStdString(m_file_name));
    file.remove();
}

