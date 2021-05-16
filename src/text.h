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
                DLOG("File_Test failed, attempting to create file...");
                createFile();
            }
        }
    }

    Glib::ustring                 getName() const { return name; }
    Glib::ustring                 getFileName() const { return fileName; }
    bool                          isEditable() const { return editable; }
    Glib::RefPtr<Gtk::TextBuffer> buffer() const { return selfbuffer; }
    Glib::RefPtr<Gio::File>       fileObj() const { return file; }

    bool createFile();
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
    size_t append(Glib::ustring name, Glib::ustring file = "", bool editable = "");

    void                          deleteBufAt(size_t);
    bool                          setCurrentBuf(size_t);
    Glib::RefPtr<Gtk::TextBuffer> getBuffer(size_t) const;
    Glib::RefPtr<ppdTextBuffer>   getPPDTB(size_t) const;
    std::vector<size_t>           getAllIDs() const;

    sigc::signal<bool(size_t)>* signalSWBuffer() { return &swBufferID; }
    bool                        IDExists(size_t id) const { return getBuffer(id) != nullptr; }

    size_t getCurrBufferID() const {
        for(auto x: buffers)
            if(x.second->buffer() == currBuffer->buffer())
                return x.first;
        // unreachable, but
        LOG("Somehow, there is no currently active buffer ID!");
        return 0;
    }

    std::vector<Glib::ustring> getAllNames() const;
    Glib::ustring              getCurrBufferName() const { return currBuffer->getName(); }

    protected:
    size_t findLowestId();

    private:
    std::unordered_map<size_t, Glib::RefPtr<ppdTextBuffer>> buffers;
    Glib::RefPtr<ppdTextBuffer>                             currBuffer;
    sigc::signal<bool(size_t)>                              swBufferID;
};

#endif
