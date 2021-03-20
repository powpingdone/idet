#include "common.h"
#include "editor.h"

// start timer
const std::chrono::time_point<std::chrono::system_clock> pt = std::chrono::system_clock::now();
bool                                                     enableDebug;

int main(int argc, char* argv[]) {
    enableDebug = false;
    for(int pos = 0; pos < argc; pos++) {
        if(std::string(argv[pos]) == "--debug") {
            enableDebug = true;
        }
    }
    LOG("Idet, Activate!");
    DLOG("### DEBUGGING ENABLED ###");
    auto app = Gtk::Application::create("xyz.powpingdone.idet");
    return app->make_window_and_run<mainWindow>(1, argv);
}
