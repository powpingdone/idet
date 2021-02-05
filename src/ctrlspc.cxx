#include "ctrlspc.h"
#include <memory>

_ctrlSpcView::_ctrlSpcView() {
    auto kH = Gtk::EventControllerKey::create();
    kH->signal_key_pressed().connect(sigc::mem_fun(*this, &_ctrlSpcView::keyboardHandler), false);
    add_controller(kH);

    // setup list factory
    ctrlSpcSLIF->signal_setup().connect(sigc::mem_fun(*this, &_ctrlSpcView::setupSLIF));
    ctrlSpcSLIF->signal_bind().connect(sigc::mem_fun(*this, &_ctrlSpcView::bindSLIF));
}

bool _ctrlSpcView::keyboardHandler(guint key, guint keycode, Gdk::ModifierType state) {
    return false;
}

void _ctrlSpcView::generate() {

}

void _ctrlSpcView::setupSLIF(const Glib::RefPtr<Gtk::ListItem> &item) {
    auto label = Gtk::Label();
    item->set_child(label);
}

void _ctrlSpcView::bindSLIF(const Glib::RefPtr<Gtk::ListItem> &item) {
    std::dynamic_pointer_cast<Gtk::Label>(item->get_item())->set_label("asdf");
}

