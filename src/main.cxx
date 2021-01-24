#include <iostream>
#include "editor.h"

int main(int argc, char * argv[]) {
    auto app = Gtk::Application::create("xyz.powpingdone.idet");
    return app->make_window_and_run<editorWindow>(argc, argv);   
}
