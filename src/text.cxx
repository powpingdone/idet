#include "text.h"

#include <gtkmm.h>

// TODO: ADD ASYNC SUPPORT
// TODO: FILE PERMS

void fileList::append(Glib::ustring name, Glib::ustring file, bool editable) {
    LOG("Loading new buffer \"%s\"", name.c_str());
    auto x = Glib::RefPtr<ppdTextBuffer>(new ppdTextBuffer(name, file, editable));
    if(file != "") {
        x->reload();
    }
    buffers.push_back(std::move(x));
}

Glib::RefPtr<Gtk::TextBuffer> fileList::getBufferByName(Glib::ustring name) const {
    for(auto x: buffers) {
        if(x->getName() == name)
            return x->buffer();
    }
    return nullptr;
}

Glib::RefPtr<ppdTextBuffer> fileList::getPPDTBByName(Glib::ustring name) const {
    for(auto x: buffers) {
        if(x->getName() == name)
            return x;
    }
    return nullptr;
}

bool fileList::setCurrentBufByName(Glib::ustring name) {
    LOG("Setting fileList's 'visible' buffer as %s", name.c_str());
    if(nameExists(name)) {
        currBuffer = name;
        return true;
    }
    LOG("Name does not exist! Not setting.");
    return false;
}

void fileList::deleteByName(Glib::ustring name) {
    for(auto x = buffers.begin(); x != buffers.end(); x++) {
        if(x->get()->getName() == name) {
            buffers.erase(x);
            return;
        }
    }
}

std::vector<Glib::ustring> fileList::getAllNames() const {
    std::vector<Glib::ustring> names;
    for(auto x: buffers) { names.push_back(x->getName()); }
    return names;
}

bool ppdTextBuffer::save() {
    if(!fileObj()) {
        LOG("Buffer \"%s\" does not have a defined file! Not saving.", getName().c_str());
        return false;
    }
    LOG("Saving file \"%s\"...", this->name.c_str());
    auto overwriteBuffer = fileObj()->replace();
    overwriteBuffer->write(buffer()->get_text());
    overwriteBuffer->close();
    return true;
}

bool ppdTextBuffer::reload() {
    // ... why does gio do this?
    if(!fileObj()) {
        LOG("Buffer \"%s\" does not have a defined file! Not reloading.", getName().c_str());
        return false;
    }
    LOG("Reloading file \"%s\"...", this->name.c_str());
    char* contents;
    gsize amt;
    fileObj()->load_contents(contents, amt);
    buffer()->set_text(contents, contents + amt);
    g_free(contents);
    return true;
}
