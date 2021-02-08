#include "ctrlspc.h"
#include "gtkmm/label.h"
#include <memory>

_ctrlSpcView::_ctrlSpcView() {
    // the whole reason why this is a subclass of Gtk::Widget
    auto kH = Gtk::EventControllerKey::create();
    kH->signal_key_pressed().connect(sigc::mem_fun(*this, &_ctrlSpcView::keyboardHandler), false);
    add_controller(kH);
    ctrlSpcSelect.set_column_spacing(12);
    ctrlSpcSelect.set_min_children_per_line(4);
}

bool _ctrlSpcView::keyboardHandler(guint key, guint keycode, Gdk::ModifierType state) {
    return false;
}

void _ctrlSpcView::generate() {
    Gtk::Label xx;
    xx.set_label("test");
    ctrlSpcSelect.insert(xx,-1);
    h++;
}


