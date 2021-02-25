#ifndef ACTIONS_H
#define ACTIONS_H

#include <gtkmm.h>
#include "common.h"
#include "text.h"

// BASE ACTION CLASS
class Action {
    public:
        virtual ~Action() {;}
        virtual bool activate() {
            if(active)
                return action();
            // else
            LOG("%s is not an activatable class! Returning false.", typeid(this).name());
            return false;
        }
        
        bool                                                          activateable() const {return active;}
        bool                                                          category()     const {return dir;}
        Glib::ustring                                                 getName()      const {return name;}
        Glib::RefPtr<std::unordered_map<guint, Glib::RefPtr<Action>>> subKey()       const {return _subKey;} 
        Glib::RefPtr<void>                                            getArgs()      const {return args;}

        void setName(Glib::ustring name)                                                     {this->name = name;}
        void setSubKey(Glib::RefPtr<std::unordered_map<guint, Glib::RefPtr<Action>>> subKey) {this->_subKey = subKey;}
        void setArgs(Glib::RefPtr<void> args)                                                {this->args = args;}

    protected:
        virtual bool       action() {return false;};
        bool               active;
        bool               dir;
        Glib::RefPtr<void> args = nullptr;
    private:
        Glib::ustring                                                 name = "";
        Glib::RefPtr<std::unordered_map<guint, Glib::RefPtr<Action>>> _subKey = nullptr;
};

// generic category
class CategoryAction : public Action {
    public:
    CategoryAction() {
        active = false;
        dir = true;
        setSubKey(Glib::RefPtr<std::unordered_map<guint, Glib::RefPtr<Action>>>(new std::unordered_map<guint, Glib::RefPtr<Action>>()));
    }
};

// OpenFile: action to open a file
class OpenFile : public Action { 
    public: OpenFile(fileList &win) {
        active = true; dir = false;
    } 
    protected: 
        bool action();
        void signal();
    private:
        fileList win;
};

#endif
