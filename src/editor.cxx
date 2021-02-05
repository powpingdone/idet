#include "editor.h"
#include <iostream>
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
    masterGrid.attach(sourceHolder, 0, 0);
    sourceHolder.set_child(slaveEditor);
    sourceHolder.set_expand();
    slaveEditor.set_column_homogeneous(false);
    slaveEditor.attach(sourceLines, 0, 0);
    slaveEditor.attach(sourceCode, 1, 0);
    masterGrid.attach(ctrlSpc, 0, 1);
    ctrlSpc.add_child(ctrlSpcView.ctrlSpcSelect);

    //setup constraints for "nice line numbering"
    constrain();
    slaveEditor.set_layout_manager(eSrcLayout);

    // setup lines
    sourceLines.set_editable(false);
    sourceLines.set_cursor_visible(false);
    sourceLines.set_monospace();
    sourceLines.set_expand();
    sourceCode.set_monospace();
    sourceCode.set_expand();
    sourceCode.get_buffer()->signal_changed().connect(sigc::mem_fun(*this, &mainWindow::updateLineNumbers));

    // setup ctrlSpc
    // inspired by both vim-which-key and vim-ctrlspace
    ctrlSpc.set_revealed(false);
    ctrlSpc.set_message_type(Gtk::MessageType::INFO);
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

bool mainWindow::keyboardHandler(guint key, guint keycode, Gdk::ModifierType state) {
    if(key == GDK_KEY_space && (state & Gdk::ModifierType::CONTROL_MASK) == Gdk::ModifierType::CONTROL_MASK) {
        if(ctrlSpcView.isActive()) {
            ctrlSpcView.stop();
            ctrlSpc.set_revealed(false);
        }
        else {
            ctrlSpc.set_revealed(true);
            ctrlSpcView.start();
            ctrlSpcView.generate();
        }
        return true;
    }
    return false;
}

void mainWindow::constrain() {
    /*
     * "muh gui boilerplate"
     * 
     * Roadmap:
     * sE, sL, sC are equivalent vars
     * sE == slaveEditor
     * sL == sourceLines
     * sC == sourceCode
     *
     * sE.start = sL.start
     * sL.height = sC.height
     * sL.end = sC.start 
     * sC.end = sE.end
     * sE.top = s{L,C}.top
     * s{L,C}.bottom = sE.bottom
     * sL.width = 48
     * */
    
    eSrcLayout->add_constraint( // bind to start of editor grid
        Gtk::Constraint::create(
            slaveEditor.make_refptr_constrainttarget(),
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
            slaveEditor.make_refptr_constrainttarget(),
            Gtk::Constraint::Attribute::END,
            1.0,
            0,
            Gtk::Constraint::Strength::REQUIRED
        )
    );
    
    eSrcLayout->add_constraint( // bind src to grid top
        Gtk::Constraint::create( 
            slaveEditor.make_refptr_constrainttarget(),
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
            slaveEditor.make_refptr_constrainttarget(),
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
            slaveEditor.make_refptr_constrainttarget(),
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
            slaveEditor.make_refptr_constrainttarget(),
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

