#ifndef CTRLSPC_H
#define CTRLSPC_H

#include <gtkmm.h>
#include <functional>
#include <algorithm>

class keyAction {
    public:
        guint key;
        Glib::RefPtr<keyAction> next = nullptr, // next potential key
            subKey = nullptr; // sub tree of keys
        Glib::ustring name;
        bool (*action)(void*) = nullptr;
        void *args = nullptr;
};

class _ctrlSpcView : public Gtk::Widget {
    public: 
        _ctrlSpcView();
        bool add_action(std::wstring, std::wstring, bool (*func)(void*), void *); 
        bool add_action(std::wstring, std::wstring, bool (*func)(void*)); 
        void start() {active = true;}
        void stop() {active = false;}
        bool isActive() {return active;}
        void generate(); // generate list values
        Gtk::Grid ctrlSpcSelect; // this is public so that it can be appended to the editor window
    protected:
        bool keyboardHandler(guint, guint, Gdk::ModifierType); // handle keyboard events for ctrlSpc
        void constrain();
    private:
        Glib::RefPtr<keyAction> head = nullptr, treeptr = nullptr;
        Glib::RefPtr<Gtk::ConstraintLayout> ctrlSpcRestrictor = Gtk::ConstraintLayout::create(); // restricts flowbox text explosions
        Gtk::FlowBox textContain; // actual text
        bool active = false;
        int h=0;
};

#endif
