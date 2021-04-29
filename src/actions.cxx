#include "actions.h"
#include <gtkmm.h>

// defined to clean out "new 0/1" buffers
void cleanOutNewBuf(fileList *);

bool NewFile::action() {
    prompt.emit("New Filename:", &signalSlot);
    return true;
}

void NewFile::signal(Glib::ustring file) {
    Glib::ustring name(file);
    auto          x = name.end();
    // if this is a multi directory name, work backwards to the last slash
    while(x != name.begin() && (*(x.base()) != '\\' || *(x.base()) != '/')) x--;
    name = std::string(x, name.end());
    size_t id = files->append(name, file, true);
    files->setCurrentBuf(id);
    cleanOutNewBuf(files);
}

bool OpenFile::action() {
    auto dialog = new Gtk::FileChooserDialog("Open File", Gtk::FileChooser::Action::OPEN);
    // set mainWindow to have this as a linked window, basically tell it to stop taking input
    dialog->set_transient_for(*(getMainWindow.emit()));
    dialog->set_modal();
    dialog->signal_response().connect(sigc::bind(sigc::mem_fun(*this, &OpenFile::signal), dialog));
    dialog->add_button("_Ok", Gtk::ResponseType::OK);
    dialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
    dialog->show();
    DLOG("Opening FileOpenDialog");
    return true;
}

void OpenFile::signal(int response, Gtk::FileChooserDialog *dialog) {
    switch(response) {
        case Gtk::ResponseType::OK:
            {
                auto file = Glib::ustring(dialog->get_file()->get_path());
                LOG("Opening file \"%s\"", file.c_str());

                // but first, generate the name
                // basically, work backwards from the end
                // to the first / or \ and then use the offset
                std::string name, tmp(file);
                int         x = tmp.length() - 1;
                while(x >= 0 && tmp[x] != '/' && tmp[x] != '\\') x--;
                name = tmp.substr((int)tmp.length() > x ? x + 1 : 0);
                DLOG("Filename generated: \"%s\"", name.c_str());

                size_t id = files->append(name, file, true);
                if(!files->setCurrentBuf(id))
                    LOG("Failed to open file!");
                else {
                    cleanOutNewBuf(files);
                }
                break;
            }
        case Gtk::ResponseType::CANCEL:
        default:
            {
                LOG("Cancelled!");
            }
    }
    delete dialog;
}

void cleanOutNewBuf(fileList *files) {
    // if there is a new * buffer then delete it
    if(files->getAllNames().size() == 2) {
        auto names = files->getAllIDs();
        for(auto fID: names) {
            auto fname = files->getPPDTB(fID)->getName();
            if(fname == "new 0" || fname == "new 1") {
                files->deleteBufAt(fID);
                break;
            }
        }
    }
}

bool SaveFile::action() {
    return files->getPPDTB(files->getCurrBufferID())->save();
}

bool SwapFile::action() {
    return changeName.emit(id);
}

bool SwapFileFactory::action() {
    LOG("Generating SwapFiles to use!");
    std::vector<size_t> ids = allbufs->getAllIDs();
    subKey()->clear();
    for(auto id: ids) {
        auto  name = allbufs->getPPDTB(id)->getName();
        guint num = gdk_keyval_to_unicode(name[0]);
        // search for a non collision in the filename first
        for(int pos = 1; subKey()->find(num) != subKey()->end() && (int)name.length() > pos; pos++) {
            num = gdk_keyval_to_unicode(name[pos]);
        }
        if(subKey()->find(num) != subKey()->end()) {
            // search for a something to map to
            // TODO: make this more robust
            num = 0;
            std::string lookfor(
                "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,./<>?[]{}\\|-_+=`~!@#$%^&*()'\"");
            for(auto potent: lookfor) {
                if(subKey()->find(gdk_keyval_to_unicode(potent)) == subKey()->end()) {
                    num = gdk_keyval_to_unicode(potent);
                    break;
                }
            }
            if(num == 0) {
                LOG("Unable to find char to map to buffer \"%s\", not adding", name.c_str());
                continue;
            }
        }
        subKey()->insert({num, Glib::RefPtr<Action>(new SwapFile(functor, id, name))});
    }
    return true;
}

bool CloseFile::action() {
    size_t file = files->getCurrBufferID();
    auto   buffers = files->getAllIDs();
    LOG("Closing file %s", files->getCurrBufferName().c_str());
    if(buffers.size() == 1) { // if buffer to be closed is the last "normal" buffer
        DLOG("Opening blank \"new\" buffer in place");
        size_t id;
        if(files->getCurrBufferName() == "new 1") {
            id = files->append("new 0");
        } else {
            id = files->append("new 1");
        }
        files->setCurrentBuf(id);
        DLOG("\"%s\" is new blank buffer", files->getCurrBufferName().c_str());
    } else { // set current buff as something else
        // TODO: Use history of switching between buffers
        files->setCurrentBuf(file == buffers.at(0) ? buffers.at(1) : buffers.at(0));
        DLOG("Using \"%s\" as buffer after closing other buffer", files->getCurrBufferName().c_str());
    }
    files->deleteBufAt(file);
    return true;
}
