#ifndef ACTIONS_H
#define ACTIONS_H

#include "common.h"
#include "text.h"
#include <gtkmm.h>

// BASE ACTION CLASS
class Action {
    public:
    virtual ~Action() { ; }
    virtual bool activate() {
        if(active)
            return action();
        // else
        LOG("%s is not an activatable class! Returning false.", typeid(this).name());
        return false;
    }

    bool                                                          activateable() const { return active; }
    bool                                                          category() const { return dir; }
    Glib::ustring                                                 getName() const { return name; }
    Glib::RefPtr<std::unordered_map<guint, Glib::RefPtr<Action>>> subKey() const { return _subKey; }
    Glib::RefPtr<void>                                            getArgs() const { return args; }

    void setName(Glib::ustring name) { this->name = name; }
    void setSubKey(Glib::RefPtr<std::unordered_map<guint, Glib::RefPtr<Action>>> subKey) { this->_subKey = subKey; }
    void setArgs(Glib::RefPtr<void> args) { this->args = args; }

    protected:
    virtual bool                                                  action() { return false; }
    bool                                                          active; // is this to be activated?
    bool                                                          dir; // can this be accessed as a subKey
    Glib::RefPtr<void>                                            args = nullptr; // possibly unused
    Glib::ustring                                                 name = ""; // name for sorting itself in _subKey
    Glib::RefPtr<std::unordered_map<guint, Glib::RefPtr<Action>>> _subKey = nullptr;
};

// generic category
class CategoryAction : public Action {
    public:
    CategoryAction() {
        active = false;
        dir = true;
        setSubKey(Glib::RefPtr<std::unordered_map<guint, Glib::RefPtr<Action>>>(
            new std::unordered_map<guint, Glib::RefPtr<Action>>()));
    }
};

/*
 * REGULAR ACTIONS
 * */

// NewFile: action to create a new buffer
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
    SwapFile(sigc::slot<bool(size_t)> func, size_t id) {
        active = true;
        dir = false;
        changeName.connect(func);
        this->id = id;
    }
    bool action();

    private:
    sigc::signal<bool(size_t)> changeName;
    size_t id;
};

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
