#ifndef TEXT_H
#define TEXT_H

#include <gtkmm.h>

// generic text buffer for handling text and files
class ppdTextBuffer {
    public:
        ppdTextBuffer(Glib::ustring name, Glib::ustring file="", bool editable=true) {
            this->name = name;
            this->fileName = file;
            this->editable = editable;
            if(file != "" && Glib::file_test(file, Glib::FileTest::IS_REGULAR)) 
                this->file = Gio::File::create_for_path(file);
        }
    
        Glib::ustring                 getName()     const {return name;}
        Glib::ustring                 getFileName() const {return fileName;}
        bool                          isEditable()  const {return editable;}
        Glib::RefPtr<Gtk::TextBuffer> buffer()      const {return selfbuffer;}
        Glib::RefPtr<Gio::File>       fileObj()     const {return file;}
        
        void save();
        void reload();
    private:
        Glib::ustring name, fileName;
        Glib::RefPtr<Gio::File> file = nullptr;
        bool editable;
        Glib::RefPtr<Gtk::TextBuffer> selfbuffer = Gtk::TextBuffer::create();
};

class fileList {
    public:
        fileList(Gtk::TextView* view) {
            this->view = view;
        }
    private:
        Gtk::TextView *view;
        std::vector<ppdTextBuffer> buffers;
};

#endif
