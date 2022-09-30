#include <DHT.h>
#include <Adafruit_NeoPixel.h>
#include <PMS.h>
#include <SPI.h>
#include <TFT_22_ILI9225.h>
#include <SoftwareSerial.h>
#include <LowPower.h>

#include "FreeMonoBold24pt7b.h"



SoftwareSerial BT(4, 3);

SoftwareSerial mySerial(6, 7);

#define TFT_RST 8
#define TFT_RS  9
#define TFT_CS  10  // SS
#define TFT_SDI 11  // MOSI
#define TFT_CLK 13  // SCK
#define TFT_LED 3   // 0 if wired to +5V directly

#define MENUBT 5 


uint16_t pm_color[] = {COLOR_BLACK, COLOR_NAVY, COLOR_GREEN, COLOR_ORANGE, COLOR_RED};
String pm_text[] = {"Waiting..", "Very Good", "Good", "Bad", "Very Bad"};
int8_t pm25_status = 0;
int8_t pm100_status = 0;
uint16_t pm25_color = pm_color[0];
uint16_t pm100_color = pm_color[0];
String pm25_text = "Setup!";
String pm100_text = "";
int a[6] = { 0 };
int menuNum = 0;

String temp;
String humi;
String pm1;
String pm2;
int dust2;
int dust3;


TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED);
uint16_t x, y;
boolean flag = false;


unsigned char pmsbytes[32];

bool prevIn = false, curIn = false;

int humiGraph[6] = {0,};
int tempGraph[6] = {0,};
int fDustGraph[6] = {0,};
int ufDustGraph[6] = {0,};

