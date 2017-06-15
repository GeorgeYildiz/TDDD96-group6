#include "saveablenode.h"

SaveableNode::SaveableNode()
{

}
SaveableNode::~SaveableNode()
{

}
TypeFactory::TypeFactory()
{

}

JsonNode *TypeFactory::get(const std::string &id) const
{
    return m_type_map.at(id);
}

void TypeFactory::set(const std::string &id, JsonNode *item)
{
    m_type_map.insert(std::make_pair(id,item));
}

/**
 * @brief FileHandler::save_saveable
 * @param savable
 * @param dir_path
 * @param save_format
 * @return Saves a file to provided directory
 */
bool SaveableNode::save_node(const std::string& file_name, const std::string& dir_path, const SAVE_FORMAT& save_format){
    QString q_dir_path = QString::fromStdString(dir_path);
    QDir dir(q_dir_path);
    if (!dir.exists(q_dir_path))
        dir.mkpath(QString::fromStdString(dir_path));
    std::cout << std::endl << dir_path + "/" + file_name + ".json" << std::endl;
    QFile save_file(save_format == JSON
                    ? QString::fromStdString(dir_path + "/" + file_name + ".json")
                    : QString::fromStdString(dir_path + "/" + file_name + ".dat"));
    if(!save_file.open(QIODevice::WriteOnly)){
        qWarning("Couldn't open save file.");
        return false;
    }
    QJsonObject document_data;
    this->write(document_data);
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
    this->read(json);
    QJsonArray json_children = json["children"].toArray();
    for (int i = 0; i < json_children.size(); ++i) {
        QJsonObject json_node = json_children[i].toObject();
        std::string type = json_node["typeid"].toString().toStdString();
        TypeFactory f;
        JsonNode* json_item = f.get(type);
        // How do we know type?
        json_item->read(json_node);
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
