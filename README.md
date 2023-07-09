## IoT smartwatch - Tutorial

Author: Charlotta Larmoijeur
StudentID: cl224dp
Github Repository link: https://github.com/Cup-of-Code/IoT-Smartwatch

# Objective

<img src="https://hackmd.io/_uploads/B1-DajdK2.jpg" alt="Drawing" style="width: 500px;"/>

#### Video demo: https://youtu.be/k_mWFVTUdE4



In this tutorial I will go through how to make a diy IoT smartwatch! This is a project that I wanted to tackle for a while, since I really liked the idea of making an open-source fully customizable smartwatch that I could build my own functionalities into. The goal for now is to implement step counting, weather forcast information and Infrared signal decoding and sending. The watch will store and access data via a MQTT broker and InfluxDB database that is set up on local Home Assistant server running on a Raspberry Pi. Then the stored data will be visualized using grafana.

This may not be the most feature-packed version of a smartwatch, but it does have the potential to be and the fun part is being to experiment with it and customize it how you want. The project  has taught me alot and if you were to build it yourself it should give a lot of insight into microcontrollers, communication protocols(both wired and wireless), home server setup, general programming and more. 
            
            
**Apporoximate timeframe:** Weekend project- assuming you're familiar with the technologies used. If you are a beginner it will probably be around 2-3 weekends. 
* [Objective](#Obejctive)
* [Material](#Material)
* [Computer setup](#Computer-Setup)
  [-Setting up Arduino IDE](#Setting-up-the-Arduino-IDE:)
* [Putting everything together](#Putting-Everything-Together)
* [Platform](#Platform)
* [Code details](#Code-details)
    * [Connectivity](#Connectivity)
    * [Step Counter](#Step-Counter)
    * [Weather Forecast](#Weather-Forecast)
    * [Battery Power](#Battery-Power)
*  [Limiting factors and future considerations](#Limiting-factors-and-future-considerations)
* [Final result](#Final-result)




### Material

| Components                       | Picture                                                                                                                                                      | Description                                                                                                                                   | Price and retailer:                                                                                 |
| -------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------ | --------------------------------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------- |
| Circuit Playground Express (CPE) | <img src="https://hackmd.io/_uploads/S1Ns-fxt3.jpg">                                                                                                         | Has many built in sensors (thermometer, accelerometer, color sensor,microphone etc), which is why I chose it.                                 | 349.00 SEK ([electrokit](https://www.electrokit.com/produkt/adafruit-circuit-playground-express/))  |
| ESP8266 ESP-01 WIFI-module       | <img src="https://hackmd.io/_uploads/SkfhNMeF3.jpg">                                                                                                         | Simple ESP8266 based wifi module.                                                                                                             | 49.00 SEK ([electrokit](https://www.electrokit.com/produkt/esp-01-esp8266-wifi-modul/))             |
| Waveshare 1.28" LCD              | <img src="https://hackmd.io/_uploads/rk_CBGlFn.jpg">                                                                                                         | LCD with GC9A01A driver.                                                                                                                      | 255.99 SEK ([Amazon](https://amzn.eu/d/9QgQvLU))                                                    |
| Wire                             | <img src="https://hackmd.io/_uploads/ryz__MxFn.jpg">                                                                                                         | You do not need any specific kind, but thin wire is easier since it saves some space in the housing. I used some AWG26 silicone wire.         | 3.50kr/meter ([electrokit](https://www.electrokit.com/produkt/kopplingstrad-awg26-silikon-rosa-m/)) |
| 16 pieces of: M2 screws, nuts and washers      | <img src="https://hackmd.io/_uploads/ryaJsGeKh.jpg"><img src="https://hackmd.io/_uploads/HJmhczxFh.jpg"><img src="https://hackmd.io/_uploads/HJVKcGxKn.jpg"> | I designed the 3D model to make space for M2 nuts so that the CPE can be easily assembled and disassembled from the case without desoldering. | [electrokit](https://www.electrokit.com/produkt-kategori/inbyggnad/fastelement-inbyggnad/)          |
| MicroLipo charger  | ![](https://hackmd.io/_uploads/r1nlr9_Y2.jpg)|  Needed to add charging functionality to the watch, the CPE does not have this built in.|95 SEK    ([electrokit](https://www.electrokit.com/produkt/micro-lipo-laddare-for-lipo-liion-microusb/))                |
| 3D case                          | ![](https://hackmd.io/_uploads/BJjqkmxYh.jpg)                                                                                                                | The model I created for the smartwatch (made with Fusion360)    Full model & download link can be found in my [GitHub-repo](https://github.com/Cup-of-Code/IoT-Smartwatch).                                 | 3D print it yourself or order from 3D printing service company.                                     |


### Tools for Assembly:

| Items for assembly  | Picture | Description |
| -------- | -------- | -------- |
| 3D printer    | ![](https://hackmd.io/_uploads/Sy6qvGxKn.png)   | I designed and 3D printed a case for my smartwatch, so having acess to a 3D printer is neccessary in order to assemble it. 
| Soldering iron    | ![](https://hackmd.io/_uploads/H1m5oMxtn.jpg)| Needed to solder connections together.










### Computer setup
I mainly used the Arduino IDE for programming my device. The Circuit Playground Express(CPE) is supported by both CircuitPython, MakeCode and Arduino, but there is more of a community around Arduino, more available code libraries as well as the fact that it is written in C++, which is generally more efficient than python, so this made it an easy decision to use the Arduino IDE as it is also simple to use as an IDE and it has built in functionality for flashing.

Everything was set up on my Linux PC (running Pop! OS v22.04 LTS). Installing the arduino IDE can be done by downloading the correct version for your OS from: https://www.arduino.cc/en/software.

***Setting up the Arduino IDE:***

1.  **Arduno Boards Manager:**
    In order to add the Circuit Playground Express and ESP8266 board support to the IDE, you need to include some custom sources in the IDE preferences. Go to *file> Preferences > 'Additional Board Manager URLs*' and add the following two lines:
        http://arduino.esp8266.com/stable/package_esp8266com_index.json
        https://adafruit.github.io/arduino-board-index/package_adafruit_index.json .
Now you should be able to choose both the 'Circuit Playground Express' or the 'Generic ESP8266 module' from the board manager option.
2. **Library Manager:**
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


#### Accessing Circuit Playground Express flash memory:
Since I am using the flash memory of the CPE to store the image that I am using as a wallpaper on the homescreen, it has to be saved to the memory first. This turned out to be a bit trickier than I realized. The solution is to first flash the microcontroller with CircuitPython firmware. This can be done by downloading the correct uf2 file [here](https://circuitpython.org/board/circuitplayground_express/), putting the CPE in flash mode(just press reset button twice) and simply drag the .uf2 file onto the CPE device that should show up in your computers filesystem. After this the device should show up as a CircuitPython device and it is here that you should drop your chosen wallpaper image(remember the exact file name, it's needed later). When you later flash the CPE from within the Arduino IDE, it will not delete the from the system even though CircuitPython is no longer installed.




------



### Putting Everything Together 
Since there are actually **two** microcontrollers in this project, the process of connecting and flashing firmware is a little bit more complicated. For the ESP-01 module,  you will need some kind of USB- to serial converter adapter to flash firmware to it. Unfortunately I did not realize this until after the parts had arrived and so I actually used the CPE as a DIY adapter instead. This is how it was wired for flashing:
![](https://hackmd.io/_uploads/HJWHO9_F3.png)


        *Diagram1 - Wiring for using the CPE as a serial to USB converter. *
        
        
 In  the diagram above  the connections are as follows: 
     

|ESP-01    | Circuit Playground Express   | 
| -------- | -------- | 
| 3.3V & EN(also labled CH_PD)     | 3.3V     | 
|GND|GND
|RX|TX
|TX|RX
|GP0|GND (this sets the device to flashing mode)

After flashing the firmware to the ESP-01 module you connect all the parts together as shown below: 

![](https://hackmd.io/_uploads/B1CWrFdK2.jpg)


        *Diagram2 - Final circuit wiring (note:battery is actually 150mAh)*    


Additional notes about wiring. When flashing the module I left the pre-soldered headers on the ESP-01 as is, but once I was ready for final assembly I desoldered them to save some space. This was the final connection of the two:
![](https://hackmd.io/_uploads/BkB2JOlF3.jpg)

        *Glued the boards together to keep them in place.
        
#### 3D printed case: 
In order to give the watch a more polished look, I wanted to design and print a case for it. I used fusion360(with student account access) to model the case. It took a few dosen revisions, but finally I ended up with some thing that something functional. I took apart an old watch and reused the watch straps, since I could not 3D print flexible material. I decided to use M2 screws and bolts to secure the screen to the CPE instead of just soldering the wires,this allows me to take dissassemble it more easily if I want to.  I ended up with a functioning watch case, although there is still room for improvement. See the pictures of final design in the Final Result section.


### Platform
The basic flow of data is as follows: 

For information that is being UPLOADED to server(like step counts):
![](https://hackmd.io/_uploads/S1oCJsdFn.jpg)

    *The cloud bubble indicates that the services inside it are set up inside of Home Assitant.
    
For information being DOWNLOADED to the watch(like the weather info):
![](https://hackmd.io/_uploads/SJzxlsuK2.jpg)


* ###     Home Assistant ([home-assitant.io](https://www.home-assistant.io/)):
Home Assistant(HA) is an extremely customizable and powerful open-source home automation service, that can be used to make a DIY smart home. I have used it in the past and since I saw that it had addons for both InfluxDB, Mosquitto broker and grafana, I thought it would be a fun challenge to use it for my project. Another reason that this is convenient is that it will integrate together with all my other smart home devices and since I have the Home Assistant app on my phone as well I can view my stats there too. I installed Home Assistant on a Raspberry pi 3B, running locally on my home network. You can find instructions on how to do this [here](https://www.home-assistant.io/installation/raspberrypi).

Then when you have set up the home assistant server. You can install the neccessary add-ons for communication with the device.

* **MQTT:**
    In order to send the data from the smartwatch to my home assistant server I decided to use the MQTT protocol. To do this you have to set up a MQTT broker in Home Assistant. Luckily there are already add-ons for this so the process is realtively easy. 
    
    Go to the *Settings > Add-ons > 'Add-on store'* and search for Mosquitto broker. There should only be one option so install it and then press start. You should not have to edit anything in the configuration tab. 
  Then go to the *Settings > 'Devices and Services' > 'Add Integration'*, search for MQTT and add that as well. and  Then go to HA settings and add a new user that will be used for the MQTT login. This username and information should also be added to the MQTT device configuration settings as well as in the arduino code that is flashed to the ESP-01(See more details in the code section).This is what will allow the smartwatch to publish MQTT messages to the broker. Then the following should be added to your configuration.yaml file: 
    
```yaml!
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
![](https://hackmd.io/_uploads/ByerGs_Yn.png)

    * Since my watch had been off for a while there was no history to show.
    * This is just a simple visualization that is accessible from home assistant dashboard itself and is not from Grafana.

* ### InfluxDB:
    By adding InfluxDB you can create a database in which the data sent from the device will be stored long-term(until deleted or storage space runs out) and in which we can preform SQL-(like)queries to search the database and analyze data in an easy way. 
    To install it, we once again go to the add-on page. Once it is installed you start the service and go to the WebUI. Here navigate to 'InfluxDB admin' > 'Data Sources', and create a new database as well as a new user. then add:
   
 ```yaml
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

Picture of InfluxDB dashboard: 
![](https://hackmd.io/_uploads/Bky173uY2.png)




* ### Grafana:
    Install from Home Assistant add-on page, then start it and go to the WebUI. Navigate to *Administration > Data Sources* and click on "Add a new data source", choose InfluxDB and fill in the URL of your influxDB and add your previously created Influx user credentials in the *user* and *password* fields. Add the database name as defined in your configuration.yaml file. Then it will be possible to add SQL queries to the database and visualize the results. 
![](https://hackmd.io/_uploads/HyJQXnOtn.png)
    *This will be later changed to show a bar graph that represents the steps the past 7 days.
 

### Code details:
Github repo link: https://github.com/Cup-of-Code/IoT-Smartwatch



#### Core functionalities:
- **Connectivity**
- **Step Counter**
- **Weather Forecast**
- **Battery Power**
 
#### Connectivity
Ideally I would have wanted to try and use the LoRaWAN protocol because of its low power consumption and long range, but since this is a wearable project I am trying to keep the size as small as possible and a antenna on your arm is perhaps not very practical... There also were not any gateways nearby were I live, so in the end I chose to use wifi for connectivity. 

The Circuit Playground Express does not have any wifi capabilities, so that had to be added via an external module of some kind. I picked the ESP-01 ESP8266 wifi module for this, since it is both very small an also cheap. This was then connected via UART to the CPE. The data from the CPE is transfered via the wifi module and it is programmed to publish mqtt messages to the MQTT broker on Home Assistant. 

The two main functions that require internet connectivity are the step count and weather forecast. The step count should be *uploaded* and the weather forecast should be *downloaded*.

Here is the code that manages that:

From CPE Arduino Sketch: 

```cpp
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
```cpp
 while (Serial1.available()) {
    // Read data from the Serial1 UART interface
    String payload = Serial1.readStringUntil('\n');
  
// Publish data to the MQTT broker
client.publish("outTopic", payload.c_str());
  }
```

#### Weather Forecast:

In order to make the smartwatch more "smart" I added some weather information in the watch as well. This was done by adding an automation in Home Assitant that send weather info to the watch through MQTT messages. The process is as follows:
    1. Add met.no as a weather service in home assitant and enter your location info. 
    2. Add an automation under *settings > automations & scenes*. Then editing the yaml code for the automation like such: 
        
```yaml
alias: Weather Info
description: "publish weather info to watch "
trigger:
- platform: time
at: "07:00:00"
action:
- service: mqtt.publish
data_template:
  topic: inTopic
  payload: "Weather: {{ states('weather.forecast_home_2') }}"
  
```
This automation is set to send the current weather data each day at 07:00 as a MQTT message with the topic "inTopic". The code also adds a prefix to that message "Weather:" which will be important in our code on the CPE.

3. add code to handle the incoming MQTT messages: 
```cpp=
void UARTconnect() {
  // This code is responsible for the communication between the CPE and ESP-01 device. 

  // It reads data from the Serial Monitor and sends it to ESP8266
  if (Serial.available()) {
    char data = Serial.read();
    Serial1.write(data);
  }

  // Reads data from ESP8266(& indirectly the mqtt broker) and sends it to the Serial Monitor
  if (Serial1.available()) {
    String data = Serial1.readStringUntil('\n');
    if (data.startsWith("Weather")) {  //gets only the MQTT messages  in "inTopic" that starts with "Weather"
      int colonIndex = data.indexOf(':');
      if (colonIndex > -1) {
        weatherData = data.substring(colonIndex+1);  // get the data part after the colon and saves it to 'weatherData' variable.
      }
    } else {
      Serial.println( data);
    }
  }
}
```
From line 11 forward is the relevant code. Since the ESP-01 "forwards" the messages to the CPE via serial1, the code first checks if that is available and if so it imports the string to the *data* variable. The next step is to check if the data starts with the "Weather"-prefix and if so it is saved to the *weatherData*-variable. The prefix is useful so that weather data can be distinguished from other MQTT messages sent to the watch. 

Then for the displaying of the data on the watch the following code is used:
```cpp=
void weatherPage(){
    if (CircuitPlayground.leftButton()){
    tft.fillScreen(0x0000); //clears screen
    bool running = true; //set to manually be able to stop loop
    while (!CircuitPlayground.rightButton() && running){
      tft.setFont();
      tft.setTextColor(0xFFFF,0x0000); //first value is text color, second value is background color. Helps to limit flickering in case of new data.
      tft.setCursor(23, 80); 
      tft.println("Weather Forecast: ");
      tft.setCursor(23,110);
      tft.print(weatherData); //prints weatherdata gathered from UARTconnect function
      
      if(CircuitPlayground.rightButton()){
        running = false; //makes sure the loop is stopped since short presses of right button didn't always stop the loop.
        homepage(); //returns too main watch screen with wallpaper,time etc
      }
    }
  }
}

```
Here the new *weatherPage* function is used to create a new "page" which will be opened by pressing the left button on the CPE and were the weather data will be shown.


#### Battery Power
Since the smartwatch is running on battery power(a small 150mAh LiPo battery), the power consumption should be minimized as much as possible. Therefor the wifi connectivity should also be limited, as that uses a lot of power. To do this, the esp-01 module can be set to be in deep sleep mode, until a signal from the CPE triggers it to wake up. As it is programmed now, the smartwatch is set to upload the step count stats to the MQTT broker once a day, at 23.59. This limits the power usage significantly, but still send enough data so that the day to day step count trends can be seen and analyzed. 



### Limiting factors and future considerations:
While working on this project I realized that there were a few things with the hardware that were limiting and there are a few things that I am planning to improve on later after finishing the course. One of these issues has to do with the connectivity. Currently it only supports wifi and is only "online" when I am connected to my home network and thus has limited use when away from home. I want to add some bluetooth functionality as well so that the watch could sync with my phone and upload data via it's mobile network as well, as this is a common feature in smartwatches. Home Assistant also seems to have a cloud functionality that I could perhaps use to still sync my data away from home.

Even though the Circuit Playground Express has a great deal of sensors and built in, it is a bit on the large size for a smartwatch(51 cm in diameter). It also lacks built in battery charging and wireless connectivity(as mentioned above), so I am considering switching to another board as the brains of my project in a future version. ESP32 based boards are a good option since they has both wifi and BLE connectivity and there are a lot of versions that are both small and have built in charging. The only issue then is that I will need to buy some external sensors for it. 

Further more I still want to add some other functionalities like heart rate measurement(this didn't get done in time) and perhaps notification forwarding from my phone. 



## Final result:
Below are pictures of the final result. I am pretty happy with it :)


![](https://hackmd.io/_uploads/HyqnisuY3.jpg)

![](https://hackmd.io/_uploads/Hye0iiOYn.jpg)

![](https://hackmd.io/_uploads/B17l2juYn.jpg)
![](https://hackmd.io/_uploads/rJ1ZChdth.png)
