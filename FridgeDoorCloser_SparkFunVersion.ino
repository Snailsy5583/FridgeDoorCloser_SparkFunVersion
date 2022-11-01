#include <SparkFun_ProDriver_TC78H670FTG_Arduino_Library.h>


///////////////////////////////////////////////////
// Setup

// pin nums
int StepperMotorBeginPin = 3;
int PirPin = 2;
int ButtonPin = 9;

// create the stepper motor
PRODRIVER Stepper;

// Runtime Variables
bool OldButtonValue = false;
int StepAmount = 0;

void setup()
{
	Stepper.settings.errorPin = 8;
	Stepper.settings.standbyPin = 10;
	Stepper.begin();

	// setup button
	pinMode(ButtonPin, INPUT);

	// setup PIR
	pinMode(PirPin, INPUT);

	Serial.begin(9600);
}

///////////////////////////////////////////////////
// Sensor funcs

// returns whether button is activated
bool CheckButton()
{
	Serial.write("Button: ");
	Serial.println(digitalRead(ButtonPin));
	return digitalRead(ButtonPin);
}

// returns whether PIR is activated
bool CheckPIR()
{
	Serial.write("PIR: ");
	Serial.println(digitalRead(PirPin));
	return digitalRead(PirPin);
}

///////////////////////////////////////////////////
// Functional Functions

// record # of steps to close the door
void CloseDoor(int& OutStepAmount)
{
	while (!CheckButton() && !CheckPIR())
	{
		Stepper.step(10, true, 1U);
		OutStepAmount+= 10;
	}
}

// unwind the String after door is closed
void UnwindString(int& Steps)
{
	Stepper.step((uint16_t) Steps, false, 1U);
	Steps = 0;
}

///////////////////////////////////////////////////
// Main Loop
void loop()
{
	// If need be, close the door
	bool CurrentButtonValue = CheckButton();
	if (!CurrentButtonValue && !CheckPIR())
	{
		CloseDoor(StepAmount);
	}

	if (OldButtonValue != CurrentButtonValue)
	{
		// If door just closed, unwind string and lower stepper motor current limit (to deal with heat issues)
		if (CurrentButtonValue)
		{
			UnwindString(StepAmount);
			Stepper.setTorque(PRODRIVER_TRQ_25);
		}
		else // else reset stepper motor current limit
		{
			Stepper.setTorque(PRODRIVER_TRQ_100);
		}
		Stepper.sendSerialCommand(); // to apply the Stepper.setTorque()
	}

	OldButtonValue = CurrentButtonValue;
}