#pragma once

#include <vector>
#include <iostream>

struct coords {
    float x_pos;
    float y_pos;

    float x_force;
    float y_force;
};

coords step(const std::vector<float>& constvec, std::vector<float>& varvec, float h, int frame); //returning fixed points

float wavepoint(float phi, float A, float o, float x, float time); //returning wave height

coords step(const std::vector<float>& cv, std::vector<float>& vv, float h, int frame) {
    //constant
    //cv[0];    m
    //cv[1];    k
    //cv[2];    r
    //cv[3];    rho
    //cv[4];    g
    //cv[5];    phi
    //cv[6];    A
    //cv[7];    o
    //cv[8];    p

    //variable
    //vv[0];    theta
    //vv[1];    dL
    //vv[2];    vx
    //vv[3];    vy

    float x0 = (cv[8] + vv[1]) * std::sinf(vv[0]);
    float y0 = (cv[8] + vv[1]) * std::cosf(vv[0]);

    float Ffy = 0, Ffx = 0, Fflyt = 0, Fs = 0, Fsx = 0, Fsy = 0, FxTot = 0, FyTot = 0, Fg = cv[0] * cv[4];

    float min = wavepoint(cv[5], cv[6], cv[7], x0 - cv[2], float(frame) * h);
    float max = min;

    float d1 = min;

    int count = 11;

    for (int i = 0; i < count - 1; i++) {
        float xpos = (x0 - cv[2] + i * 2 * cv[2]) / float(count);

        float ypos = wavepoint(cv[5], cv[6], cv[7], x0, float(frame) * h);

        if (ypos > max)
            max = ypos;
        if (ypos < min)
            min = ypos;

        d1 += ypos;
    }

    d1 /= float(11);

    if (y0 - cv[2] > max) {
        Fflyt = 0; 
        //std::cout << "reached" << std::endl;
    }
    else if (y0 + cv[2] <= min) {
        Fflyt = cv[3] * cv[4] * std::abs(max - y0);
        
        Ffx = -1 * 1.f;
        //std::cout << "The buoy is below the water line" << std::endl;
    }
    else {
        float A = cv[2] * cv[2];

        if (A >= std::abs(d1 - y0) * cv[2])
            A = (A - std::abs(d1 - y0) * cv[2]) / A;
        else
            A = 0.5f;

        Fflyt = cv[3] * A * cv[4];

        //std::cout << "The buoy is inside of the water line" << std::endl;

        Ffx = -1 * 1.f * A;
    }

    Ffy = (Fflyt);

    Fs = cv[1] * vv[1];
    if (vv[1] <= 0.f)
        Fs = 0;

    Fsx = Fs * std::sinf(vv[0]);
    Fsy = (Fs * std::cosf(vv[0]));

    FxTot = Ffx - Fsx;
    FyTot = Ffy - Fsy - Fg;

    float ax = (1.f / cv[0]) * FxTot;
    float ay = (1.f / cv[0]) * FyTot;

    float vx = vv[2] + h * ax;
    float vy = vv[3] + h * ay;

    vv[2] = vx; vv[3] = vy;

    float x = x0 + h * vx;
    float y = y0 + h * vy;

    float L = std::sqrt(std::pow(x, 2.f) + std::pow(y, 2.f));

    vv[0] = std::atan2f(x, y);

    vv[1] = L - cv[8];

    coords c;

    c.x_pos = L * std::sinf(vv[0]);
    c.y_pos = L * std::cosf(vv[0]);

    c.x_force = FxTot;
    c.y_force = FyTot;

    return c;
}

float wavepoint(float phi, float A, float o, float x, float time) {
    return A * std::sinf(phi * (x + time)) + o;
}