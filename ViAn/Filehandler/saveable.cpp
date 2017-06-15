//#include "saveable.h"

//const SaveableTree::SAVE_FORMAT SaveableTree::DEFAULT_SAVE_FORMAT;
///**
// * @brief Saveable::~Saveable
// * virtual constructor
// */
//SaveableTree::SaveableTree()
//{

//}

///**
// * @brief Saveable::~Saveable
// * virtual destructor
// */
//SaveableTree::~SaveableTree()
//{

//}

///**
// * @brief SaveableTree::delete_saveable
// * @return
// */
//bool SaveableTree::delete_tree()
//{
//    for(auto it = m_map_saveables.begin(); it != m_map_saveables.end(); it++){
//        std::vector<SaveableNode> nodes = it->second;
//        foreach (SaveableNode sav, nodes) {
//            sav.delete_node();
//        }
//        std::string dir_path = it->first;
//        dir.rmdir(QString::fromStdString(dir));
//    }
//}

//bool SaveableTree::save_tree()
//{
//    save_nodes();
//    for(auto it = m_map_saveables.begin(); it != m_map_saveables.end(); it++){
//        std::vector<SaveableNode> nodes = it->second;
//        foreach (SaveableNode sav, nodes) {
//            sav.save_();
//        }
//    }
//}

//bool SaveableTree::load_tree(const std::string &full_path)
//{
//    QFile load_file(save_format == JSON
//        ? QString::fromStdString(full_path)
//        : QString::fromStdString(full_path));
//    if (!load_file.open(QIODevice::ReadOnly)) {
//        qWarning("Couldn't open load file %s. ", load_file.fileName().toStdString().c_str());
//        return false;
//    }
//    QByteArray save_data = load_file.readAll();
//    QJsonDocument load_doc(save_format == JSON
//        ? QJsonDocument::fromJson(save_data)
//        : QJsonDocument::fromBinaryData(save_data));
//    this->read(load_doc.object());
//    this->save_name = load_file.fileName().toStdString();

//    load_nodes();
//    return true;
//}

//bool SaveableTree::load_nodes()
//{
//    for(auto it = m_map_saveables.begin(); it != m_map_saveables.end(); it++){
//        std::vector<SaveableNode> nodes = it->second;
//        foreach (SaveableNode sav, nodes) {
//            sav.load_node();
//        }
//    }
//}

