#include "projectcomponent.h"

/**
 * @brief MainWindow::on_action_add_project_triggered
 */
void MainWindow::on_action_add_project_triggered() {
    MakeProject *make_project = new MakeProject(this, this->file_handler, this->file_handler->get_work_space().absolutePath().toStdString());
    make_project->show();
    set_status_bar("Adding project");
}

/**
 * @brief MainWindow::add_project_to_tree
 * @param proj to add to tree
 * also adds all videos of the project to the tree
 */
void MainWindow::add_project_to_tree(Project* proj) {
    MyQTreeWidgetItem *project_in_tree = new MyQTreeWidgetItem(TYPE::PROJECT, QString::fromStdString(proj->name), proj->id);
    project_in_tree->setText(0, QString::fromStdString(proj->name));
    ui->project_tree->addTopLevelItem(project_in_tree);
    ui->project_tree->clearSelection();
    project_in_tree->setSelected(true);
    for(auto vid = proj->get_videos().begin(); vid != proj->get_videos().end(); ++vid){
        VideoProject* v = vid->second;
        add_video_to_tree(v->get_video()->file_path, v->get_video()->id);
        // Add bookmarks
        std::vector<Bookmark*> bookmarks = v->get_bookmarks();
        for(auto it2 = bookmarks.begin(); it2 != bookmarks.end(); it2++){
            Bookmark* bm = *it2;
            bookmark_view->add_bookmark(bm);
        }
    }
}

/**
 * @brief MainWindow::add_video_to_tree
 * @param file_path of the video
 */
void MainWindow::add_video_to_tree(std::string file_path, ID id) {
    QTreeWidgetItem *project;
    project = ui->project_tree->selectedItems().first();
    MyQTreeWidgetItem *video_in_tree = new MyQTreeWidgetItem(TYPE::VIDEO, QString::fromStdString(file_path), id);
    video_in_tree->set_text_from_filepath(file_path);
    project->addChild(video_in_tree);
    project->setExpanded(true);
}

/**
 * @brief MainWindow::on_action_close_project_triggered
 * Remove project from the tree without deleting.
 */
void MainWindow::on_action_close_project_triggered() {
    QTreeWidgetItem *item;
    MyQTreeWidgetItem *my_project;
    if(ui->project_tree->selectedItems().size() == 1) {
        item = ui->project_tree->selectedItems().first();
        my_project = (MyQTreeWidgetItem*) get_project_from_object(item);
        set_status_bar("Closed " + my_project->name.toStdString());
        file_handler->close_project(my_project->id);
        remove_item_from_tree(my_project);
    } else {
        set_status_bar("Multiple or nothing selected.");
    }
}

/**
 * @brief MainWindow::on_action_add_video_triggered
 * Prompts user with file browser to add video
 * to selected project
 */
void MainWindow::on_action_add_video_triggered() {
    QTreeWidgetItem *project;
    if(ui->project_tree->selectedItems().size() == 1) {
        project = ui->project_tree->selectedItems().first();
        MyQTreeWidgetItem *my_project = (MyQTreeWidgetItem*) project;
        if (my_project->type == TYPE::PROJECT){
            Project *proj = this->file_handler->get_project(my_project->id);
            std::string video_dir_path = this->file_handler->get_dir(proj->dir_videos).absolutePath().toStdString();
            QString q_video_file_path = QFileDialog::getOpenFileName(this, tr("Choose video"), video_dir_path.c_str(),
                                                       tr("Videos (*.avi *.mkv *.mov *.mp4 *.3gp *.flv *.webm *.ogv *.m4v)"));
            if(!q_video_file_path.isEmpty()) { // Check if you have selected something.
                std::string video_file_path = q_video_file_path.toStdString();
                ID id = file_handler->add_video(proj, video_file_path);
                add_video_to_tree(video_file_path, id);
                set_status_bar("Video " + video_file_path + " added.");
            }
        } else {
            set_status_bar("No project selected.");
        }
    } else {
        set_status_bar("Multiple or no projects selected.");
    }
}
