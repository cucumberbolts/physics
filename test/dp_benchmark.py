"""
Benchmark to test the accuracy of the
C version in src/doublependulum.c
Code taken from:
https://gist.github.com/lawrenceccheung/3dfa8b710b51be9f9348234f36b5ed8d
"""

import numpy as np
from scipy.integrate import solve_ivp, RK45
from matplotlib import pyplot as plt
import csv


def dp_ode(t: float, y: np.array, l: float, g: float) -> np.array:
    """
    Differential equation for the double pendulum
    """
    t1, t2, w1, w2 = y

    rhs = np.zeros(2)
    rhs[0] = -0.5*l*(w2**2) * np.sin(t1 - t2) - 1.5*g*np.sin(t1)
    rhs[1] =  0.5*l*(w1**2) * np.sin(t1 - t2) - 0.5*g*np.sin(t2)

    lhs = np.zeros((2,2))
    lhs[0][0] = 4.0/3.0*l
    lhs[0][1] = 0.5*np.cos(t1 - t2)
    lhs[1][0] = 1.0/3.0*l
    lhs[1][1] = 0.5*l*np.cos(t1 - t2)

    thdd  = np.linalg.solve(lhs, rhs)
    yd    = np.zeros(4)
    yd[0] = w1
    yd[1] = w2
    yd[2] = thdd[0]
    yd[3] = thdd[1]

    return yd


class SimParams:
    def __init__(self, *,
                 t1_init: float,
                 t2_init: float,
                 w1_init: float,
                 w2_init: float,
                 l: float,
                 g: float,
                 tspan: float,
                 fps: float):
        """
        Parameters:
        -----------
        t1_init: float
            initial theta 1
        t2_init: float
            initial theta 2
        w1_init: float
            initial angular velocity 1
        w2_init: float
            initial angular velocity 2
        l: float
            length of the pendulum bars
        g: float
            acceleration due to gravity
        tspan: float
            simulation time
        fps: float
            timesteps per second
        """
        self.t1_init = t1_init
        self.t2_init = t2_init
        self.w1_init = w1_init
        self.w2_init = w2_init

        self.l = l
        self.g = g

        self.tspan = tspan
        self.fps = fps


def solve_dp(p: SimParams):
    """
    Simulate the double pendulum using Runge-Kutta 4 5
    """
    teval = np.linspace(0, p.tspan, int(p.fps * p.tspan))
    ode_sol4 = solve_ivp(dp_ode, [0, p.tspan],
        [p.t1_init, p.t2_init, p.w1_init, p.w2_init],
        'RK45', args=(p.l,p.g,), t_eval=teval)
    return ode_sol4


def main():
    """
    Main function
    """
    params = SimParams(
        t1_init = 2.0,
        t2_init = 2.2,
        w1_init = 0.0,
        w2_init = 0.0,

        l = 200.0,
        g = 3000.0,

        tspan = 10.0,
        fps = 60.0,
    )

    sol = solve_dp(params)
    plt.figure(figsize=(10,6))
    plt.plot(sol.t, sol.y[0,:]) # Plot theta 1
    plt.plot(sol.t, sol.y[1,:]) # Plot theta 2
    plt.show()

    print(sol.y[0,:])


if __name__ == "__main__":
    main()
