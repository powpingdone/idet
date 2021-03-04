#include "text.h"

#include <gtkmm.h>

// TODO: ADD ASYNC SUPPORT
// TODO: FILE PERMS

void fileList::append(Glib::ustring name, Glib::ustring file, bool editable) {
    LOG("Loading new buffer \"%s\"", name.c_str());
    auto x = ppdTextBuffer(name, file, editable);
    if(file != "") {
        x.reload();
    }
    buffers.push_back(std::move(x));
}

Glib::RefPtr<Gtk::TextBuffer> fileList::getBufferByName(Glib::ustring name) const {
    for(auto x: buffers) {
        if(x.getName() == name)
            return x.buffer();
    }
    return nullptr;
}

bool fileList::setCurrentBufByName(Glib::ustring name) {
    LOG("Setting fileList's 'visible' buffer as %s", name.c_str());
    if(nameExists(name)) {
        currBuffer = name;
        return true;
    }
    LOG("Name does not exist!");
    return false;
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
    LOG("Saving file \"%s\"...", this->name.c_str());
    auto overwriteBuffer = fileObj()->replace();
    overwriteBuffer->write(buffer()->get_text());
    overwriteBuffer->close();
}

void ppdTextBuffer::reload() {
    // ... why does gio do this?
    LOG("Reloading file \"%s\"...", this->name.c_str());
    char* contents;
    gsize amt;
    fileObj()->load_contents(contents, amt);
    buffer()->set_text(contents, contents+amt);
    g_free(contents);
}
