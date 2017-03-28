#include "filehandler.h"

/**
 * @brief FileHandler::FileHandler
 */
file_handler::file_handler() {
    this->pid = 0; // zero out counter ids
    this->fid = 0;
    this->did = 0;
    this->lastError = 0;

}

/**
 * @brief FileHandler::create_project
 * creates project and associated files.
 * @param std::string name
 * @return Project* created project
 */
project* file_handler::create_project(std::string proj_name){
    project* proj =  new project(this->pid, proj_name);
    this->projects.insert(std::make_pair(proj->id, proj));
    save_project(proj);
    this->pid++;
    return proj;
}

/**
 * @brief FileHandler::create_directory
 * @param dirpath
 * @return unique directory ID
 */
ID file_handler::create_directory(std::string dirpath){
    this->lastError = make_dir(dirpath); //varying implementation, OS dependant
    ID id = this->add_dir(dirpath);
    return id;
}

/**
 * @brief FileHandler::delete_directory
 * @param id
 * @return errorcode, if deletion was done code is 0;
 * otherwise see OS relevant directoryfile.
 */
FH_ERROR file_handler::delete_directory(ID id){
    FH_ERROR err = remove_dir(this->get_dir(id)); //varying implementation, OS dependant
    return err;
}

/**
 * @brief FileHandler::save_project
 * @param id
 */
void file_handler::save_project(ID id){
    save_project(get_project(id));
}

/**
 * @todo unfinished, needs full project structure
 * and program to file parser to finish
 * @brief FileHandler::save_project
 * @param Project* name
 * Creates project and associated files.
 * @return void
 */
void file_handler::save_project(project* proj){
    std::string proj_file = proj->name + std::string(".txt"); //filename
    if(!proj->saved){
        ID dir_id = create_directory(std::string(WORKSPACE) + "/"+ proj->name);//project directory

        proj->files->dir = dir_id;

        proj->files->f_proj = create_file(proj_file, dir_id); //create project file

        std::string vid_file = proj->name + "_videos.txt";
        proj->files->f_videos = create_file(vid_file, dir_id); //create video file


        std::string analysis_file = proj->name + "_analyses.txt";
        proj->files->f_analysis = create_file(analysis_file, dir_id); //create analysis file


        std::string drawing_file = proj->name + "_drawings.txt";
        proj->files->f_drawings =create_file(drawing_file, dir_id); //create drawings file
    }
    update_proj_files(proj);

}

/**
 * @todo unfinished, will be released with parser
 * however, is needed for creating
 * @brief FileHandler::save_project
 * @param Project* name
 * Creates project and associated files.
 * @return void
 */
void file_handler::update_proj_files(project* proj){
    ProjectStream ps;
    ps << *proj;
    write_file(proj->files->f_proj, ps.projfile.str(), WRITE_OPTION::OVERWRITE);
    write_file(proj->files->f_videos, ps.videos.str(), WRITE_OPTION::OVERWRITE);
    write_file(proj->files->f_analysis, ps.analyzes.str(), WRITE_OPTION::OVERWRITE);
    write_file(proj->files->f_drawings, ps.drawings.str(), WRITE_OPTION::OVERWRITE);
}
void file_handler::load_proj_files(std::string str){
    ID id;
    std::string filepath;
    std::stringstream sstr;
    sstr << str;    
    //read files until empty
    while(sstr >> id >> filepath){
        add_file(id, filepath);
    }
}

/**
 * @brief FileHandler::load_project
 * @param dirpath
 * @return
 */
project* file_handler::load_project(std::string full_project_path){
    std::string dirpath = full_project_path.substr(0, full_project_path.find_last_of("/"));
    std::string proj_name = full_project_path.substr(full_project_path.find_last_of("/")+1, full_project_path.length());
    proj_name = proj_name.substr(0, proj_name.find(".txt"));
    return load_project(proj_name, dirpath);
}

/**
 * @todo load analyses (in project <</>> operators)
 * @todo load drawings (in project <</>> operators)
 * @brief FileHandler::load_project
 * @param projname
 * @param dirpath
 * @return project
 */
