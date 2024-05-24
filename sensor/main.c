#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
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

int gpio_open(const char *p_pin) {
    if (mkfifo(p_pin, 0666) == -1 && errno != EEXIST) {
        perror("FIFO creating failed");
        return -2;
    }
    return open(p_pin, O_WRONLY);
}

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
        pinsfd[i] = gpio_open(va("/var/www/webview/system/gpio%d", i));
        if(pinsfd[i] < 0) {
            perror(va("failed to open gpio%d", i));
            return 1;
        }
    }
    
    while(1) {
        unsigned char value = simulate_temperature(25.0, 30.0, 0.01);
        for(int i = 0; i < 8; i++) {
            gpio_write(pinsfd[i], value & (1 << i));
        }
        bitprint(bits_str, value);
        printf("writed bits (%s) to 8 pins\n", bits_str);
        usleep(500000); //0.5 sec
    }
    return 0;
}
