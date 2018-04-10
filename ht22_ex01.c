#include <16F877.h>
#device ADC=10
#use delay(crystal=8000000)
#include"lcd_sam3.c"

#BIT Data_Pin = 0x006.0                       // Pin mapped to PORTb.0
#BIT Data_Pin_Direction = 0x086.0             // Pin direction mapped to TRISD.0



byte const num[12] = {'0','1','2','3','4','5','6','7','8','9'};

int8 temp100,temp10,temp0;
int8 hum100,hum10,hum0;

short Time_out ;
unsigned int8 T_byte1, T_byte2, RH_byte1, RH_byte2, CheckSum ;
unsigned int16 Temp, RH;
void start_signal(){
  Data_Pin_Direction = 0;              // Configure connection pin as output
  Data_Pin = 0;                        // Connection pin output low
  delay_ms(25);
  Data_Pin = 1;                        // Connection pin output high
  delay_us(30);
  Data_Pin_Direction = 1;              // Configure connection pin as input
}
short check_response(){
  delay_us(40);
  if(!Data_Pin){                     // Read and test if connection pin is low
    delay_us(80);
    if(Data_Pin){                    // Read and test if connection pin is high
      delay_us(50);
      return 1;}
    }
}
unsigned int8 Read_Data(){
  unsigned int8 i, k, _data = 0;     // k is used to count 1 bit reading duration
  if(Time_out)
    break;
  for(i = 0; i < 8; i++){
    k = 0;
    while(!Data_Pin){                          // Wait until pin goes high
      k++;
      if (k > 100) {Time_out = 1; break;}
      delay_us(1);}
    delay_us(30);
    if(!Data_Pin)
      bit_clear(_data, (7 - i));               // Clear bit (7 - i)
    else{
      bit_set(_data, (7 - i));                 // Set bit (7 - i)
      while(Data_Pin){                         // Wait until pin goes low
      k++;
      if (k > 100) {Time_out = 1; break;}
      delay_us(1);}
    }
  }
  return _data;
}



void main(){
  lcd_init();                                 // Initialize LCD module
  lcd_gotoxy(1, 3);
  lcd_putc(" sam Ht22 ");                             // LCD clear 
  while(TRUE){
    delay_ms(1000);
    Time_out = 0;
    Start_signal();
    if(check_response()){                    // If there is response from sensor
      RH_byte1 = Read_Data();                 // read RH byte1
      RH_byte2 = Read_Data();                 // read RH byte2
      T_byte1 = Read_Data();                  // read T byte1
      T_byte2 = Read_Data();                  // read T byte2
      Checksum = Read_Data();                 // read checksum
      if(Time_out){                           // If reading takes long time
      
        lcd_gotoxy(1,0); 
        lcd_putc("                ");                       // LCD clear
        lcd_gotoxy(2,0); 
        lcd_putc("                ");                       // LCD clear  
        
        
        lcd_gotoxy(1, 0);                     // Go to column 5 row 1
        lcd_putc("Time out!");
      }
      else{
       if(CheckSum == ((RH_Byte1 + RH_Byte2 + T_Byte1 + T_Byte2) & 0xFF)){
        RH = RH_byte1;
        RH = (RH << 8) | RH_byte2;
        Temp = T_byte1;
        Temp = (Temp << 8) | T_byte2;
        if (Temp > 0X8000){
        // message1[6] = '-';
         Temp = Temp & 0X7FFF; }
        else

        
        temp100 =  (Temp / 100) % 10 ;//////////////sam
        temp10  = (Temp / 10) % 10 ;////////////////sam
        temp0 = Temp % 10 ;////////////////////////sam
        
        
 
       
       hum100 =  (RH / 100) % 10 ;
       hum10 =  (RH / 10) % 10;
       hum0  = RH % 10;
        

       
        lcd_gotoxy(2, 0); lcd_putc("Temp");lcd_gotoxy(2, 5);
        
        lcd_putc(num[temp100]); lcd_putc(num[temp10]);lcd_putc("."); lcd_putc(num[temp0]); 
       
       lcd_gotoxy(2, 10); lcd_putc("RH"); lcd_gotoxy(2, 12);
       lcd_putc(num[hum100]); lcd_putc(num[hum10]);lcd_putc("."); lcd_putc(num[hum0]);  lcd_putc("%");      
       
       }
        else {
          //lcd_putc('\f');                     // LCD clear
          lcd_gotoxy(1, 1);                   // Go to column 1 row 1
          lcd_putc("Checksum Error!");
        }
      }
    }
    else {
      //lcd_putc('\f');             // LCD clear
      lcd_gotoxy(1, 0);           // Go to column 3 row 1
      lcd_putc("No response");
      lcd_gotoxy(1, 2);           // Go to column 1 row 2
      lcd_putc("from the sensor");
    }
  }
}
