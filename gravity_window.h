#ifndef GRAVITY_WINDOW_H
#define GRAVITY_WINDOW_H

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/scale.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/adjustment.h>
#include <gdk/gdk.h>
#include <vector>
#include <deque>
#include <array>


struct Mouse_Info {

    double  x;          // x coordinate of mouse pointer
    double  y;          // y coordinate of mouse pointer
    double  click_x;    // x coordinate of right click  
    double  click_y;    // y coordinate of right click
    bool    clicked;    // true if mouse is right clicked
    double  click_time; // time for which right click has been held
};

class Gravity_Draw: public Gtk::DrawingArea {

public:
    Gravity_Draw(); // constructor

    // event function overrides
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& draw_context) override;
    bool on_scroll_event(GdkEventScroll * scroll_event) override;
    bool on_motion_notify_event(GdkEventMotion * motion_event) override;
    bool on_button_press_event(GdkEventButton * button_event) override;
    bool on_button_release_event(GdkEventButton * button_event) override;
    bool on_timeout();

    void framerate_changed(); // called when framerate is changed
    void gravity();           // calculates gravitational forces between bodies
    void join();              // joins bodies together if they collide
    void store_trails();      // stores data used to draw trails
    void reset();             // resets members to initial values
    void clear();             // clears bodies from simulation

    bool paused;      // true when simulation is paused
    int traillength;  // length of trails behind bodies
    double framerate; // target framerate of simulation
    double timescale; // scaling of simulation time

protected:
    std::vector<std::array<double, 2>>  pos;                // positions of all bodies
    std::vector<std::array<double, 2>>  vel;                // velocities of all bodies
    std::vector<double>                 mass;               // masses of all bodies
    std::vector<std::deque< 
                std::array<double, 2>>> trails;             // stores data for the bodies' trails
    std::vector<double>                 centre;             // coordinates of origin - used for drawing
    std::vector<double>                 background;         // colour of background
    double                              zoom;               // level of zoom
    double                              prev_zoom;          // previous level of zoom
    Mouse_Info                          mouse_info;         // stores info about mouse eg current position and click position
    sigc::connection                    timeout_connection; // connection to timeout event used for scheduling screen refreshes

};


class Gravity_Window: public Gtk::Window {

public:
    Gravity_Window();           // constructor

    void reset_sim();           // function attached to reset button
    void toggle_pause();        // function attached to pause button
    void traillength_changed(); // function attached to trail length slider
    void framerate_changed();   // function attached to framerate slider
    void timescale_changed();   // function attached to timescale slider

protected:
    Gtk::Box          main_box;     // main box
    Gtk::Box          side_box;     // side box

    Gtk::Label        title;        // main title
    Gtk::Button       reset;        // button to reset simulation
    Gtk::Button       clear;        // button to clear bodies
    Gtk::Button       pause;        // button to pause simulation

    Gtk::Box          trail_box;    // box to hold trail length labels
    Gtk::Label        traillabel;   // label for trail length slider
    Gtk::Label        trailspacer;  // spacer for trail length labels
    Gtk::Label        trailvalue;   // label for value of trail length
    Gtk::Scale        traillength;  // controls trail length

    Gtk::Box          frame_box;    // box to hold framerate labels
    Gtk::Label        framelabel;   // label for framerate slider
    Gtk::Label        framespacer;  // spacer for framerate labels
    Gtk::Label        framevalue;   // label for value of framerate
    Gtk::Scale        framerate;    // controls simulation framerate

    Gtk::Box          time_box;     // box to hold time scale labels
    Gtk::Label        timelabel;    // label for time scale slider
    Gtk::Label        timespacer;   // spacer for time scale labels
    Gtk::Label        timevalue;    // label for value of time scale
    Gtk::Scale        timescale;    // controls simulation time scaling

    Gtk::Label        instructions; // label containing instructions for using the program

    Gravity_Draw      gravity_draw; // draws and calculates gravity simulation

    Glib::RefPtr<Gtk::Adjustment> traillength_adjust_ptr; // pointer to adjustment for trailength slider
    Glib::RefPtr<Gtk::Adjustment> framerate_adjust_ptr;   // pointer to adjustment for framerate slider
    Glib::RefPtr<Gtk::Adjustment> timescale_adjust_ptr;   // pointer to adjustment for timescale slider
};

#endif /* GRAVITY_WINDOW_H */