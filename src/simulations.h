#ifndef __SIMULATIONS_H__
#define __SIMULATIONS_H__

#define DECLARE_SIMULATION(NAME)\
    void NAME##_init();\
    void NAME##_update(float dt);\
    void NAME##_render();

DECLARE_SIMULATION(bouncing_ball);
DECLARE_SIMULATION(simple_pendulum);
DECLARE_SIMULATION(double_pendulum);

#endif
