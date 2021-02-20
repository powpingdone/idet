#ifndef CTRLSPC_H
#define CTRLSPC_H

#include <gtkmm.h>
#include "actions.h"
#include "glibmm/refptr.h"

class _ctrlSpcView {
    public: 
        _ctrlSpcView();
        bool add_action(std::vector<Glib::ustring>, Glib::ustring, Glib::RefPtr<Action>, Glib::RefPtr<void>);
        bool add_action(std::vector<Glib::ustring>, Glib::ustring, Glib::RefPtr<Action>);
        void generate(); // generate list values
        bool keyboardHandler(guint, guint, Gdk::ModifierType); // handle keyboard events for ctrlSpc
        
        void start()    {active = true;}
        void stop()     {active = false; treeptr = nullptr;}
        bool isActive() {return active;}
        
        Gtk::Grid ctrlSpcSelect; // this is public so that it can be appended to the editor window
    private:
        Glib::RefPtr<std::unordered_map<guint, Glib::RefPtr<Action>>> 
            head = Glib::RefPtr<std::unordered_map<guint, Glib::RefPtr<Action>>>(new std::unordered_map<guint, Glib::RefPtr<Action>>()),
            treeptr = nullptr;
        Gtk::FlowBox textContain; // actual text
        bool active = false;
};

#endif
