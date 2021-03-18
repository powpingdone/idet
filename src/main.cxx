#include "common.h"
#include "editor.h"

// start timer
const std::chrono::time_point<std::chrono::system_clock> pt = std::chrono::system_clock::now();
bool                                                     enableDebug = false;

int main(int argc, char* argv[]) {
    for(int pos = 0; pos < argc; pos++) {
        if(strcmp("--debug", argv[pos])) {
            enableDebug = true;
        }
    }
    LOG("Idet, Activate!");
    auto app = Gtk::Application::create("xyz.powpingdone.idet");
    return app->make_window_and_run<mainWindow>(argc, argv);
}
