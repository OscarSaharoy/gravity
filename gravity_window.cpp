#include "gravity_window.h"
#include <glibmm/main.h>
#include <cairomm/context.h>
#include <gtkmm/cssprovider.h>
#include <iostream>
#include <algorithm>
#include <string>

#define _USE_MATH_DEFINES
#include <cmath>

Gravity_Window::Gravity_Window() :
    side_box(Gtk::ORIENTATION_VERTICAL, 0),

    title(       "Gravity"      ),
    reset(       "Reset"        ),
    clear(       "Clear"        ),
    pause(       "Pause"        ),
    traillabel(  "Trail Length" ),
    trailvalue(  "360"          ),
    framelabel(  "Framerate"    ),
    framevalue(  "60.0"         ),
    timelabel(   "Time Scale"   ),
    timevalue(   "1.00"         ),

    instructions("Move around by dragging,\nzoom with scroll wheel;\nto add new bodies right\nclick and hold, then drag."),

    framerate(Gtk::ORIENTATION_HORIZONTAL),
    timescale(Gtk::ORIENTATION_HORIZONTAL) {

    // set window border and title
    set_border_width(10);
    set_title("Gravity");
    set_default_size(1000, 800);

    // place main_box in window and side_box in main_box
    add(main_box);
    side_box.set_margin_left(15);
    main_box.pack_start(side_box, Gtk::PACK_SHRINK, 0);

    /* add widgets to boxes */

    // setup title label
    title.set_markup("<span font_weight=\"bold\" font_size=\"larger\">Gravity</span>");
    title.set_width_chars(20);
    side_box.pack_start(title, Gtk::PACK_SHRINK, 0);

    // setup reset button
    reset.set_margin_top(15);
    side_box.pack_start(reset, Gtk::PACK_SHRINK, 0);
    reset.signal_clicked().connect( [this](){reset_sim();} );

    // setup clear button
    clear.set_margin_top(5);
    side_box.pack_start(clear, Gtk::PACK_SHRINK, 0);
    clear.signal_clicked().connect( [this](){gravity_draw.clear();} );

    // setup pause button
    pause.set_margin_top(5);
    side_box.pack_start(pause, Gtk::PACK_SHRINK, 0);
    pause.signal_clicked().connect( [this](){toggle_pause();} );

    // setup trail scale labels
    trail_box.set_margin_top(15);
    side_box.pack_start(trail_box,    Gtk::PACK_SHRINK, 0);
    trail_box.pack_start(traillabel,  Gtk::PACK_SHRINK, 0);
    trail_box.pack_start(trailspacer, Gtk::PACK_EXPAND_WIDGET, 0);
    trail_box.pack_start(trailvalue,  Gtk::PACK_SHRINK, 0);
    
    // setup trail scale
    side_box.pack_start(traillength, Gtk::PACK_SHRINK, 0);
    traillength_adjust_ptr = Gtk::Adjustment::create(360.0, 0.0, 1001.0, 1.0, 1.0, 1.0);
    traillength.set_adjustment(traillength_adjust_ptr);
    traillength_adjust_ptr->signal_value_changed().connect( [this](){traillength_changed();} );
    traillength.set_draw_value(false);

    // setup framerate scale labels
    side_box.pack_start(frame_box,    Gtk::PACK_SHRINK, 0);
    frame_box.pack_start(framelabel,  Gtk::PACK_SHRINK, 0);
    frame_box.pack_start(framespacer, Gtk::PACK_EXPAND_WIDGET, 0);
    frame_box.pack_start(framevalue,  Gtk::PACK_SHRINK, 0);
    
    // setup framerate scale
    side_box.pack_start(framerate, Gtk::PACK_SHRINK, 0);
    framerate_adjust_ptr = Gtk::Adjustment::create(60.0, 5.0, 145.0, 0.1, 1.0, 1.0);
    framerate.set_adjustment(framerate_adjust_ptr);
    framerate_adjust_ptr->signal_value_changed().connect( [this](){framerate_changed();} );
    framerate.set_draw_value(false);

    // setup time scale labels
    side_box.pack_start(time_box,   Gtk::PACK_SHRINK, 0);
    time_box.pack_start(timelabel,  Gtk::PACK_SHRINK, 0);
    time_box.pack_start(timespacer, Gtk::PACK_EXPAND_WIDGET, 0);
    time_box.pack_start(timevalue,  Gtk::PACK_SHRINK, 0);

    // setup time scale
    side_box.pack_start(timescale, Gtk::PACK_SHRINK, 0);
    timescale_adjust_ptr = Gtk::Adjustment::create(1.0, -1.0, 11.0, 0.1, 1.0, 1.0);
    timescale.set_adjustment(timescale_adjust_ptr);
    timescale_adjust_ptr->signal_value_changed().connect( [this](){timescale_changed();} );
    timescale.set_draw_value(false);

    // setup instructions label
    side_box.pack_start(instructions, Gtk::PACK_SHRINK, 0);

    // setup gravity_draw pane
    gravity_draw.set_margin_left(25);
    gravity_draw.set_hexpand(true);
    gravity_draw.set_vexpand(true);
    main_box.pack_start(gravity_draw);


    show_all_children();
}

