#include "text.h"

#include <gtkmm.h>

// TODO: ADD ASYNC SUPPORT

void ppdTextBuffer::save() {
    auto overwriteBuffer = fileObj()->replace();
    overwriteBuffer->write(buffer()->get_text());
    overwriteBuffer->close();
}

void ppdTextBuffer::reload() {
    // ... why does gio do this?
    char* contents;
    gsize amt;
    fileObj()->load_contents(contents, amt);
    buffer()->set_text(contents, contents+amt);
    g_free(contents);
}
