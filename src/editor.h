#ifndef EDITOR_H
#define EDITOR_H

#include "ctrlspc.h"
#include "text.h"
#include <gtkmm.h>

class mainWindow : public Gtk::Window {
    public:
    mainWindow();

    /* internal api calls for handling various things, in editor.cxx */
    public:
    void regenSCSignals() {
        // regenerate the signals when the buffer is changed
        sourceCode.get_buffer()->signal_changed().connect(sigc::mem_fun(*this, &mainWindow::updateLineNumbers));
        updateLineNumbers();
    }

    bool         swBuffer(Glib::ustring);
    Gtk::Window* selfReturn() { return (Gtk::Window*)this; }

    protected:
    void focusCtrlSpc() {
        ctrlSpc.set_visible(true);
        sourceLines.grab_focus();
    }
    void focusTextEditor() {
        ctrlSpc.set_visible(false);
        sourceCode.grab_focus();
    }

    void updateLineNumbers(); // update textview sourceLines with new line numbers
    void constrain(); // setup function that gives constraints to eWlayout
    bool keyboardHandler(guint, guint, Gdk::ModifierType); // handles keyboard events
    void generateCtrlSpc(); // signal handler for ctrlspc

    private:
    Gtk::Grid                           masterGrid; // contains rest of boxes
    Gtk::Grid                           textEditor; // contains textviews relating to actual code editing
    Gtk::ScrolledWindow                 sourceHolder; // scrolls through source code
    Gtk::TextView                       sourceCode; // actual code
    Gtk::TextView                       sourceLines; // line numbers
    _ctrlSpcView                        ctrlSpcView; // handler class for ctrlspc
    Gtk::Box                            ctrlSpc; // ctrlspc container
    int                                 lines = -1; // used by updateLineNumbers to check if line numbers changed
    fileList                            buffers; // all text buffers
    Glib::RefPtr<Gtk::ConstraintLayout> eSrcLayout
        = Gtk::ConstraintLayout::create(); // specific layout for line numbers + srccode

    /* external api calls for handling prompts and buffer stuff, each has a callback, in inputacts.cxx */
    public:
    void textPrompt(
        Glib::ustring, sigc::slot<void(Glib::ustring)>*); // prompt user input with a text entry (Gtk::entry)

    protected:
    void textPromptSignal(); // callback signal after user presses enter for textPrompt

    private:
    void*                userCallslot; // user function to be typically run after the pmode callback is run
    sigc::signal<void()> pmodeCallback; // signal to trigger on activation of pmode
};

#endif // EDITOR_H
