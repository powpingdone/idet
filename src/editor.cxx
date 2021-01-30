#include "editor.h"
#include <sigc++/functors/mem_fun.h>
#include <gtkmm/enums.h>
#include <sstream>

editorWindow::editorWindow() : slaveEditor(Gtk::Orientation::HORIZONTAL) {
    set_title("editorWindow");
    
    // setup box containing the other component boxes
    masterGrid.set_margin(10);
    set_child(masterGrid);

    // setup editor box
    masterGrid.attach(sourceHolder, 0, 0);
    sourceHolder.set_child(slaveEditor);
    slaveEditor.set_start_child(sourceLines);
    slaveEditor.set_shrink_start_child(false);
    slaveEditor.set_shrink_end_child(false);
    slaveEditor.set_resize_start_child(false);
    slaveEditor.set_resize_end_child(false);
    slaveEditor.set_wide_handle(false);
    slaveEditor.set_position(48);
    slaveEditor.set_end_child(sourceCode);
    sourceHolder.set_expand();
    
    // setup lines
    sourceLines.set_editable(false);
    sourceLines.set_cursor_visible(false);
    sourceLines.set_monospace();
    sourceCode.set_monospace();
    sourceCode.get_buffer()->signal_changed().connect(sigc::mem_fun(*this, &editorWindow::updateLineNumbers));
}

void editorWindow::updateLineNumbers() {
    auto buf = sourceLines.get_buffer(), src = sourceCode.get_buffer();
    if(src->get_line_count() != lines) {
        std::stringstream newNumbers;
        lines = src->get_line_count();
        for(int x = 1; x < lines+1; x++) {
            newNumbers << x << std::endl;
        }
        buf->set_text(newNumbers.str());
    }
}