void Gravity_Window::reset_sim() {

    // resets slider values back to intial and then calls method to reset members of gravity_draw
    traillength_adjust_ptr->set_value(360.0);
    framerate_adjust_ptr->set_value(60.0);
    timescale_adjust_ptr->set_value(1.0);
    gravity_draw.reset();
}

void Gravity_Window::toggle_pause() {

    // toggle the value of the pause variable and the text on the button
    gravity_draw.paused = !gravity_draw.paused;
    pause.set_label(gravity_draw.paused ? "Unpause" : "Pause");
}

void Gravity_Window::traillength_changed() {
    
    // gets the value from the adjustment pointer and sets the traillength label to the new value
    gravity_draw.traillength = traillength_adjust_ptr->get_value();
    std::string text = std::to_string(gravity_draw.traillength);
    trailvalue.set_label(text);
}

void Gravity_Window::framerate_changed() {

    // gets the value from the adjustment pointer, calls the method of gravity_draw to respond to the
    // change and sets the framevalue label to the first 4 characters of the new value
    gravity_draw.framerate = framerate_adjust_ptr->get_value();
    gravity_draw.framerate_changed();
    std::string text = std::to_string(gravity_draw.framerate).substr(0, 4);
    framevalue.set_label(text);
}

void Gravity_Window::timescale_changed() {

    // gets the value from the adjustment pointer and sets the framevalue label to
    // the first 4 characters of the new value
    gravity_draw.timescale = timescale_adjust_ptr->get_value();
    std::string text = std::to_string(gravity_draw.timescale).substr(0, 4);
    timevalue.set_label(text);
}



Gravity_Draw::Gravity_Draw() :
    paused(false),
    mouse_info{0, 0, 0, 0, false, 0},
    background{0, 0, 0},
    framerate(60.0),
    timescale(1.0) {

    // set members to intial values
    reset();

    // triggers the on_timeout method each frame
    timeout_connection = Glib::signal_timeout().connect( sigc::mem_fun(*this, &Gravity_Draw::on_timeout), 1000.0/framerate );

    // lets the widget detect the events it needs to
    set_events(Gdk::SCROLL_MASK | Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);    
}

void Gravity_Draw::reset() {

    // set variables to initial values
    pos         = {{  0,   0}, {  0, 100}, {100,   0}, { 100, 100}};
    vel         = {{1.2,   0}, {  0,-1.2}, {  0, 1.2}, {-1.2,   0}};
    mass        = {1.0, 1.0, 1.0, 1.0};
    trails      = {{}, {}, {}, {}};
    zoom        = 1.0;
    centre      = {350, 320};
    traillength = 360;
}

void Gravity_Draw::clear() {

    // remove all bodies
    pos    = {};
    vel    = {};
    mass   = {};
    trails = {};

    // set background to white causing a flash upon clearing bodies
    background = {1.0, 1.0, 1.0};
}

