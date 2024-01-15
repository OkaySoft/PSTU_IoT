#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include "../bitprint.h"

const char *va(const char *p_format, ...)
{
 static char buf[1024];
 va_list argptr;
 va_start(argptr, p_format);
 vsnprintf(buf, sizeof(buf), p_format, argptr);
 va_end(argptr);
 return (const char *)&buf[0];
}

bool open_gpio_pins(int *p_fds, int count)
{
 for(int i = 0; i < count; i++) {
  p_fds[i] = open(va("/home/nechaev/gpio/gpio%d", i), O_RDONLY);
  if(p_fds[i] == -1) {
   perror(va("failed to open %d gpio pin", i));
   return false;
  }
 }
 return true;
}

bool read_gpio_pins(unsigned char *p_dst, const int *p_fds)
{
 unsigned char value = 0, retval = 0;
 for(int i = 0; i < 8; i++) {
  if(read(p_fds[i], &value, sizeof(value)) == -1)
   return false;
  
  retval |= (value << i);
 }
 *p_dst = retval;
 return true;
}

void close_gpio_pins(const int *p_dfs)
{
 for(int i = 0; i < 8; i++) {
  close(p_dfs[i]);
 }
}

int main()
{
 FILE         *fp;
 unsigned char value;
 int           fds[8];
 char          bits[10];
 int           num_records = 0;
 const int     max_records = 100;
 const char   *p_format_spec[] = { "%d", ",%d" };
 const char   *p_cur_format_spec = p_format_spec[0];

 if(!open_gpio_pins(fds, 8))
  return 1;

 const char *p_file_path = "/var/www/html/temperature.txt";
 fp = fopen(p_file_path, "wt");
 if(!fp) {
  perror("failed to open shared text file");
  close_gpio_pins(fds);
  return 2;
 }

 while(1) {
  if(read_gpio_pins(&value, fds)) {
   bitprint(bits, value);
   printf("read from gpio pins: %s\n", bits);
   
   if(num_records < max_records) {
    num_records++;
    fprintf(fp, p_cur_format_spec, value);
    fflush(fp);
    if(p_cur_format_spec == p_format_spec[0])
     p_cur_format_spec = p_format_spec[1]; //switch to next format specifier   
   }
   else {
    num_records = 0;   
    int fd = fileno(fp);
    if(ftruncate(fd, 0) == -1) {
     perror("truncate failed");
    }
    rewind(fp);
    p_cur_format_spec = p_format_spec[0];
    printf("clear file records\n");
   }
  }
  usleep(500000); //0.5 sec
 }
 close_gpio_pins(fds);
 fclose(fp);
 return 0;
}


