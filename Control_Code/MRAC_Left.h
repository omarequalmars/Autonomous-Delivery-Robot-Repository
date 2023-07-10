int saturated_L(float ctrlaction, float satlim) { // saturation status
  /* this function returns
    1 if the ctrlaction is upper-limit-saturated
    and -1 if lower-limit-saturated */
  static int flag = 0;
  //  if(ctrlaction>=satlim)
  //    flag=1;
  //
  //  else if(ctrlaction<=-satlim)
  //    flag=-1;
  //  else
  //  flag=0;
  flag = ctrlaction >= satlim ? 1 : ctrlaction <= -satlim ? -1 : 0;
  return flag;
}
float P_ctrlr_L(float error, float P) {
  static float P_ctrlaction = 0;
  P_ctrlaction = error * P; // calculating ctrl action based on P
  return P_ctrlaction;
}
float I_ctrlr_L(float sumerror, float I, float T_samp) {
  static float I_ctrlaction = 0;
  I_ctrlaction = T_samp * sumerror * I; // calculating ctrl action based on I
  return I_ctrlaction;
}



float PID_ctrlr_withZOH_L(float error, float P, float I, float saturation_limit, float T_samp, int anti_windup)
{
  static float integrator_sum = 0;// integrator sum
  static float differror = 0;// error difference
  static float sumerror = 0;// cumulative error sum
  static float last_error = 0;// delayed error
  static short sat = 0;// saturation flag
  static short clamp_cond = 0;// clamping into operation
  static short zero_cross = 0;
  static float ctrlaction = 0;
  // differencing the error
  differror = error - last_error;

  // saturation flag
  sat = saturated_L(ctrlaction, saturation_limit);// check saturated or not
  switch (anti_windup) {
    case 0:// if clamping is deactivated
      sumerror += error;
      ctrlaction = P_ctrlr_L(error, P) + I_ctrlr_L(sumerror, I, T_samp);
      break;
    // if clamping is activated
    default:
      // check if it's clamping time
      zero_cross = (((last_error < 0) && (error > 0)) || ((last_error > 0) && (error < 0)));// 0,1
      clamp_cond = ((sat == 1) && (error >= 0)) || ((sat == -1) && (error <= 0));//0,1
        if (clamp_cond && zero_cross){
             sumerror = 0;
        integrator_sum = 0;
        ctrlaction = P_ctrlr_L(error, P) + integrator_sum;
        }
      else if (clamp_cond) {// stop integ. in clamping time
        ctrlaction = P_ctrlr_L(error, P) + integrator_sum;
      }
      else if (zero_cross) {
        sumerror = 0;
        integrator_sum = 0;

        ctrlaction = P_ctrlr_L(error, P) + integrator_sum;
      }
      else {

        sumerror += error;
        integrator_sum = I_ctrlr_L(sumerror, I, T_samp);
        ctrlaction = P_ctrlr_L(error, P) + integrator_sum;
      }
  }

  // delaying the error
  last_error = error;
  sat = saturated_L(ctrlaction, saturation_limit);// check saturated_R or not
  if (sat == 1)
    ctrlaction = saturation_limit;
  else if (sat == -1)
    ctrlaction = -saturation_limit;
  return ctrlaction;
  //ctrlaction = sat == 1 ? saturation_limit : sat == -1 ? -saturation_limit : ctrlaction;
}



float error_L(float state, float setpoint) { // calculating error
  return (setpoint - state);
}

float modelrefO1_L(float time_constant, float setpoint, float T_samp){
    static float state = 0;
    static float B_m = 1/time_constant;
    static float A_m = B_m;
    static float last_setpoint = 0;
    static float a = B_m*T_samp/(2+A_m*T_samp);
    static float b = (A_m*T_samp - 2)/(A_m*T_samp + 2);
    state = a*(setpoint + last_setpoint) - b*state;
    last_setpoint = setpoint;
    return state;
}

float ImproviseAdaptOvercome_L(float state_real, float gain_initial, float gain_max, float learning_rate, float time_constant, float setpoint, float T_samp){
    static float gain = gain_initial;
    static float tracking_error = 0;
    static float model_error = 0;
    tracking_error = setpoint - state_real;
    model_error = state_real - modelrefO1_L(time_constant, setpoint, T_samp);
    gain -= T_samp*learning_rate*model_error*tracking_error;
    return gain;
}

float Trap_L(float variable, float T_samp){
    static float X = 0;
    static float X_last = 0;
    static float integral = 0;
    X = variable;
    integral += (X + X_last)*T_samp/2;
    X_last = X;
    return integral;
}

float ImproviseAdaptOvercomeIntegral_L(float state_real, float gain_initial, float gain_max, float learning_rate, float time_constant, float setpoint, float T_samp){
    static float gain = gain_initial;
    static float tracking_error = 0;
    static float model_error = 0;
    tracking_error = setpoint - state_real;
    model_error = state_real - modelrefO1_L(time_constant, setpoint, T_samp);
    gain -= T_samp*learning_rate*model_error*Trap_L(tracking_error, T_samp);
    gain = (gain <= 0) ? 0 : gain;
    gain = (gain >= gain_max) ? gain_max : gain ;
    return gain;
}


float UnderControl_L(float speed_, float setpoint, float gain_initial, float gain_max, float gain_max_I, float learning_rate, float time_constant, float T_samp){
    static float gain = 0;
    static float gain_integral = 0;
    static float tracking_err = 0;
    static float ctrlaction = 0;
    gain = ImproviseAdaptOvercome_L(speed_, gain_initial, gain_max, learning_rate, time_constant, setpoint, T_samp);
    gain_integral = ImproviseAdaptOvercomeIntegral_L(speed_, gain_initial, gain_max_I, learning_rate, time_constant, setpoint, T_samp);
    tracking_err = error_L(speed_, setpoint);
    ctrlaction = PID_ctrlr_withZOH_L(tracking_err, gain, gain_integral, 255, T_samp, 1);
    return ctrlaction;
}
