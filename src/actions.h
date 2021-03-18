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
    SaveFile(fileList *lis) {
        active = true;
        dir = false;
        this->lis = lis;
    }
    bool action();

    private:
    fileList *lis;
};

// SwapFileFactory: Dir for switching active file
class SwapFileFactory : public Action {
    public:
    SwapFileFactory(fileList *allbufs) {
        active = true;
        dir = true;
        this->allbufs = allbufs;
        functor = sigc::mem_fun(*allbufs, &fileList::setCurrentBufByName);
        setSubKey(Glib::RefPtr<std::unordered_map<guint, Glib::RefPtr<Action>>>(
            new std::unordered_map<guint, Glib::RefPtr<Action>>()));
    }
    bool action();

    private:
    fileList *                      allbufs;
    sigc::slot<bool(Glib::ustring)> functor;
};

// SwapFile: allows choosing of alternative files, subaction of SwapFileFactory
class SwapFile : public Action {
    public:
    SwapFile(sigc::slot<bool(Glib::ustring)> func, Glib::ustring name) {
        active = true;
        dir = false;
        changeName.connect(func);
        this->name = name;
    }
    bool action();

    private:
    sigc::signal<bool(Glib::ustring)> changeName;
};

class CloseFile : public Action {
    public:
    CloseFile(fileList *lis) {
        active = true;
        dir = false;
        this->lis = lis;
    }
    bool action();

    private:
    fileList *lis;
};

#endif
