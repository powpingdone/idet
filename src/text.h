#ifndef TEXT_H
#define TEXT_H

#include "common.h"
#include <gtkmm.h>

// generic text buffer for handling text and files
class ppdTextBuffer {
    public:
    ppdTextBuffer(Glib::ustring name, Glib::ustring file = "", bool editable = true) {
        this->name = name;
        this->fileName = file;
        this->editable = editable;
        if(file != "") {
            if(Glib::file_test(file, Glib::FileTest::IS_REGULAR)) {
                this->file = Gio::File::create_for_path(file);
            } else {
                LOG("File_Test failed!");
            }
        }
    }

    Glib::ustring                 getName() const { return name; }
    Glib::ustring                 getFileName() const { return fileName; }
    bool                          isEditable() const { return editable; }
    Glib::RefPtr<Gtk::TextBuffer> buffer() const { return selfbuffer; }
    Glib::RefPtr<Gio::File>       fileObj() const { return file; }

    bool save();
    bool reload();

    private:
    Glib::ustring                 name, fileName;
    Glib::RefPtr<Gio::File>       file = nullptr;
    bool                          editable;
    Glib::RefPtr<Gtk::TextBuffer> selfbuffer = Gtk::TextBuffer::create();
};

// container of files
class fileList {
    public:
    void                          append(Glib::ustring name, Glib::ustring file = "", bool editable = "");
    void                          deleteByName(Glib::ustring name);
    bool                          setCurrentBufByName(Glib::ustring name);
    Glib::RefPtr<Gtk::TextBuffer> getBufferByName(Glib::ustring name) const;
    Glib::RefPtr<ppdTextBuffer>   getPPDTBByName(Glib::ustring name) const;
    std::vector<Glib::ustring>    getAllNames() const;

    sigc::signal<bool(Glib::ustring)>* signalSWBuffer() { return &swBuffer; }
    bool                               nameExists(Glib::ustring name) const { return getBufferByName(name) != nullptr; }
    Glib::ustring                      getCurrBuffer() const { return currBuffer; }

    private:
    std::vector<Glib::RefPtr<ppdTextBuffer>> buffers;
    Glib::ustring                            currBuffer;
    sigc::signal<bool(Glib::ustring)>        swBuffer;
};

#endif
