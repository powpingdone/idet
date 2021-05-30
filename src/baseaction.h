#ifndef BASEACTION_H
#define BASEACTION_H

#include <gtkmm.h>
#include "common.h"

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

#endif
