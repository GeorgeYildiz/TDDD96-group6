#include "drawingcomponent.h"

/**
 * @brief MainWindow::on_action_undo_triggered
 * Undo the drawings on the overlay.
 */
void MainWindow::on_action_undo_triggered() {
    mvideo_player->undo_overlay();
    set_status_bar("Undo drawing.");
}

/**
 * @brief MainWindow::on_action_clear_triggered
 * Clear the drawings on the overlay.
 */
void MainWindow::on_action_clear_triggered() {
    mvideo_player->clear_overlay();
    set_status_bar("Cleared drawings.");
}

/** @brief MainWindow::on_action_show_hide_overlay_triggered
* Toggles the showing/hiding of the overlay.
* Invoked by menu item.
*/
void MainWindow::on_action_show_hide_overlay_triggered() {
   mvideo_player->toggle_overlay();
   toggle_toolbar();
   if (mvideo_player->is_showing_overlay()) {
       set_status_bar("Overlay: On.");
   } else {
       set_status_bar("Overlay: Off.");
   }
}

/**
* @brief MainWindow::on_action_colour_triggered
* Selects a colour for the overlay drawing tool.
*/
void MainWindow::on_action_colour_triggered() {
   QColor col = QColorDialog::getColor();
   if (col.isValid()) {
       mvideo_player->set_overlay_colour(col);
       std::string msg = "Color: ";
       msg.append(col.name().toStdString());
       set_status_bar(msg);
   }
}

/**
* @brief MainWindow::on_action_rectangle_triggered
* Selects the rectangle shape for the overlay drawing tool.
*/
void MainWindow::on_action_rectangle_triggered() {
   mvideo_player->set_overlay_tool(RECTANGLE);
   set_status_bar("Tool: rectangle.");
   deselect_overlay_tool();
   ui->action_rectangle->setChecked(true);
}

/**
* @brief MainWindow::on_action_circle_triggered
* Selects the circle shape for the overlay drawing tool.
*/
void MainWindow::on_action_circle_triggered() {
   mvideo_player->set_overlay_tool(CIRCLE);
   set_status_bar("Tool: circle.");
   deselect_overlay_tool();
   ui->action_circle->setChecked(true);
}

/**
* @brief MainWindow::on_action_line_triggered
* Selects the line shape for the overlay drawing tool.
*/
void MainWindow::on_action_line_triggered() {
   mvideo_player->set_overlay_tool(LINE);
   set_status_bar("Tool: line.");
   deselect_overlay_tool();
   ui->action_line->setChecked(true);
}

/**
* @brief MainWindow::on_action_arrow_triggered
* Selects the arrow shape for the overlay drawing tool.
*/
void MainWindow::on_action_arrow_triggered() {
   mvideo_player->set_overlay_tool(ARROW);
   set_status_bar("Tool: arrow.");
   deselect_overlay_tool();
   ui->action_arrow->setChecked(true);
}

/**
* @brief MainWindow::on_action_pen_triggered
* Selects the pen for the overlay drawing tool.
*/
void MainWindow::on_action_pen_triggered() {
   mvideo_player->set_overlay_tool(PEN);
   set_status_bar("Tool: pen.");
   deselect_overlay_tool();
   ui->action_pen->setChecked(true);
}
/**
 * @brief MainWindow::on_action_zoom_in_triggered
 * Sets a state in the video overlay
 * for the user to choose an area.
 */
void MainWindow::on_action_zoom_in_triggered() {
    mvideo_player->zoom_in();
    set_status_bar("Zoom in. Choose your area.");
}

/**
 * @brief MainWindow::on_action_zoom_out_triggered
 * Reset the zoom level to the full video size.
 */
void MainWindow::on_action_zoom_out_triggered() {
    mvideo_player->zoom_out();
    set_status_bar("Zoom out.");
}

/** @brief MainWindow::on_action_contrast_brightness_triggered
 * Opens a window to choose contrast and brightness in.
 */
void MainWindow::on_action_contrast_brightness_triggered() {
    float contrast = mvideo_player->get_contrast();
    int brightness = mvideo_player->get_brightness();

    // Create the texts shown in the dialog
    std::stringstream contrast_ss;
    contrast_ss << "Contrast [" << mvideo_player->CONTRAST_MIN << " – " << mvideo_player->CONTRAST_MAX <<
                   "] (default: "<< mvideo_player->CONTRAST_DEFAULT <<"): ";
    QString contrast_text = QString::fromStdString(contrast_ss.str());
    std::stringstream brightness_ss;
    brightness_ss << "Brightness [" << mvideo_player->BRIGHTNESS_MIN << " – " << mvideo_player->BRIGHTNESS_MAX <<
                     "] (default: "<< mvideo_player->BRIGHTNESS_DEFAULT <<"): ";
    QString brightness_text = QString::fromStdString(brightness_ss.str());

    // Create the dialog
    CustomDialog dialog("Contrast & Brightness", this);
    dialog.addLabel("Enter values:");
    dialog.addDblSpinBoxF(contrast_text, (float) mvideo_player->CONTRAST_MIN, (float) mvideo_player->CONTRAST_MAX,
                          &contrast, mvideo_player->CONTRAST_DECIMALS, (float) mvideo_player->CONTRAST_STEP,
                          "Choose contrast value with the input box.");
    dialog.addSpinBox(brightness_text, mvideo_player->BRIGHTNESS_MIN, mvideo_player->BRIGHTNESS_MAX,
                      &brightness, mvideo_player->BRIGHTNESS_STEP, "Choose brightness value with the input box.");

    // Show the dialog (execution will stop here until the dialog is finished)
    dialog.exec();

    if (dialog.wasCancelled()) {
        return;
    }
    mvideo_player->set_contrast(contrast);
    mvideo_player->set_brightness(brightness);
}

/**
 * @brief MainWindow::on_action_text_triggered
 * Selects the text for the overlay drawing tool.
 */
void MainWindow::on_action_text_triggered() {
    mvideo_player->set_overlay_tool(TEXT);
    set_status_bar("Tool: text.");
    deselect_overlay_tool();
    ui->action_text->setChecked(true);
}

/**
 * @brief deselct_overlay_tool
 * Deselects all overlay tools.
 */
void MainWindow::deselect_overlay_tool(){
    ui->action_rectangle->setChecked(false);
    ui->action_circle->setChecked(false);
    ui->action_line->setChecked(false);
    ui->action_arrow->setChecked(false);
    ui->action_pen->setChecked(false);
    ui->action_text->setChecked(false);
}
