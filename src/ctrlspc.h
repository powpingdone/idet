#ifndef CTRLSPC_H
#define CTRLSPC_H

#include <gtkmm.h>

class _cSV_SIItemFactory : public Gtk::SignalListItemFactory {
    public:
        _cSV_SIItemFactory() : Gtk::SignalListItemFactory() {;}
};

class _ctrlSpcView : public Gtk::Widget {
    public: 
        _ctrlSpcView();
    private:
        _cSV_SIItemFactory spcGenerator;
        Gtk::GridView ctrlSpcSelect;
};

#endif
