#include "text.h"
#include <gtkmm.h>

// TODO: ADD ASYNC SUPPORT
// TODO: FILE PERMS

void fileList::append(Glib::ustring name, Glib::ustring file, bool editable) {
    DLOG("Loading new buffer \"%s\"", name.c_str());
    auto x = Glib::RefPtr<ppdTextBuffer>(new ppdTextBuffer(name, file, editable));
    if(file != "") {
        x->reload();
    }
    buffers.push_back(x);
}

Glib::RefPtr<Gtk::TextBuffer> fileList::getBufferByName(Glib::ustring name) const {
    auto potentBuf = getPPDTBByName(name);
    return potentBuf ? potentBuf->buffer() : nullptr;
}

Glib::RefPtr<ppdTextBuffer> fileList::getPPDTBByName(Glib::ustring name) const {
    DLOG("Searching for \"%s\"...", name.c_str());
    for(auto x: buffers) {
        if(x->getName() == name) {
            DLOG("Found matching buffer.");
            return x;
        }
    }
    DLOG("No matching buffer found");
    return nullptr;
}

bool fileList::setCurrentBufByName(Glib::ustring name) {
    DLOG("Setting fileList's 'visible' buffer as %s", name.c_str());
    if(nameExists(name)) {
        currBuffer = name;
        return swBuffer.emit(name);
    }
    DLOG("Name does not exist! Not setting.");
    return false;
}

void fileList::deleteByName(Glib::ustring name) {
    // NOTE: this does not account for duplicates
    DLOG("Deleting buffer by name \"%s\"", name.c_str());
    for(auto x = buffers.begin(); x != buffers.end(); x++) {
        if(x->get()->getName() == name) {
            DLOG("Found buffer.");
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
    DLOG("Saving file \"%s\"...", this->name.c_str());
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
    char* contents;
    gsize amt;
    fileObj()->load_contents(contents, amt);
    DLOG("Reloading file \"%s\" with amt %lu...", this->name.c_str(), amt);
    buffer()->set_text(contents, contents + amt);
    g_free(contents);
    return true;
}
