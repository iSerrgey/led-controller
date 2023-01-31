//***Функция считывания температуры c Далласов*****
void dallRead(unsigned long interval) {
  static unsigned long prevTime = 0;
  if (millis() - prevTime > interval) { //Проверка заданного интервала
    static boolean flagDall = 0; //Признак операции
    prevTime = millis();
    flagDall = ! flagDall; //Инверсия признака
    if (flagDall) {
      ///////////
      if ( !ds.search(addr)) {
        //no more sensors on chain, reset search
        ds.reset_search();
        return +1000;
      }
      if ( addr[0] != 0x10 && addr[0] != 0x28) {
      //  Serial.print("Device is not recognized");
        return -1000;
      }
      ds.reset(); // сброс шины
      ds.select(0xCC); //выставить адрес
      ds.write(0x4E); // разрешение записать конфиг
      ds.write(0x7F); // точность 0,5гр = 1F; 0,25гр = 3F; 0,125гр = 5F; 0,0625гр = 7F
      ds.write(0xFF); //Tl контроль температуры мин -128грд
   //   ds.write(0x7F); // точность 0,5гр = 1F; 0,25гр = 3F; 0,125гр = 5F; 0,0625гр = 7F
   
  /*   ds.reset(); // сброс шины
      ds.select(0xCC); //выставить адрес
      ds.write(0x4E); // разрешение записать конфиг
      ds.write(0x00); // точность 0,5гр = 1F; 0,25гр = 3F; 0,125гр = 5F; 0,0625гр = 7F
      ds.write(0x00); //Tl контроль температуры мин -128грд
      ds.write(0x7F); // точность 0,5гр = 1F; 0,25гр = 3F; 0,125гр = 5F; 0,0625гр = 7F*/
      ////////////
      ds.reset();
      ds.write(0xCC); //Обращение ко всем датчикам
      ds.write(0x44, 1); //Команда на конвертацию 1 -паразитное питание
      flagDallRead = 1; //Время возврата в секундах
    }
    else {
      ds.reset();
      ds.select(addr); // обращаемся по адресу
      ds.write(0xBE, 1); // чтение из памяти
      for (int i = 0; i < 9; i++) { // we need 9 bytes
        data[i] = ds.read ();
      }
      ds.reset_search();
      int16_t raw = ((data[1] << 8) | data[0]);//=======Пересчитываем в температуру
      Temp_val = (float)raw / 16.0;
    }
  }
  /*sensor.requestTemp();
  delay(1000); //нужно попробовать убрать
  if ( sensor.readTemp()) {
    Temp_val = sensor.getTemp();
  }
  else {
    if (Temp_val>0){
      Temp_val=Temp_val;
    }
    else{
      Temp_val=0;
    }             
  }*//*
  if (Temp_val > (x_temp - delta)){
    if (Temp_val > (x_temp + delta)) {digitalWrite(cool_pin, HIGH);}
    if ((Temp_val > (x_temp - delta)) && (Temp_val <= (x_temp + delta))){
      fanSpeed = map (Temp_val, x_temp - delta, x_temp + delta, 120, 255);
      analogWrite(cool_pin, fanSpeed);
    }
  }
  if (Temp_val < (x_temp - 2*delta)) {
    digitalWrite(cool_pin, LOW);
  }
*/
    
  if ((Temp_val - x_temp) > delta) { 
    digitalWrite(cool_pin, HIGH);
  }
  if ((x_temp - Temp_val) > delta) {
    digitalWrite(cool_pin, LOW);
  }
      //Temp_val = (float)raw / 16.0;
    //  Serial.print("/temp "+String(Temp_val)+"/");
 //   }
 // }
}
//--------------------------------------------------
