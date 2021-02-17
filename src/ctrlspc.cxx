#include "ctrlspc.h"
#include "common.h"
#include <sstream>

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
    textContain.set_selection_mode(Gtk::SelectionMode::NONE);
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
    while(textContain.get_child_at_index(0)) { // while there are children
        textContain.remove(*(textContain.get_child_at_index(0)->get_child())); // remove them
    } // woow gtk how insensitive of you
    
    if(!head) {
        LOG("Somehow attemting to generate with a empty head! Not generating!");
        return;
    }

    if(treeptr) { // keyboardHandler will select the correct thing to use
        auto node = treeptr;
        while(node) {
            Glib::ustring build;// = "[" + (wchar_t)gdk_keyval_to_unicode(node->key) + "] " + node->name;
            build.append("[");
            build.append(gdk_keyval_to_unicode(node->key));
            Gtk::Label newLabel;
            newLabel.set_label(build);
            node = node->next;
        }
    }
}

bool _ctrlSpcView::add_action(std::wstring name, std::wstring keybind, bool (*func)(void*)) {
    return add_action(name, keybind, func, nullptr);
}

bool _ctrlSpcView::add_action(std::wstring name, std::wstring keybind, bool (*func)(void*), void *args) {
    if(!head) { // create action tree
        head = Glib::RefPtr<keyAction>(new keyAction());
        head->key = gdk_unicode_to_keyval(keybind[0]);
    }
    LOG("Creating %ls, keybinding %ls", name.c_str(), keybind.c_str());

    // actual action adding
    auto base = head, curr = head;
    for(unsigned int x = 0; keybind.length() > x; x++) {
        guint32 keycheck = gdk_unicode_to_keyval(keybind[x]);
        if(!curr) {
            LOG("Unable to build keybind \"%ls\": current node does not exist!", keybind.c_str());
            return false;
        }
        while(curr) { // go through current (sub)tree
            if(curr->key == keycheck) {
                if(!curr->subKey && keybind.length() > x + 1) {
                    LOG("%c (%d) found, but no subKey allocated.", keybind[x], keycheck);
                    curr->subKey = Glib::RefPtr<keyAction>(new keyAction());
                    curr->subKey->key = gdk_unicode_to_keyval(keybind[x + 1]);
                    curr = curr->subKey;
                    break;
                }
                else if((!curr->subKey && curr->action) || (curr->subKey && keybind.length() <= x + 1)) {
                    LOG("Unable to build keybind \"%ls\": %s. Freeing args...", keybind.c_str(), (!curr->subKey) ?
                            "at end of already made keybind chain, but keybind has action predefined" :
                            "last keybind char found, but was already allocated to a pattern"); 
                    free(args);
                    return false;
                }
            }
            else if((curr->next && gdk_keyval_to_unicode(curr->next->key) > keybind[x]) || !curr->next) {
                LOG("%c (%d) not found, and %s. Allocating.", keybind[x], keycheck, curr->next ? 
                        "next potential keybind is greater than the current keybind" :
                        "there are no more nodes in the list");
                curr->next = Glib::RefPtr<keyAction>(new keyAction());
                curr->next->key = keycheck;
            }
            curr = curr->next;
        }
    }
    return true;
}
