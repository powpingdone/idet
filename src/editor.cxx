#include "editor.h"
#include "actions.h"
#include "common.h"
#include "text.h"
#include <sstream>
#include <string>

mainWindow::mainWindow() {
    DLOG("INIT setup!");
    set_title("IdET");
    auto kH = Gtk::EventControllerKey::create();
    kH->signal_key_pressed().connect(sigc::mem_fun(*this, &mainWindow::keyboardHandler), false);
    add_controller(kH);
    // TODO: Use cached value for default size or something else like monitor size
    set_default_size(500, 500);

    // setup box containing the other component boxes
    masterGrid.set_margin(10);
    set_child(masterGrid);

    // setup editor box
    masterGrid.set_name("masterGrid");
    masterGrid.attach(sourceHolder, 0, 0);
    masterGrid.set_column_homogeneous();
    masterGrid.attach(ctrlSpc, 0, 1);

    // setup lines
    DLOG("Setup sourceHolder tree");
    sourceLines.set_name("sourceLines");
    sourceLines.set_editable(false);
    sourceLines.set_cursor_visible(false);
    sourceLines.set_monospace();
    sourceLines.set_expand();

    sourceCode.set_name("sourceCode");
    sourceCode.set_monospace();
    sourceCode.set_expand();

    textEditor.set_name("textEditor");
    textEditor.attach(sourceLines, 0, 0);
    textEditor.attach(sourceCode, 1, 0);
    textEditor.set_expand();
    textEditor.set_column_homogeneous(false);

    sourceHolder.set_name("sourceHolder");
    sourceHolder.set_child(textEditor);
    sourceHolder.set_expand();

    // setup constraints for "nice line numbering"
    constrain();
    textEditor.set_layout_manager(eSrcLayout);

    // setup ctrlSpc
    // inspired by both vim-which-key and vim-ctrlspace
    DLOG("Setup ctrlSpc");
    ctrlSpc.set_name("ctrlSpc");
    ctrlSpc.append(ctrlSpcView.ctrlSpcSelect);
    ctrlSpc.set_visible(false);
    ctrlSpc.set_vexpand(false);
    ctrlSpc.set_focus_on_click();

    // for now, use just a sample buffer
    DLOG("BUFFERS up!");
    buffers.signalSWBuffer()->connect(sigc::mem_fun(*this, &mainWindow::swBufferByID));
    buffers.append("new 1");
    buffers.setCurrentBuf(1);

    // and then actions
    // but first: macros
    // clang-format off
#define NAME(x...) std::vector<Glib::ustring>(x)
#define ACTION(x) Glib::RefPtr<Action>(new x)

    ctrlSpcView.add_action(
        NAME({"File", "New"}),
        "fn",
        ACTION(NewFile(&buffers, sigc::mem_fun(*this, &mainWindow::textPrompt)))
    );

    ctrlSpcView.add_action(
        NAME({"File", "Open"}),
        "fo",
        ACTION(OpenFile(&buffers, sigc::mem_fun(*this, &mainWindow::selfReturn)))
    );
    
    ctrlSpcView.add_action(
        NAME({"File", "Save"}),
        "fs",
        ACTION(SaveFile(&buffers))
    );
    
    ctrlSpcView.add_action(
        NAME({"File", "Close"}),
        "fc",
        ACTION(CloseFile(&buffers))
    );
    
    ctrlSpcView.add_action(
        NAME({"Swap"}),
        "s",
        ACTION(SwapFileFactory(&buffers))
    );

    // clang-format on

    LOG("Rock and Roll, we're ready to start writing!");
}

void mainWindow::updateLineNumbers() {
    auto buf = sourceLines.get_buffer(), src = sourceCode.get_buffer();
    if(src->get_line_count() != lines) {
        DLOG("Updating line numbers");
        lines = src->get_line_count();
        std::stringstream newNumbers;
        for(int x = 1; x < lines + 1; x++) { newNumbers << std::to_string(x) << std::endl; }
        buf->set_text(newNumbers.str());
    }
}

bool mainWindow::swBufferByID(size_t id) {
    // NOTE: This function should be handled with a signal,
    // which should be done with the fileList object
    DLOG("Swapping to buffer ID %lu", id);
    return swBuffer(buffers.getBuffer(id));
}

bool mainWindow::swBuffer(Glib::RefPtr<Gtk::TextBuffer> buf) {
    if(buf == nullptr) {
        LOG("No buffer found to switch to.");
        return false;
    }

    sourceCode.set_buffer(buf);
    regenSCSignals();
    return true;
}

bool mainWindow::keyboardHandler(guint keyval, guint keycode, Gdk::ModifierType state) {
    // ctrl + spc
    if(keyval == GDK_KEY_space && (state & Gdk::ModifierType::CONTROL_MASK) == Gdk::ModifierType::CONTROL_MASK) {
        if(ctrlSpcView.isActive()) {
            LOG("Deactivating ctrlSpc.");
            ctrlSpcView.stop();
            if(ctrlSpcView.isPMode()) {
                ctrlSpcView.promptModeOff();
                pmodeCallback.clear();
                // please dont leak memory
                free(userCallslot);
                userCallslot = nullptr;
            }
            focusTextEditor();
        } else {
            LOG("Activating ctrlSpc.");
            ctrlSpcView.start();
            focusCtrlSpc();
            ctrlSpcView.generate();
        }
        return true;
    } // check if the current pmode is handling it
    else if(ctrlSpcView.isPMode() && pmodeCallback.emit(keyval, keycode, state)) {
        return true; // it did handle it
    }
    // if the previous does not happen, try checking here
    return ctrlSpcView.keyboardHandler(keyval, keycode, state);
}

