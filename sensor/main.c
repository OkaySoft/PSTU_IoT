#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>
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

/*int mkdir_auto(const char *path, mode_t mode) {
 char tmp[256];
 char *p = NULL;
 size_t len;
 snprintf(tmp, sizeof(tmp), "%s", path);
 len = strlen(tmp);
 if (tmp[len - 1] == '/') {
  tmp[len - 1] = 0;
 }

 for (p = tmp + 1; *p; p++) {
  if (*p == '/') {
   *p = 0;
   if (mkdir(tmp, mode) == -1 && errno != EEXIST) {
    return -1;
   }
   *p = '/';
  }
 }

 if (mkdir(tmp, mode) == -1 && errno != EEXIST) {
  return -1;
 }
 return 0;
}*/

int gpio_open(const char *p_pin) {
 if (mkfifo(p_pin, 0666) == -1 && errno != EEXIST) {
  perror("FIFO creating failed");
  return -2;
 }
 return open(p_pin, O_WRONLY);
}

/*int gpio_open(const char *p_pin) {
 char path[256];
 const char *p_dir = "/home/gav/gpio";
 snprintf(path, sizeof(path), "%s/%s", p_dir, p_pin);
    
 // Разделяем путь на директорию и файл
 char *dir = strdup(path);
 char *last_slash = strrchr(dir, '/');
 if (!last_slash || last_slash == dir) {
  printf("Invalid path '%s'\n", p_pin);
  free(dir);
  return -2;
 }
 *last_slash = '\0'; // Отсекаем имя файла, оставляем путь к директории

 // Создаем необходимые каталоги
 if (mkdir_auto(dir, 0700) != 0) {
  free(dir);
  return -3; // Не удалось создать каталоги
 }

 // Создаем FIFO
 if (mkfifo(path, 0666) == -1 && errno != EEXIST) {
  perror("FIFO creating failed");
  free(dir);
  return -4;
 }
 free(dir);
 printf("opening '%s'...\n", path);
 return open(path, O_WRONLY | O_NONBLOCK);
}*/


#define L_LOW 0
#define L_HIGHT 1

int gpio_write(int fd, int level)
{
 unsigned char data = (!level) ? 0 : 1;
 return write(fd, &data, sizeof(data));
}

uint8_t simulate_temperature(double min, double max, double per)
{
 time_t now = (time_t)time(NULL);
 double sval = sin(((double)now / per) * 2. * M_PI);
 double value = min + (max - min) * (sval + 1.f) / 2.;
 return (uint8_t)round(value);
}

int gpio_close(int fd)
{
 return close(fd);
}

int main()
{
 char bits_str[10];
 const int used_pins = 8; 
 int pinsfd[used_pins];
 for(int i = 0; i < used_pins; i++) {
  pinsfd[i] = gpio_open(va("/home/gav/gpio/gpio%d", i));
  if(pinsfd[i] < 0) {
   perror(va("failed to open gpio%d", i));
   return 1;
  }
 }
 
 while(1) {
  unsigned char value = simulate_temperature(25.0, 30.0, 0.1);
  for(int i = 0; i < 8; i++) {
   gpio_write(pinsfd[i], value & (1 << i));
  }
  bitprint(bits_str, value);
  printf("writed bits (%s) to 8 pins\n", bits_str);
  usleep(500000); //0.5 sec
 }
 return 0;
}

/*
int m2ai2n()
{
 char bits[10];
 unsigned char vpins;
 const char *p_fifopath = "../gpio0";

 if(access(p_fifopath, F_OK) == 0)
    remove(p_fifopath);
 
 if(mkfifo(p_fifopath, 0666) == -1) {
  perror("fifo creating failed");
  return 1;
 }

 int fd = open(p_fifopath, O_WRONLY|O_NONBLOCK);
 if(fd == -1) {
  perror("creating gpio0 file failed");
  return 1;
 }
 
 while(1) {
  vpins = (1 << (rand() % 7));
  if(write(fd, &vpins, sizeof(vpins)) != -1) {
   bitprint(bits, vpins);
   printf("write to gpio0 file: %s\n", bits); 
  }
  usleep(500000); //0.5 sec
 }
 close(fd);
 remove("gpio0");
 return 0;
}*/
