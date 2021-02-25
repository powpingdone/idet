#include "actions.h"
#include <gtkmm.h>

bool OpenFile::action() {
    auto dialog = new Gtk::FileChooserDialog("Open File");
    return true;
} 
