#include "ctrlspc.h"
#include "actions.h"
#include "common.h"

ctrlSpcHandler::ctrlSpcHandler() {
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

void ctrlSpcHandler::useCustomTree(std::vector<Glib::RefPtr<Gtk::Widget>> widgets) {
    if(!pmode) {
        DLOG("PMODE is not on, not ingesting widgets.");
        return;
    }
    extermChildren();
    for(auto widget: widgets) { textContain.insert(*widget, -1); }
}

std::vector<Gtk::Widget*> ctrlSpcHandler::giveCustomTree() {
    std::vector<Gtk::Widget*> ret;
    if(!pmode) {
        DLOG("PMODE is not on, not giving widgets out.");
        return ret;
    }
    for(int index = 0; textContain.get_child_at_index(index); index++)
        ret.push_back(textContain.get_child_at_index(index)->get_child());
    return ret;
}

bool ctrlSpcHandler::keyboardHandler(guint keyval, guint keycode, Gdk::ModifierType state) {
    if(!active || pmode)
        return false;

    if(treeptr->find(keyval) != treeptr->end()) { // if there is a valid key in the treeptr
        DLOG("Caught action %d, ie '%c', which is in tree.", keyval, gdk_keyval_to_unicode(keyval));
        if(treeptr->at(keyval)->activateable())
            treeptr->at(keyval)->activate();

        if(!swapNextTree(keyval)) { // set up next tree
            if(pmode) { // promptmode
                DLOG("Entered Prompt mode during tree traversal, most likely due to action");
                this->cachedKeyval = keyval;
                return true;
            } else { // deactivate
                LOG("Hit endof tree, cleaning up");
                this->stop();
            }
        }
        return true;
    }
    return false;
}

bool ctrlSpcHandler::swapNextTree() {
    return swapNextTree(this->cachedKeyval);
}

bool ctrlSpcHandler::swapNextTree(guint keyval) {
    if(treeptr && !pmode && treeptr->at(keyval)->category()) {
        DLOG("Setting next tree.");
        treeptr = treeptr->at(keyval)->subKey();
        this->generate();
        return true;
    }
    DLOG("Did not find category to swap to. PMODE is %s, and treeptr is %s.", pmode ? "on" : "off",
        treeptr ? "not null" : "null");
    return false;
}

void ctrlSpcHandler::generate() {
    extermChildren();

    if(active && !pmode) {
        if(!head) {
            DLOG("Somehow attempting to generate with a NULL head! Not generating!");
            return;
        }
        DLOG("(re)generating!");

        if(!treeptr) { // if keyboardHandler hasnt activated yet
            DLOG("Using head.");
            treeptr = head;
        }
        auto node = *treeptr;

        for(auto x: node) {
            Glib::ustring build;
            build.append("[");
            // note that is is possibly a temp fix
            // idk if this will work for actual unicode chars
            // WE WILL SEE, FOR NOW THIS WORKS
            build += (char)gdk_keyval_to_unicode(x.first);
            build.append("] ");
            build.append(x.second->getName());
            DLOG("Label \"%s\" created", build.c_str());

            Gtk::Label newLabel;
            newLabel.set_label(build);
            textContain.insert(newLabel, -1);
        }
    }
}

bool ctrlSpcHandler::add_action(std::vector<Glib::ustring> names, Glib::ustring keybind, Glib::RefPtr<Action> func) {
    return add_action(names, keybind, func, nullptr);
}

bool ctrlSpcHandler::add_action(
    std::vector<Glib::ustring> names, Glib::ustring keybind, Glib::RefPtr<Action> func, Glib::RefPtr<void> args) {
    DLOG("Creating %s, keybinding \"%s\"", names.at(names.size() - 1).c_str(), keybind.c_str());

    // some error cases
    if(keybind.length() == 0) {
        DLOG("Keybinding length is 0, which is impossible to make bindings for! Returning false.");
        return false;
    }

    if(keybind.length() < names.size()) {
        DLOG("Keybinding size is less than names specified, returning false.");
        return false;
    }

    // pre-add operation offset
    int offset = 0;
    if(keybind.length() > names.size()) {
        // this usually means that less names were specified than were mapped to keybinds
        // thats fine, just offset the accessor
        offset = keybind.length() - names.size();
        DLOG("Less names used than keybinds found, offset is %d for keybind len %lu", offset, keybind.length());
    }

    // head initalization
    if(head->empty()) { // oh the jokes we will say
        DLOG("Head empty! Placing CategoryAction with name \"%s\" for init", names.at(0).c_str());
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
            } else { // it is activateable
                DLOG("Keybinding \"%s\" at pos %d (aka '%c') already has activateable binding."
                     " Returning false.",
                    keybind.c_str(), pos, keybind[pos]);
                return false;
            }
        } else { // it is not in table
            if(pos + 1 < keybind.length()) // if pos is not last, insert a category
                table->insert({keyval, Glib::RefPtr<Action>(new CategoryAction())});
            else { // insert the final specified function
                table->insert({keyval, func});
                table->at(keyval)->setArgs(args);
            }

            if((int)(pos)-offset >= 0) // set name if the offset is not negative
                table->at(keyval)->setName(names.at(pos - offset));
            else {
                DLOG("Keybinding \"%s\" at pos %d (aka '%c') does not have a corresponding default name,"
                     " despite needing one! Amt of default names given is %zu, using \"\" instead.",
                    keybind.c_str(), pos, keybind[pos], names.size());
                table->at(keyval)->setName("");
            }
        }

        if(pos + 1 == keybind.length() && table->find(keyval) == table->end()) { // if last binding and not in table
            DLOG("After iterating through keybinding \"%s\", "
                 "there are no more values to attempt to bind to. Returning false.",
                keybind.c_str());
            return false;
        }
    }

    return true;
}
