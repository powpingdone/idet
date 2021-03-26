#include "text.h"
#include "glibmm/fileutils.h"
#include <gtkmm.h>

// TODO: ADD ASYNC SUPPORT
// TODO: FILE PERMS

size_t fileList::append(Glib::ustring name, Glib::ustring file, bool editable) {
    DLOG("Loading new buffer \"%s\"", name.c_str());
    auto x = Glib::RefPtr<ppdTextBuffer>(new ppdTextBuffer(name, file, editable));
    if(file != "") {
        x->reload();
    }
    size_t id = findLowestId();
    DLOG("ID assigned %lu", id);
    buffers.insert({id, x});
    return id;
}

size_t fileList::findLowestId() {
    size_t id;
    for(id = 0; buffers.find(id) != buffers.end(); id++)
        ;
    DLOG("Lowest ID avail %lu", id);
    return id;
}

Glib::RefPtr<Gtk::TextBuffer> fileList::getBuffer(size_t id) const {
    auto potentBuf = getPPDTB(id);
    return potentBuf ? potentBuf->buffer() : nullptr;
}

Glib::RefPtr<ppdTextBuffer> fileList::getPPDTB(size_t id) const {
    DLOG("Searching for ID %zu...", id);
    if(buffers.find(id) != buffers.end()) {
        DLOG("Found matching buffer.");
        return buffers.at(id);
    }
    DLOG("No matching buffer found");
    return nullptr;
}

bool fileList::setCurrentBuf(size_t id) {
    if(IDExists(id)) {
        DLOG("Setting fileList's 'visible' buffer as %s", buffers.at(id)->getName().c_str());
        currBuffer = getPPDTB(id);
        return swBufferID.emit(id);
    }
    DLOG("Name does not exist! Not setting.");
    return false;
}

void fileList::deleteBufAt(size_t id) {
    DLOG("Deleting buffer by ID %lu", id);
    if(buffers.find(id) != buffers.end()) {
        DLOG("Found buffer.");
        buffers.erase(id);
        return;
    }
}

std::vector<size_t> fileList::getAllIDs() const {
    std::vector<size_t> names;
    for(auto x: buffers) { names.push_back(x.first); }
    return names;
}

std::vector<Glib::ustring> fileList::getAllNames() const {
    std::vector<Glib::ustring> names;
    for(auto x: buffers) { names.push_back(x.second->getName()); }
    return names;
}

bool ppdTextBuffer::createFile() {
    if(file) {
        DLOG("File object already exists!");
        return true;
    }

    if(fileName == "") {
        DLOG("FileName is blank! Returning true.");
        return true;
    }

    if(Glib::file_test(fileName, Glib::FileTest::EXISTS)) {
        DLOG("%s aleady exists here!", Glib::file_test(fileName, Glib::FileTest::IS_DIR)? "Directory" : "File");
        return !Glib::file_test(fileName, Glib::FileTest::IS_DIR); // return false if directory
    }

    file = Gio::File::create_for_path(fileName);
    DLOG("File created.");
    return save(); 
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
