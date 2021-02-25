#include "editor.h"
#include "common.h"
#include "text.h"
#include <sstream>

mainWindow::mainWindow() {
    set_title("editorWindow");
    auto kH = Gtk::EventControllerKey::create();
    kH->signal_key_pressed().connect(sigc::mem_fun(*this, &mainWindow::keyboardHandler), false);
    add_controller(kH);

    // setup box containing the other component boxes
    masterGrid.set_margin(10);
    set_child(masterGrid);

    // setup editor box
    masterGrid.set_name("masterGrid");
    masterGrid.attach(sourceHolder, 0, 0);
    masterGrid.set_column_homogeneous();
    masterGrid.attach(ctrlSpc, 0, 1);

    // setup constraints for "nice line numbering"
    constrain();
    textEditor.set_layout_manager(eSrcLayout);
    
    // setup lines
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

    // setup ctrlSpc
    // inspired by both vim-which-key and vim-ctrlspace
    ctrlSpc.set_name("ctrlSpc");
    ctrlSpc.append(ctrlSpcView.ctrlSpcSelect);
    ctrlSpc.set_visible(false);
    ctrlSpc.set_vexpand(false);
    ctrlSpc.set_focus_on_click();

    // for now, use just a sample buffer
    buffers.push_back(ppdTextBuffer("new 1"));
    sourceCode.set_buffer(buffers.at(0).buffer());
    regenSCSignals();
}

void mainWindow::updateLineNumbers() {
    auto buf = sourceLines.get_buffer(), src = sourceCode.get_buffer();
    if(src->get_line_count() != lines) {
        lines = src->get_line_count();
        std::stringstream newNumbers;
        for(int x = 1; x < lines+1; x++) {
            newNumbers << x << std::endl;
        }
        buf->set_text(newNumbers.str());
    }
}

bool mainWindow::keyboardHandler(guint keyval, guint keycode, Gdk::ModifierType state) {
    if(keyval == GDK_KEY_space && (state & Gdk::ModifierType::CONTROL_MASK) == Gdk::ModifierType::CONTROL_MASK) {
        if(ctrlSpcView.isActive()) {
            LOG("Deactivating ctrlSpc.");
            ctrlSpcView.stop();
            ctrlSpc.set_visible(false);
        }
        else {
            LOG("Activating ctrlSpc.");
            ctrlSpcView.start();
            ctrlSpc.set_visible(true);
            ctrlSpcView.generate();
        }
        return true;
    }
    // if the previous does not happen, try checking here
    return ctrlSpcView.keyboardHandler(keyval, keycode, state); 
}

void mainWindow::constrain() {
    /*
     * "muh gui boilerplate"
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
        Gtk::Constraint::create( //TODO: resize dynamically to font
            sourceLines.make_refptr_constrainttarget(),
            Gtk::Constraint::Attribute::WIDTH,
            Gtk::Constraint::Relation::EQ,
            48.0,
            Gtk::Constraint::Strength::REQUIRED
        )
    );
}

