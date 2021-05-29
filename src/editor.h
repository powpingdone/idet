#ifndef EDITOR_H
#define EDITOR_H

#include "actions.h"
#include "ctrlspc.h"
#include "text.h"
#include <gtkmm.h>
#include <queue>

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

    bool         swBufferByID(size_t); // used to swap to text buffers in buffers
    Gtk::Window* selfReturn() { return (Gtk::Window*)this; } // transient windows

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
    bool swBuffer(Glib::RefPtr<Gtk::TextBuffer>); // change the currently viewed buffer directly

    private:
    Gtk::Grid           masterGrid; // contains rest of boxes
    Gtk::Grid           textEditor; // contains textviews relating to actual code editing
    Gtk::ScrolledWindow sourceHolder; // scrolls through source code
    Gtk::TextView       sourceCode; // actual code
    Gtk::TextView       sourceLines; // line numbers
    Gtk::Box            ctrlSpc; // ctrlspc container

    ctrlSpcHandler ctrlSpcView; // handler class for ctrlspc
    int            lines = -1; // used by updateLineNumbers to check if line numbers changed
    fileList       buffers; // all text buffers

    Glib::RefPtr<Gtk::ConstraintLayout> eSrcLayout
        = Gtk::ConstraintLayout::create(); // specific layout for line numbers + srccode

    /* external api calls for handling prompts and buffer stuff, each has a callback, in inputacts.cxx */
    public:
    void textPrompt(
        Glib::ustring, sigc::slot<void(Glib::ustring)>*); // prompt user input with a text entry (Gtk::entry)
    void promptYesNo(Glib::ustring, bool, sigc::slot<void(bool)>*); // prompt user with yes/no question

    void queueAction(Action); //queue an action to happen
    bool popAction(); // do next action, returns false when there are no actions

    protected:
    bool textPromptSignal(guint, guint, Gdk::ModifierType); // callback signal after user presses enter for textPrompt
    bool promptYesNoSignal(guint, guint, Gdk::ModifierType); // callback signal for promptYesNo

    bool checkCallslot(); // utility function to check if the callslot is there 
    bool checkKey(guint, std::vector<guint>); // utility function to check if a key is a certain key 
    void cleanupPostSignal(); // utility function to generically cleanup action stuff

    private:
    void* userCallslot; // user function to be typically run after the pmode callback is run
    sigc::signal<bool(guint, guint, Gdk::ModifierType)> pmodeCallback; // signal to trigger on activation of pmode
    std::queue<Action> actionAbleQueue;
};

#endif // EDITOR_H
