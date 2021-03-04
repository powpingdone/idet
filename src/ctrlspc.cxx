#include "ctrlspc.h"
#include "actions.h"
#include "common.h"

_ctrlSpcView::_ctrlSpcView() {
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

bool _ctrlSpcView::keyboardHandler(guint keyval, guint keycode, Gdk::ModifierType state) {
    if(!active)
        return false;
    
    if(treeptr->find(keyval) != treeptr->end()) { // if there is a valid key in the treeptr
        LOG("Caught action %d, ie '%c', which is in tree.", keyval, gdk_keyval_to_unicode(keyval));
        if(treeptr->at(keyval)->activateable()) 
            treeptr->at(keyval)->activate();
        
        if(treeptr->at(keyval)->category()) { // set up next tree
            LOG("Generating next tree.");
            treeptr = treeptr->at(keyval)->subKey();
        }
        else { // deactivate
            LOG("Hit endof tree, cleaning up");
            this->stop();
            treeptr = nullptr;
        }
        this->generate(); 
        return true;
    }
    return false;
}

void _ctrlSpcView::generate() {
    while(textContain.get_child_at_index(0)) { // while there are children
        textContain.remove(*(textContain.get_child_at_index(0)->get_child())); // remove them
    } // woow gtk how insensitive of you
    
    if(active) {
        if(!head) {
            LOG("Somehow attempting to generate with a NULL head! Not generating!");
            return;
        }
        
        if(!treeptr) // if keyboardHandler hasnt activated yet
            treeptr = head;
        auto node = *treeptr; 

        for(auto x : node) {
            Glib::ustring build;
            build.append("[");
            // note that is is possibly a temp fix
            // idk if this will work for actual unicode chars
            // WE WILL SEE, FOR NOW THIS WORKS
            build += (char)gdk_keyval_to_unicode(x.first);
            build.append("] ");
            build.append(x.second->getName());
            Gtk::Label newLabel;
            newLabel.set_label(build);
            textContain.insert(newLabel, -1);
        }
    }
}

bool _ctrlSpcView::add_action(std::vector<Glib::ustring> names, Glib::ustring keybind, Glib::RefPtr<Action> func) {
    return add_action(names, keybind, func, nullptr);
}

bool _ctrlSpcView::add_action(std::vector<Glib::ustring> names, Glib::ustring keybind, Glib::RefPtr<Action> func, Glib::RefPtr<void> args) {
    LOG("Creating %s, keybinding \"%s\"", names.at(names.size()-1).c_str(), keybind.c_str());
    
    // some error cases
    if(keybind.length() == 0) {
        LOG("Keybinding length is 0, which is impossible to make bindings for! Returning false.");
        return false;
    }

    if(keybind.length() < names.size()) {
        LOG("Keybinding size is less than names specified, returning false.");
        return false;
    }

    // pre-add operation offset
    int offset = 0;
    if(keybind.length() > names.size()) {
        // this usually means that less names were specified than were mapped to keybinds
        // thats fine, just offset the accessor
        offset = keybind.length() - names.size();
    }
    
    // head initalization
    if(head->empty()) { // oh the jokes we will say
        LOG("Head empty! Placing CategoryAction with name \"%s\" for init", names.at(0).c_str());
        // for this, we can just use zero 
        // because this is initalized by the native code,
        // not by any external sources
        guint keyval = gdk_unicode_to_keyval(keybind[0]);
        head->insert({keyval, Glib::RefPtr<Action>(new CategoryAction())});
        head->at(keyval)->setName(names.at(0));
    }
    
    // actual adding
    auto table = head;
    for(uint pos = 0; pos < keybind.length(); pos++) {
        guint keyval = gdk_unicode_to_keyval(keybind[pos]);
        if(table->find(keyval) != table->end()) { // if already in table 
            if(!table->at(keyval)->activateable()) { // and not activateable
                // then use that table
                table = table->at(keyval)->subKey();
                continue;
            }
            else { // it is activateable
                LOG("Keybinding \"%s\" at pos %d (aka '%c') already has activateable binding."
                        " Returning false.", keybind.c_str(), pos, keybind[pos]);
                return false;
            }
        }
        else { // it is not in table
            if(pos + 1 < keybind.length()) // if pos is not last, insert a category
                table->insert({keyval, Glib::RefPtr<Action>(new CategoryAction())});
            else { // insert the final specified function
                table->insert({keyval, func});
                table->at(keyval)->setArgs(args);
            }

            if((int)(pos)-offset >= 0) // set name if the offset is not negative
                table->at(keyval)->setName(names.at(pos-offset));
            else 
                LOG("Keybinding \"%s\" at pos %d (aka '%c') does not have a corresponding default name,"
                        " despite needing one! Amt of default names given is %zu, using \"\" instead.",
                        keybind.c_str(), pos, keybind[pos], names.size());
        }
        
        if(pos + 1 == keybind.length() && table->find(keyval) == table->end()) {// if last binding and not in table
            LOG("After iterating through keybinding \"%s\", "
                    "there are no more values to attempt to bind to. Returning false.", keybind.c_str());
            return false;
        }
    }

    return true;
}
