#include "text.h"

#include <gtkmm.h>

// TODO: ADD ASYNC SUPPORT
// TODO: FILE PERMS

void fileList::append(Glib::ustring name, Glib::ustring file, bool editable) {
    auto x = ppdTextBuffer(name, file, editable);
    if(file != "")
        x.reload();
    buffers.push_back(x);
}

Glib::RefPtr<Gtk::TextBuffer> fileList::getBufferByName(Glib::ustring name) const {
    for(auto x: buffers) 
        if(x.getName() == name)
            return x.buffer();
    return nullptr;
}

void fileList::deleteByName(Glib::ustring name) {
    for(auto x = buffers.begin(); x != buffers.end(); x++) {
        if(x->getName() == name) {
            buffers.erase(x);
            return;
        }
    }
}

void ppdTextBuffer::save() {
    auto overwriteBuffer = fileObj()->replace();
    overwriteBuffer->write(buffer()->get_text());
    overwriteBuffer->close();
}

void ppdTextBuffer::reload() {
    // ... why does gio do this?
    char* contents;
    gsize amt;
    fileObj()->load_contents(contents, amt);
    buffer()->set_text(contents, contents+amt);
    g_free(contents);
}
