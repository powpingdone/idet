#include "editor.h"
#include "common.h"

// start timer
const std::chrono::time_point<std::chrono::system_clock> pt = std::chrono::system_clock::now();

int main(int argc, char* argv[]) {
    LOG("Idet, Activate!");
    auto app = Gtk::Application::create("xyz.powpingdone.idet");
    return app->make_window_and_run<mainWindow>(argc, argv);
}
