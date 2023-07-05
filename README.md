# IoT-Smartwatch Project Tutorial
Author: Charlotta Larmoijeur
StudentID: cl224dp

# Introduction
In this tutorial I will go through how to make a diy IoT smartwatch! This is a project that I wanted to tackle for a while, since I really liked the idea of making an open-source fully customizable smartwatch that I could build my own functionalities into. For now this will be step counting, weather forcast information and Infrared signal decoding and sending. The watch will then sync its data to a local Home Assistant server and with the help of MQTT, InfluxDB and Grafana you should be able to see the step count data in a visualized form. This will be useful to see which days you reach your step goals.

This may not be the most feature-packed version of a smartwatch, but it does have the potential to be and the fun part is being to experiment with it and customize it how you want.
            
            
Apporoximate timeframe: Weekend project (assumes experience with Arduino and Home assistant)
* [Introduction](#Introduction)
* [Material](#Material)
* [Computer setup](#Computer-Setup)
  [-Setting up Arduino IDE](#Setting-up-the-Arduino-IDE:)
* [Trial and ERROR, ERROR, ERROR…](#Trial-and-ERROR,-ERROR,-ERROR…)
* [Communication and visualization](#Communications-and-visualization)
* [Code details](#Code-details)
    * [Connectivity](#Connectivity)
    * [Step Counter](#Step-Counter)
    * [Weather Forecast](#Weather-Forecast)
    * [Infrared Decoder & Sender](#Infrared-Decoder-&-Sender)
    * [Battery Power](#Battery-Power)
*  [Limiting factors and future considerations](#Limiting-factors-and-future-considerations)
* [Final result](#Final-result)




### Material

| Components | Picture | Description | Price and retailer: |
| --- | --- | --- | --- |
| Circuit Playground Express (CPE) | <img src="https://hackmd.io/_uploads/S1Ns-fxt3.jpg"> | Has many built in sensors (thermometer, accelerometer, color senso,microphone etc), which is why I chose it. | 349.00 SEK ([electrokit](https://www.electrokit.com/produkt/adafruit-circuit-playground-express/)) |
| ESP8266 ESP-01 WIFI-module | <img src="https://hackmd.io/_uploads/SkfhNMeF3.jpg"> | Simple ESP8266 based wifi module | 49.00 SEK ([electrokit](https://www.electrokit.com/produkt/esp-01-esp8266-wifi-modul/)) |
| Waveshare 1.28" LCD | <img src="https://hackmd.io/_uploads/rk_CBGlFn.jpg"> | Simple LCD with GC9A01A driver. | 255.99 SEK ([Amazon](https://amzn.eu/d/9QgQvLU)) |
| Wire | <img src="https://hackmd.io/_uploads/ryz__MxFn.jpg"> | You do not need any specific kind, but thin wire is easier since it saves some space in the housing. (I used some AWG26 silicone wire) | 3.50kr/meter ([electrokit](https://www.electrokit.com/produkt/kopplingstrad-awg26-silikon-rosa-m/)) |
| M2 screws, nuts and washers | <img src="https://hackmd.io/_uploads/ryaJsGeKh.jpg"><img src="https://hackmd.io/_uploads/HJmhczxFh.jpg"><img src="https://hackmd.io/_uploads/HJVKcGxKn.jpg"> | I designed the 3D model to make space for M2 nuts so that the CPE can be easily assembled and disassembled from the case without desoldering. | |

### Tools for Assembly:

| Items for assembly  | Picture | Description |
| -------- | -------- | -------- |
| 3D printer    | ![](https://hackmd.io/_uploads/Sy6qvGxKn.png)   | I designed and 3D printed a case for my smartwatch, so having acess to a 3D printer is neccessary in order to assemble it. 
| Soldering iron    | ![](https://hackmd.io/_uploads/H1m5oMxtn.jpg)| Need to solder connections together.
|3D case model| ![](https://hackmd.io/_uploads/BJjqkmxYh.jpg)| The model I created for the smartwatch (made with Fusion360)    *Full model & download link to be added later.











### Computer setup
I mainly used the Arduino IDE for programming my device. The Circut Playground Express(CPE) is supported by both CircuitPython and Arduino, but there is more of a community around arduino and many available code libraries as well, so this made it an easy decision to use the Arduino IDE since it is very simple to use and also has built in functionality for flashing.

Everything was set up on my Linux PC (running Pop! OS v22.04 LTS). Installing the arduino IDE can be done by downloading the correct version for your OS from: https://www.arduino.cc/en/software.

***Setting up the Arduino IDE:***
1.   Arduno Boards Manager:  
    In order to add the Circuit Playground Express and ESP8266 board support to the IDE, you need to include some custom sources in the IDE preferences. Go to *file> Preferences > 'Additional Board Manager URLs*' and add the following two lines:
    http://arduino.esp8266.com/stable/package_esp8266com_index.json
https://adafruit.github.io/arduino-board-index/package_adafruit_index.json .
Now you should be able to choose both the 'Circuit Playground Express' or the 'Generic ESP8266 module' from the board manager option.
2. Library Manager:
You will also need some additional libraries installed in your code. 
The ones used in the project code are:
  
    *  ESP8266WiFi
    *  PubSubClient
    *  Adafruit_GFX
    *  Adafruit_GC9A01A 
    *  Adafruit_SPIFlash 
    *  Adafruit_ImageReader 
    *  Adafruit_CircuitPlayground
    *  RTCZero
    
You can open the library manager from *Tools > Manage Library* and add neccessary lirbaries there by searching for the ones mentioned above.





------
------



### Trial and ERROR, ERROR, ERROR....    
The ESP-01 wifi module comes preinstalled with AT command firmware, which allows you to send commands over the serial interface to the ESP-01 that then are executed on the module. The firmware version that was installed did however not include support for MQTT-related commands, which I was going to need later, so I went on a side-quest to try and build and compile my own AT command firmware for it, but this took several days of trial and error and for some reason still did not end up working. 
So I decided instead to flash arduino code on to it, since that was more familiar to me. 


To be able to flash the module, you will need some kind of USB- to serial converter adapter. Unfortunately I did not realize this until after the parts had arrived and so I actually used the CPE as a diy adapter instead. This is how it was wired for flashing:
![](https://hackmd.io/_uploads/BkDjFwgYh.jpg)

        *Diagram1 - Wiring for using the CPE as a serial to USB converter. *
        
        
 In  the diagram above  the connections are as follows: 
     

|ESP-01    | Circuit Playground Express   | 
| -------- | -------- | 
| 3.3V & EN(also labled CH_PD)     | 3.3V     | 
|GND|GND
|RX|TX
|TX|RX
|GP0|GND (this sets the device to flashing mode)

After flashing the firmware to the CPE you connect all the parts together as shown below: 

![](https://hackmd.io/_uploads/HJKKSwxF2.jpg)

        *Diagram2 - Final circuit wiring (note:battery is actually 150mAh)*    


Additional notes about wiring. When flashing the module I left the pre-soldered headers on the ESP-01 as is, but once I was ready for final assembly I desoldered them to save some space. This was the final connection of the two:
![](https://hackmd.io/_uploads/BkB2JOlF3.jpg)

        *Glued the boards together to keep them in place.

### Communication and visualization
**Home Assistant (HA):**
I set up a Home Assistant server on my local network, because I had used it before and thought it would be fun to integrate my smartwatch with this as well. Another reason that this is convenient is that it will integrate together with all my other smart home devices and since I have the Home Assistant app on my phone as well I can view my stats there too. I installed Home Assistant on a Raspberry pi 3B. You can find instructions on how to do this [here](https://www.home-assistant.io/installation/raspberrypi).

Then when you have set up the home assistant server. You can install the neccessary add-ons for communication with the device.

* **MQTT:**
    In order to send the data from the smartwatch to my home assistant server I decided to use the MQTT protocol. To do this you have to set up a MQTT broker in Home Assistant. Luckily there are already add-ons for this so the process is realtively easy. 
    
    Go to the *Settings > Add-ons > 'Add-on store'* and search for Mosquitto broker. There should only be one option so install it and then press start. You should not have to edit anything in the configuration tab. 
  Then go to the *Settings > 'Devices and Services' > 'Add Integration'*, search for MQTT and add that as well. and  Then go to HA settings and add a new user that will be used for the MQTT login. This username and information should also be added to the MQTT device configuration settings as well as in the arduino code that is flashed to the ESP-01(See more details in the code section).This is what will allow the smartwatch to publish MQTT messages to the broker. Then the following should be added to your configuration.yaml file: 
    
```
        mqtt:
            sensor:
              - state_topic: "outTopic"  #Is the topic name that the ESP-01 publishes to(as defined in arduino code)
                name: "my_sensor"
                value_template: "{{ value }}"
                unique_id: "ESP8266_my_sensor"
                device:
                  identifiers: "ESP8266_my_sensor"
                  name: "ESP8266 Sensor"
                  model: "ESP8266"
                  manufacturer: "Espressif"
```
Make sure to restart your HomeAssistant device after all of this to make sure it is activated. Then you should see the newly defined MQTT sensor show up under the devices tab and you should also be able to add it to your dashboard. That should look something like this:
![](https://hackmd.io/_uploads/ryVT9ceY2.png)
*The my_sensor value is just the latest mqtt message published to the chosen topic(in this case "setup done" was just a part of my test-code, later it should display an actual step count.)

* **InfluxDB:**
    By adding InfluxDB you can create a database in which the data sent from the device will be stored long-term(until deleted or storage space runs out) and in which we can preform SQL-(like)queries to search the database and analyze data in an easy way. 
    To install it, we once again go to the add-on page. Once it is installed you start the service and go to the WebUI. Here navigate to 'InfluxDB admin' > 'Data Sources', and create a new database as well as a new user. then add:
   
 ```
         influxdb:
              host: '<Home Assistant host IP here>'
              port: 8086
              database: "database_name"
              username: "<InfluxDB_user>"
              password: "<password_here>"
              max_retries: 3
              include: 
                entities:
                - sensor.my_sensor
              default_measurement: steps
      
```
Because the entity "sensor.my_sensor"(as defined earlier as an mqtt sensor) was set to be included here, the data gathered from that "sensor", AKA the smartwatch, will be automatically stored in your new database.


* **Grafana:**
    Install from HA add-on page, then start it and go to the WebUI. Navigate to *Administration > Data Sources* and click on "Add a new data source", choose InfluxDB and fill in the URL of your influxDB and add your previously created Influx user credentials in the *user* and *password* fields. Add the database name as defined in your configuration.yaml file. Then it will be possible to add SQL queries to the database and visualize the results. 

 

### Code details:
Github repo link: https://github.com/Cup-of-Code/IoT-Smartwatch



**Core functionalities:
    - Connectivity
    - Step Counter
    - Weather Forecast
    - Infrared Signal  decoder/sender
    - Battery Power
 
#### Connectivity
Ideally I would have wanted to try and use the LoRaWAN protocol because of its low power consumption and long range, but since this is a wearable project I am trying to keep the size as small as possible and a antenna on your arm is perhaps not very practical... There also were not any gateways nearby were I live, so in the end I chose to use wifi for connectivity. 

The Circuit Playground Express does not have any wifi capabilities, so that had to be added via an external module of some kind. I picked the ESP-01 ESP8266 wifi module for this, since it is both very small an also cheap. This was then connected via UART to the CPE. The data from the CPE is transfered via the wifi module and it is programmed to publish mqtt messages to the MQTT broker on Home Assistant. 

The two main functions that require internet connectivity are the step count and weather forecast. The step count should be *uploaded* and the weather forecast should be *downloaded*.

Here is the code that manages that:

From CPE Arduino Sketch: 

```
void stepCountSend(){
  if (rtc.getHours()==23 && rtc.getMinutes()==59){  //checks if time is 23:59 
  01 module only supports Wifi and it is set up to only send data to a local server on my home network, it is quite limited in the range of use. By that I mean that if I were to use it away from home, it would not be very usable. Of course the main function of the device itself will still work, but it would be offline. 

    Serial1.write(step_count); //sends total step_count of the day
    step_count = 0; //resets the step count for next day after sending the data
  }
}
```
The data that is sent as a "Serial1.write()" statement, will be formwarded as an MQTT message to the topic "outTopic". 
This is done in the ESP-01 side of the code:
```
 while (Serial1.available()) {
    // Read data from the Serial1 UART interface
    String payload = Serial1.readStringUntil('\n');
  
// Publish data to the MQTT broker
client.publish("outTopic", payload.c_str());
  }
```
#### Weather Forecast:
    ......to be updated......
#### Infrared Decoder & Sender
    ......to be updated......

#### Battery Power
Since the smartwatch is running on battery power(a small 150mAh LiPo battery), the power consumption should be minimized as much as possible. Therefor the wifi connectivity should also be limited, as that uses a lot of power. To do this, the esp-01 module can be set to be in deep sleep mode, until a signal from the CPE triggers it to wake up. As it is programmed now, the smartwatch is set to upload the step count stats to the MQTT broker once a day, at 23.59. This limits the power usage significantly, but still send enough data so that the day to day step count trends can be seen and analyzed. 
....code to be added later....


### Limiting factors and future considerations:
While working on this project I realized that there were a few things with the hardware that were limiting and there are a few things that I am planning to improve on later after finishing the course project. One of these issues has to do with the connectivity. Currently it only supports wifi and is only "online" when I am connected to my home network and thus has limited use when away from home. I want to add some bluetooth functionality as well so that the watch could sync with my phone and upload data via it's mobile network as well, as this is a common feature in smartwatches. Home Assistant also seems to have a cloud functionality that I could perhaps use to still sync my data away from home.

Even though the Circuit Playground Express has a great deal of sensors and built in, it is a bit on the large size for a smartwatch(51 cm in diameter). It also lacks built in battery charging and wireless connectivity(as mentioned above), so I am considering switching to another board as the brains of my project in a future version. ESP32 based boards are a good option since they has both wifi and BLE connectivity and there are a lot of versions that are both small and have built in charging. The only issue then is that I will need to buy some external sensors for it. 

Even further down the line I could possibly design my own PCB for the project as this will make it more space efficent as well and as I have seen people develop kits like this for similar open-source smartwatch projects online. 



## Final result:

(Work in progress, more info and pictures to be added later)


![](https://hackmd.io/_uploads/B1n-IGfth.jpg)
