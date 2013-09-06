/* Projeto de automacao de Aquario*/
#include <Wire.h>
#include <Time.h>
#include <DS3231RTC.h>

#define DS1307_ADDRESS      0x68
#define HORA_ACENDIMENTO    18
#define MINUTO_ACENDIMENTO  30
#define HORA_DESLIGAMENTO   22
#define MINUTO_DESLIGAMENTO 30

boolean _luzesAcesas;

void setup(void){
  Wire.begin();
  Serial.begin(9600);
  _luzesAcesas = false;
}

void loop(){
  verificarHorarioLuzes();
  delay(1000);
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
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.print(second);
  Serial.print("\n");
  if(hour > HORA_DESLIGAMENTO || ( hour == HORA_DESLIGAMENTO && minute >= MINUTO_DESLIGAMENTO)){
    //Desliga Rele
    Serial.write("Luzes Apagadas 1\n");
    mudarEstadoLuzes(false);
    return;
  } else if(hour > HORA_ACENDIMENTO || (hour == HORA_ACENDIMENTO && minute >= MINUTO_ACENDIMENTO)) {    
    //Liga Rele
    Serial.write("Luzes Acesas\n");
    mudarEstadoLuzes(true);
    return;
  } else {
    //Por default o rele fica desligado
    Serial.write("Luzes Apagadas 2\n");
    mudarEstadoLuzes(false);
    return;
  }

}

void mudarEstadoLuzes(boolean acender){
/*  if(acender && !_luzesAcesas){
    //Procedimento Acender Luzes
    _luzesAcesas = true;
  }
  
  if(!acender && _luzesAcesas){
    //Procedimento Apagar Luzes
    _luzesAcesas = false;
  }*/
}

