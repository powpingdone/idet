#ifndef TEXT_H
#define TEXT_H

#include "baseaction.h"
#include "common.h"
#include <gtkmm.h>

// generic text buffer for handling text and files
class ppdTextBuffer {
    public:
    ppdTextBuffer(Glib::ustring name, Glib::ustring file = "", bool editable = true, size_t id = 0) {
        this->name = name;
        this->fileName = file;
        this->editable = editable;
        this->id = id;
        if(file != "") {
            if(Glib::file_test(file, Glib::FileTest::IS_REGULAR)) {
                this->file = Gio::File::create_for_path(file);
            } else {
                DLOG("File_Test failed, attempting to create file...");
                if(!createFile()) {
                    LOG("UNABLE TO OPEN FILE!");
                    // TODO: something....?
                }
            }
        }
        if(this->file) {
            monitor = this->file->monitor();
            monitor->signal_changed().connect(sigc::mem_fun(*this, &ppdTextBuffer::reloadPromptCreate));
            DLOG("Monitor on!");
        }
    }

    // getters/setters
    Glib::ustring                                                     getName() const { return name; }
    Glib::ustring                                                     getFileName() const { return fileName; }
    bool                                                              isEditable() const { return editable; }
    Glib::RefPtr<Gtk::TextBuffer>                                     buffer() const { return selfbuffer; }
    Glib::RefPtr<Gio::File>                                           fileObj() const { return file; }
    sigc::signal<void(Glib::ustring, bool, sigc::slot<void(bool)>*)>& reloadPromptSignal() { return reloadPrompt; }
    sigc::signal<void(Glib::RefPtr<Action>)>&                         pushSignal() { return push; }
    sigc::signal<bool()>&                                             popSignal() { return pop; }
    void selfDestructSlot(sigc::slot<void(size_t)> func) { selfDestruct = func; }

    // generic file functions
    bool createFile();
    bool save();
    bool reload();

    // file reloading handling
    // this->monitor signals -> reloadPromptCreate -> mainWindow::queueAction -> wait -> reloadPromptSend ->
    // mainWindow::promptYesNo -> wait -> mainWindow::promptYesNoSignal -> reloadPromptRecieve
    void reloadPromptCreate(const Glib::RefPtr<Gio::File>&, const Glib::RefPtr<Gio::File>&, Gio::FileMonitor::Event);
    void reloadPromptSend();
    void reloadPromptReceive(bool);

    private:
    Glib::ustring                  name, fileName;
    Glib::RefPtr<Gio::File>        file = nullptr;
    bool                           editable;
    size_t                         id;
    Glib::RefPtr<Gtk::TextBuffer>  selfbuffer = Gtk::TextBuffer::create();
    Glib::RefPtr<Gio::FileMonitor> monitor;

    // used in reloading files
    sigc::signal<void(Glib::ustring, bool, sigc::slot<void(bool)>*)> reloadPrompt; // signal mainWindow::promptYesNo
    sigc::signal<void(Glib::RefPtr<Action>)>                         push; // signal mainWindow::queueAction()
    sigc::signal<bool()>                                             pop; // signal mainWindow::popAction()
    sigc::slot<void(size_t)>                                         selfDestruct;
    std::vector<Glib::RefPtr<Gio::File>>                             tmpFiles;
    Gio::FileMonitor::Event                                          tmpEvent;
    sigc::slot<void(bool)> reloadPromptReceiveSlot = sigc::mem_fun(*this, &ppdTextBuffer::reloadPromptReceive);
};

// container of files
class fileList {
    public:
    size_t append(Glib::ustring name, Glib::ustring file = "", bool editable = "");

    void                          deleteBufAt(size_t);
    void                          deleteAndSwapToValidBuffer(size_t);
    bool                          setCurrentBuf(size_t);
    Glib::RefPtr<Gtk::TextBuffer> getBuffer(size_t) const;
    Glib::RefPtr<ppdTextBuffer>   getPPDTB(size_t) const;
    std::vector<size_t>           getAllIDs() const;

    sigc::signal<bool(size_t)>& signalSWBuffer() { return swBufferID; }
    bool                        IDExists(size_t id) const { return getBuffer(id) != nullptr; }

    void setSlotReloadYN(sigc::slot<void(Glib::ustring, bool, sigc::slot<void(bool)>*)> reloadSlot) {
        this->reloadSlot = reloadSlot;
    }
    void setSlotPopAction(sigc::slot<bool()> popFunc) { this->popFunc = popFunc; }
    void setSlotPushAction(sigc::slot<void(Glib::RefPtr<Action>)> pushFunc) { this->pushFunc = pushFunc; }

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
    std::unordered_map<size_t, Glib::RefPtr<ppdTextBuffer>> buffers; // all current text buffers
    Glib::RefPtr<ppdTextBuffer>                             currBuffer; // reference buffer to currently active buffer
    sigc::signal<bool(size_t)>                              swBufferID; // signal to call mainWindow::swBufferByID
    sigc::slot<void(Glib::ustring, bool, sigc::slot<void(bool)>*)> reloadSlot; // slot to hold mainWindow::promptYesNo
    sigc::slot<bool()>                                             popFunc; // slot to hold mainWindow::popAction
    sigc::slot<void(Glib::RefPtr<Action>)>                         pushFunc; // slot to hold mainWindow::queueAction
};

#endif
