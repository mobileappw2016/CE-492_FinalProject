/*
 * Wiegand API Raspberry Pi
 * By Kyle Mallory
 * 12/01/2013
 * Based on previous code by Daniel Smith (www.pagemac.com) and Ben Kent (www.pidoorman.com)
 * Depends on the wiringPi library by Gordon Henterson: https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 * The Wiegand interface has two data lines, DATA0 and DATA1.  These lines are normall held
 * high at 5V.  When a 0 is sent, DATA0 drops to 0V for a few us.  When a 1 is sent, DATA1 drops
 * to 0V for a few us. There are a few ms between the pulses.
 *   *************
 *   * IMPORTANT *
 *   *************
 *   The Raspberry Pi GPIO pins are 3.3V, NOT 5V. Please take appropriate precautions to bring the
 *   5V Data 0 and Data 1 voltges down. I used a 330 ohm resistor and 3V3 Zenner diode for each
 *   connection. FAILURE TO DO THIS WILL PROBABLY BLOW UP THE RASPBERRY PI!
 */
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <time.h>
#include <unistd.h>
#include <memory.h>
#include <string.h>

#define D0_PIN 0
#define D1_PIN 1

#define WIEGANDMAXDATA 32
#define WIEGANDTIMEOUT 3000000

static unsigned char __wiegandData[WIEGANDMAXDATA];    // can capture upto 32 bytes of data -- FIXME: Make this dynamically allocated in init?
static unsigned long __wiegandBitCount;            // number of bits currently captured
static struct timespec __wiegandBitTime;        // timestamp of the last bit received (used for timeouts)

void data0Pulse(void) {
    if (__wiegandBitCount / 8 < WIEGANDMAXDATA) {
        __wiegandData[__wiegandBitCount / 8] <<= 1;
        __wiegandBitCount++;
    }
    clock_gettime(CLOCK_MONOTONIC, &__wiegandBitTime);
}

void data1Pulse(void) {
    if (__wiegandBitCount / 8 < WIEGANDMAXDATA) {
        __wiegandData[__wiegandBitCount / 8] <<= 1;
        __wiegandData[__wiegandBitCount / 8] |= 1;
        __wiegandBitCount++;
    }
    clock_gettime(CLOCK_MONOTONIC, &__wiegandBitTime);
}

int wiegandInit(int d0pin, int d1pin) {
    // Setup wiringPi
    wiringPiSetup() ;
    pinMode(d0pin, INPUT);
    pinMode(d1pin, INPUT);

    wiringPiISR(d0pin, INT_EDGE_FALLING, data0Pulse);
    wiringPiISR(d1pin, INT_EDGE_FALLING, data1Pulse);
}

void wiegandReset() {
    memset((void *)__wiegandData, 0, WIEGANDMAXDATA);
    __wiegandBitCount = 0;
}

int wiegandGetPendingBitCount() {
    struct timespec now, delta;
    clock_gettime(CLOCK_MONOTONIC, &now);
    delta.tv_sec = now.tv_sec - __wiegandBitTime.tv_sec;
    delta.tv_nsec = now.tv_nsec - __wiegandBitTime.tv_nsec;

    if ((delta.tv_sec > 1) || (delta.tv_nsec > WIEGANDTIMEOUT))
        return __wiegandBitCount;

    return 0;
}

/*
 * wiegandReadData is a simple, non-blocking method to retrieve the last code
 * processed by the API.
 * data : is a pointer to a block of memory where the decoded data will be stored.
 * dataMaxLen : is the maximum number of -bytes- that can be read and stored in data.
 * Result : returns the number of -bits- in the current message, 0 if there is no
 * data available to be read, or -1 if there was an error.
 * Notes : this function clears the read data when called. On subsequent calls,
 * without subsequent data, this will return 0.
 */
