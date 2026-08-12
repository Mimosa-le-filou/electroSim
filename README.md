# ElectroSim

A quick 3D electrostatic simulation using C and raylib.\
This tiny project is inspired by [Daniel Hirsch](https://www.youtube.com/watch?v=pbAse_S2-q0&t=2128s&pp=ygURZWxlY3Ryb3N0YXRpYyBzaW0%3D)

## Installation
First, clone this repo:\
git clone https://github.com/Mimosa-le-filou/electroSim

The raylib library is also needed to run the simulation :\

### Arch
sudo pacman -S raylib

### Ubuntu
sudo apt update install raylib

### Other distribution
You probably know how to use your packet manager

### Windows
Hah.

## Running the simulation
Execute the following command:\
	gcc electro.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 && ./a.out

## Usage
The simulation launches a 1000x650 window in which you can do several things:
- zoom in and out using mouse scroll
- Move the camera around the cube using left mouse click
- Create chaos using right mouse click

Enjoy !
