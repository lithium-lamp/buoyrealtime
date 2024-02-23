// buoyrealtime.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <time.h>
#include "step.h"
#include <ctime>

int main()
{
    const float m = 1.f, k = 0.01f, r = 0.5f, rho = 1.f, g = 9.82f, phi = 2.f * 3.14f, A = 2.f/10.f, o = 3.f, p = 1.f;// consts
    const std::vector<float> constvec { m, k, r, rho, g, phi, A, o, p };

    float theta = 0.f, dL = 2.f, vx = 0.f, vy = 0.f;//variables
    std::vector<float> varvec{ theta, dL, vx, vy };

    // create the window
    const float window_width = 800.f;
    const float window_height = 600.f;

    const float window_width_half = window_width/2.f;
    const float window_height_half = window_height/2.f;

    float scale_constant = 20.f;
    float spring_initial_position = 5.f * scale_constant;

    float x_coords = 0.f;
    float y_coords = o;

    float h = 0.016666f; // 1/60

    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "My window");

    const float buoy_diameter = 2 * r * scale_constant;

    sf::CircleShape buoy(buoy_diameter); //buoy
    buoy.setFillColor(sf::Color(255,0,0));
    buoy.setPosition(window_width_half - buoy_diameter + x_coords, window_height_half - buoy_diameter - y_coords);

    sf::RectangleShape wave(sf::Vector2f(window_width, 3.f)); //wave
    wave.setFillColor(sf::Color(0, 0, 255));
    wave.setPosition(0, window_height_half - o);

    sf::Vertex spring[] = { //spring
        sf::Vertex(sf::Vector2f(window_width_half, window_height_half)),
        sf::Vertex(sf::Vector2f(window_width_half, window_height_half + spring_initial_position)),
    };

    spring[0].color = sf::Color(0, 0, 0);
    spring[1].color = sf::Color(0, 0, 0);

    sf::RectangleShape force_x(sf::Vector2f(0.f, 0.f)); //force x
    sf::RectangleShape force_y(sf::Vector2f(0.f, 0.f)); //force y

    force_x.setFillColor(sf::Color(0, 255, 0));
    force_y.setFillColor(sf::Color(0, 255, 0));

    force_x.setPosition(window_width_half, window_height_half - o * scale_constant);
    force_y.setPosition(window_width_half, window_height_half - o * scale_constant);

    time_t currenttime = time(NULL);

    const int start_time = time(&currenttime);

    int last_time_int = start_time;

    window.setFramerateLimit(60);

    int frame = 0; 

    // run the program as long as the window is open
    while (window.isOpen()) {
        //int current_time_int = time(&currenttime);

        //if (current_time_int == last_time_int) //to update each second
        //    continue;

        //last_time_int = current_time_int;

        // check all the window's events that were triggered since the last iteration of the loop

        // clear the window with white color
        window.clear(sf::Color::White);


        coords pos = step(constvec, varvec, h, frame);

        x_coords = pos.x_pos;

        y_coords = pos.y_pos;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                float x_mouse = float(event.mouseButton.x);
                float y_mouse = float(event.mouseButton.y);
                
                x_coords = x_mouse - window_width_half;
                y_coords = window_height_half - y_mouse;

                float x_diff = (x_mouse - window_width_half); //window_width_half, 
                float y_diff = (y_mouse - window_height_half + spring_initial_position); //window_height_half + spring_initial_position

                x_diff /= scale_constant;
                y_diff /= scale_constant;

                varvec[0] = std::atan2(x_diff, y_diff);
                varvec[1] = std::sqrt(std::pow(x_diff, 2.f) + std::pow(y_diff, 2.f)) - p; //dL

                varvec[2] = 0.f;
                varvec[3] = 0.f;
            }

            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }

        buoy.setPosition(window_width_half - buoy_diameter + x_coords, window_height_half - buoy_diameter - y_coords);

        force_x.setSize(sf::Vector2f(pos.x_force,3.f));
        force_y.setSize(sf::Vector2f(3.f, -1 * pos.y_force));

        force_x.setPosition(window_width_half + x_coords, window_height_half - y_coords);
        force_y.setPosition(window_width_half + x_coords, window_height_half - y_coords);

        spring[0].position.x = window_width_half + x_coords;
        spring[0].position.y = window_height_half - y_coords;

        // draw everything here...
        window.draw(buoy);
        window.draw(wave);
        window.draw(force_x);
        window.draw(force_y);
        window.draw(spring, 2, sf::Lines);

        // end the current frame
        window.display();

        frame++;
    }

    return 0;
}