// EmonLibrary examples openenergymonitor.org, Licence GNU GPL V3

#include "EmonLib.h"                   // Include Emon Library
EnergyMonitor back_row_power, front_row_power_black, front_row_power_red, air_living_room, air_bed_room;                   // Create an instance

void setup()
{  
  Serial.begin(9600);
  Serial3.begin(9600);
   // Current: input pin, calibration.
  back_row_power.current(7, 11.75);
  front_row_power_black.current(5, 320);
  front_row_power_red.current(6, 11.2);
  air_living_room.current(4, 14);  // �b���q�y���ɭԥi��n, 16)
  air_bed_room.current(3, 11.0);

}

void loop()
{
   double Irms_back_row = back_row_power.calcIrms(1480);  // Calculate Irms only
   double Irms_front_row_black = front_row_power_black.calcIrms(1480);  // Calculate Irms only
   double Irms_front_row_red = front_row_power_red.calcIrms(1480);  // Calculate Irms only
   
   double Irms_air_living_room = air_living_room.calcIrms(1480);  // Calculate Irms only
   double Irms_air_bed_room = air_bed_room.calcIrms(1480);  // Calculate Irms only

/*  Serial.print(" Watt : ");
   Serial.println(Irms_back_row*110.0);	       // Apparent power
   Serial.println(Irms_front_row_black*110.0);
   Serial.println(Irms_front_row_red*110.0);
*/
  Serial.println(" << Current >> ");
  Serial.print(" A7 Room Row : ");  Serial.println(Irms_back_row);
  Serial.print(" A5 Front Row (black) : "); Serial.println(Irms_front_row_black);
  Serial.print(" A6 Front Row (red) : "); Serial.println(Irms_front_row_red);

  Serial.print(" A4 Living Air Row  : "); Serial.println(Irms_air_living_room);
  Serial.print(" A3 Bed Air Row  : "); Serial.println(Irms_air_bed_room);
  delay(1000);
  ///transmit format>>> Topic=oooo;Data=xxxx/n
  Serial3.print("Topic=Current/Room_Row;Data="); Serial3.println(Irms_back_row);
  delay(2000);
  Serial3.print("Topic=Current/Front_Row(Black);Data="); Serial3.println(Irms_front_row_black);
  delay(2500);
  Serial3.print("Topic=Current/Front_Row(red);Data="); Serial3.println(Irms_front_row_red);
  delay(2500);
  Serial3.print("Topic=Current/Living_Air;Data="); Serial3.println(Irms_air_living_room);
  delay(2500);
  Serial3.print("Topic=Current/Bed_Air;Data="); Serial3.println(Irms_air_bed_room);


  
}
