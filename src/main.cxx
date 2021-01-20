#include <stdio.h>
#include <gtkmm.h>

class fWindow : public Gtk::Window {
    public:
        fWindow();
};

fWindow::fWindow() {
    set_title("self");
    set_default_size(200, 200);
}

int main(int argc, char * argv[]) {
    printf("Hello world!\n");
    auto app = Gtk::Application::create("org.gtkmm.examples.base");
    return app->make_window_and_run<fWindow>(argc, argv);   
}