void menu1(){
      tft.clear();
      tft.setFont(Terminal6x8);
      tft.setBackgroundColor(COLOR_BLACK);
      tft.drawText(5 + 100 - 72, 6, "Temp:      `C", COLOR_WHITE);
      tft.drawText(5 + 100 + 10, 6, "Humi:       %", COLOR_WHITE);
    
      //tft.fillRectangle(5, 20 + 2, 5 + 100, 20 + 2 + 74, pm_color[0]);
      tft.fillRectangle(0 + 30 + 0, 20 + 2 + 85, 0 + 30 + 15 * 1.8, 20 + 2 + 22 + 85, pm_color[1]);
      tft.fillRectangle(0 + 30 + 16 * 1.8, 20 + 2 + 85, 0 + 30 + 35 * 1.8, 20 + 2 + 22 + 85, pm_color[2]);
      tft.fillRectangle(0 + 30 + 36 * 1.8, 20 + 2 + 85, 0 + 30 + 75 * 1.8, 20 + 2 + 22 + 85, pm_color[3]);
      tft.fillRectangle(0 + 30 + 76 * 1.8, 20 + 2 + 85, 0 + 30 + 100 * 1.8, 20 + 2 + 22 + 85, pm_color[4]);
    
    
      //tft.fillRectangle(5, 20 + 78 + 2, 5 + 100, 20 + 78 + 2 + 74, pm_color[0]);
      tft.fillRectangle(0 + 30 + 0, 20 + 78 + 2 + 45, 0 + 30 + 15 * 1.8, 20 + 78 + 2 + 22 + 45, pm_color[1]);
      tft.fillRectangle(0 + 30 + 16 * 1.8, 20 + 78 + 2 + 45, 0 + 30 + 40 * 1.8, 20 + 78 + 2 + 22 + 45, pm_color[2]);
      tft.fillRectangle(0 + 30 + 41 * 1.8, 20 + 78 + 2 + 45, 0 + 30 + 75 * 1.8, 20 + 78 + 2 + 22 + 45, pm_color[3]);
      tft.fillRectangle(0 + 30 + 76 * 1.8, 20 + 78 + 2 + 45, 0 + 30 + 100 * 1.8, 20 + 78 + 2 + 22 + 45, pm_color[4]);
      
      tft.drawRectangle(0 + 25 + 0, 20 + 2 + 80, 5 + 30 + 180, 20 + 78 + 2 + 22 + 50, COLOR_YELLOW);

      int16_t x;
      x = map(min(dust3, 200), 0, 200, 0, 180);
      tft.fillRectangle(30 + x, 20 + 78 + 2 + 45, 32 + x, 20 + 78 + 2 + 22 + 45, COLOR_WHITE);
      x = map(min(dust2, 100), 0, 100, 0, 180);
      tft.fillRectangle(30 + x, 20 + 2 + 85, 32 + x, 20 + 2 + 22 + 85, COLOR_WHITE);
    
      tft.setFont(Terminal6x8);
      tft.drawText(2, 20 + 2 + 85, "pm");
      tft.drawText(2, 20 + 2 + 85 + 12, "2.5");
      
      tft.drawText(2, 20 + 78 + 2 + 45, "pm");
      tft.drawText(2, 20 + 78 + 2 + 45 + 12, "10");


      tft.setFont(Terminal6x8);
      tft.setBackgroundColor(COLOR_BLACK);
      
      if (temp.toInt() < 10) {
        tft.drawText(5 + 100 - 72 + 33 + 2 + 6, 6, temp, COLOR_WHITE);
      } else {
        tft.drawText(5 + 100 - 72 + 33 + 2, 6, temp, COLOR_WHITE);
      }
      
      
      
      if (humi.toInt() < 10) {
        tft.drawText(5 + 100 + 10 + 33 + 6, 6, humi, COLOR_WHITE);
      } else {
        tft.drawText(5 + 100 + 10 + 33, 6, humi, COLOR_WHITE);
      }    
      
      bool pm25_change, pm100_change;
      
      
      if (0 == dust2) {
        pm25_change = pm25_status != 0;
        pm25_status = 0;
      }
      
      if (0 < dust2 && dust2 < 16) {
        pm25_change = pm25_status != 1;
        pm25_status = 1;
      }
      
      if (15 < dust2 && dust2 < 36) {
        pm25_change = pm25_status != 2;
        pm25_status = 2;
      }
      
      if (36 < dust2 && dust2 < 76) {
        pm25_change = pm25_status != 3;
        pm25_status = 3;
      }
      
      if (75 < dust2) {
        pm25_change = pm25_status != 4;
        pm25_status = 4;
      }
      
      
      
      if (0 == dust3) {
        pm100_change = pm100_status != 0;
        pm100_status = 0;
      }
      
      if (0 < dust3 && dust3 < 31) {
        pm100_change = pm100_status != 1;
        pm100_status = 1;
      }
      
      if (30 < dust3 && dust3 < 81) {
        pm100_change = pm100_status != 2;
        pm100_status = 2;
      }
      
      if (80 < dust3 && dust3 < 151) {
        pm100_change = pm100_status != 3;
        pm100_status = 3;
      }
      
      if (150 < dust3) {
        pm100_change = pm100_status != 4;
        pm100_status = 4;
      }


      pm25_color = pm_color[pm25_status];
      pm25_text = pm_text[pm25_status];
      pm100_color = pm_color[pm100_status];
      pm100_text = pm_text[pm100_status];



      int16_t w, h;

      tft.setFont(Terminal11x16);
      tft.drawText(5 + 2, 20 + 2 + 2, "pm 2.5", COLOR_WHITE);
      
      tft.drawRectangle(5, 20 + 2, 5 + 100, 20 + 2 + 74, COLOR_YELLOW);
      
      
      tft.getGFXTextExtent(String(constrain(dust2, 0, 999)), 0, 0, &w , &h);
      
      if (dust2 != 0) {
        tft.drawGFXText(5 + (100 - w) / 2, 20 + 2 + 74 - 10, String(constrain(dust2, 0, 999)), COLOR_WHITE);
      }
      else {
        tft.drawGFXText(5 + ( 100 - w) / 2, 20 + 2 + 74 - 10, "-", COLOR_WHITE);
      }



      tft.drawText(5 + 2 + 110,  20 + 2 + 2, "pm 10.0", COLOR_WHITE);
      tft.drawRectangle(5 + 110, 20 + 2, 5 + 210, 20 + 2 + 74, COLOR_YELLOW);
      

      tft.getGFXTextExtent(String(constrain(dust3, 0, 999)), 0, 0, &w, &h);
      
      if (dust3 != 0) {
        tft.drawGFXText(5 + (100 - w) / 2 + 110, 20 + 2 + 74 - 10, String(constrain(dust3, 0, 999)), COLOR_WHITE);
      }
      else {
        tft.drawGFXText(5 + (100 - w) / 2 + 110, 20 + 2 + 74 - 10, "-", COLOR_WHITE);
      }


      
      
//      int16_t x;
//      tft.fillRectangle(5 + 100 + 10 + 0 - 5,  20 + 2 + 22 + 1, 5 + 100 + 10 + 100 + 5, 20 + 2 + 22 + 24, COLOR_BLACK);
//      x = map(min(dust2, 100), 0, 100, 0, 100);
//      if (dust2 != 0) {
//        tft.fillTriangle(5 + 100 + 10 + 0 + 0 + x,  20 + 2 + 22 + 1 + 0, 5 + 100 + 10 + 0 + 5 + x,  20 + 2 + 22 + 10, 5 + 100 + 10 + 0 - 5 + x,  20 + 2 + 22 + 10, COLOR_WHITE);
//        tft.drawLine(5 + 100 + 10 + 0 + 0 + x,  20 + 2 + 22 + 1, 5 + 100 + 10 + 0 + 0 + x,  20 + 2 + 22 + 24, COLOR_WHITE);
//      }
//      tft.fillRectangle(5 + 100 + 10 + 0 - 5, 20 + 78 + 2 + 22 + 1, 5 + 100 + 10 + 100 + 5, 20 + 78 + 2 + 22 + 24, COLOR_BLACK);
//      x = map(min(dust3, 200), 0, 200, 0, 100);
//      if (dust3 != 0) {
//        tft.fillTriangle(5 + 100 + 10 + 0 + 0 + x, 20 + 78 + 2 + 22 + 1 + 0, 5 + 100 + 10 + 0 + 5 + x, 20 + 78 + 2 + 22 + 10, 5 + 100 + 10 + 0 - 5 + x, 20 + 78 + 2 + 22 + 10, COLOR_WHITE);
//        tft.drawLine(5 + 100 + 10 + 0 + 0 + x, 20 + 78 + 2 + 22 + 1, 5 + 100 + 10 + 0 + 0 + x, 20 + 78 + 2 + 22 + 24, COLOR_WHITE);
//      }
}

