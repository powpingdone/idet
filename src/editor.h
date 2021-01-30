#ifndef EDITOR_H
#define EDITOR_H

#include <gtkmm/enums.h>
#include <gtkmm/window.h>
#include <gtkmm/textview.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/grid.h>
#include <gtkmm/paned.h>

class editorWindow : public Gtk::Window {
    public:
        editorWindow();
    protected:
        void updateLineNumbers();


    private:
        Gtk::Grid masterGrid; // contains rest of boxes
        Gtk::Paned slaveEditor; // contains textviews relating to actual code editing
        Gtk::ScrolledWindow sourceHolder; // scrolls through source code
        Gtk::TextView sourceCode, // actual code
            sourceLines; // line numbers
        int lines = -1; // used by updateLineNumbers
};

#endif //EDITOR_H