project* file_handler::load_project(std::string projname, std::string dirpath){
    project* proj = new project();
    proj->id = this->pid;
    proj->files->dir = add_dir(dirpath);

    add_project(std::make_pair(this->pid++, proj));
    ProjectStream proj_stream;

    proj->saved = true;
//    Read project file
    std::string proj_filepath = dirpath + "/" + projname + ".txt";
    proj->files->f_proj = load_project_file(proj_filepath, proj_stream.projfile);

//    Read video file
    std::string video_filepath = dirpath + "/" + projname + "_videos.txt";
    proj->files->f_videos = load_project_file(video_filepath, proj_stream.videos);
//    Read Analyzes
    std::string analysis_filepath = dirpath + "/" + projname + "_analyses.txt";
    proj->files->f_analysis = load_project_file(analysis_filepath, proj_stream.videos);
//    Read Drawings
    std::string drawing_filepath = dirpath + "/" + projname + "_drawings.txt";
    proj->files->f_drawings = load_project_file(drawing_filepath, proj_stream.drawings);
//    Read project from projstream
    proj_stream >> *proj;
    return proj;
}

/**
 * @brief FileHandler::load_project_file
 * help function for load_project
 * @param filePath
 * @param projFileStream
 * @return ID
 */
ID file_handler::load_project_file(std::string filepath, std::stringstream& proj_file_stream){
    std::string buf;
    ID projFileID = add_file(filepath);
    read_file(projFileID, buf);
    proj_file_stream << buf; // Read project name
    return projFileID;
}

/**
 * @brief FileHandler::delete_project
 * Deletes project, its associated files and contents.
 * OBS! This operation is as of now irreversible
 * @param Project*
 * @return FH_ERROR errorcode
 */
FH_ERROR file_handler::delete_project(project* proj){
    proj_files* pf = proj->files;
    delete_file(pf->f_proj);
    delete_file(pf->f_videos);
    delete_file(pf->f_analysis);
    delete_file(pf->f_drawings);
    return delete_directory(proj->files->dir);

}
/**
 * @todo make threadsafe
 * @brief FileHandler::add_video
 * @param Project*,string filepath
 *
 * string
 * Add a video filepath to a given project.
 * Creates Video object which is accessed further by returned id.
 */
void file_handler::add_video(project* proj, std::string filepath){
    video* v = new video(filepath);
    proj->add_video(v);
    this->add_file(filepath);
}

 /**
  * @brief FileHandler::create_file
  * create a file by given name in already excisting
  * application tracked directory
  * @param std::string file name, ID directory id
  */

ID file_handler::create_file(std::string filename, ID dir_id){
    std::ofstream f;
    std::string filePath = this->get_dir(dir_id)+"/"+filename;
    f.open(filePath.c_str());
    return this->add_file(filePath);
  }

