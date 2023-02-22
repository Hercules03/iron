# Reminders
Please download and include the following libraries:
VarSpeedServo --> better function for servo --> control the servo with speed, angle, and whether it will turn till it's done or not
ButtonEvents(Optional) --> Obtaining tap, double-tap, and press-and-hold events for buttons (Requires Library Bounce2)
Bounce2(Optional) --> Filter undesired false states (similar to noise) caused by slightly vibration when close or open buttons and switches
IRremote --> Library for the IR sensor
SoftwareSerial --> Allows serial communication on other digital pins of an Arduino board

# INSTALLATION

In Arduino, select SKETCH>IMPORT LIBRARY...>ADD LIBRARY... and find the .zip file
This will install the library in your My Documents (Windows) or Documents (Mac) folder under Arduino/libraries

Put the "ButtonEvents" folder in your Arduino "libraries" folder. To identify this location from the Arduino IDE, open "menubar-> File -> Preferences".
Select "menubar -> Sketch -> Import Library -> ButtonEvents" to import the library into your sketch. An "#include ButtonEvents.h" line will appear at the top of your Sketch.