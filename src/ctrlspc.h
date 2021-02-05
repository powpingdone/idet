#ifndef CTRLSPC_H
#define CTRLSPC_H

#include "glibmm/refptr.h"
#include "gtkmm/listitem.h"
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
        Gtk::GridView ctrlSpcSelect; // this is public so that it can be appended to the editor window
    protected:
        bool keyboardHandler(guint, guint, Gdk::ModifierType); // handle keyboard events for ctrlSpc
        void setupSLIF(const Glib::RefPtr<Gtk::ListItem>&);
        void bindSLIF(const Glib::RefPtr<Gtk::ListItem>&);
    private:
        Glib::RefPtr<keyAction> head, treeptr;
        Glib::RefPtr<Gtk::SignalListItemFactory> ctrlSpcSLIF = Gtk::SignalListItemFactory::create();
        bool active = false;
};

#endif
