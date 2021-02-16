#include "ctrlspc.h"
#include "common.h"

_ctrlSpcView::_ctrlSpcView() {
    // the whole reason why this is a subclass of Gtk::Widget
    auto kH = Gtk::EventControllerKey::create();
    kH->signal_key_pressed().connect(sigc::mem_fun(*this, &_ctrlSpcView::keyboardHandler), false);
    add_controller(kH);

    // setup flow container to use proper settings
    textContain.set_column_spacing(12);
    textContain.set_row_spacing(1);
    textContain.set_min_children_per_line(2);
    textContain.set_max_children_per_line(8);
    textContain.set_orientation(Gtk::Orientation::HORIZONTAL);
    textContain.set_hexpand();
    textContain.set_name("textContain");

    // setup container
    ctrlSpcSelect.attach(textContain, 0, 0);
    ctrlSpcSelect.set_hexpand(false);
    ctrlSpcSelect.set_halign(Gtk::Align::CENTER);
    ctrlSpcSelect.set_name("ctrlSpcSelect");
}

bool _ctrlSpcView::keyboardHandler(guint key, guint keycode, Gdk::ModifierType state) {
    return false;
}

void _ctrlSpcView::generate() {
    for(int x = 0; x< 10; x++){
        Gtk::Label xx;
        xx.set_label("test");
        textContain.insert(xx, -1);
    }
    h++;
}

bool _ctrlSpcView::add_action(std::string name, std::string keybind, bool (*func)(void*)) {
    return add_action(name, keybind, func, nullptr);
}

bool _ctrlSpcView::add_action(std::string name, std::string keybind, bool (*func)(void*), void *args) {
    if(!head) { // create action tree
        head = Glib::RefPtr<keyAction>(new keyAction());
        head->key = gdk_unicode_to_keyval(keybind[0]);
    }
    LOG("Creating %s, keybinding %s", name.c_str(), keybind.c_str());

    // actual action adding
    auto base = head, curr = head;
    for(unsigned int x = 0; keybind.length() > x; x++) {
        guint32 keycheck = gdk_unicode_to_keyval(keybind[x]);
        if(!curr) return false;
        while(curr) { // go through current (sub)tree
            if(curr->key == keycheck) {
                if(!curr->subKey && keybind.length() > x + 1) {
                    LOG("%c (%d) found, but no subKey allocated.", keybind[x], keycheck);
                    curr->subKey = Glib::RefPtr<keyAction>(new keyAction());
                    curr->subKey->key = gdk_unicode_to_keyval(keybind[x + 1]);
                    curr = curr->subKey;
                    break;
                }
                else if ((!curr->subKey && curr->action) || (curr->subKey && keybind.length() <= x + 1)) {
                    LOG("Unable to build keybind \"%s\": %s. Freeing args...", keybind.c_str(), curr->subKey ?
                            "last keybind char found, but was already allocated to a pattern" :
                            "at end of already made keybind chain, but keybind has action predefined");
                    free(args);
                    return false;
                }
            }
        }
    }
    return true;
}
