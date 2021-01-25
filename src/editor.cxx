#include "editor.h"
#include <gtkmm/enums.h>

editorWindow::editorWindow() : slaveEditor(Gtk::Orientation::HORIZONTAL) {
    set_title("editorWindow");
    
    // setup box containing the other component boxes
    masterGrid.set_margin(10);
    set_child(masterGrid);

    // setup editor box
    masterGrid.attach(sourceHolder, 0, 0);
    sourceHolder.set_child(slaveEditor);
    slaveEditor.set_start_child(sourceLines);
    slaveEditor.set_resize_start_child(false);
    slaveEditor.set_wide_handle(false);
    slaveEditor.set_position(48);
    slaveEditor.set_end_child(textView);
    sourceLines.set_editable(false);
    sourceHolder.set_expand();

}
