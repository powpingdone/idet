#ifndef ACTIONS_H
#define ACTIONS_H

#include <gtkmm.h>
#include "common.h"
#include "gtkmm/filechooserdialog.h"
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
        setSubKey(
                Glib::RefPtr<std::unordered_map<guint, Glib::RefPtr<Action>>>
                (new std::unordered_map<guint, Glib::RefPtr<Action>>())
        );
    }
};

// OpenFile: action to open a file
template<typename T> class OpenFile : public Action { 
    public:
        OpenFile(fileList &win, T *windowPtr) {
            active = true; dir = false;
            this->files = win;
            this->windowPtr = windowPtr;
        }
    protected: 
        bool action() {
            auto dialog = new Gtk::FileChooserDialog("Open File", Gtk::FileChooser::Action::OPEN);
            dialog->set_transient_for(*windowPtr);
            dialog->set_modal();
            dialog->signal_response().connect(sigc::bind(sigc::mem_fun(*this, &OpenFile<T>::signal), dialog));
            dialog->add_button("_Ok", Gtk::ResponseType::OK);
            dialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
            dialog->show();
            return true;
        }
        
        void signal(int response, Gtk::FileChooserDialog* dialog) {
            switch(response) {
                case Gtk::ResponseType::OK: {
                    auto file = dialog->get_file()->get_path();
                    LOG("Opening file %s", file);
                    files.append(file, file, true);
                    windowPtr->swBuffer(file);
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
        fileList files;
        T *windowPtr; //hack to get around recursive definition
};

#endif
