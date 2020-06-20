# Automatic Bird Feeding System

An automated feeding system for baby birds.

[First prototype test](https://www.youtube.com/watch?v=i5xlb1sEyUY)

<p align="left">
  <img width="550" src="images/preview.gif">
  <i>Feeding orphaned baby birds is a manual task, which we aim to automate on this project</i>
</p>

## Description and System Overview

<p align="left">
  <img width="550" src="images/block-diagram.png">
</p>

The goal of this project is to automate the process of feeding baby birds. The system overview is shown bellow. It's composed of 4 main parts:

-   The stainless steel food container, that has to be kept cooled
-   The pump system, that pushes the food up to the bird when needed
-   The heating system, so that the food has adequate temperature when it is fed to the bird
-   The robotic arm system, that directs the feeding hose to be near the bird's mouth

We also made a web interface to interact with the system, that can be used on the cellphone or a laptop.

## Architecture

<p align="left">
  <img width="550" src="images/system-diagram.jpeg">
</p>

A Raspberry Pi runs the main controller, that provides a user interface via web server to the system, and communicates with other microcontrollers to perform the main functions.

A PIC microcontroller is used to communicate with temperature sensors, control the Roller Pump as well as the Heating and Cooling elements, using PWM signal.

Finally, an Arduino Due is used to control a robot Arm, via USB communication with the Raspberry Pi, receiving the desired angles for the Arm's servos.

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

### Robot Arm

Is used to direct the feeding hose to the birds mouth.
