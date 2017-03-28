#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#define WINDOWS 1
#define UNIX 2
#ifdef _WIN32
    #include <windows.h>
    #include "stringhelper.h"
#elif __APPLE__
#elif __unix__
#endif
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <mutex>
#include <sstream>
#include "project.h"
#include "dir.h"
#include <algorithm>
#ifdef _WIN32
    #define WORKSPACE "C:"
#elif __APPLE__
    #define WORKSPACE "/Applications"
#elif __unix__
    #define  WORKSPACE "~"

#endif


enum WRITE_OPTION{APPEND, OVERWRITE};
typedef int FH_ERROR; // file handler error code
typedef int ID;
struct project; // fix for include issue
struct proj_files;
class file_handler
{
public:
    file_handler();
    std::string workSpace;
    //
    //  Project manipulation
    //
    project* open_project(std::string dirpath);
    project* create_project(std::string proj_name);
    FH_ERROR delete_project(project* proj);
    project* load_project(std::string full_project_path);
    project* load_project(std::string projname, std::string dirpath);
    void save_project(ID id);
    void save_project(project* proj);


    void add_video(project* proj, std::string filepath);
    //directory manipulation
    //varying implementation
    ID create_directory(std::string dirpath);
    FH_ERROR delete_directory(ID id);

    //file manipulation

    ID create_file(std::string filename, ID dir_id);
    FH_ERROR delete_file(ID id);
    void write_file(ID id, std::string text, WRITE_OPTION opt = WRITE_OPTION::APPEND);
    void read_file(ID id,  std::string& buf, int lines_to_read = -1);

    friend bool operator==(proj_files& pf, proj_files& pf2);
    friend bool operator==(project& proj, project& proj2);

    bool proj_equals(project& proj, project& proj2);
    bool projfiles_equal(proj_files& pf, proj_files& pf2);
    bool dirs_equal(ID id, ID id2);
    bool files_equal(ID id, ID id2);
    // thread safe read operations for maps


    std::string get_dir(ID id);
    project* get_project(ID id);
    std::string get_file(ID id);

    // Last error
    FH_ERROR lastError;

private:

    void update_proj_files(project* proj); // used to update existing project files and maps
    // thread safe add operations for maps
    ID add_file(std::string filepath);
    void add_project(std::pair<ID,project*> pair);
    ID add_dir(std::string dirpath);
    ID load_project_file(std::string filepath, std::stringstream& proj_file_stream);
    void load_proj_files(std::string str);
    //add used for loading project from file
    void add_file(ID id ,std::string filepath);
    /**
     * @brief m_projects, m_fileMap, m_dirMap
     * map structures for keeping track of projects, files and directories.
     */
    std::map<ID,project*> projects;
    std::map<ID, std::string> file_map;
    std::map<ID, std::string> dir_map;
    /**
     * @todo implement smarter lock mechanism to avoid overhead
     * of only 1 reader/writer at a time
     * @brief dirMapLock, fileMapLock, projMapLock
     */
    std::mutex dir_map_lock; // lock for handling directory write/read
    std::mutex file_map_lock;// lock for handling file write/read
    std::mutex proj_map_lock;// lock for handling project write/read
    ID pid; //counter for project ids
    ID fid; //counter for file ids
    ID did; //counter for directory ids

};

#endif // FILEHANDLER_H
