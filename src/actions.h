#ifndef ACTIONS_H
#define ACTIONS_H

#include <gtkmm.h>
#include "common.h"
#include "glibmm/refptr.h"

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
        
        bool                                                          activateable() {return active;}
        bool                                                          category()     {return dir;}
        Glib::ustring                                                 getName()      {return name;}
        Glib::RefPtr<std::unordered_map<guint, Glib::RefPtr<Action>>> subKey()       {return _subKey;} 
        Glib::RefPtr<void>                                            getArgs()      {return args;}

        void setName(Glib::ustring name)                                                     {this->name = name;}
        void setSubKey(Glib::RefPtr<std::unordered_map<guint, Glib::RefPtr<Action>>> subKey) {this->_subKey = subKey;}
        void setArgs(Glib::RefPtr<void> args)                                                {this->args = args;}

    protected:
        virtual bool action() {return false;};
        bool         active;
        bool         dir;
    private:
        Glib::ustring                                                 name = "";
        Glib::RefPtr<std::unordered_map<guint, Glib::RefPtr<Action>>> _subKey = nullptr;
        Glib::RefPtr<void>                                            args = nullptr;
};

class CategoryAction : public Action {
    public:
    CategoryAction() {
        active = false;
        dir = true;
        setSubKey(Glib::RefPtr<std::unordered_map<guint, Glib::RefPtr<Action>>>(new std::unordered_map<guint, Glib::RefPtr<Action>>()));
    }
};

#endif