void mainWindow::constrain() {
    // clang-format off
    // TODO: resize numbers dynamically to font
    /*
     * "muh gui boilerplate"
     * You can close this function.
     * tldr: how to get fancy line numbers
     *
     * Roadmap:
     * tE, sL, sC are equivalent vars
     * tE == textEditor
     * sL == sourceLines
     * sC == sourceCode
     *
     * tE.start = sL.start
     * sL.height = sC.height
     * sL.end = sC.start 
     * sC.end = tE.end
     * tE.top = s{L,C}.top
     * s{L,C}.bottom = tE.bottom
     * sL.width = 48
     * */
    DLOG("Constructing Constraints");

    eSrcLayout->add_constraint( // bind to start of editor grid
        Gtk::Constraint::create(
            textEditor.make_refptr_constrainttarget(),
            Gtk::Constraint::Attribute::START,
            Gtk::Constraint::Relation::EQ,
            sourceLines.make_refptr_constrainttarget(),
            Gtk::Constraint::Attribute::START,
            1.0,
            0,
            Gtk::Constraint::Strength::REQUIRED
        )
    );
    
    eSrcLayout->add_constraint( // sync height source code to the linenums
        Gtk::Constraint::create(
            sourceLines.make_refptr_constrainttarget(),
            Gtk::Constraint::Attribute::HEIGHT,
            Gtk::Constraint::Relation::EQ,
            sourceCode.make_refptr_constrainttarget(),
            Gtk::Constraint::Attribute::HEIGHT,
            1.0,
            0,
            Gtk::Constraint::Strength::REQUIRED
        )
    );

    eSrcLayout->add_constraint( // connect the source code to the linenums
        Gtk::Constraint::create(
            sourceLines.make_refptr_constrainttarget(),
            Gtk::Constraint::Attribute::END,
            Gtk::Constraint::Relation::GE,
            sourceCode.make_refptr_constrainttarget(),
            Gtk::Constraint::Attribute::START,
            1.0,
            0,
            Gtk::Constraint::Strength::REQUIRED
        )
    );

    eSrcLayout->add_constraint( // bind to end of editor grid
        Gtk::Constraint::create(
            sourceCode.make_refptr_constrainttarget(),
            Gtk::Constraint::Attribute::END,
            Gtk::Constraint::Relation::EQ,
            textEditor.make_refptr_constrainttarget(),
            Gtk::Constraint::Attribute::END,
            1.0,
            0,
            Gtk::Constraint::Strength::REQUIRED
        )
    );
    
    eSrcLayout->add_constraint( // bind src to grid top
        Gtk::Constraint::create( 
            textEditor.make_refptr_constrainttarget(),
            Gtk::Constraint::Attribute::TOP,
            Gtk::Constraint::Relation::EQ,
            sourceCode.make_refptr_constrainttarget(),
            Gtk::Constraint::Attribute::TOP,
            1.0,
            0,
            Gtk::Constraint::Strength::REQUIRED
        )
    );

    eSrcLayout->add_constraint( // bind linenumbers to grid top
        Gtk::Constraint::create( 
            textEditor.make_refptr_constrainttarget(),
            Gtk::Constraint::Attribute::TOP,
            Gtk::Constraint::Relation::EQ,
            sourceLines.make_refptr_constrainttarget(),
            Gtk::Constraint::Attribute::TOP,
            1.0,
            0,
            Gtk::Constraint::Strength::REQUIRED
        )
    );

    eSrcLayout->add_constraint( // bind src to grid bottom
        Gtk::Constraint::create( 
            sourceCode.make_refptr_constrainttarget(),
            Gtk::Constraint::Attribute::BOTTOM,
            Gtk::Constraint::Relation::EQ,
            textEditor.make_refptr_constrainttarget(),
            Gtk::Constraint::Attribute::BOTTOM,
            1.0,
            0,
            Gtk::Constraint::Strength::REQUIRED
        )
    );

    eSrcLayout->add_constraint( // bind linenumbers to grid bottom
        Gtk::Constraint::create( 
            sourceLines.make_refptr_constrainttarget(),
            Gtk::Constraint::Attribute::BOTTOM,
            Gtk::Constraint::Relation::EQ,
            textEditor.make_refptr_constrainttarget(),
            Gtk::Constraint::Attribute::BOTTOM,
            1.0,
            0,
            Gtk::Constraint::Strength::REQUIRED
        )
    );

    eSrcLayout->add_constraint( // make line numbers 48 pix long
        Gtk::Constraint::create( 
            sourceLines.make_refptr_constrainttarget(),
            Gtk::Constraint::Attribute::WIDTH,
            Gtk::Constraint::Relation::EQ,
            48.0,
            Gtk::Constraint::Strength::REQUIRED
        )
    );

    // man all of this just to have dynamic source line nums that look nice
    // yes i could have done the lang, but thats no fun
    // clang-format on
}