/**
 * @todo make threadsafe
 * @brief FileHandler::delete_file
 * delete application tracked file
 * @param ID file id
 */
 FH_ERROR file_handler::delete_file(ID id){
    std::string file = this->get_file(id);
    return std::remove(file.c_str());
 }

 /**
  * @todo make threadsafe
  * @brief FileHandler::write_file
  *  Write given text to an application tracked file
  * @param ID file id, std::string text
  * @return void
  */
 void file_handler::write_file(ID id, std::string text, WRITE_OPTION opt){
    std::string fileName = this->get_file(id);
    std::ofstream f;
    switch(opt){
    case WRITE_OPTION::OVERWRITE:
        f.open(fileName.c_str(), std::ios::in | std::ios::out);
        break;
    case WRITE_OPTION::APPEND:
        f.open(fileName.c_str(), std::ios::in | std::ios::out | std::ios::ate);
        break;
    default:
        return; // no open file
        break;
    }
    if(f.is_open()) f << text.c_str() << std::endl;
 }

 /**
  * @brief FileHandler::read_file
  *  Read given lenght of lines to buffer from application
  *  tracked file. OBS! If number of lines exceeds =>
  *  reads to end of file (EOF)
  *  @param ID file id, std::string text
  *  @return voi
  */
 void file_handler::read_file(ID id,  std::string& buf, int linesToRead){
     std::ifstream f(this->get_file(id));
     std::string temp;
     if(f.is_open()){
         while(linesToRead-- && std::getline(f, temp)){
            buf += temp;
         }
     }
 }

 /**
  * @brief FileHandler::get_project
  * Getter
  * @param ID project id
  * @return Project*
  */
 project* file_handler::get_project(ID pid){
    this->proj_map_lock.lock();
    project* p = this->projects.at(pid);
    this->proj_map_lock.unlock();
    return p;
 }

 /**
  * @brief FileHandler::get_file
  * Getter
  * @param ID project file id
  * @return std::string filepath
  */
 std::string file_handler::get_file(ID id){
    this->file_map_lock.lock();
    std::string file = this->file_map.at(id);
    this->file_map_lock.unlock();
    return file;
 }

 /**
  * @brief FileHandler::get_dir
  * @param ID directory id
  * @return directory path
  */
 std::string file_handler::get_dir(ID id){
    this->dir_map_lock.lock();
    std::string dir = this->dir_map.at(id);
    this->dir_map_lock.unlock();
    return dir;
 }

 /**
  * @brief FileHandler::add_projectr
  * @param std::pari<<ID, Project*> pair
  * @return void
  */
 void file_handler::add_project(std::pair<ID,project*> pair){
    this->proj_map_lock.lock();
    this->projects.insert(pair);
    this->proj_map_lock.unlock();

 }

 /**
  * @brief FileHandler::add_file
  * @param std::string filepath
  * @return unique file identifier
  */
ID file_handler::add_file(std::string filepath){
    add_file(this->fid, filepath);
    return this->fid++;
 }

/**
 * @brief FileHandler::add_file
 * @param id
 * @param filepath
 */
void file_handler::add_file(ID id ,std::string filepath){
    std::pair<ID,std::string> pair = std::make_pair(id, filepath);
    this->file_map_lock.lock();
    this->file_map.insert(pair);
    this->file_map_lock.unlock();
}

 /**
  * @brief FileHandler::add_dir
  * @param std::string dirpath
  * @return unique directory identifier
  */
ID file_handler::add_dir(std::string dirpath){
    std::pair<ID,std::string> pair = std::make_pair(this->did, dirpath);
    this->dir_map_lock.lock();
    this->dir_map.insert(pair);
    this->dir_map_lock.unlock();
    return this->did++;
 }

/**
 * @brief FileHandler::proj_equals
 * @param proj
 * @param proj2
 * @return true if project contents are the same
 */
bool file_handler::proj_equals(project& proj, project& proj2){
    bool videoEquals =  std::equal(proj.videos.begin(), proj.videos.end(),
               proj2.videos.begin(),
               [](const video* v, const video* v2){return *v == *v2;}); // lambda function comparing using video==
                                                                        // by dereferencing pointers in vector
    return projfiles_equal(*proj.files , *proj2.files) && //probably unnecessary as projfiles have projname followed by default suffix
           proj.name == proj2.name &&
           videoEquals;
}

/**
 * @brief FileHandler::projfiles_equal
 * @param pf
 * @param pf2
 * @return true if files are the same paths
 */
bool file_handler::projfiles_equal(proj_files& pf, proj_files& pf2){
    return dirs_equal(pf.dir, pf2.dir) &&
        this->files_equal(pf.f_proj, pf2.f_proj) &&
        this->files_equal(pf.f_analysis, pf2.f_analysis)&&
        this->files_equal(pf.f_drawings, pf2.f_drawings) &&
        this->files_equal(pf.f_videos, pf2.f_videos);
}

/**
 * @brief FileHandler::files_equal
 * @param id
 * @param id2
 * @return
 */
bool file_handler::files_equal(ID id, ID id2){
    return this->get_file(id) == this->get_file(id2);
}

/**
 * @brief FileHandler::dirs_equal
 * @param id
 * @param id2
 * @return true if dirs are same path
 */
bool file_handler::dirs_equal(ID id, ID id2){
    return this->get_dir(id) == this->get_dir(id2);
}
