#include <stdio.h>
#include "sim_lib.h"

#define NUM_STATE 12
#define NUM_TEST_STEPS 20

static void print_state(float* state) {
    printf("  Rays:     %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f\n",
        state[0], state[1], state[2], state[3], state[4],
        state[5], state[6], state[7], state[8]);
    printf("  Speed:    %.3f  Accel: %.3f  Steering: %.3f\n",
        state[9], state[10], state[11]);
}

int main(void) {

    // --- 1. Init ---
    printf("=== sim_init ===\n");
    // TODO: Replace with your actual track path and start position/heading
    int ok = sim_init("tracks/track_001.txt", 13.0f, 12.3f, 0.0f);
    if (ok != 0) {
        printf("FAIL: sim_init returned %d\n", ok);
        return 1;
    }
    printf("PASS: track loaded\n\n");

    // --- 2. Reset ---
    printf("=== sim_reset ===\n");
    float state[NUM_STATE];
    sim_reset(state);
    printf("Initial state:\n");
    print_state(state);
    printf("\n");

    // --- 3. Step forward with gentle acceleration, no steering ---
    printf("=== sim_step x%d (accel=0.1, steer=0.0) ===\n", NUM_TEST_STEPS);
    float reward;
    int alive, success;
    int survived = 0;

    for (int i = 0; i < NUM_TEST_STEPS; i++) {
        sim_step(0.1f, 0.0f, state, &reward, &alive, &success);
        printf("Step %2d | reward=%.4f alive=%d success=%d\n",
            i + 1, reward, alive, success);
        print_state(state);

        if (!alive || success) {
            printf("  --> Episode ended at step %d (alive=%d success=%d)\n",
                i + 1, alive, success);
            break;
        }
        survived++;
    }
    printf("Survived %d steps\n\n", survived);

    // --- 4. Test reset clears state properly ---
    printf("=== sim_reset (second time) ===\n");
    sim_reset(state);
    printf("State after second reset:\n");
    print_state(state);
    printf("\n");

    // --- 5. Test steering response ---
    printf("=== sim_step x5 (accel=0.2, steer=0.1) ===\n");
    for (int i = 0; i < 5; i++) {
        sim_step(0.2f, 0.1f, state, &reward, &alive, &success);
        printf("Step %2d | reward=%.4f alive=%d success=%d\n",
            i + 1, reward, alive, success);
        print_state(state);
        if (!alive || success) break;
    }
    printf("\n");

    // --- 6. Close ---
    printf("=== sim_close ===\n");
    sim_close();
    printf("PASS: closed cleanly\n");

    return 0;
}