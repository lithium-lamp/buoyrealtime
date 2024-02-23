// buoyrealtime.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <time.h>
#include "step.h"
#include <ctime>

sf::VertexArray getWaveVertexArray(float window_width, float window_height_half, float phi, float A, float o, float time, float spring_initial_position);

int main()
{
    const float m = 1.f, k = 0.6f, r = 0.5f, rho = 1.f, g = 9.82f, phi = 2.f * 3.14f, A = 5.f, o = 3.f, p = 1.f, b = 1.f;// consts
    const std::vector<float> constvec { m, k, r, rho, g, phi, A, o, p, b };

    float theta = 0.f, dL = 2.f, vx = 0.f, vy = 0.f;//variables
    std::vector<float> varvec{ theta, dL, vx, vy };

    

    // create the window
    const float window_width = 800.f;
    const float window_height = window_width;

    const float window_width_half = window_width/2.f;
    const float window_height_half = window_height/2.f;

    float scale_constant = window_width / 40.f;
    float spring_initial_position = 10.f * scale_constant;

    float x_coords = 0.f;
    float y_coords = o;

    float h = 1.f/60.f; // 1/60

    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "My window", sf::Style::Titlebar | sf::Style::Close);

    const float buoy_diameter = 2 * r * scale_constant;

    sf::CircleShape buoy(buoy_diameter); //buoy
    buoy.setFillColor(sf::Color(255,0,0));
    buoy.setPosition(window_width_half - buoy_diameter + x_coords, window_height_half - buoy_diameter - y_coords);

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

    sf::RectangleShape velocity_x(sf::Vector2f(0.f, 0.f)); //velocity x
    sf::RectangleShape velocity_y(sf::Vector2f(0.f, 0.f)); //velocity y

    velocity_x.setFillColor(sf::Color(0, 0, 0));
    velocity_y.setFillColor(sf::Color(0, 0, 0));

    velocity_x.setPosition(window_width_half, window_height_half - o * scale_constant);
    velocity_y.setPosition(window_width_half, window_height_half - o * scale_constant);

    sf::VertexArray curve = getWaveVertexArray(window_width, window_height_half, phi, A, o, 0.f, spring_initial_position); //wave

    time_t currenttime = time(NULL);

    const int start_time = time(&currenttime);

    int last_time_int = start_time;

    window.setFramerateLimit(60);

    int frame = 0; 

    // run the program as long as the window is open
    while (window.isOpen()) {
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

                varvec[0] = std::atan2(x_coords, y_coords);
                varvec[1] = std::sqrt(std::pow(x_coords, 2.f) + std::pow(y_coords, 2.f)) - p;

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

        velocity_x.setSize(sf::Vector2f(varvec[2], 3.f));
        velocity_y.setSize(sf::Vector2f(3.f, -1 * varvec[3]));

        velocity_x.setPosition(window_width_half + x_coords, window_height_half - y_coords);
        velocity_y.setPosition(window_width_half + x_coords, window_height_half - y_coords);

        spring[0].position.x = window_width_half + x_coords;
        spring[0].position.y = window_height_half - y_coords;

        curve = getWaveVertexArray(window_width, window_height_half, phi, A, o, h * frame, spring_initial_position);

        // draw everything here...
        window.draw(curve);
        window.draw(buoy);
        window.draw(force_x);
        window.draw(force_y);
        window.draw(velocity_x);
        window.draw(velocity_y);
        window.draw(spring, 2, sf::Lines);
        
        // end the current frame
        window.display();

        frame++;
    }

    return 0;
}

sf::VertexArray getWaveVertexArray(float window_width, float window_height_half, float phi, float A, float o, float time, float spring_initial_position) {
    sf::VertexArray curve(sf::PrimitiveType::LineStrip, window_width + 2); //wave
    curve.append(sf::Vertex(sf::Vector2f(1.f, window_height_half + spring_initial_position), sf::Color::Blue));
    
    for (int x = 1; x < window_width; x++) {
        curve.append(sf::Vertex(sf::Vector2f(x, -1 * wavepoint(phi, A, o, float(x), time) + window_height_half), sf::Color::Blue));
    }

    curve.append(sf::Vertex(sf::Vector2f(window_width, window_height_half + spring_initial_position), sf::Color::Blue));

    curve.append(sf::Vertex(sf::Vector2f(1.f, window_height_half + spring_initial_position), sf::Color::Blue));

    return curve;
}