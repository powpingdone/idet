#include "actions.h"
#include <gtkmm.h>

bool SaveFile::action() {
    return lis->getPPDTBByName(lis->getCurrBuffer())->save();
}

bool SwapFile::action() {
    return sig.emit(name);
}

bool SwapFileFactory::action() {
    LOG("Generating SwapFiles to use!");
    std::vector<Glib::ustring> names = getNames.emit();
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
            std::string lookfor("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,./<>?[]{}\\|-_+=`~!@#$%^&*()'\"");
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
