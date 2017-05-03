#include "filehandlercomponent.h"

/**
 * @brief MainWindow::setup_filehandler
 * Sets up filehandler and loads projects.
 */
void MainWindow::setup_file_handler(){
    file_handler = new FileHandler();
    for(auto it = file_handler->open_projects.begin(); it != file_handler->open_projects.end(); it++){
        ID id = *it;
        Project* proj = file_handler->get_project(id);
        add_project_to_tree(proj);
    }
}

/**
 * @brief MainWindow::on_action_save_triggered
 * saves project which is selected in tree view,
 * checks if there is one
 */
void MainWindow::on_action_save_triggered() {
    QTreeWidgetItem *item;
    MyQTreeWidgetItem *my_project;
    if(ui->project_tree->selectedItems().size() == 1) {
        item = ui->project_tree->selectedItems().first();
        my_project = (MyQTreeWidgetItem*)get_project_from_object(item);
        this->file_handler->save_project(my_project->id);
        std::string text = "Saved project " + my_project->name.toStdString();
        set_status_bar(text);
    } else {
        set_status_bar("Nothing to save");
    }
}

/**
 * @brief MainWindow::on_action_load_triggered
 */
void MainWindow::on_action_load_triggered() {
    QString dir = QFileDialog::getOpenFileName(this, tr("Choose project"),this->file_handler->get_work_space().absolutePath().toStdString().c_str(),tr("*.json"));
    if(!dir.isEmpty()) { // Check if you have selected something.
        Project* load_proj= this->file_handler->load_project(dir.toStdString());
        add_project_to_tree(load_proj);
        set_status_bar("Project " + load_proj->name + " loaded.");
    }
}



/**
 * @brief MainWindow::on_action_choose_workspace_triggered
 * Opens file explorer and requests a workspace select from user, updates
 * filehandler workspace accordingly.
 */
void MainWindow::on_action_choose_workspace_triggered() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Workspace"),this->file_handler->get_work_space().absolutePath().toStdString().c_str());
    if (!dir.isEmpty()) {
        this->file_handler->set_work_space(dir.toStdString() + "/");
        set_status_bar("New wokspace set to " + this->file_handler->work_space);
    }
}
