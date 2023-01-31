void PWM_LED(){
  PWMflag = false;
  Day_time = h*hr_ul+m*mn_ul+s;
  for (byte i = 0; i < 6; i++) {
    if ((rgb_motion[i][0]>0) && (! PWMflag)) {
      sunrise_start= rgb_motion[i][1]*hr_ul+rgb_motion[i][2]*mn_ul;//Начало восхода
      sunrise_duration=rgb_motion[i][3]*mn_ul;//Длительность восхода
      sunset_start=rgb_motion[i][4]*hr_ul+rgb_motion[i][5]*mn_ul;
      sunset_duration=rgb_motion[i][6]*mn_ul;
      if ((Day_time>=sunrise_start) &&//Если с начала суток ,больше чем начало восхода 
  (Day_time<=sunset_start+sunset_duration)) { //и меньше чем начало заката + длительность 
        //********************************************************************************************* 
        // обработка интервала восхода
        //*********************************************************************************************
        if ((Day_time>=sunrise_start) && //Если с начала суток больше чем начало восхода 
  (Day_time<sunrise_start+sunrise_duration)){ //И меньше чем начало восхода + длительность 
          LR = ((Day_time - sunrise_start)*(rgb_motion[i][7]-PWM_LR_MIN)) / sunrise_duration; //Вычисляем для рассвета величину для записи в порт ШИМ 
          LG = ((Day_time - sunrise_start)*(rgb_motion[i][8]-PWM_LG_MIN)) / sunrise_duration; 
          LB = ((Day_time - sunrise_start)*(rgb_motion[i][9]-PWM_LB_MIN)) / sunrise_duration; //Вычисляем для рассвета величину для записи в порт ШИМ 
          LW = ((Day_time - sunrise_start)*(rgb_motion[i][10]-PWM_LW_MIN)) / sunrise_duration;
          LC = ((Day_time - sunrise_start)*(rgb_motion[i][11]-PWM_LC_MIN)) / sunrise_duration;
          PWMflag = true;
        }
        //********************************************************************************************* 
        // обработка интервала заката 
        //*********************************************************************************************
        else if ((Day_time>=sunset_start) && //Если начала суток больше чем начало заката и меньше чем 
  (Day_time<=sunset_start+sunset_duration)){//начало заката плюс длительность 
          LR = ((sunset_start+sunset_duration - Day_time)*(rgb_motion[i][7]-PWM_LR_MIN)) / sunset_duration; //Вычисляем для заката величину для записи в порт ШИМ 
          LG = ((sunset_start+sunset_duration - Day_time)*(rgb_motion[i][8]-PWM_LG_MIN)) / sunset_duration; 
          LB = ((sunset_start+sunset_duration - Day_time)*(rgb_motion[i][9]-PWM_LB_MIN)) / sunset_duration;
          LW = ((sunset_start+sunset_duration - Day_time)*(rgb_motion[i][10]-PWM_LW_MIN)) / sunset_duration;
          LC = ((sunset_start+sunset_duration - Day_time)*(rgb_motion[i][11]-PWM_LC_MIN)) / sunset_duration;
          PWMflag = true;
        }
        //******************************************************************************************** 
        // обработка интервала от конца рассвета и до начала заката, 
        // когда свет должен быть включен на максимальную яркость 
        //********************************************************************************************  
        else { 
          LR = rgb_motion[i][7]; //Устанавливаем максимальную величину для записи в порт ШИМ 
          LG = rgb_motion[i][8];
          LB = rgb_motion[i][9]; //Устанавливаем максимальную величину для записи в порт ШИМ 
          LW = rgb_motion[i][10];
          LC = rgb_motion[i][11]; 
          PWMflag = true;
        } 
      }

    }
  }
  if (! PWMflag)
  {
    LR = PWM_LR_MIN; //Величина для записи в порт равна минимуму 
    LG = PWM_LG_MIN; 
    LB = PWM_LB_MIN;
    LW = PWM_LW_MIN;
    LC = PWM_LC_MIN; 
  }
 
  pwm.setPWM(2, 0, map(LR,0, 100, 0, 4095));
  pwm.setPWM(0, 0, map(LG,0, 100, 0, 4095));
  pwm.setPWM(1, 0, map(LB,0, 100, 0, 4095));
  pwm.setPWM(4, 0, map(LW,0, 100, 0, 4095));
  pwm.setPWM(3, 0, map(LC,0, 100, 0, 4095));
//  Serial.print(" LR"+String(LR)+"/");  
}