bool Gravity_Draw::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {

    // reduce value of background toward black to provide flash after clearing bodies
    if( background[0] != 0.0) {

        background[0] -= 0.05;
        background[1] -= 0.05;
        background[2] -= 0.05;
    }

    // paint background
    cr -> set_source_rgb(background[0], background[1], background[2]);
    cr -> paint();

    // set line cap style
    cr -> set_line_cap(Cairo::LINE_CAP_ROUND);

    // for each body, draw the trail
    for(int i = 0; i<pos.size(); i++) {

        cr -> set_source_rgb(1.0, 1.0, 1.0);
        cr -> set_line_width(1*zoom);
        cr -> move_to(centre[0] + trails[i][0][0]*zoom, centre[1] + trails[i][0][1]*zoom);

        cr -> set_source_rgba(1.0, 1.0, 1.0, 0.65);
        int trail_actual_length = trails[i].size();

        for(int j = 0; j<trail_actual_length-1; j++) {

            cr -> line_to(centre[0] + trails[i][j+1][0]*zoom, centre[1] + trails[i][j+1][1]*zoom);
        }

        cr -> stroke();
    }

    // for each body, draw the body
    for(int i = 0; i<pos.size(); i++) {

        cr -> set_line_width(5*zoom);
        cr -> arc(centre[0] + pos[i][0]*zoom, centre[1] + pos[i][1]*zoom, 10*mass[i]*zoom, 0, M_PI*2);
        cr -> set_source_rgb(0.0, 0.0, 0.0);
        cr -> fill_preserve();
        cr -> set_source_rgb(0.4, 0.8, 0.1);
        cr -> stroke();
    }

    // if the mouse is right clicked...
    if(mouse_info.clicked) {

        // if the mouse is still at the place it was clicked then increment click time
        if(mouse_info.click_x == mouse_info.x & mouse_info.click_y == mouse_info.y) {

            mouse_info.click_time += 0.03;
        }

        // draw a blue line from where the click happened to the current mouse position
        cr -> set_source_rgb(0.1, 0.6, 0.8);
        cr -> move_to(mouse_info.click_x, mouse_info.click_y);
        cr -> line_to(mouse_info.x, mouse_info.y);
        cr -> stroke();

        // draw a blue circle where the click happened 
        cr -> arc(mouse_info.click_x, mouse_info.click_y, 10*mouse_info.click_time*zoom, 0, M_PI*2);
        cr -> set_source_rgb(0.0, 0.0, 0.0);
        cr -> fill_preserve();
        cr -> set_source_rgb(0.1, 0.6, 0.8);
        cr -> stroke();
    }

    return true;
}

bool Gravity_Draw::on_timeout() {

    // advance simulation if it isnt paused
    if(!paused){

        store_trails();
        join();
        gravity();
    }

    // schedule drawing area to be redrawn
    queue_draw();

    return true;
}

bool Gravity_Draw::on_scroll_event(GdkEventScroll * scroll_event) {

    // store previous zoom level for arithmatic later
    prev_zoom  = zoom;

    // zoom in or out depending on scroll_event->direction - 0 when scrolling down and 1 when scrolling up
    zoom      *=  scroll_event->direction ? 0.95 : 1.05;

    // move centre of coordinate system such that the zoom is centred on the mouse position
    centre[0] += (scroll_event->x - centre[0])/zoom * (prev_zoom-zoom);
    centre[1] += (scroll_event->y - centre[1])/zoom * (prev_zoom-zoom);

    return true;
}

bool Gravity_Draw::on_button_press_event(GdkEventButton * button_event) {

    // if mouse button 3 is pressed, store mouse info
    if(button_event->button == 3) {

        mouse_info.click_x = button_event->x;
        mouse_info.click_y = button_event->y;
        mouse_info.clicked = true;
    }

    return true;
}

bool Gravity_Draw::on_button_release_event(GdkEventButton * button_event) {

    // is mouse button 3 is released...
    if(button_event->button == 3) {
    
        // calculate coordinates of new body from screen coordinates of mouse click
        double nx = (mouse_info.click_x - centre[0])/zoom;
        double ny = (mouse_info.click_y - centre[1])/zoom;

        // calculate velocity of new body from the drag of the mouse while right clicked
        double dx = (mouse_info.x - mouse_info.click_x)/50;
        double dy = (mouse_info.y - mouse_info.click_y)/50;

        // add new body by adding new element to vectors storing body info
        pos.push_back({nx, ny});
        vel.push_back({dx, dy});
        mass.push_back(mouse_info.click_time);
        trails.push_back({});

        // store mouse info
        mouse_info.clicked    = false;
        mouse_info.click_time = 0.0;
    }

    return true;
}

