/** @file opcontrol.c
 * @brief File for operator control code
 *
 * This file should contain the user operatorControl() function and any functions related to it.
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"

/*
 * Runs the user operator control code. This function will be started in its own task with the
 * default priority and stack size whenever the robot is enabled via the Field Management System
 * or the VEX Competition Switch in the operator control mode. If the robot is disabled or
 * communications is lost, the operator control task will be stopped by the kernel. Re-enabling
 * the robot will restart the task, not resume it from where it left off.
 *
 * If no VEX Competition Switch or Field Management system is plugged in, the VEX Cortex will
 * run the operator control task. Be warned that this will also occur if the VEX Cortex is
 * tethered directly to a computer via the USB A to A cable without any VEX Joystick attached.
 *
 * Code running in this task can take almost any action, as the VEX Joystick is available and
 * the scheduler is operational. However, proper use of delay() or taskDelayUntil() is highly
 * recommended to give other tasks (including system tasks such as updating LCDs) time to run.
 *
 * This task should never exit; it should end with some kind of infinite loop, even if empty.
 */

 //sets the value of all four drive motors with one argument
 void setMotorPower (int power){
 	motorSet(1, power);
 	motorSet(2, power);
 	motorSet(3, -power);
 	motorSet(4, -power);
}

void operatorControl() {
//control loop variables, these variables are updated each loop to control motors
//X control loop variables
float P_X = 0;    //
float I_X = 0;
float Error_X = 0;
float output_X = 0;

//For Y control loop
float P_Y = 0;
float I_Y = 0;
float Error_Y = 0;

//For angle control loop
float P_Angle = 0;
float I_Angle = 0;
float Error_Angle = 0;

//target positions
float set_point_X = 30.0;
float set_point_Y = 30.0;
float set_point_Angle = 0;

//proportionality constants scale error term to motor output
float kP_y = 8;
float kP_x = 2.0;
float kI_x = 0.0001;
float I_X_UPPER_LIMIT = 185000.0;
float I_X_LOWER_LIMIT = -185000.0;

//kill loop bools
bool is_kill_toggle_pressed = false;
bool is_dead = true;


while (true){


	//when 8U is pressed to kill
if (is_kill_toggle_pressed && !joystickGetDigital(1, 8, JOY_UP)) {
    is_kill_toggle_pressed = false;
    is_dead = !is_dead;  //cf recursion
    if (is_dead){
      setMotorPower(0);
    }
  }

//when 8U is first pressed...
if (!is_kill_toggle_pressed && joystickGetDigital(1, 8, JOY_UP)) {
  is_kill_toggle_pressed = true;
}

//if it's dead (is_dead is true) go to start of while loop
if (is_dead){
  delay(20);
  continue;
}

	//ultrasonic sensor integer value cast to float, saved to varialbe
  //Sensor values are a unitless number representing distance
  //TODO Convert sensor data to some unit we can use, default is cm
	float sensor_x1 = (float) ultrasonicGet(frontRight);

  float sensor_x2 = (float) ultrasonicGet(frontLeft);

  float sensor_y1 = (float) ultrasonicGet(sideRight);

	//ignore negative one from ultrasonic (ie all negative values)
	/*If no object was found or if the ultrasonic sensor is polled
	while it is pinging and waiting for a response, -1 is returned*/
  //TODO consider instead of waiting 5ms for another reading, set
  //the sensor value to the previous reading.
  /*
	if(sensor_x1 == -1.0 || sensor_x2 == -1.0 || sensor_y1 == -1.0) {
		delay (5);
		continue;
	}
  */

 //Get error values //TODO need a target angle
 //error term, difference between target and sensed value
 Error_Angle = sensor_x1 - sensor_x2;

 Error_Y = sensor_y1 - set_point_Y;

 Error_X = sensor_x1 - set_point_X;

	//
  P_X = Error_X*kP_x;
  //integral term for X, accumulates errors over time
  I_X = I_X + Error_X;
  if(I_X >= I_X_UPPER_LIMIT) {
    I_X = I_X_UPPER_LIMIT;
  } else if(I_X <= I_X_LOWER_LIMIT) {
    I_X = I_X_LOWER_LIMIT;
  }

 output_X = (P_X + (I_X*kI_x));
/*
  float Kp3 = 1.25;
  float P3 = Yerror *Kp3;
  float output1 = P1 + (I1*ki1);
  float output2 = P2;
  float output3 = P3;
  */

/*
  motorSet(1, output1 + output2   + output3);
  motorSet(2, -output1 + output2  + output3);
  motorSet(3, output1 - output2  + output3);
  motorSet(4, -output1 - output2  + output3);
*/

  motorSet(1, output_X);
  motorSet(2, output_X);
  motorSet(3, -output_X);
  motorSet(4, -output_X);

 delay(5);
	//constant of proportionality, scales error term to acceptable motor input


	//motor power equals kp times error
	//setMotorPower (output2);
  // Hello

}
}
