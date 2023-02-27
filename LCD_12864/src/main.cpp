#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>





U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, /* clock=*/ 14, /* data=*/ 13, /* CS=*/ 15);
//esp8266 gpio14-clok,

int temp = 5;
void setup(void) {

  
  u8g2.begin();  
}

void loop(void) {
  u8g2.firstPage();
  do {
   u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.print(temp);
    
  } while ( u8g2.nextPage() );

  delay(1000);
  temp++;
  
}