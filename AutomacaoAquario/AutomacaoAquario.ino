/* Projeto de automacao de Aquario*/
#include <Wire.h>
#include <Time.h>
#include <DS3231RTC.h>
#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define DS1307_ADDRESS          0x68
#define HORA_ACENDIMENTO        16
#define MINUTO_ACENDIMENTO      30
#define HORA_DESLIGAMENTO       22
#define MINUTO_DESLIGAMENTO     30
#define PORTA_PWM_RELE          8
#define PORTA_LED_MEGA          13

/* Portas Sensores Temperatura */
#define SENSOR_TEMPERATURA_1 10

/* Portas do LCD */
#define PORTA_RS        12
#define PORTA_ENABLE    11
#define PORTA_D4        5
#define PORTA_D5        4
#define PORTA_D6        3
#define PORTA_D7        2
#define PORTA_LUZ_FUNDO 9

boolean _luzesAcesas;
LiquidCrystal lcd(PORTA_RS, PORTA_ENABLE, PORTA_D4, PORTA_D5, PORTA_D6, PORTA_D7);
OneWire oneWire(SENSOR_TEMPERATURA_1);
DallasTemperature sensor1(&oneWire);

void setup(void){
  Wire.begin();
  Serial.begin(9600);
  lcd.begin(20, 4);
  sensor1.begin();


  pinMode(PORTA_PWM_RELE, OUTPUT);
  pinMode(PORTA_LED_MEGA, OUTPUT);
  pinMode(PORTA_LUZ_FUNDO, OUTPUT);
  digitalWrite(PORTA_PWM_RELE, HIGH);
  digitalWrite(PORTA_LUZ_FUNDO, HIGH);
  _luzesAcesas = false;
}

void loop(){
  verificarHorarioLuzes();
  verificarTemperatura();
  digitalWrite(PORTA_LED_MEGA, LOW);
  delay(500);
  digitalWrite(PORTA_LED_MEGA, HIGH);
  delay(500);
}

void verificarTemperatura(){
  sensor1.requestTemperatures();
  lcd.setCursor(0,3);
  lcd.print("1 - ");
  lcd.print(sensor1.getTempCByIndex(0));
}
byte bcdToDec(byte val)  {
// Convert binary coded decimal to normal decimal numbers
  return ( (val/16*10) + (val%16) );
}

void verificarHorarioLuzes(){
  
  Wire.beginTransmission(DS1307_ADDRESS);

  byte zero = 0x00;
  Wire.write(zero);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_ADDRESS, 7);

  int second = bcdToDec(Wire.read());
  int minute = bcdToDec(Wire.read());
  int hour = bcdToDec(Wire.read() & 0b111111); //24 hour time
  int weekDay = bcdToDec(Wire.read()); //0-6 -> sunday - Saturday
  int monthDay = bcdToDec(Wire.read());
  int month = bcdToDec(Wire.read());
  int year = bcdToDec(Wire.read());
  
  if(hour > 23)
    hour = 0;
  
  if(minute > 59)
    minute = 0;
  
  if(second > 59)
    second = 0;
  
  lcd.setCursor(0,1);
  lcd.print(hour <  10 ? "0" + String(hour) : hour);
  lcd.print(":");
  lcd.print(minute < 10 ? "0" + String(minute) : minute);
  lcd.print(":");
  lcd.print(second < 10 ? "0" + String(second) : second);
  lcd.print(" ");
  if(hour > HORA_DESLIGAMENTO || ( hour == HORA_DESLIGAMENTO && minute >= MINUTO_DESLIGAMENTO)){
    //Desliga Rele
    if(_luzesAcesas){
       digitalWrite(PORTA_PWM_RELE, HIGH);
       _luzesAcesas = false;
    }    
  } else if(hour > HORA_ACENDIMENTO || (hour == HORA_ACENDIMENTO && minute >= MINUTO_ACENDIMENTO)) {    
    //Liga Rele
    if(!_luzesAcesas){
       digitalWrite(PORTA_PWM_RELE, LOW);
       _luzesAcesas = true;
    }
  } else {
    //Por default o rele fica desligado
    if(_luzesAcesas){
       digitalWrite(PORTA_PWM_RELE, HIGH);
       _luzesAcesas = false;
    }   
  }
  
  if(_luzesAcesas)
    lcd.print("Luz Acesa");
  else
    lcd.print("Luz Apagada");

}

void mudarEstadoLuzes(boolean acender){
  if(acender && !_luzesAcesas){
    //Procedimento Acender Luzes
     digitalWrite(PORTA_PWM_RELE, LOW);
    _luzesAcesas = true;
  }
  
  if(!acender && _luzesAcesas){
    //Procedimento Apagar Luzes
    digitalWrite(PORTA_PWM_RELE, HIGH);
    _luzesAcesas = false;
  }
}