bool Gravity_Draw::on_motion_notify_event(GdkEventMotion * motion_event) {

    // if mouse button 1 is pressed...
    if(motion_event->state & GDK_BUTTON1_MASK) {

        // calculate motion of mouse in x and y
        double dx  = motion_event->x - mouse_info.x;
        double dy  = motion_event->y - mouse_info.y;
    
        // shift centre of coordinate system by this amount to drag simulation around
        centre[0] += dx;
        centre[1] += dy;
    }

    // store new mouse position
    mouse_info.x = motion_event->x;
    mouse_info.y = motion_event->y;

    return true;
}

void Gravity_Draw::framerate_changed() {

    // change the timing of the on_timout event to match the new framerate
    timeout_connection.disconnect();
    timeout_connection = Glib::signal_timeout().connect( sigc::mem_fun(*this, &Gravity_Draw::on_timeout), 1000.0/framerate );
}

void Gravity_Draw::store_trails() {

    // for each body...
    for(int i = 0; i<pos.size(); i++) {

        // push current position onto the front of the deque
        trails[i].push_front(pos[i]);

        // if the length of the deque is greater than the desired trail length, remove an item from the end of the deque
        // using a loop to remove up to 3 items
        for(int j = 0; j<3; j++) {
            if(trails[i].size() > traillength) {
    
                trails[i].pop_back();
            }
        }
    }
}

void Gravity_Draw::gravity() {

    // for each combination of bodies...
    for(int i1 = 0; i1 < pos.size(); i1++) {
        for(int i2 = i1+1; i2 < pos.size(); i2++) {

            double dx   = pos[i1][0]-pos[i2][0]; // x distance
            double dy   = pos[i1][1]-pos[i2][1]; // y distance
            double d    = sqrt( dx*dx + dy*dy ); // total distance

            // if distance > 2000, skip calculations
            if(d > 2000) { continue; } 

            // force on each body
            double f    = 100 * mass[i1]*mass[i2] / (d*d); 
         
            double s    = dy/d; // sine of angle
            double c    = dx/d; // cosine of angle
          
            vel[i1][0] -= f*c/mass[i1] * 60/framerate * timescale; // applying accelaration to bodies
            vel[i1][1] -= f*s/mass[i1] * 60/framerate * timescale;    
            vel[i2][0] += f*c/mass[i2] * 60/framerate * timescale;
            vel[i2][1] += f*s/mass[i2] * 60/framerate * timescale;
        }
            
        pos[i1][0] += vel[i1][0] * 60/framerate * timescale; // move body by velocity
        pos[i1][1] += vel[i1][1] * 60/framerate * timescale;
    }
}

void Gravity_Draw::join() {

    for(int i1 = 0; i1 < pos.size(); i1++) {
        for(int i2 = i1+1; i2 < pos.size(); i2++) {

            double dx     = pos[i1][0]-pos[i2][0]; // x distance
            double dy     = pos[i1][1]-pos[i2][1]; // y distance
            double d      = sqrt( dx*dx + dy*dy ); // total distance

            // if the distance between the centres is smaller than the sum of the radii of the bodies they collide
            if(d < 10*(mass[i1] + mass[i2])) {

                // calculate x and y momentum
                double px = mass[i1]*vel[i1][0] + mass[i2]*vel[i2][0];
                double py = mass[i1]*vel[i1][1] + mass[i2]*vel[i2][1];
            
                // total mass
                double m  = mass[i1] + mass[i2];
            
                // conservation of momentum
                double vx = px/m;
                double vy = py/m;

                // l is index of larger body and s is index of smaller body - larger absorbs smaller
                int l     = (mass[i1] == std::max(mass[i1], mass[i2])) ? i1 : i2;
                int s     = (l == i1) ? i2 : i1;
            
                // set new values in mass and vel vectors
                mass[l]   = m;
                vel[l]    = {vx, vy};
            
                // erase values associated with absorbed body
                pos.erase(    pos.begin()    + s );
                vel.erase(    vel.begin()    + s );
                mass.erase(   mass.begin()   + s );
                trails.erase( trails.begin() + s );
            }
        }
    }
}