#include "documentlistcomponent.h"

DocumentListComponent::DocumentListComponent()
{
}
/**
 * @brief MainWindow::on_bookmark_button_clicked
 * Button to add a bookmark to the bookmark view.
 */
void MainWindow::on_bookmark_button_clicked() {
    QTreeWidgetItem *item;
    MyQTreeWidgetItem *my_project;
    if(ui->project_tree->selectedItems().size() == 1) {
        // Get current project.
        item = ui->project_tree->selectedItems().first();
        my_project = (MyQTreeWidgetItem*)get_project_from_object(item);
        // Add bookmarks-folder to the project-folder.
        Project* proj = file_handler->get_project(my_project->id);
        QDir dir = file_handler->get_dir(proj->dir_bookmarks);
        // Export the current frame in the bookmarks-folder.
        // Get bookmark description
        QString bookmark_text("");
        bool ok;
        bookmark_text = bookmark_view->get_input_text(&ok);
        if(!ok) return;
        int frame_number = mvideo_player->get_current_frame_num();
        QImage frame = mvideo_player->get_current_frame_unscaled();
        Bookmark* bookmark = new Bookmark(frame_number, frame, dir.absolutePath(), bookmark_text);
        proj->add_bookmark(((MyQTreeWidgetItem*)item)->id, bookmark);
        bookmark_view->add_bookmark(bookmark);
        set_status_bar("Bookmark created.");
    }
}
