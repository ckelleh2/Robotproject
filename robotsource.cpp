#include <stdio.h>
#include <sys/ioctl.h> // For FIONREAD
#include <termios.h>
#include <stdbool.h>
#include <string.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <time.h>

//Pin Definitions
const int M1E=3; //motor 1 enable
const int M1A=4; //motor 1 direction 1
const int M1B=5; //motor 1 direction 2
const int M2E=6; //same syntax motor 2
const int M2A=8;
const int M2B=9;
const int L1G=10; //Light 1 Green Cathode
const int L1B=11; //Light 1 Blue Cathode
const int L1R=12; //Light 1 Red Cathode
const int L2G=13;
const int L2B=14;
const int L2R=15;

//kbhit is a windows function, code to mimic it for linux taken from stackoverflow user ssinfod
//https://stackoverflow.com/questions/421860/capture-characters-from-standard-input-without-waiting-for-enter-to-be-pressed
//servos controlled using servoblaster
//because of the libraries being used and the need to read keyboard buffer as an interrupt, <iostream> is not available to be used

//menu functions for remote control mode
void headMenu(void);  
void armsMenu(void);
void moveMenu(void);
void routineMenu(void);
void remoteMenu(void); //top level

//functions that cause a specific action
void turn(double);
void takeStep(int);
void pinSetup(void);
void pinShutdown(void);

int kbhit(void) {
    static bool initflag = false;
    static const int STDIN = 0;

    if (!initflag) {
        // Use termios to turn off line buffering
        struct termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initflag = true;
    }

    int nbbytes;
    ioctl(STDIN, FIONREAD, &nbbytes);  // 0 is STDIN
    return nbbytes;
}

// main
#include <unistd.h>

int main(int argc, char** argv) 
{
	pinSetup();
	char c;
	int sad_level=100;
	int direction;
	int lightnum;
	int lightstate;
	pinSetup();
	printf("Enter r at any time to go to remote mode. \n");
	while(c!='x'){
    //setbuf(stdout, NULL); // Optional: No buffering.
    //setbuf(stdin, NULL);  // Optional: No buffering.
    while (!kbhit()) {
        printf("Press r for remote control \n");
	lightnum=rand()%6+10;   //generates a random number in the range of pins that controls the lights
	lightstate=rand()%2;    //random 0 or 1
	direction=rand%360;	//will be the orientation for the next step
	digitalWrite(lightnum,lightstate); //this writes a random HIGH/LOW to random light color each step
	turn(direction);	//turn to new direction
	takeStep(1);		//take step forward
        fflush(stdout);
        sleep(1);
    }
    c = getchar();
	printf("\n");
    if(c == 'r')
	{
		remoteMenu();
	}
    else
	{
		printf("Enter r for remote mode or x to end program. \n");
	}
   }
	pinShutdown();
    return 0;
}

