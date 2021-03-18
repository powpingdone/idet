#include "actions.h"
#include <gtkmm.h>

bool OpenFile::action() {
    auto dialog = new Gtk::FileChooserDialog("Open File", Gtk::FileChooser::Action::OPEN);
    dialog->set_transient_for(*getMainWindow.emit());
    dialog->set_modal();
    dialog->signal_response().connect(sigc::bind(sigc::mem_fun(*this, &OpenFile::signal), dialog));
    dialog->add_button("_Ok", Gtk::ResponseType::OK);
    dialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
    dialog->show();
    return true;
}

void OpenFile::signal(int response, Gtk::FileChooserDialog *dialog) {
    switch(response) {
        case Gtk::ResponseType::OK: {
            auto file = Glib::ustring(dialog->get_file()->get_path());
            LOG("Opening file \"%s\"", file.c_str());

            // but first, generate the name
            std::string name, tmp(file);
            int         x = tmp.length() - 1;
            while(x >= 0 && tmp[x] != '/' && tmp[x] != '\\') x--;
            name = tmp.substr((int)tmp.length() > x ? x + 1 : 0);

            files->append(name, file, true);
            if(!files->setCurrentBufByName(name))
                LOG("Failed to open file!");
            else {
                // if there is a new * buffer then delete it
                if(files->getAllNames().size() == 2) {
                    auto names = files->getAllNames();
                    for(auto fname: names) {
                        if(fname == "new 0" || fname == "new 1") {
                            files->deleteByName(fname);
                            break;
                        }
                    }
                }
            }
            break;
        }
        case Gtk::ResponseType::CANCEL:
        default: {
            LOG("Cancelled!");
        }
    }
    delete dialog;
}

bool SaveFile::action() {
    return lis->getPPDTBByName(lis->getCurrBuffer())->save();
}

bool SwapFile::action() {
    return changeName.emit(name);
}

bool SwapFileFactory::action() {
    LOG("Generating SwapFiles to use!");
    std::vector<Glib::ustring> names = allbufs->getAllNames();
    subKey()->clear();
    for(auto name: names) {
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
        subKey()->insert({num, Glib::RefPtr<Action>(new SwapFile(functor, name))});
    }
    return true;
}

bool CloseFile::action() {
    Glib::ustring file = lis->getCurrBuffer();
    auto          buffers = lis->getAllNames();
    LOG("Closing file %s", file.c_str());
    if(buffers.size() == 1) {
        if(buffers.at(0) == "new 1") {
            lis->append("new 0");
            lis->setCurrentBufByName("new 0");
        } else {
            lis->append("new 1");
            lis->setCurrentBufByName("new 1");
        }
    } else {
        lis->setCurrentBufByName(buffers.at(0) == file ? buffers.at(1) : buffers.at(0));
    }
    lis->deleteByName(file);
    return true;
}
