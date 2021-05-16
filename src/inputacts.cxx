#include "editor.h"
#include <vector>

bool mainWindow::checkCallslot() {
    if(userCallslot) // safety check, also quick check
        return true;
    LOG("callback is nullptr! Not doing signal, deleting self and deactivating pmode!");
    pmodeCallback.clear();
    ctrlSpcView.promptModeOff();
    ctrlSpcView.stop();
    focusTextEditor();
    return false;
}

bool mainWindow::checkKey(guint keyval, std::vector<guint> vals) {
    for(guint val: vals)
        if(keyval == val)
            return true;
    DLOG("This is not my job!");
    return false;
}

void mainWindow::cleanupPostSignal() {
    this->userCallslot = nullptr;
    this->pmodeCallback.clear();
    ctrlSpcView.promptModeOff();
    if(!ctrlSpcView.swapNextTree()) {
        ctrlSpcView.stop();
        focusTextEditor();
    } else {
        focusCtrlSpc();
    }
}

void mainWindow::textPrompt(Glib::ustring prompt, sigc::slot<void(Glib::ustring)> *callback) {
    DLOG("\"%s\" ::: Prompting text.", prompt.c_str());
    Glib::RefPtr<Gtk::Label> label(new Gtk::Label(prompt)),
        // hack to get around various fun things concerning focus
        // tbh? I wouldnt do this if i could figure out signal_editing_done()
        toolTip(new Gtk::Label("Tip: Press Shift+Enter to enter your prompt"));
    Glib::RefPtr<Gtk::Entry>               entry(new Gtk::Entry());
    std::vector<Glib::RefPtr<Gtk::Widget>> tree({label, entry, toolTip});

    this->userCallslot = reinterpret_cast<void *>(callback);
    this->pmodeCallback.connect(sigc::mem_fun(*this, &mainWindow::textPromptSignal));
    ctrlSpcView.promptModeOn();
    ctrlSpcView.useCustomTree(tree);
    entry->grab_focus_without_selecting();
}

bool mainWindow::textPromptSignal(guint keyval, guint keycode, Gdk::ModifierType state) {
    if(!(checkCallslot() && checkKey(keyval, {GDK_KEY_Return}))) {
        return false;
    }

    DLOG("This is my job!");
    sigc::signal<void(Glib::ustring)> sig;
    auto                              functor = reinterpret_cast<sigc::slot<void(Glib::ustring)> *>(this->userCallslot);
    sig.connect(*functor);
    auto widgets = ctrlSpcView.giveCustomTree();
    Gtk::Entry *entry = dynamic_cast<Gtk::Entry *>(widgets[1]);
    DLOG("Got %s from buffer", entry->get_buffer()->get_text().c_str());
    sig.emit(entry->get_buffer()->get_text());
    
    cleanupPostSignal();
    return true;
}

void mainWindow::promptYesNo(Glib::ustring prompt, bool defaultOption, sigc::slot<void(bool)> *callback) {
    DLOG("\"%s\" ::: Prompting YES/NO with default '%c'", prompt.c_str(), defaultOption ? 'y' : 'n');
    Glib::RefPtr<Gtk::Label>               label(new Gtk::Label(prompt.append(defaultOption ? " (Y/n)" : " (y/N)")));
    std::vector<Glib::RefPtr<Gtk::Widget>> tree({label});

    this->userCallslot = reinterpret_cast<void *>(callback);
    this->pmodeCallback.connect(sigc::mem_fun(*this, &mainWindow::promptYesNoSignal));
    ctrlSpcView.promptModeOn();
    ctrlSpcView.useCustomTree(tree);
}

bool mainWindow::promptYesNoSignal(guint keyval, guint keycode, Gdk::ModifierType state) {
    if(!(checkCallslot() && checkKey(keyval, {GDK_KEY_Return, GDK_KEY_y, GDK_KEY_Y, GDK_KEY_n, GDK_KEY_N}))) {
        return false;
    }

    DLOG("This is my job!");
    sigc::signal<void(bool)> sig;
    auto                     functor = reinterpret_cast<sigc::slot<void(bool)> *>(userCallslot);
    sig.connect(*functor);
    Gtk::Label *prompt = dynamic_cast<Gtk::Label *>(ctrlSpcView.giveCustomTree()[0]);
    if(keyval == GDK_KEY_Return)
        // use the default option
        // tldr: compare if the N is capitalized or not for making a decision if defaultOption was set to true
        sig.emit(prompt->get_text().at(prompt->get_text().length() - 3) != 'N');
    else
        sig.emit(keyval == GDK_KEY_y || keyval == GDK_KEY_Y);
    
    cleanupPostSignal();
    return true;
}
