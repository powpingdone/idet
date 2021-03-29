#include "editor.h"

void mainWindow::textPrompt(Glib::ustring prompt, sigc::slot<void(Glib::ustring)> *callback) {
    Glib::RefPtr<Gtk::Label> label(new Gtk::Label(prompt)),
        // hack to get around various fun things concerning focus
        toolTip(new Gtk::Label("Tip: Press Shift+Enter to enter your prompt"));
    Glib::RefPtr<Gtk::Entry>               entry(new Gtk::Entry());
    std::vector<Glib::RefPtr<Gtk::Widget>> tree({label, entry, toolTip});

    this->userCallslot = reinterpret_cast<void *>(callback);
    this->pmodeCallback.connect(sigc::mem_fun(*this, &mainWindow::textPromptSignal));
    ctrlSpcView.promptModeOn();
    ctrlSpcView.useCustomTree(tree);
    entry->grab_focus_without_selecting();
    DLOG("\"%s\" ::: Prompting text.", prompt.c_str());
}

void mainWindow::textPromptSignal() {
    if(!userCallslot) {
        LOG("callback is nullptr! Not doing signal.");
    } else {
        sigc::signal<void(Glib::ustring)> sig;
        auto functor = reinterpret_cast<sigc::slot<void(Glib::ustring)> *>(this->userCallslot);
        sig.connect(*functor);
        auto widgets = ctrlSpcView.giveCustomTree();
        // hack to get around casting problems
        // Gtk::Widget* -> GtkWidget* -> GtkEntry* -> Gtk::Entry*
        // if there is an official way to do this, I wouldnt know lmao
        Gtk::Entry *entry = Glib::wrap(GTK_ENTRY(widgets.at(1)->gobj()));
        DLOG("Got %s from buffer", entry->get_buffer()->get_text().c_str());
        sig.emit(entry->get_buffer()->get_text());
    }
    // cleanup
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