void menu2(){
  tft.clear();

  tft.setFont(Terminal11x16);

  tft.drawText(75, 5, "Humidity", COLOR_WHITE);

  tft.drawRectangle(20, 170, 200, 30, COLOR_GRAY);
  tft.drawLine(30, 160, 190, 160, COLOR_GRAY); 
  tft.drawLine(30, 130, 190, 130, COLOR_GRAY); 
  tft.drawLine(30, 100, 190, 100, COLOR_GRAY); 
  tft.drawLine(30, 70, 190, 70, COLOR_GRAY); 
  tft.drawLine(30, 40, 190, 40, COLOR_GRAY); 
      
  for(int i = 0; i < 6; i++)
  {
    tft.drawRectangle(i * 30 + 30, 160 - map(humiGraph[i], 0, 100, 0, 120), i * 30 + 34, 156 - map(humiGraph[i], 0, 100, 0, 120), COLOR_BLUE);
  }
  for(int i = 1; i < 6; i++)
  {
    tft.drawLine(i * 30 + 32, 158 - map(humiGraph[i], 0, 100, 0, 120), (i - 1) * 30 + 32, 158 - map(humiGraph[i - 1], 0, 100, 0, 120), COLOR_BLUE);
  }
  
  for(int i = 0; i < 6; i++){
    String humiValue(humiGraph[i]);
    tft.setFont(Terminal6x8);
    if(humiGraph[i] < 10){
      tft.drawText(i * 30 + 30 - 1, 160 - map(humiGraph[i], 0, 100, 0, 120) - 15, humiValue, COLOR_WHITE);
    }
    else{
      tft.drawText(i * 30 + 30 - 4, 160 - map(humiGraph[i], 0, 100, 0, 120) - 15 + 2, humiValue, COLOR_WHITE);
    }
  }

  tft.drawText(6, 160 - 3, "0", COLOR_WHITE);
  tft.drawText(3, 130 - 3, "25", COLOR_WHITE);
  tft.drawText(3, 100 - 3, "50", COLOR_WHITE);
  tft.drawText(3, 70 - 3, "75", COLOR_WHITE);
  tft.drawText(0, 40 - 3, "100", COLOR_WHITE);
  
  
}

