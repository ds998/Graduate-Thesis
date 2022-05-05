#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <regex.h>
#define REG_NUM 64

char write_buf[1024];
char read_buf[1024];


int* memory=NULL;

int match(const char *string, const char *pattern)
{
  regex_t re;
  if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) return 0;
  int status = regexec(&re, string, 0, NULL, 0);
  regfree(&re);
  if (status != 0) return 0;
  return 1;
}



int main(){
  int fd;
  char option;

  memory=(int*)malloc(REG_NUM*sizeof(int));

  printf("Welcome to the device emulator...%d\n",getpid());

  fd = open("/dev/my_device",O_RDWR);

  if(fd<0){
    printf("Cannot open the device file...\n");
    return -1;
  }

  while(1){
      /* Constant reading */
      read(fd,read_buf,1024);

      if(match(read_buf,"^r0x[0-9abcdef]{4}$")){
        /* Reading register value */
        char hex_ch1[7];

        for(int i=0;i<6;i++){
          hex_ch1[i]=read_buf[i+1];
        }
        hex_ch1[6]='\0';
        int hex_num1 = (int) strtol(hex_ch1,NULL,16);

        if(hex_num1>=REG_NUM || hex_num1<0){
          printf("Bad number range for register reading.\n");
        }else{
          int val = memory[hex_num1];
          sprintf(write_buf,"%d",val);
          write(fd,write_buf,strlen(write_buf)+1);
          sleep(2);
        }
      }else if(match(read_buf,"^w0x[0-9abcdef]{4}-([0-9]+)$")){
        /* Writing register value */
        char hex_ch2[7];
        for(int i=0;i<6;i++){
          hex_ch2[i]=read_buf[i+1];
        }
        hex_ch2[6]='\0';
        int hex_num2 = strtol(hex_ch2,NULL,16);
        if(hex_num2>=REG_NUM || hex_num2<0){
          printf("Bad number range for register reading.\n");
        }else{
          char value[360];
          strncpy(value,&read_buf[8],360);
          int val;
          if(sscanf(value,"%d",&val)==0){
            printf("Error with the value to be written.\n");
          }
          else{
            memory[hex_num2]=val;
            printf("Register number %d value is %d.\n",hex_num2,memory[hex_num2]);
          }
        }
      }else if(match(read_buf,"^(exit)$")){
        /* Exiting */
        free(memory);
        memory=NULL;
        close(fd);
        exit(0);
      }


/*    printf("Please enter your options...\n");
    printf("      1. Write              \n");
    printf("      2. Exit              \n");
    option=getchar();
    printf(" Your options = %c\n",option);

    switch(option){
      case '1':
        printf("Enter the string to write into the driver:\n");
        scanf(" %[^\t\n]s",write_buf);
        printf("Data written....");
        write(fd,write_buf,strlen(write_buf)+1);
        printf("DONE...\n");
        break;
      case '2':
        printf("Closing file...%d\n",fd);
        exit(1);
        break;
      default:
        printf("Enter a valid option = %c\n",option);
        break;

    }

    while(getchar()!='\n');
    */

  }
  close(fd);
}
