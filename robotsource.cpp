#include <stdio.h>
#include <sys/ioctl.h> // For FIONREAD
#include <termios.h>
#include <stdbool.h>
#include <string.h>

//kbhit is a windows function, code to mimic it for linux taken from stackoverflow user ssinfod
//https://stackoverflow.com/questions/421860/capture-characters-from-standard-input-without-waiting-for-enter-to-be-pressed
//servos controlled using servoblaster

//menu functions for remote control mode
void headMenu(void);  
void armsMenu(void);
void moveMenu(void);
void routineMenu(void);
void remoteMenu(void); //top level

//functions that cause a specific action

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
	char c;
	int sad_level=100;
	printf("Enter r at any time to go to remote mode. \n");
	while(1){
    //setbuf(stdout, NULL); // Optional: No buffering.
    //setbuf(stdin, NULL);  // Optional: No buffering.
    while (!kbhit()) {
        printf("Would be doing autonomous stuff right now... \n");
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
		printf("Enter r for remote mode. \n");
	}
   }
    return 0;
}

void moveMenu(void)
{
	printf("move menu reached \n");
	printf("Acceptable commands: STEP, TURN, SPEED, HELP, EXIT \n");	
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
void armsMenu(void)
{
	printf("arms menu reached \n");	
	printf("Acceptable commands: LEFT, RIGHT, BOTH, HELP, EXIT \n");
	int flag=0;
	while(flag!=1)
	{
		char input[50];
		scanf ("%79s",input);
		if(strcmp(input,"EXIT")==0)
		{
			flag=1;
		}
		if(strcmp(input,"LEFT")==0)
		{
			printf("SHOULDER or ELBOW");
			char joint[50];
			scanf ("%79s",joint);
			if(strcmp(joint,"SHOULDER")==0)
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
  				fprintf(pFile, "whatever pin left shoulder is=%lf", pulse);
  				fclose (pFile);
			}
			if(strcmp(joint,"ELBOW")==0)
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
  				fprintf(pFile, "whatever pin left elbow is=%lf", pulse);
  				fclose (pFile);
			}
		if(strcmp(input,"RIGHT")==0)
		{
			printf("SHOULDER or ELBOW");
			char joint[50];
			scanf ("%79s",joint);
			if(strcmp(joint,"SHOULDER")==0)
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
  				fprintf(pFile, "whatever pin right shoulder is=%lf", pulse);
  				fclose (pFile);
			}
			if(strcmp(joint,"ELBOW")==0)
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
  				fprintf(pFile, "whatever pin right elbow is=%lf", pulse);
  				fclose (pFile);
			}
			
		}
	}	
	return;
}
void headMenu(void)
{
	printf("head menu reached \n");
	printf("Acceptable commands: TURN, EYES, HELP, EXIT \n");
	int flag=0;
	while(flag!=1)
	{
		char input[50];
		scanf ("%79s",input);
		if(strcmp(input,"EXIT")==0)
		{
			flag=1;
		}
		if(strcmp(joint,"SHOULDER")==0)
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
  			fprintf(pFile, "whatever servo head is=%lf", pulse);
 			fclose (pFile);
		}
		if(strcmp(input,"LIGHTS")==0)
		{
			flag=1;
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
