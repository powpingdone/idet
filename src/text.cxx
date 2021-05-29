#include "text.h"
#include <gtkmm.h>

// TODO: ADD ASYNC SUPPORT
// TODO: FILE PERMS

// fileList methods

size_t fileList::append(Glib::ustring name, Glib::ustring file, bool editable) {
    DLOG("Loading new buffer \"%s\"", name.c_str());
    auto x = Glib::RefPtr<ppdTextBuffer>(new ppdTextBuffer(name, file, editable));
    if(file != "") {
        x->reload();
    }
    x->reloadPromptSignal().connect(reloadSlot);
    size_t id = findLowestId();
    DLOG("ID assigned %lu", id);
    buffers.insert({id, x});
    return id;
}

size_t fileList::findLowestId() {
    size_t id;
    for(id = 1; buffers.find(id) != buffers.end(); id++)
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

// ppdTextBuffer methods

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
        DLOG("%s aleady exists here at %s!", Glib::file_test(fileName, Glib::FileTest::IS_DIR) ? "Directory" : "File",
            fileName.c_str());
        return !Glib::file_test(fileName, Glib::FileTest::IS_DIR); // return false if directory
    }

    file = Gio::File::create_for_path(fileName);
    DLOG("File created.");
    monitor = file->monitor();
    monitor->signal_changed().connect(sigc::mem_fun(*this, &ppdTextBuffer::reloadPromptCreate));
    DLOG("Monitor on!");
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
    char *contents;
    gsize amt;
    fileObj()->load_contents(contents, amt);
    DLOG("Reloading file \"%s\" with amt %lu...", this->name.c_str(), amt);
    buffer()->set_text(contents, contents + amt);
    g_free(contents);
    return true;
}

void ppdTextBuffer::reloadPromptCreate(
    const Glib::RefPtr<Gio::File> &file1, const Glib::RefPtr<Gio::File> &file2, Gio::FileMonitor::Event event) {
    tmpFiles.push_back(file1);
    tmpFiles.push_back(file2);
    tmpEvent = event;

}

void ppdTextBuffer::reloadPromptSend() {
    using Event = Gio::FileMonitor::Event;
    switch(tmpEvent) {
        case Event::CHANGED:
            {
                reloadPrompt.emit("File on disk changed. Reload?", true, &reloadPromptReceiveSlot);
                break;
            }
        case Event::DELETED:
            {
                reloadPrompt.emit("File on disk deleted. Unload buffer?", false, &reloadPromptReceiveSlot);
                break;
            }
        case Event::MOVED:
        case Event::RENAMED:
            {
                reloadPrompt.emit("File on disk moved/renamed. Change path?", true, &reloadPromptReceiveSlot);
                break;
            }
        default:
            {
                LOG("%d is the event caught. Not screwing with it.", tmpEvent);
                break;
            }
    }

}

void ppdTextBuffer::reloadPromptReceive(bool outcome) {
    if(!outcome) { // failed
        DLOG("Outcome refused!");
    } else {
        if(tmpFiles.at(1)) { // change to moved file
            this->name = tmpFiles.at(1)->get_basename();
            this->fileName = tmpFiles.at(1)->get_path();
            this->file = tmpFiles.at(1);
            this->monitor = this->file->monitor();
            this->monitor->signal_changed().connect(sigc::mem_fun(*this, &ppdTextBuffer::reloadPromptCreate));
        } else if(tmpEvent == Gio::FileMonitor::Event::DELETED) {
            
        } else { // event == CHANGED
            reload();
        }

    }
    tmpFiles.clear();
    pop.emit();
}
