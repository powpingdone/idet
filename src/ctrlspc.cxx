#include "ctrlspc.h"
#include "gtkmm/constraint.h"
#include <memory>
#include <chrono>

_ctrlSpcView::_ctrlSpcView() {
    // the whole reason why this is a subclass of Gtk::Widget
    auto kH = Gtk::EventControllerKey::create();
    kH->signal_key_pressed().connect(sigc::mem_fun(*this, &_ctrlSpcView::keyboardHandler), false);
    add_controller(kH);

    // setup flow container to use proper settings
    textContain.set_column_spacing(12);
    textContain.set_row_spacing(1);
    textContain.set_min_children_per_line(15);
    textContain.set_max_children_per_line(15);
    textContain.set_orientation(Gtk::Orientation::HORIZONTAL);
    textContain.set_name("textContain");

    // setup container
    ctrlSpcSelect.attach(textContain, 0, 0);
    ctrlSpcSelect.set_name("ctrlSpcSelect");
}

bool _ctrlSpcView::keyboardHandler(guint key, guint keycode, Gdk::ModifierType state) {
    return false;
}

void _ctrlSpcView::generate() {
    for(int x = 0; x< 10; x++){
    Gtk::Label xx;
    xx.set_label("test");
    textContain.insert(xx,0);}
    h++;
}

