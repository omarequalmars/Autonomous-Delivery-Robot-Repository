

void MoveitMoveit(int PulseWidth, int A_channel, int B_channel, int INPUT_PIN){

  // Saturate output at 255
    if (PulseWidth>=255){
        PulseWidth = 255;
    }else if(PulseWidth<=-255){
      PulseWidth = -255;
    }
    // decide directions based on PWM sign
    if(PulseWidth >=0){ 
      // if command is positive, go CCW/CW
        digitalWrite(A_channel,HIGH);
        digitalWrite(B_channel,LOW);
        analogWrite(INPUT_PIN,PulseWidth);
    }
    else if(PulseWidth < 0){ 
      // if it switches signs, go the other way
        digitalWrite(A_channel,LOW);
        digitalWrite(B_channel,HIGH);
        analogWrite(INPUT_PIN,-PulseWidth);
    }
    
}

void Freebird(int PWM, int PWM_pin, int DIR_pin){
   if (PWM>=255){
        PWM = 255;
    }else if(PWM<=-255){
      PWM = -255;
    }
    // decide directions based on PWM sign
    if(PWM >=0){ 
      // if command is positive, go CCW/CW
        digitalWrite(DIR_pin,HIGH);
        analogWrite(PWM_pin,PWM);
    }
    else if(PWM < 0){ 
      // if it switches signs, go the other way
        digitalWrite(DIR_pin,LOW);
        analogWrite(PWM_pin,-PWM);
}
}
