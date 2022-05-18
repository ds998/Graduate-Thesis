#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define RD_VALUE _IOR('a','b',int32_t*)

char write_buf[1024];
char read_buf[1024];
int32_t interrupt_count;

struct stat info;

int main(){
  int fd;
  char option;
  int reg_num;
  int val;

  printf("Welcome to the demo of character device driver...%d\n",getpid());

  fd = open("/dev/my_device",O_RDWR);

  if(fd<0){
    printf("Cannot open the device file...\n");
    return -1;
  }

  while(1){
    printf("Please enter your options...\n");
    printf("      1. Write to a register             \n");
    printf("      2. Read from a register             \n");
    printf("      3. Get interrupt count             \n");
    printf("      4. Exit              \n");

    option=getchar();
    printf(" Your options = %c\n",option);
    int success=0;
    int val_success=0;
    char hex_str[5];
    char sep[2];
    char val_str[50];
    switch(option){
      case '1':
        success=0;
        val_success=0;
        printf("Enter the number of a register:\n");
        while(!success){
          if(scanf(" %d",&reg_num)==0){
            printf("This is not a number.\n");
          }
          else{
            if(reg_num<0 || reg_num>63){
              printf("Register number out of range.\n");
            }else{
              success=1;
            }
          }
        }
        printf("Enter the value to be written:\n");
        while(!val_success){
          if(scanf(" %d",&val)==0){
            printf("This is not a number.\n");
          }
          else{
            val_success=1;
          }
        }
        write_buf[0]='w';
        write_buf[1]='0';
        write_buf[2]='x';
        write_buf[3]='\0';
        hex_str[0]='0';
        hex_str[1]='0';
        hex_str[2]='0';
        hex_str[3]='0';
        hex_str[4]='\0';
        sep[0]='-';
        sep[1]='\0';
        sprintf(hex_str,"%04x",reg_num);
        strcat(write_buf,hex_str);
        strcat(write_buf,sep);
        sprintf(val_str,"%d",val);
        strcat(write_buf,val_str);
        write(fd,write_buf,strlen(write_buf)+1);
        printf("DONE...\n");
        break;
      case '2':
        success=0;

        printf("Enter the number of a register:\n");
        while(!success){
          if(scanf(" %d",&reg_num)==0){
            printf("This is not a number.\n");
          }
          else{
            if(reg_num<0 || reg_num>63){
              printf("Register number out of range.\n");
            }else{
              success=1;
            }
          }
        }
        write_buf[0]='r';
        write_buf[1]='0';
        write_buf[2]='x';
        write_buf[3]='\0';
        hex_str[0]='0';
        hex_str[1]='0';
        hex_str[2]='0';
        hex_str[3]='0';
        hex_str[4]='\0';
        sprintf(hex_str,"%04x",reg_num);
        strcat(write_buf,hex_str);
        write(fd,write_buf,strlen(write_buf)+1);
        printf("Data is reading...\n");
        sleep(1);
        read(fd,read_buf,1024);
        printf("Done...\n\n");
        printf("Register %d value = %s\n\n",reg_num,read_buf);
        break;
      case '3':
        printf("Reading interrupt count from driver...\n");
        ioctl(fd, RD_VALUE, (int32_t*) &interrupt_count);
        printf("Interrupt count is %d\n", interrupt_count);
        break;
      case '4':
        write_buf[0]='e';
        write_buf[1]='x';
        write_buf[2]='i';
        write_buf[3]='t';
        write_buf[4]='\0';
        write(fd,write_buf,strlen(write_buf)+1);
        close(fd);
        exit(0);
        break;
      default:
        printf("Enter a valid option = %c\n",option);
        break;

    }

    while(getchar()!='\n');

  }
  close(fd);
}
