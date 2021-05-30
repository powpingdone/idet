#ifndef ACTIONS_H
#define ACTIONS_H

#include "baseaction.h"
#include "common.h"
#include "text.h"
#include <gtkmm.h>

/*
 * REGULAR ACTIONS
 * */

// NewFile: action to create a new buffer
/* CALL ORDER:
 * action() -> mainWindow::textPrompt() -> wait for gtk to signal (keyboard event)
 * -> gtk signals -> mainWindow::textPromptSignal() -> signal() */
class NewFile : public Action {
    public:
    NewFile(fileList *files, sigc::slot<void(Glib::ustring, sigc::slot<void(Glib::ustring)> *)> prompt) {
        active = true;
        dir = false;
        this->files = files;
        this->signalSlot = sigc::mem_fun(*this, &NewFile::signal);
        this->prompt.connect(prompt);
    }
    bool action();
    void signal(Glib::ustring);

    private:
    fileList *                                                           files;
    sigc::signal<void(Glib::ustring, sigc::slot<void(Glib::ustring)> *)> prompt;
    sigc::slot<void(Glib::ustring)>                                      signalSlot;
};

// OpenFile: action to open a file
/* CALL ORDER:
 * action() -> Gtk::FileChooserDialog waiting -> signal()*/
class OpenFile : public Action {
    public:
    OpenFile(fileList *win, sigc::slot<Gtk::Window *()> func) {
        active = true;
        dir = false;
        this->files = win;
        getMainWindow.connect(func);
    }
    bool action();

    protected:
    void signal(int, Gtk::FileChooserDialog *);

    private:
    fileList *                    files;
    sigc::signal<Gtk::Window *()> getMainWindow;
};

// SaveFile: saves current file
class SaveFile : public Action {
    public:
    SaveFile(fileList *files) {
        active = true;
        dir = false;
        this->files = files;
    }
    bool action();

    private:
    fileList *files;
};

// SwapFileFactory: Dir for switching active file
class SwapFileFactory : public Action {
    public:
    SwapFileFactory(fileList *allbufs) {
        active = true;
        dir = true;
        this->allbufs = allbufs;
        functor = sigc::mem_fun(*allbufs, &fileList::setCurrentBuf);
        setSubKey(Glib::RefPtr<std::unordered_map<guint, Glib::RefPtr<Action>>>(
            new std::unordered_map<guint, Glib::RefPtr<Action>>()));
    }
    bool action();

    private:
    fileList *               allbufs;
    sigc::slot<bool(size_t)> functor;
};

// SwapFile: allows choosing of alternative files, subaction of SwapFileFactory
class SwapFile : public Action {
    public:
    SwapFile(sigc::slot<bool(size_t)> func, size_t id, Glib::ustring name) {
        active = true;
        dir = false;
        changeName.connect(func);
        this->id = id;
        this->name = name;
    }
    bool action();

    private:
    sigc::signal<bool(size_t)> changeName;
    size_t                     id;
};

// CloseFile: close the currently opened file
class CloseFile : public Action {
    public:
    CloseFile(fileList *files) {
        active = true;
        dir = false;
        this->files = files;
    }
    bool action();

    private:
    fileList *files;
};

#endif
