#include "physics.h"
#include "physics_constants.h"
#include "car_internals.h"
#include <math.h>
#include <stdio.h>

void normalize_heading(float *heading);
float clamp(float x, float min, float max);
static void clamp_forward_speed(Car *car, float fx, float fy);

void update_car_physics(Car *car, float acceleration, float steering_angle, const float dt) {
    if (car == NULL || !car->is_alive) {
        return; // Invalid car pointer or car is not alive
    }

    float delta_acceleration = acceleration - car->acceleration;
    delta_acceleration = clamp(delta_acceleration, -MAX_DELTA_ACCELERATION, MAX_DELTA_ACCELERATION);

    float delta_steering = steering_angle - car->steering_angle;
    delta_steering = clamp(delta_steering, -MAX_DELTA_STEERING, MAX_DELTA_STEERING);

    car->acceleration = clamp(car->acceleration + delta_acceleration, -MAX_ACCELERATION, MAX_ACCELERATION);

    car->steering_angle = clamp(car->steering_angle + delta_steering, -MAX_STEERING_ANGLE, MAX_STEERING_ANGLE);

    // Velocity = Acceleration in Direction * Time Step * Drag Coefficient normalized to [min speed, max speed]
    float accel_x = car->acceleration * cosf(car->heading);
    float accel_y = car->acceleration * sinf(car->heading);
    car->velocity.x = (car->velocity.x + accel_x*dt) * DRAG_COEFFICIENT;
    car->velocity.y = (car->velocity.y + accel_y*dt) * DRAG_COEFFICIENT;

    float fx = cosf(car->heading);
    float fy = sinf(car->heading);
    float v_forward = car->velocity.x * fx + car->velocity.y * fy;

    // Lateral Friction
    float lateral_x = -fy;  // perpendicular to heading
    float lateral_y = fx;
    float v_lateral = (car->velocity.x * lateral_x + car->velocity.y * lateral_y) * FRICTION_COEFFICIENT;
    car->velocity.x = v_forward * fx + v_lateral * lateral_x;
    car->velocity.y = v_forward * fy + v_lateral * lateral_y;

    clamp_forward_speed(car, fx, fy);

    // Heading = Steering Angle * speed * turn factor * time step normalized to [0, 2pi]
    car->angular_velocity = car->steering_angle * car->speed * TURN_FACTOR;
    car->heading += car->angular_velocity * dt;
    normalize_heading(&car->heading);

    car->position.x += car->velocity.x * dt;
    car->position.y += car->velocity.y * dt;

    car->time_alive += dt;
    
}

float clamp(float x, float min, float max) {
    if (x > max)
        return max;
    if (x < min) 
        return min;
    return x;
}

void normalize_heading(float* heading) {
    float two_pi = PI * 2;
    *heading = fmodf(*heading, two_pi);
    if (*heading < 0)
        *heading += two_pi;
}

static void clamp_forward_speed(Car *car, float fx, float fy) {
    float v_forward = car->velocity.x * fx + car->velocity.y * fy;
    float clamped = v_forward;
    if (clamped > MAX_FORWARD_SPEED) clamped = MAX_FORWARD_SPEED;
    if (clamped < -MAX_REVERSE_SPEED) clamped = -MAX_REVERSE_SPEED;

    float delta = clamped - v_forward;

    // add delta only along forward direction
    car->velocity.x += delta * fx;
    car->velocity.y += delta * fy;

    // update car->speed as signed speed if you want it signed
    car->speed = clamped;
}
