# Self-Driving-Car
Two wheel bluetooth controlled car with automatic safety features and self-driving capabilities.

### Project Desctiption
The project is a self driving car that can also be controlled with a joystick android application through a Bluetooth connection to control the car manually. Bluetooth commands are sent from the Android application to the Bluetooth module and they are processed by the arduino. The Arduino sends the control signals based on the application to the H-Bridge, which generates the required voltage for each motor to either make the car go forward, backward, left or right.

The main program loop communicates with the Ultrasonic module to check the distance of the closest object in the way. If that distance is smaller than the minimum range defined by the program. The program switches off the car motors then switches them backwards for a short period and then the car starts turning into a random direction (left or right) till the distance is further than the safe range defined in the program. Later on the car continues moving based on the last command received before seeing the close object.

### Automatic Mode
The automatic mode is activated by pressing the stop button twice which then makes the car start moving forward at a fixed speed. It relies on the Ultrasonic module to detect things in the way and to avoid it using the same program part used in the manual mode for avoiding close obstacles. 
