#include <Adafruit_GFX.h>         // Core graphics library
#include <Adafruit_GC9A01A.h>      // Hardware-specific library for GC9A01A
#include <Adafruit_SPIFlash.h>    // SPI / QSPI flash library
#include <Adafruit_ImageReader.h> // Image-reading functions
#include <Adafruit_CircuitPlayground.h>
#include <RTCZero.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <PubSubClient.h>


//LCD pin connections on the CPE:
#define _DC 10
#define _CS 9
#define _MOSI 2
#define _SCLK 3
#define _MISO -1
#define _RST -1
Adafruit_GC9A01A tft( _CS, _DC, _MOSI,  _SCLK, _MISO, _RST); //initilized LCD as "tft".


//////// SPI FLASH RELATED ///////// Taken and adapted from example code in Adafruit ImageReader library: https://github.com/adafruit/Adafruit_ImageReader/blob/master/examples/TFTGizmo/TFTGizmo.ino
// SPI or QSPI flash filesystem (i.e. CIRCUITPY drive)
#if (SPI_INTERFACES_COUNT == 1 || defined(ADAFRUIT_CIRCUITPLAYGROUND_M0)) //true for Circuit playground express
  Adafruit_FlashTransport_SPI flashTransport(SS, &SPI);
#endif

// TFT SPI interface selection
#if (SPI_INTERFACES_COUNT == 1)
  SPIClass* spi = &SPI;
#else
  SPIClass* spi = &SPI1;
#endif

Adafruit_SPIFlash    flash(&flashTransport);
FatVolume        filesys;
Adafruit_ImageReader reader(filesys); // Image-reader, pass in flash filesys
Adafruit_Image       img;        // An image loaded into RAM
int32_t              width  = 0, // BMP image dimensions
                     height = 0;
//////////////////////////////////////////////

//STEP-COUNTER Related /////////////////////////////////////
float last_accel = 0;   // Hold the last acceleration
int step_count = 0;     // Step counter
float threshold = 5;   // Threshold to determine a step
unsigned long lastStepTime = 0; // Time of last step
unsigned int minStepDelay = 250; // Minimum delay between steps
//////////////////////////////////////////////////////////////////

/////////////////////////// Clock related////////////////////// 
RTCZero rtc;
static uint8_t conv2d(const char *p){
  uint8_t v = 0;
  return (10 * (*p - '0')) + (*++p - '0');
}
static int display_hour;
static int display_minute;
static int display_second;
////////////////////////////////////////////////////////

//IR record Related/////////////
uint8_t IR_protocol;
uint32_t IR_value;
uint16_t IR_bits;
////////////////////////////////


//// BITMAP SPRITES //////
  //// FOOTPRINT /////
const unsigned char footprint[] PROGMEM ={
  //created sprites using: https://javl.github.io/image2cpp/
	// 'footprint, 30x30px
	0xff, 0x9f, 0xff, 0xfc, 0xff, 0x0d, 0xff, 0xfc, 0xff, 0x09, 0xff, 0xfc, 0xff, 0x08, 0x3f, 0xfc, 
	0xff, 0x89, 0x2f, 0xfc, 0xff, 0xdf, 0xaf, 0xfc, 0xff, 0xff, 0xe3, 0xfc, 0xff, 0xe0, 0xf7, 0xfc, 
	0xff, 0xc0, 0x1f, 0xfc, 0xff, 0x80, 0x0f, 0xfc, 0xff, 0x80, 0x07, 0xfc, 0xff, 0xc0, 0x07, 0xfc, 
	0xff, 0xc0, 0x03, 0xfc, 0xff, 0xe0, 0x03, 0xfc, 0xff, 0xf0, 0x03, 0xfc, 0xff, 0xf8, 0x03, 0xfc, 
	0xff, 0xfc, 0x07, 0xfc, 0xff, 0xfc, 0x07, 0xfc, 0xff, 0xfc, 0x07, 0xfc, 0xff, 0xfc, 0x0f, 0xfc, 
	0xff, 0xf8, 0x0f, 0xfc, 0xff, 0xf8, 0x1f, 0xfc, 0xff, 0xf0, 0x1f, 0xfc, 0xff, 0xe0, 0x1f, 0xfc, 
	0xff, 0xe0, 0x3f, 0xfc, 0xff, 0xe0, 0x3f, 0xfc, 0xff, 0xe0, 0x3f, 0xfc, 0xff, 0xe0, 0x3f, 0xfc, 
	0xff, 0xe0, 0x7f, 0xfc, 0xff, 0xf8, 0xff, 0xfc
};

