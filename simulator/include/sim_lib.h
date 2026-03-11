#ifndef SIM_LIB_H
#define SIM_LIB_H

int  sim_init(const char* track_filename, float car_start_x, float car_start_y, float car_start_heading);
void sim_reset(float* state_out);
void sim_step(float delta_accel, float delta_steering, float* state_out, float* reward_out, int* alive_out, int* success_out);
void sim_get_state(float* state_out);
void sim_close(void);

#endif