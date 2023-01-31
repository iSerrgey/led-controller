void parsing() {
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();        // обязательно ЧИТАЕМ входящий символ
    if (getStarted) {                         // если приняли начальный символ (парсинг разрешён)
      if (incomingByte != ' ' && incomingByte != ';') {   // если это не пробел И не конец
        string_convert += incomingByte;       // складываем в строку
      } else {                                // если это пробел или ; конец пакета
        intData[index] = string_convert.toInt();  // преобразуем строку в int и кладём в массив
        string_convert = "";                  // очищаем строку
        index++;                              // переходим к парсингу следующего элемента массива
      }
    }
    if (incomingByte == '$') {                // если это $
      getStarted = true;                      // поднимаем флаг, что можно парсить
      index = 0;                              // сбрасываем индекс
      string_convert = "";                    // очищаем строку
    }
    if (incomingByte == ';') {                // если таки приняли ; - конец парсинга
      getStarted = false;                     // сброс
      recievedFlag = true;                    // флаг на принятие
    }
  }
}

/*
 * код 400 таймеры для ргб
401 - новый таймер;
402 - перезапись таймера;
Структура строки данных $401 0 9 45 20 15 20 30 1 2 3 4 5;
401 - код действия
0 - код таймера (1,2,3,4,5)
9 - час старта таймера
45 - минуты старта таймера
20 - длительность рассвета
15 - час начала заката
20 - минуты начала заката
30 - длительность заката
1 - канал LR (от 0 до 100)
2 - канал LG (от 0 до 100)
3 - канал LB (от 0 до 100)
4 - канал LW (от 0 до 100)
5 - канал LC (от 0 до 100)
в еепром сохраняем в ячейки с 400 на один таймер 20 ячеек, т.е. по 600.
391-395 принимает значение 0 или 1, таймер активен или нет
403 - удалить таймер (пример запроса $404 1;);
404 - информация по таймерам; (пример запроса $404;)
301 запрос температуры датчика (пример запроса $301;)
302 запрос ограничения по температуре и дельте (пример запроса $302;)
303 обновить ограничение температуры для вентилятора (пример запроса $303 37;)
304 обновить дельту (пример запроса $304 2;)
305 текущее время;
405 текущее состояние переменных светодиодов;
*/
void btcommand() {
  if (recievedFlag) {                           // если получены данные
    recievedFlag = false;
    for (byte i = 0; i < PARSE_AMOUNT; i++) { // выводим элементы массива
      Serial.print(intData[i]); Serial.print(" ");
    } Serial.println();
    switch (intData[0]) {  // согласно коду команды
      case 401:   // тут можно читать данные из buffer согласно коду команды
        PWMSaveflag=false;
        if ((intData[2]>=0) && (intData[2]<24) && (intData[3]>=0) && (intData[3]<61) && (intData[4]>=0) && (intData[4]<121) && (intData[5]>=0) && (intData[5]<24) && 
        (intData[6]>=0) && (intData[6]<61) && (intData[7]>=0) && (intData[7]<121) && (intData[8]>=0) && (intData[8]<101) && (intData[9]>=0) && (intData[9]<101) && 
        (intData[10]>=0) && (intData[10]<101)  && (intData[11]>=0) && (intData[11]<101) && (intData[12]>=0) && (intData[12]<101)){
          for (byte i = 0; i < 5; i++) {
            if ((rgb_motion[i][0]==0)) {
              for (byte z = 0; z < PARSE_AMOUNT-2; z++) {
                rgb_motion[i][z+1]=intData[z+2];//EEPROM.read(i*20 + z + 400);
                EEPROM.write(i*20 + z + 400,rgb_motion[i][z+1]);
              }
              EEPROM.write(i + 391,1);
              rgb_motion[i][0]=1;
              Serial.println("timer saved");
              PWMSaveflag=true;
              i=6;
            }
          }
          if (!PWMSaveflag){Serial.println("all cells are occupied");}
        } 
        else{
          Serial.println("error saving");
        }
        break;
      case 402:   // тут можно читать данные из buffer согласно коду команды 
        if ((intData[1]>0) && (intData[1]<6) && (intData[2]>=0) && (intData[2]<24) && (intData[3]>=0) && (intData[3]<61) && (intData[4]>=0) && (intData[4]<121) && (intData[5]>=0) && (intData[5]<24) && 
        (intData[6]>=0) && (intData[6]<61) && (intData[7]>=0) && (intData[7]<121) && (intData[8]>=0) && (intData[8]<101) && (intData[9]>=0) && (intData[9]<101) && 
        (intData[10]>=0) && (intData[10]<101)  && (intData[11]>=0) && (intData[11]<101) && (intData[12]>=0) && (intData[12]<101)){
          for (byte z = 0; z < PARSE_AMOUNT-2; z++) {
            rgb_motion[intData[1]-1][z+1]=intData[z+2];//EEPROM.read(i*20 + z + 400);
            EEPROM.write((intData[1]-1)*20 + z + 400,rgb_motion[intData[1]-1][z+1]);
          }
          EEPROM.write(intData[1] + 390,1);
          rgb_motion[intData[1]-1][0]=1;
          Serial.println("timer saved");
        } 
        else{
            Serial.println("error saving");
        }
        break;
      case 403:   // тут можно читать данные из buffer согласно коду команды
        if ((intData[1]>0) && (intData[1]<6)){
          EEPROM.write(intData[1] + 390,0);
          rgb_motion[intData[1]-1][0]=0;
          Serial.println("timer deleted");
        } 
        else{
          Serial.println("error delete");
        }
        break;
      case 404:   // тут можно читать данные из buffer согласно коду команды
        for (byte i = 0; i < 5; i++) {
          if ((rgb_motion[i][0]>0)) {
            Serial.print("$");
            for (byte z = 0; z < PARSE_AMOUNT-2; z++) {
              Serial.print(rgb_motion[i][z+1]); Serial.print(" ");
            }
            Serial.print(";"); 
            Serial.println();
          }
        }
        break;
      case 301:   // тут можно читать данные из buffer согласно коду команды
        Serial.print("$");
        Serial.print(Temp_val);
        Serial.print(";"); 
        Serial.println();
        break;
      case 302:   // тут можно читать данные из buffer согласно коду команды
        Serial.print("$");
        Serial.print(x_temp);
        Serial.print(" ");
        Serial.print(delta);
        Serial.print(";"); 
        Serial.println();      
        break;
      case 303:   // тут можно читать данные из buffer согласно коду команды
        if ((intData[1]>0) && (intData[1]<60)){
          EEPROM.write(35,intData[1]);
          x_temp=intData[1];
          Serial.println("the temperature limit has been updated");
        } 
        else{
          Serial.println("error updating");
        }
        break;
      case 304:   // тут можно читать данные из buffer согласно коду команды
        delta = EEPROM.read(36);
        if ((intData[1]>=0) && (intData[1]<20)){
          EEPROM.write(36,intData[1]);
          delta=intData[1];
          Serial.println("the temperature delta has been updated");
        } 
        else{
          Serial.println("error updating");
        }
        break;
      case 305:   // тут можно читать данные из buffer согласно коду команды
        Serial.print(String(h)+":"+String(m)+":"+String(s)); 
        Serial.println();      
        break;
      case 405:   // тут можно читать данные из buffer согласно коду команды
        Serial.print(String(LR)+" "+String(LG)+" "+String(LB)+" "+String(LW)+" "+String(LC)); 
        Serial.println();      
        break;
    }
  }
}