const unsigned char heart[] PROGMEM ={
  // 'heart, 30x30px
	0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xfc, 
	0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xfc, 0xfe, 0x07, 0x83, 0xfc, 0xfc, 0x03, 0x00, 0xfc, 
	0xf8, 0x00, 0x00, 0x7c, 0xf0, 0x00, 0x00, 0x7c, 0xf0, 0x00, 0x00, 0x3c, 0xf0, 0x00, 0x00, 0x3c, 
	0xf0, 0x00, 0x00, 0x3c, 0xf0, 0x00, 0x00, 0x3c, 0xf0, 0x00, 0x00, 0x3c, 0xf0, 0x00, 0x00, 0x3c, 
	0xf8, 0x00, 0x00, 0x7c, 0xf8, 0x00, 0x00, 0x7c, 0xfc, 0x00, 0x00, 0xfc, 0xfe, 0x00, 0x01, 0xfc, 
	0xff, 0x00, 0x03, 0xfc, 0xff, 0x80, 0x0f, 0xfc, 0xff, 0xc0, 0x1f, 0xfc, 0xff, 0xf0, 0x3f, 0xfc, 
	0xff, 0xf8, 0x7f, 0xfc, 0xff, 0xfd, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xfc, 
	0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xfc
};
//////////////////////////////////////////////////////////////////////


void heartrateSprite(){
    //draws the previously defined sprite and puts a background rectangle behind it 
  tft.fillRoundRect(130, 170,40 , 40, 5, 0x05f5);  
  tft.drawBitmap(135, 175, heart, 30, 30, 0x05f5, 0xFFFF);
}

void footprintSprite(){
  //draws the previously defined sprite and puts a background rectangle behind it 
  tft.fillRoundRect(45, 170, 60, 40, 5, 0x05f5);  
  tft.drawBitmap(50, 175, footprint, 30, 30, 0x05f5, 0xFFFF);
}




///////////////////////// SETUP ///////////////////////////////////
void setup() {
  // Initialize neccessary components:
  CircuitPlayground.begin();
  rtc.begin();
  Serial.begin(115200);
  Serial1.begin(115200);
  delay(2000);
  tft.begin();
  //CircuitPlayground.irReceiver.enableIRIn(); // Start the IR receiver //commented out, waiting to be implemented
  //IR_protocol = 0; 
  homepage(); //runs the homepage function to display time etc
  
}


  
void loop() {  
  stepCounter();
  updateTime();  
  //menuFunc();
  UARTconnect();
  // IR_send();
  //rightButtonIR();
}
 



String weatherData = ""; // Global variable to store weather data

void UARTconnect() {
  // This code is responsible for the communication between the CPE and ESP-01 device. 
  // It reads data from the Serial Monitor and sends it to ESP8266
  if (Serial.available()) {
    char data = Serial.read();
    Serial1.write(data);
  }

  // Reads data from ESP8266 and sends it to the Serial Monitor
  if (Serial1.available()) {
    String data = Serial1.readStringUntil('\n');
    if (data.startsWith("Weather")) {  // assuming data format is "inTopic:someValue"
      int colonIndex = data.indexOf(':');
      if (colonIndex > -1) {
        weatherData = data.substring(colonIndex+1);  // get the data part after the colon
        homepage();
        tft.setCursor(50, 40);
        tft.setFont();
        tft.setTextSize(2);
        tft.print("Weather: ");
        tft.setCursor(50,50);
        tft.print(weatherData);
      }
    } else {
      Serial.println( data);
    }
  }
}



void homepage(){

  printWallpaper(); //PRINT WALLPAPER function
  initialTime(); //starts clock 
  footprintSprite(); 
  heartrateSprite();
}




void printWallpaper(){
  //Also adapted from the Adafruit ImageReader library example code https://github.com/adafruit/Adafruit_ImageReader/blob/master/examples/TFTGizmo/TFTGizmo.ino
  ImageReturnCode stat; // Status from image-reading functions
  tft.setRotation(0);  

  // The Adafruit_ImageReader constructor call (above, before setup())
  // accepts an uninitialized SdFat or FatVolume object. This MUST
  // BE INITIALIZED before using any of the image reader functions!
  Serial.print(F("Initializing filesystem..."));
  // SPI or QSPI flash requires two steps, one to access the bare flash
  // memory itself, then the second to access the filesystem within...
  if(!flash.begin()) {
    Serial.println(F("flash begin() failed"));
    for(;;);
  }
  if(!filesys.begin(&flash)) {
    Serial.println(F("filesys begin() failed"));
    for(;;);
  }
  // Notice the 'reader' object performs this, with 'tft' as an argument.
  stat = reader.drawBMP("/pixel.bmp", tft, 0, 0);
}



void initialTime(){
  tft.setTextSize(2);  // scaling of font
  tft.setTextColor(0xFFFF); // sets text color to white
  tft.setFont(&FreeSansBold18pt7b);

  display_hour = conv2d(__TIME__);
  display_minute = conv2d(__TIME__ + 3);
  display_second = conv2d(__TIME__ + 6);
  rtc.setTime(display_hour, display_minute, display_second);

  //bar behind the displayed time:
  tft.fillRoundRect( 20, tft.height()*0.37, 195,  65, 20, 0x05f5);
  tft.drawRoundRect( 20, tft.height()*0.37, 195,  65, 20, 0x03ad);

  ///Prints current time in middle of display
  tft.setCursor(30, tft.height() * 0.6);
  tft.print(display_hour < 10 ? "0" + String(display_hour) : String(display_hour)); //adds extra zero if hour <10.

  tft.setCursor(106, tft.height() * 0.6);
  tft.print(":");
  
  tft.setCursor(123, tft.height() * 0.6);
  tft.print(display_minute < 10 ? "0" + String(display_minute) : String(display_minute));  //adds extra zero if minute <10
}