void moveMenu(void)
{
	printf("move menu reached \n");
	printf("Acceptable commands: STEP, TURN, EXIT \n");	
	int flag=0;
	while(flag!=1)
	{
		char input[50];
		scanf ("%79s",input);
		if(strcmp(input,"EXIT")==0)
		{
			flag=1;
		}
		else if(strcmp(input,"STEP")==0
		{
			printf("FORWARD, BACK, LEFT, RIGHT or CANCEL \n");
			char dir[50];
			scanf ("%79s",dir);
			if(strcmp(dir,"FORWARD")==0)
			{
				takeStep(1);
			}
			else if(strcmp(dir,"BACK")==0)
			{
				takeStep(2);
			}
			else if(strcmp(dir,"RIGHT")==0)
			{
				takeStep(3);
			}
			else if(strcmp(dir,"LEFT")==0)
			{
				takeStep(4);
			}
			else
			{
				printf("FORWARD, BACK, LEFT, RIGHT or CANCEL \n");
			}
		}
		else if(strcmp(joint,"TURN")==0)
		{
			double pos;
			printf("What is your new angle? (0-360 degrees) \n");
			scanf ("%lf",&pos);
			if(pos<0)
			{
				pos=0.;
			}
			else if(pos>360.)
			{
				pos=360.;
			}
			turn(pos);
		}
		else
		{
			printf("Acceptable commands: STEP, TURN, EXIT \n");
		}
	}	
	return;
}
void takeStep(int direction)
{
	switch(direction)
	{
		case 1: //forwards
			//enable both motors, and turn on direction a for both
			digitalWrite(M1E,HIGH);
			digitalWrite(M1A,HIGH);
			digitalWrite(M1B,LOW);
	
			digitalWrite(M2E,HIGH); 
			digitalWrite(M2A,HIGH);
			digitalWrite(M2B,LOW);
			sleep(1); //wait for step to finish
			//turn off motors
			digitalWrite(M1E,LOW);
			digitalWrite(M1A,LOW);
			digitalWrite(M1B,LOW);
	
			digitalWrite(M2E,LOW); 
			digitalWrite(M2A,LOW);
			digitalWrite(M2B,LOW);
		case 2: //backwards
			//enable both motors, and turn on direction b for both
			digitalWrite(M1E,HIGH);
			digitalWrite(M1A,LOW);
			digitalWrite(M1B,HIGH);
	
			digitalWrite(M2E,HIGH); 
			digitalWrite(M2A,LOW);
			digitalWrite(M2B,HIGH);
			sleep(1); //wait for step to finish
			//turn off motors
			digitalWrite(M1E,LOW);
			digitalWrite(M1A,LOW);
			digitalWrite(M1B,LOW);
	
			digitalWrite(M2E,LOW); 
			digitalWrite(M2A,LOW);
			digitalWrite(M2B,LOW);
		case 3: //right
			turn(90.0); //turn 90 degrees to the right
			takeStep(1); //step forward
		case 4: //left
			turn(270.0); //turn 270 degrees to the right, (facing left after)
			takeStep(1); //step forward
			
	}
}
void turn(double angle)
{
	double time_constant=.0083; //maps the angle to the amount of time the motors need to run to get there
				//time_constant=time for full rotation/360 degrees,
	//enable both motors, and turn on direction a for 1 and b for 2 so it pivots in place
	digitalWrite(M1E,HIGH);
	digitalWrite(M1A,HIGH);
	digitalWrite(M1B,LOW);
	
	digitalWrite(M2E,HIGH); 
	digitalWrite(M2A,LOW);
	digitalWrite(M2B,HIGH);
	
	sleep(time_constant*angle); //wait for step to finish, proportional to the angle its going to
	//turn off motors
	digitalWrite(M1E,LOW);
	digitalWrite(M1A,LOW);
	digitalWrite(M1B,LOW);
	
	digitalWrite(M2E,LOW); 
	digitalWrite(M2A,LOW);
	digitalWrite(M2B,LOW);
}
void armsMenu(void)
{
	/* ServoBlaster creates an executable servod that you run with parameters that assign the pins of the servos
	*it then creates a file /dev/servoblaster. By writing <servo_number>=<pulse width> to /dev/servoblaster, the program
	*will emulate pwm with digital pins in the background */
	printf("arms menu reached \n");	
	printf("Acceptable commands: LEFT, RIGHT, HELP, EXIT \n");
	int flag=0;
	while(flag!=1)
	{
		char input[50];
		scanf ("%79s",input);
		if(strcmp(input,"EXIT")==0)
		{
			flag=1;
		}
		else if(strcmp(input,"LEFT")==0)
		{
			char joint[50];
			int jointflag=0;
			while(jointflag!=1)
			{	
				printf("Left Arm: SHOULDER, ELBOW or CANCEL");
				scanf ("%79s",joint);
				else if(strcmp(joint,"SHOULDER")==0)
				{
					double pos;
					double pulse;
					printf("What position? (0-180 degrees) \n");
					scanf ("%lf",&pos);
					if(pos<0)
					{
						pos=0;
					}
					else if(pos>180.)
					{
						pos=180;
					}
					pulse=.555*pos+100;
					FILE * pFile;  
  					pFile = fopen ("/dev/servoblaster", "a");
  					fprintf(pFile, "1=%lf", pulse);
  					fclose (pFile);
				}
				else if(strcmp(joint,"ELBOW")==0)
				{
					double pos;
					double pulse;
					printf("What position? (0-180 degrees) \n");
					scanf ("%lf",&pos);
					if(pos<0)
					{
						pos=0;
					}
					else if(pos>180.)
					{
						pos=180;
					}
					pulse=.555*pos+100;
					FILE * pFile;  
  					pFile = fopen ("/dev/servoblaster", "a");
  					fprintf(pFile, "2=%lf", pulse);
  					fclose (pFile);
				}
				else
				{
					printf("SHOULDER, ELBOW or CANCEL \n);
				}
			}
		}
		else if(strcmp(input,"RIGHT")==0)
		{
			char joint[50];
			int jointflag=0;
			while(jointflag!=1)
			{	
				printf("Right Arm: SHOULDER, ELBOW or CANCEL");
				scanf ("%79s",joint);
				else if(strcmp(joint,"SHOULDER")==0)
				{
					double pos;
					double pulse;
					printf("What position? (0-180 degrees) \n");
					scanf ("%lf",&pos);
					if(pos<0)
					{
						pos=0;
					}
					else if(pos>180.)
					{
						pos=180;
					}
					pulse=.555*pos+100;
					FILE * pFile;  
  					pFile = fopen ("/dev/servoblaster", "a");
  					fprintf(pFile, "3=%lf", pulse);
  					fclose (pFile);
				}
				else if(strcmp(joint,"ELBOW")==0)
				{
					double pos;
					double pulse;
					printf("What position? (0-180 degrees) \n");
					scanf ("%lf",&pos);
					if(pos<0)
					{
						pos=0;
					}
					else if(pos>180.)
					{
						pos=180;
					}
					pulse=.555*pos+100;
					FILE * pFile;  
  					pFile = fopen ("/dev/servoblaster", "a");
  					fprintf(pFile, "0=%lf", pulse);
  					fclose (pFile);
				}
				else
				{
					printf("SHOULDER, ELBOW or CANCEL \n);
				}
			}
		}
		else
		{
			printf("Arms Menu \n Acceptable commands: LEFT, RIGHT, HELP, EXIT \n");
		}
	}	
	return;
}
void headMenu(void)
{
	printf("head menu reached \n");
	printf("Acceptable commands: EYES, HELP, EXIT \n");
	int flag=0;
	while(flag!=1)
	{
		char input[50];
		scanf ("%79s",input);
		if(strcmp(input,"EXIT")==0)
		{
			flag=1;
		}
		else if(strcmp(input,"EYES")==0)
		{
			printf("LEFT or RIGHT or CANCEL \n");
			//this is the section we lost a lot of code in, there would be a right eye option
			//and all the colors would be supported but this illustrates the structure and idea
			char eye[50];
			int eyeflag=0;
			while(eyeflag!=1);
			{
				scanf("%79s",eye);
				if(strcmp(eye,"CANCEL")==0)
				{
					flag=1;
				}
				else if(strcmp(eye,"LEFT")==0)
				{
					printf("RED, GREEN, BLUE, PURPLE or EXIT \n");
					char color[50];
					scanf("%79s",color);
					int colorflag=0;
					while(colorflag!=1)
					{
						if(strcmp(color,"EXIT")==0
						{
							colorflag=1;
						}
						else if(strcmp(color,"BLUE")==0
						{
							digitalWrite(L1G,HIGH); //light turns on when a color cathode does not have current so this keeps them off
							digitalWrite(L1B,LOW);
							digitalWrite(L1R,HIGH);	
						}
						else
						{
							printf("Left Eye \n RED, GREEN, BLUE, PURPLE or EXIT \n");
						}
					}
				}
				else
				{
					printf("Eyes \n LEFT or RIGHT or CANCEL \n");
				}
			}
		}
		else
		{
			printf("Head Menu \n Acceptable commands: EYES, HELP, EXIT \n");
		}
	return;
}
void routineMenu(void)
{
	printf("routine menu reached \n");
	printf("Acceptable commands: Numbers 1-?, EYES, HELP, EXIT \n");
	int flag=0;
	while(flag!=1)
	{
		char input[50];
		scanf ("%79s",input);
		if(strcmp(input,"EXIT")==0)
		{
			flag=1;
		}
		
	}	
	return;
}
void remoteMenu(void)
{
	int flag=0;
	printf("remoteMode activated \n");
	printf("Acceptable commands: MOVE, ARMS, HEAD, ROUTINES, HELP, EXIT \n");
	while(flag!=1)
	{
		char input[50];
		scanf ("%79s",input);
		if(strcmp(input,"EXIT")==0)
		{
			flag=1;
		}
		else if(strcmp(input,"MOVE")==0)
		{
			moveMenu();
			printf("Remote Control Main Menu \n");
		}
		else if(strcmp(input,"ARMS")==0)
		{
			armsMenu();
			printf("Remote Control Main Menu \n");
		}
		else if(strcmp(input,"HEAD")==0)
		{
			headMenu();
			printf("Remote Control Main Menu \n");
		}
		else if(strcmp(input,"ROUTINES")==0)
		{
			routineMenu();
			printf("Remote Control Main Menu \n");
		}
		else if(strcmp(input,"HELP")==0)
		{
			printf("Help Message \n");
		}
		else
		{
			printf("Acceptable commands: MOVE, ARMS, HEAD, HELP, EXIT \n");
		}

	}
	return;
}
void pinSetup(void)
{
	wiringPiSetup(); //initializes library, allows for control of pins
	
	pinMode(M1E, OUTPUT);
	pinMode(M1A, OUTPUT);
	pinMode(M1B, OUTPUT);
	//motor2
	pinMode(M2E, OUTPUT);
	pinMode(M2A, OUTPUT);
	pinMode(M2B, OUTPUT);
	//light1
	pinMode(L1G, OUTPUT);
	pinMode(L1B, OUTPUT);
	pinMode(L1R, OUTPUT);
	//light2
	pinMode(L2G, OUTPUT);
	pinMode(L2B, OUTPUT);
	pinMode(L2R, OUTPUT);
	
	digitalWrite(M1E,HIGH); //enable motor 1, no signal to it
	digitalWrite(M1A,LOW);
	digitalWrite(M1B,LOW);
	
	digitalWrite(M2E,HIGH); //enable motor 2, no signal to it
	digitalWrite(M2A,LOW);
	digitalWrite(M2B,LOW);
	
	digitalWrite(L1G,HIGH); //light turns on when a color cathode does not have current so this keeps them off
	digitalWrite(L1B,HIGH);
	digitalWrite(L1R,HIGH);
	
	digitalWrite(L2G,HIGH);
	digitalWrite(L2B,HIGH);
	digitalWrite(L2R,HIGH);
}
void pinShutdown(void)
{
	
	digitalWrite(M1E,LOW);
	digitalWrite(M1A,LOW);
	digitalWrite(M1B,LOW);
	
	digitalWrite(M2E,LOW); //enable motor 2, no signal to it
	digitalWrite(M2A,LOW);
	digitalWrite(M2B,LOW);
	
	digitalWrite(L1G,LOW); //this actually causes the lights to be on for all colors but we want all pins to be low when program
	digitalWrite(L1B,LOW); //exits
	digitalWrite(L1R,LOW);
	
	digitalWrite(L2G,LOW);
	digitalWrite(L2B,LOW);
	digitalWrite(L2R,LOW);
}
