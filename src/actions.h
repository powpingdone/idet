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
 * TEMPLATED ACTIONS
 * aka, usually actions that are forced to reference the editor window directly in some way
 * */

// OpenFile: action to open a file
// The main reason why this is templated is due to the high usage of GTK stuff
template<typename T> class OpenFile : public Action {
    public:
    OpenFile(fileList *win, T *windowPtr) {
        active = true;
        dir = false;
        this->files = win;
        this->windowPtr = windowPtr;
    }

    protected:
    bool action() {
        auto dialog = new Gtk::FileChooserDialog("Open File", Gtk::FileChooser::Action::OPEN);
        dialog->set_transient_for(*windowPtr); // why this is a template
        dialog->set_modal();
        dialog->signal_response().connect(sigc::bind(sigc::mem_fun(*this, &OpenFile<T>::signal), dialog));
        dialog->add_button("_Ok", Gtk::ResponseType::OK);
        dialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
        dialog->show();
        return true;
    }

    void signal(int response, Gtk::FileChooserDialog *dialog) {
        switch(response) {
            case Gtk::ResponseType::OK: {
                auto file = Glib::ustring(dialog->get_file()->get_path());
                LOG("Opening file \"%s\"", file.c_str());

                // but first, generate the name
                std::string name, tmp(file);
                int         x = tmp.length() - 1;
                while(x >= 0 && tmp[x] != '/' && tmp[x] != '\\') x--;
                name = tmp.substr((int)tmp.length() > x ? x + 1 : 0);

                files->append(name, file, true);
                if(!windowPtr->swBuffer(name))
                    LOG("Failed to open file!");
                break;
            }
            case Gtk::ResponseType::CANCEL:
            default: {
                LOG("Cancelled!");
            }
        }
        delete dialog;
    }

    private:
    fileList *files;
    T *       windowPtr; // hack to get around recursive definition
};

/*
 * REGULAR ACTIONS
 * */

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
    SwapFileFactory(sigc::slot<bool(Glib::ustring)> functor, sigc::slot<std::vector<Glib::ustring>()> getNames) {
        active = true;
        dir = true;
        this->functor = functor;
        this->getNames.connect(getNames);
        setSubKey(Glib::RefPtr<std::unordered_map<guint, Glib::RefPtr<Action>>>(
            new std::unordered_map<guint, Glib::RefPtr<Action>>()));
    }
    bool action();

    private:
    sigc::signal<std::vector<Glib::ustring>()> getNames;
    sigc::slot<bool(Glib::ustring)>            functor;
};

// SwapFile: allows choosing of alternative files, subaction of SwapFileFactory
class SwapFile : public Action {
    public:
    SwapFile(sigc::slot<bool(Glib::ustring)> func, Glib::ustring name) {
        active = true;
        dir = false;
        sig.connect(func);
        this->name = name;
    }
    bool action();

    private:
    sigc::signal<bool(Glib::ustring)> sig;
};

#endif
