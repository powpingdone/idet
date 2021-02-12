#ifndef EDITOR_H
#define EDITOR_H

#include "ctrlspc.h"
#include <gtkmm.h>

class mainWindow : public Gtk::Window {
    public:
        mainWindow();
        bool isCtrlSpc() {return ctrlSpcUp;}

    protected:
        void updateLineNumbers(); // update textview sourceLines with new line numbers
        void constrain(); // setup function that gives constraints to eWlayout
        bool keyboardHandler(guint, guint, Gdk::ModifierType); // handles keyboard events
        void generateCtrlSpc(); // signal handler for ctrlspc

    private:
        Gtk::Grid masterGrid, // contains rest of boxes
            textEditor; // contains textviews relating to actual code editing
        Gtk::ScrolledWindow sourceHolder; // scrolls through source code
        Gtk::TextView sourceCode, // actual code
            sourceLines; // line numbers
        _ctrlSpcView ctrlSpcView;
        Gtk::ScrolledWindow ctrlSpc;
        bool ctrlSpcUp = false;
        Glib::RefPtr<Gtk::ConstraintLayout> eSrcLayout = Gtk::ConstraintLayout::create(); // specific layout for line numbers + srccode
        int lines = -1; // used by updateLineNumbers
};

#endif //EDITOR_H