void menu3(){
  tft.clear();

  tft.setFont(Terminal11x16);

  tft.drawText(55, 5, "Temperature", COLOR_WHITE);

  tft.drawRectangle(20, 170, 200, 30, COLOR_GRAY);
  tft.drawLine(30, 160, 190, 160, COLOR_GRAY); 
  tft.drawLine(30, 130, 190, 130, COLOR_GRAY); 
  tft.drawLine(30, 100, 190, 100, COLOR_GRAY); 
  tft.drawLine(30, 70, 190, 70, COLOR_GRAY); 
  tft.drawLine(30, 40, 190, 40, COLOR_GRAY); 
      
  for(int i = 0; i < 6; i++)
  {
    tft.drawRectangle(i * 30 + 30, 160 - map(tempGraph[i], 0, 40, 0, 120), i * 30 + 34, 156 - map(tempGraph[i], 0, 40, 0, 120), COLOR_RED);
  }
  for(int i = 1; i < 6; i++)
  {
    tft.drawLine(i * 30 + 32, 158 - map(tempGraph[i], 0, 40, 0, 120), (i - 1) * 30 + 32, 158 - map(tempGraph[i - 1], 0, 40, 0, 120), COLOR_RED);
  }

  for(int i = 0; i < 6; i++){
    String tempValue(tempGraph[i]);
    tft.setFont(Terminal6x8);
    if(tempGraph[i] < 10){
      tft.drawText(i * 30 + 30 - 1, 160 - map(tempGraph[i], 0, 40, 0, 120) - 15, tempValue, COLOR_WHITE);
    }
    else{
      tft.drawText(i * 30 + 30 - 4, 160 - map(tempGraph[i], 0, 40, 0, 120) - 15 + 2, tempValue, COLOR_WHITE);
    }
  }

  tft.drawText(6, 160 - 3, "0", COLOR_WHITE);
  tft.drawText(3, 130 - 3, "10", COLOR_WHITE);
  tft.drawText(3, 100 - 3, "20", COLOR_WHITE);
  tft.drawText(3, 70 - 3, "30", COLOR_WHITE);
  tft.drawText(3, 40 - 3, "40", COLOR_WHITE);
  
}

void menu4(){
  tft.clear();

  
  tft.setFont(Terminal11x16);

  tft.drawText(85, 5, "PM 2.5", COLOR_WHITE);

  tft.drawRectangle(20, 170, 200, 30, COLOR_GRAY);
  tft.drawLine(30, 160, 190, 160, COLOR_GRAY); 
  tft.drawLine(30, 130, 190, 130, COLOR_GRAY); 
  tft.drawLine(30, 100, 190, 100, COLOR_GRAY); 
  tft.drawLine(30, 70, 190, 70, COLOR_GRAY); 
  tft.drawLine(30, 40, 190, 40, COLOR_GRAY); 
  
      
  for(int i = 0; i < 6; i++)
  {
    tft.drawRectangle(i * 30 + 30, 160 - map(ufDustGraph[i], 0, 90, 0, 120), i * 30 + 34, 156 - map(ufDustGraph[i], 0, 90, 0, 120), COLOR_GRAY);
  }
  for(int i = 1; i < 6; i++)
  {
    tft.drawLine(i * 30 + 32, 158 - map(ufDustGraph[i], 0, 90, 0, 120), (i - 1) * 30 + 32, 158 - map(ufDustGraph[i - 1], 0, 90, 0, 120), COLOR_GRAY);
  }

  for(int i = 0; i < 6; i++){
    String ufDustValue(ufDustGraph[i]);
    tft.setFont(Terminal6x8);
    if(ufDustGraph[i] < 10){
      tft.drawText(i * 30 + 30 - 1, 160 - map(ufDustGraph[i], 0, 90, 0, 120) - 15, ufDustValue, COLOR_WHITE);
    }
    else{
      tft.drawText(i * 30 + 30 - 4, 160 - map(ufDustGraph[i], 0, 90, 0, 120) - 15 + 2, ufDustValue, COLOR_WHITE);
    }
  }

  tft.drawText(6, 160 - 3, "0", COLOR_WHITE);
  tft.drawText(3, 130 - 3, "22", COLOR_WHITE);
  tft.drawText(3, 100 - 3, "45", COLOR_WHITE);
  tft.drawText(3, 70 - 3, "67", COLOR_WHITE);
  tft.drawText(3, 40 - 3, "90", COLOR_WHITE);
  
}

