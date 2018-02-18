# ChiliLight

This ESP8266 example measures light intesity (LDR via analog input) and temperature (DS18B20 via "one wire") in a room 
and distribute values periodically via MQTT.

Additionally a GPIO output is used to control a relay to switch light on off. This output is controlled via MQTT, too.

The main control is (switching chili light on/off when light intensity drops below some value) is done in another project via Node-Red.