void updateTime() { //function that updates time and refreshes background
  tft.setTextSize(2);  // scaling of font
  tft.setFont(&FreeSansBold18pt7b);
  // Determine the text sizes based on the font you're using
  int textWidth = 40;
  int textHeight = 45;

  //Clear SCREEN PORTION only where new value is:
  if(rtc.getHours() != display_hour) {
    tft.fillRect(29, tft.height() * 0.5625-textHeight, textWidth*2, textHeight*1.4, 0x05f5); //fills area behind new number to overwrite the old one
  }
  if(rtc.getMinutes() != display_minute) {
    tft.fillRect(122, tft.height() * 0.5625-textHeight, textWidth*2, textHeight*1.4, 0x05f5);
  }

  // Refresh the time before displaying
  display_hour = rtc.getHours();
  display_minute = rtc.getMinutes();
 
  tft.setCursor(30, tft.height() * 0.6);
  tft.print(display_hour < 10 ? "0" + String(display_hour) : String(display_hour)); 
  tft.setCursor(123, tft.height() * 0.6); 
  tft.print(display_minute < 10 ? "0" + String(display_minute) : String(display_minute));
}





void stepCounter(){  
  tft.setTextSize(3);
  tft.setFont();
  tft.setTextColor(0xFFFF,0x05f5);
  unsigned long startMillis = millis();  // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  unsigned int old_step_count = step_count;

  // collect data for 50 mS
  while (millis() - startMillis < 50) {
    int16_t accel = sqrt(pow(CircuitPlayground.motionX(), 2) + pow(CircuitPlayground.motionY(), 2) + pow(CircuitPlayground.motionZ(), 2));
    // toss out spurious readings
    if (accel < 1024)   {
      if (accel > signalMax){
        signalMax = accel;}
      else if (accel < signalMin){
        signalMin = accel;  } } }

  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  // Calculate the total acceleration
  float totalAccel = (float)peakToPeak;

  if (totalAccel > threshold && (millis() - lastStepTime) > minStepDelay) {
    step_count++;
    lastStepTime = millis(); }
  if (step_count> old_step_count){
  // Clear the previous step count
    tft.setCursor(80, 180);
    tft.print(step_count); 
  }
}

void stepCountSend(){
  if (rtc.getHours()==23 && rtc.getMinutes()==59){ //checks if time is 23:59
    Serial1.write(step_count); //sends total step_count of the day
    step_count = 0; //resets the step count for next day after sending the data
  }
}




void rightButtonIR() {
    //Adapted from example code: https://github.com/adafruit/Adafruit_CircuitPlayground/tree/master/examples/Infrared_Demos/Infrared_Record
    //////// Function that records incomming IR signal and saves them if decoding sucessful.
    if (CircuitPlayground.rightButton()) {
      tft.println("recording");
      if(CircuitPlayground.irReceiver.getResults()) {
        //attempt to decode it
        if(CircuitPlayground.irDecoder.decode()) {
          Serial.println("IR decoded");
          //Print the results.  Change parameter to "true" for verbose output.
          CircuitPlayground.irDecoder.dumpResults(false);
          Serial.println("Saving results. Press left button to retransmit.");
          IR_protocol=CircuitPlayground.irDecoder.protocolNum;
          IR_value= CircuitPlayground.irDecoder.value;
          IR_bits= CircuitPlayground.irDecoder.bits;  
        }
        CircuitPlayground.irReceiver.enableIRIn();      //Restart receiver
      }
    }
}

void IR_send(){
  //Adapted from example code: https://github.com/adafruit/Adafruit_CircuitPlayground/tree/master/examples/Infrared_Demos/Infrared_Send
  //Sends previously recorded IR signsl when pressing left button

   if (CircuitPlayground.leftButton()) {
    Serial.println("Left button pressed!");

    if(IR_protocol) {
      CircuitPlayground.irSend.send(IR_protocol, IR_value, IR_bits);
      Serial.println("Sending recorded IR signal");
      Serial.print("Protocol:"); Serial.print(IR_protocol,DEC);
      Serial.print("  Value:0x");  Serial.print(IR_value,HEX);
      Serial.print("  Bits:"); Serial.println(IR_bits,DEC);}
    else {
      Serial.println("No signal saved yet.");
    }
  }
}


void menuFunc(){
//function that adds menu-like functionality to the watch. Left buttons opens menu, right-button returns to home.
  if (CircuitPlayground.leftButton()){
    tft.fillScreen(0x0000);

    while (!CircuitPlayground.rightButton()){
      tft.setFont();
      tft.setTextColor(0xFFFF,0x0000);
      tft.setCursor(45, 40);
      tft.println("Record");
      tft.setCursor(45, 58);
      tft.println("IR-signal");}

    if(CircuitPlayground.rightButton()){
      homepage();}
  }
}
