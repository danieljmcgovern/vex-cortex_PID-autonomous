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
 	motorSet(2, -power);
 	motorSet(3, power);
 	motorSet(4, -power);
 }

void operatorControl() {

while (true){

	//kill button, motors set to zero while holding down 8U
	if (joystickGetDigital(1, 8, JOY_UP)){
		setMotorPower(0);
		//When continue statement is encountered in loop, control jumps to beginning of loop
		//ie keep checking if 8U is still pressed
		continue;
	}

	//target point, how far from wall bot should station keep
	float set_point = 30.0;
	//frong right ultrasonic sensor integer value cast to float, saved to varialbe
	float sensor_x = (float) ultrasonicGet(frontRight);

	//ignore negative one from ultrasonic (ie all negative values)
	/*If no object was found or if the ultrasonic sensor is polled
	while it is pinging and waiting for a response, -1 is returned*/
	if(sensor_x == -1.0) {
		delay (5);
		continue;
	}

	//error term, difference between target and present location
	float error = set_point - sensor_x;
	//constant of proportionality, scales error term to acceptable motor input
	float kp = 2.0;

	//motor power equals kp times error
	setMotorPower (error*kp);

}
}
