#ifndef CTRLSPC_H
#define CTRLSPC_H

#include <gtkmm.h>

class keyAction {
    public:
        guint key;
        Glib::RefPtr<keyAction> next = nullptr, // next potential key
            subKey = nullptr; // sub tree of keys
};

class _ctrlSpcView : public Gtk::Widget {
    public: 
        _ctrlSpcView();
        void start() {active = true;}
        void stop() {active = false;}
        bool isActive() {return active;}
        void generate(); // generate list values
        Gtk::Grid ctrlSpcSelect; // this is public so that it can be appended to the editor window
    protected:
        bool keyboardHandler(guint, guint, Gdk::ModifierType); // handle keyboard events for ctrlSpc
        void constrain();
    private:
        Glib::RefPtr<keyAction> head, treeptr;
        Glib::RefPtr<Gtk::ConstraintLayout> ctrlSpcRestrictor = Gtk::ConstraintLayout::create(); // restricts flowbox text explosions
        Gtk::FlowBox textContain; // actual text
        bool active = false;
        int h=0;
};

#endif
