#include "analysiscomponent.h"

AnalysisComponent::AnalysisComponent()
{
}

/** @brief MainWindow::on_action_show_hide_analysis_area_triggered
 * Toggles the choosing of an analysis area.
 */
void MainWindow::on_action_show_hide_analysis_area_triggered() {
    mvideo_player->toggle_analysis_area();
    toggle_toolbar();
    if (mvideo_player->is_showing_analysis_tool()) {
        set_status_bar("Showing analysis area tool. Select your area by clicking on the video.");
    } else {
        set_status_bar("Hiding analysis area tool.");
    }
}

/**
 * @brief MainWindow::on_action_invert_analysis_area_triggered
 * Switches between choosing area for analysing and area for not analysing.
 */
void MainWindow::on_action_invert_analysis_area_triggered() {
    mvideo_player->invert_analysis_area();
    if (mvideo_player->is_including_area()) {
        set_status_bar("Choose an area to run the analysis on.");
    } else {
        set_status_bar("Choose an area to exclude from the analysis.");
    }
}

/**
 * @brief MainWindow::on_action_show_hide_analysis_overlay_triggered
 * Toggles the state of showing the analysis overlay.
 */
void MainWindow::on_action_show_hide_analysis_overlay_triggered() {
    mvideo_player->toggle_analysis_overlay();
    if (mvideo_player->is_showing_analysis_overlay()) {
        set_status_bar("Showing analysis overlay: on.");
    } else {
        set_status_bar("Showing analysis overlay: off.");
    }
}
