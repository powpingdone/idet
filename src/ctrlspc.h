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
        Gtk::FlowBox ctrlSpcSelect; // this is public so that it can be appended to the editor window
    protected:
        bool keyboardHandler(guint, guint, Gdk::ModifierType); // handle keyboard events for ctrlSpc
        void setupSLIF(const Glib::RefPtr<Gtk::ListItem>&);
        void bindSLIF(const Glib::RefPtr<Gtk::ListItem>&);
    private:
        Glib::RefPtr<keyAction> head, treeptr;
        bool active = false;
        int h=0;
};

#endif
