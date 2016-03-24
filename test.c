#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main(void)
{
        FILE *pipein_fp, *pipeout_fp;
        char readbuf[80];
	struct tm *timeinfo ;
	time_t rawtime ;
    	char strResponse [128] ;
	int rfid = 18460;
	char rfidstring[10];

	rawtime = time (NULL) ;
  	timeinfo = localtime(&rawtime) ;  
  	strftime(strResponse,128," %Y-%m-%d_%H:%M:%S",timeinfo);  

  	printf("%s\n", strResponse) ;
	
	sprintf(rfidstring, " %d", rfid);
	
	char stringrequest[100] = "php test.php AB1234";
	strcat(stringrequest, rfidstring);
	strcat(stringrequest, strResponse);

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
        
        return(0);
}
