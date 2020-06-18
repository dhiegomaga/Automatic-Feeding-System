# Automatic Bird Feeding System

_An automated feeding system for orphaned baby birds._

<p align="left">
  <img width="550" src="images/preview.gif">
</p>

## Description and System Overview

The goal of this project is to automate the process of feeding baby birds. The system overview is shown bellow. It's composed of 4 main parts:

-   The stainless steel food container, that has to be kept cooled
-   The pump system, that pushes the food up to the bird when needed
-   The heating system, so that the food has adequate temperature when it is fed to the bird
-   The robotic arm system, that directs the feeding hose to be near the bird's mouth

We also made a web interface to interact with the system, that can be used on the cellphone or a laptop.

<p align="left">
  <img width="550" src="images/system-diagram.jpeg">
</p>

## Modules

### Webserver

We make a user-friendly interface via web server to control the system and access the variables

<p align="left">
  <img height="300" src="images/webserver.png">
</p>

### Electric Circuit

<p align="left">
  <img width="550" src="images/circuit.jpeg">
</p>

We created a PCB to solder the main electrical components, including the microcontroller, the PWM control signal pins for the Pump, Heater and Cooling elements, as well as MOSFET's for supplying current.