int wiegandReadData(void* data, int dataMaxLen) {
    if (wiegandGetPendingBitCount() > 0) {
        int bitCount = __wiegandBitCount;
        int byteCount = (__wiegandBitCount / 8) + 1;
        memcpy(data, (void *)__wiegandData, ((byteCount > dataMaxLen) ? dataMaxLen : byteCount));

        wiegandReset();
        return bitCount;
    }
    return 0;
}

void printCharAsBinary(unsigned char ch) {
    int i;
    for (i = 0; i < 8; i++) {
        printf("%d", (ch & 0x80) ? 1 : 0);
        ch <<= 1;
    }
}

int printRFIDvalue(unsigned char* data) {
	char i;    
	int rfidValue;
	int temp;
	
	rfidValue = (int)data[0];
	
	for (i = 1; i < 4; i++) {
		rfidValue <<= 8;		
		temp = (int)data[i];
		rfidValue += temp;
	}	
	    
	rfidValue >>= 7;
	rfidValue &= 0x0000FFFF;
    	//printf("%i \n",rfidValue); 
return rfidValue;
}

int binaryToDecimal(char *binArray, int numValues){
/*
 Purpose: convert an array of binary values to a decimal integer
 Input:
     char *binArray: character array of binary values (msb is at index 0, lsb is at the highest index)
     int numValues: number of elements in the array
 Output:
     int: converted decimal value
 */
    
    int decimalValue = 0; //decimal value of binary array
    int lsbIndex = numValues-1; //
    int currentIndex; //position in the binary array
    
    for(currentIndex = lsbIndex; currentIndex >= 0; currentIndex--){
        //decimal += (value @ current index) x 2^(bit number)
        //since the lsb is stored at the highest index, bit number = lsb index - current index
        decimalValue += binArray[currentIndex]*(1<<(lsbIndex-currentIndex));
        
    }
    
    return decimalValue;
}

//This is the main program that looks and checks for the signals from the Wiegand reader.
//When the RFID number is found, we send the data to a php script to check for access.

void main(void) {

// Declare the variables to be set
	FILE *pipein_fp, *pipeout_fp;    
	int i;
    	char subset[16];
    	int index;    
    	struct tm *timeinfo ;
    	time_t rawtime ;
    	char strResponse [128] ;
        char readbuf[80];
        char rfidstring[10];
	int rfidValue;


    	wiegandInit(D0_PIN, D1_PIN);


    while(1) {
		int bitLen = wiegandGetPendingBitCount();
	        if (bitLen == 0) {
        		usleep(5000);
        } 
	else {
            	char data[4];
            	bitLen = wiegandReadData((void *)data, 4);
            	int bytes = bitLen / 8 + 1;
            	printf("Read %d bits (%d bytes): ", bitLen, bytes);
	    	
		for (i = 0; i < bytes; i++)
                	printf("%02X", (int)data[i]);
            	
		printf(" : ");
			
            	//Trying to print only the RFID number in binary
		for (i = 0; i < bytes; i++){
                	printCharAsBinary(data[i]);
            	}
		printf("\n");
	    	
		rfidValue = printRFIDvalue(data);

		rawtime = time (NULL) ;
  	    	timeinfo = localtime(&rawtime) ;  
  	    	strftime(strResponse,128," %Y-%m-%d_%H:%M:%S",timeinfo); 

  	    	//printf ("%s\n", strResponse) ;
		sprintf(rfidstring, " %d", rfidValue); //convert rfid value to string

		//Put together the string for the popen command, room # always 206
        	char stringrequest[100] = "php test.php 206";
		strcat(stringrequest, rfidstring);
		strcat(stringrequest, strResponse);
		
		//Test if string request looks right
		printf("%s \n", stringrequest);

		/* Create one way pipe line with call to popen() */
		if (( pipein_fp = popen(stringrequest, "r")) == NULL)
		{
		        perror("popen");
		        exit(1);
		}

		/*Processing loop */
		while(fgets(readbuf,80, pipein_fp))
		        printf("%s", readbuf);

		/* Close the pipes */
		pclose(pipein_fp);

	
		}
    }
}
