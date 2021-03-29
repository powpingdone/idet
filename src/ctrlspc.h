#ifndef CTRLSPC_H
#define CTRLSPC_H

#include "actions.h"
#include <gtkmm.h>

class ctrlSpcHandler {
    public:
    ctrlSpcHandler();
    bool add_action(std::vector<Glib::ustring>, Glib::ustring, Glib::RefPtr<Action>, Glib::RefPtr<void>);
    bool add_action(std::vector<Glib::ustring>, Glib::ustring, Glib::RefPtr<Action>);
    void generate(); // generate list values
    bool keyboardHandler(guint, guint, Gdk::ModifierType); // handle keyboard events for ctrlSpc
    bool swapNextTree(); // use internal cached value
    bool swapNextTree(guint); // check if category and not pmode and switch to next tree
    void useCustomTree(std::vector<Glib::RefPtr<Gtk::Widget>>); // use customtree for pmode
    std::vector<Gtk::Widget*> giveCustomTree(); // get custom tree for pmode

    bool isActive() const { return active; }
    bool isPMode() const { return pmode; }
    void start() {
        active = true;
        DLOG("ctrlSpcView is now active");
    }
    void stop() {
        active = false;
        treeptr = nullptr;
        extermChildren();
        DLOG("ctrlSpcView is now *not* active");
    }
    void promptModeOn() {
        pmode = true;
        DLOG("PMODE active");
    }
    void promptModeOff() {
        pmode = false;
        DLOG("PMODE *not* active");
    }
    void extermChildren() {
        while(textContain.get_child_at_index(0)) { // while there are children
            textContain.remove(*(textContain.get_child_at_index(0)->get_child())); // remove them
        } // woow gtk how insensitive of you
    }

    Gtk::Grid ctrlSpcSelect; // this is public so that it can be appended to the editor window
    private:
    Glib::RefPtr<std::unordered_map<guint, Glib::RefPtr<Action>>> head
        = Glib::RefPtr<std::unordered_map<guint, Glib::RefPtr<Action>>>(
            new std::unordered_map<guint, Glib::RefPtr<Action>>()),
        treeptr = nullptr;
    Gtk::FlowBox textContain; // actual text
    bool         active = false, pmode = false;
    guint        cachedKeyval = 0; // internal cached value for swapNextTree
};

#endif
