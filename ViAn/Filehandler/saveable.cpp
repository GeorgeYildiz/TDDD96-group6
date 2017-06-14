#include "saveable.h"

const SaveableTree::SAVE_FORMAT SaveableTree::DEFAULT_SAVE_FORMAT;
/**
 * @brief Saveable::~Saveable
 * virtual constructor
 */
SaveableTree::SaveableTree()
{

}

/**
 * @brief Saveable::~Saveable
 * virtual destructor
 */
SaveableTree::~SaveableTree()
{

}

/**
 * @brief SaveableTree::delete_saveable
 * @return
 */
bool SaveableTree::delete_tree()
{
    for(auto it = m_saveables.begin(); it != m_saveables.end(); it++){
        std::vector<SaveableNode> nodes = it->second;
        foreach (SaveableNode sav, nodes) {
            sav.delete_saveable();
        }
        std::string dir_path = it->first;
        dir.rmdir(QString::fromStdString(dir));
    }
}

bool SaveableTree::save_tree()
{
    for(auto it = m_saveables.begin(); it != m_saveables.end(); it++){
        std::vector<SaveableNode> nodes = it->second;
        foreach (SaveableNode sav, nodes) {
            //sav.save_saveable();
        }
        std::string dir_path = it->first;
        dir.rmdir(QString::fromStdString(dir));
    }
}

bool SaveableTree::load_tree(const std::string &full_path_trunk)
{
    for(auto it = m_saveables.begin(); it != m_saveables.end(); it++){
        std::vector<SaveableNode> nodes = it->second;
        foreach (SaveableNode sav, nodes) {
            //sav.load_saveable();
        }
        std::string dir_path = it->first;
        dir.rmdir(QString::fromStdString(dir));
    }
}
