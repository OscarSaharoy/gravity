// Oscar Saharoy 2019

#include <gtkmm/application.h>
#include "gravity_window.cpp"

int main(int argc, char *argv[]) {
	
	// create and run the Gtk application
	auto app = Gtk::Application::create(argc, argv);

	Gravity_Window gravity_window;

	return app->run(gravity_window);
}