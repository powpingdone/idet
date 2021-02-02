#ifndef EDITOR_H
#define EDITOR_H

#include <gtkmm.h>

class _eWLayout : public Gtk::ConstraintLayout {
    public:
        _eWLayout() : Gtk::ConstraintLayout() {;}
};

class editorWindow : public Gtk::Window {
    public:
        editorWindow();
    protected:
        void updateLineNumbers();
        void constrain();


    private:
        Gtk::Grid masterGrid, // contains rest of boxes
            slaveEditor; // contains textviews relating to actual code editing
        Gtk::ScrolledWindow sourceHolder; // scrolls through source code
        Gtk::TextView sourceCode; // actual code
        Gtk::TextView sourceLines;
        Glib::RefPtr<_eWLayout> eWLayout = Glib::RefPtr<_eWLayout>(new _eWLayout());
        int lines = -1; // used by updateLineNumbers
};

#endif //EDITOR_H
