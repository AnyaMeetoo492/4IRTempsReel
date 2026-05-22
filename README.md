# 4IRTempsReel

## Overview

**4IRTempsReel** is a project focused on real-time robotics simulation and control using Webots. It provides an example of connecting a custom C client (“client.c”) to a Webots simulation, demonstrating robot command and control over TCP/IP. The repository is organized by Webots conventions and is intended for educational or prototyping purposes in the field of real-time embedded systems and robotics.

---

## Directory Structure

```
webots_insa_world/
│
├── client/
│   ├── client.c      # C program for TCP client controlling robot
│   └── Makefile      # To build the client
│
├── controllers/
│   └── my_controller/
│       ├── my_controller.c  # C Controller for Webots robot
│       ├── Makefile
│       └── build/           # Build output folder
│
└── worlds/
    └── insa_tp_temps_reel_2025.wbt  # Webots world file
```

- **client/** contains the C TCP client to connect to the robot simulation.
- **controllers/my_controller/** contains a custom Webots controller implementation in C.
- **worlds/** has the main simulation environment file for Webots.

---

## How It Works

- The Webots simulation (in the `worlds/` folder) creates a virtual environment.
- The C client (`client.c`) connects to a TCP/IP server (expected to be running inside the Webots controller or another process) at `127.0.0.1:10020`.
- The client sends robot commands (“move”; “turn”, etc.) to the simulation at regular intervals using multiple threads.

---

## Getting Started

### Prerequisites

- [Webots](https://cyberbotics.com/) installed
- GCC or compatible C compiler (for client/controller)
- Basic knowledge of Linux and networking

### Build the Client

```sh
cd webots_insa_world/client
make
```

This will produce an executable (`client`) that can connect to the simulation.

### Running the Simulation

1. Open Webots and load the world:  
   `webots_insa_world/worlds/insa_tp_temps_reel_2025.wbt`

2. Ensure the controller in `webots_insa_world/controllers/my_controller/` is selected for the appropriate robot in the world.

3. Build the controller if necessary:
   ```sh
   cd webots_insa_world/controllers/my_controller
   make
   ```

### Running the Client

Once the Webots simulation is running and the TCP/IP server is ready, execute the client:

```sh
./client
```

- The client will connect to `127.0.0.1:10020` and start sending commands to the robot.
- Watch the Webots simulation to see the effect.

---

## Customization

- To change robot movement, edit `client.c` (see `command` structs with 'M' [move] and 'T' [turn]).
- To use a different server IP/port, edit `SOCKET_SERVER` and `SOCKET_PORT` defines in `client.c`.
- To modify robot behavior, edit the controller in `controllers/my_controller/my_controller.c`.

---

## Troubleshooting

- Ensure no firewall blocks TCP/IP localhost communication.
- Verify the Webots simulation is running before starting the client.
- Rebuild any modified C code with `make` before running.