void menu5(){
  tft.clear();

  tft.setFont(Terminal11x16);

  tft.drawText(95, 5, "PM 10", COLOR_WHITE);

  tft.drawRectangle(20, 170, 200, 30, COLOR_GRAY);
  tft.drawLine(30, 160, 190, 160, COLOR_GRAY); 
  tft.drawLine(30, 130, 190, 130, COLOR_GRAY); 
  tft.drawLine(30, 100, 190, 100, COLOR_GRAY); 
  tft.drawLine(30, 70, 190, 70, COLOR_GRAY); 
  tft.drawLine(30, 40, 190, 40, COLOR_GRAY); 
  
      
  for(int i = 0; i < 6; i++)
  {
    tft.drawRectangle(i * 30 + 30, 160 - map(fDustGraph[i], 0, 180, 0, 120), i * 30 + 34, 156 - map(fDustGraph[i], 0, 180, 0, 120), COLOR_GRAY);
  }
  for(int i = 1; i < 6; i++)
  {
    tft.drawLine(i * 30 + 32, 158 - map(fDustGraph[i], 0, 180, 0, 120), (i - 1) * 30 + 32, 158 - map(fDustGraph[i - 1], 0, 180, 0, 120), COLOR_GRAY);
  }
  
  for(int i = 0; i < 6; i++){
    String fDustValue(fDustGraph[i]);
    tft.setFont(Terminal6x8);
    if(fDustGraph[i] < 10){
      tft.drawText(i * 30 + 30 - 1, 160 - map(fDustGraph[i], 0, 180, 0, 120) - 15, fDustValue, COLOR_WHITE);
    }
    else{
      tft.drawText(i * 30 + 30 - 4, 160 - map(fDustGraph[i], 0, 180, 0, 120) - 15 + 2, fDustValue, COLOR_WHITE);
    }
  }

  tft.drawText(6, 160 - 3, "0", COLOR_WHITE);
  tft.drawText(3, 130 - 3, "45", COLOR_WHITE);
  tft.drawText(3, 100 - 3, "90", COLOR_WHITE);
  tft.drawText(0, 70 - 3, "135", COLOR_WHITE);
  tft.drawText(0, 40 - 3, "180", COLOR_WHITE);
  
}


void setup() {
  tft.begin();
  Serial.begin(9600); 
  BT.begin(9600);
  mySerial.begin(9600);

  pinMode(MENUBT, INPUT_PULLUP);

  tft.setOrientation(1);

  tft.setGFXFont(&FreeMonoBold24pt7b);
  pinMode(2, INPUT_PULLUP);

}

void loop() {
  attachInterrupt(0, wakeUp, LOW);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  detachInterrupt(0);
  static int CheckFirst = 0;
  static int pm_add[3][5] = {0, };
  static int pm_old[3] = {0, };
  int chksum = 0, res = 0;
  unsigned char pms [32] = {0, };


  int count = 0;
  String ill1, ill2;

  

  BT.listen();
  while(!count){  
    while(!BT.available()){
      
    }
    if(BT.available()) {
      ill1 = BT.readStringUntil(0x0d);
    }
    
    while(!BT.available());
    if(BT.available()){
      ill2 = BT.readStringUntil(0x0d);
    }
    count = 1;
  }

  count = 0;

  if(ill1.substring(0, 1) == "t"){
    temp = ill1.substring(1);
  }
  else{
    humi = ill1.substring(1);
  }

  if(ill2.substring(0, 1) == "t"){
    temp = ill2.substring(1);
  }
  else{
    humi = ill2.substring(1);
  }

  mySerial.listen();
  while(!count){
    if (mySerial.available() >=  32) {
      
      for (int j = 0; j < 32; j++) {
        pms[j] = mySerial.read();
        if (j < 30)
        chksum += pms[j];
      }
  
      if (pms[30] != (unsigned char)(chksum >> 8) || pms[31] != (unsigned char)(chksum)) {
        return res;
      }
      if (pms[0] != 0x42 || pms[1] != 0x4d) {
        return res;
      }

  
      String str3(pms[12]);
      String str4(pms[13]);
      String str5(pms[14]);
      String str6(pms[15]);

      pm1 = str3 + str4;
      pm2 = str5 + str6;
  
      int inheritance_str3 = str3.toInt();
      int inheritance_str4 = str4.toInt();
      int inheritance_str5 = str5.toInt();
      int inheritance_str6 = str6.toInt();
  
      dust2 = inheritance_str3 + inheritance_str4;
      dust3 = inheritance_str5 + inheritance_str6;

      
int newHumiData = humi.toInt();
int newTempData = temp.toInt();
  for(int i = 0; i < 5; i++)
  {
    humiGraph[i] = humiGraph[i + 1];
    tempGraph[i] = tempGraph[i + 1];
    fDustGraph[i] = fDustGraph[i + 1];
    ufDustGraph[i] = ufDustGraph[i + 1];
  }
  humiGraph[5] = newHumiData;
  tempGraph[5] = newTempData;
  fDustGraph[5] = dust3;
  ufDustGraph[5] = dust2;

      

curIn = !digitalRead(MENUBT);
if(curIn == HIGH && curIn != prevIn) menuNum = (menuNum + 1) % 5;
      switch(menuNum)
      {
        case 0: menu1(); break;
        case 1: menu2(); break;
        case 2: menu3(); break;
        case 3: menu4(); break;
        case 4: menu5(); break;
      }
      
    }
    
    count = 0;
  }



}

void wakeUp()
{
  delay(100);
}
