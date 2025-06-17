# Arduino-based Remote Physics Experiment Apparatus (Freefall &amp; Inclined Plane)
**Hardware part of the Prototype submitted for the Group Undergraduate Thesis for Computer Engineering Degree completion.**
**What I'm assigned to:**
- Design of the Inclined Plane apparatus based on the research leader's proposed functionalities.
- Schematic Wiring of the two apparatus.
- Implementing the arduino code of Free Fall and Inclined Plane apparatus.
- Aided my co-researchers in maintaining seamless remote control of hardware through accessing it in the developed Django-based website made by my co-researchers.

## Constructed Prototype 
<img src="https://github.com/leantdr04/arduino-thesis-repo/blob/main/physics%20experiment%20apparatus.png" width="500" height="500"/> <img src="https://github.com/leantdr04/arduino-thesis-repo/blob/main/Apparatus%20setup.png" width="300" height="300"/>

Each hardware is separately powered by its own Arduino Microcontroller, Nema Stepper motor, needed motor drivers and modules, and arduino sensors.


---


**Free Fall**:   Can choose the distance where the ball will be dropped. They are given the option to drop the ball or reset the position to initial.
- Code calculates the time it took from ball is released to ball hits the cushion. Sensed by vibration sensor.

**Inclined Plane**: Users can choose the angle of the platform. They are given the option to roll the ball or reset the position to initial.
- Code calculates the time from initial ball position to other end of the platform. Sensed by IR sensor.
