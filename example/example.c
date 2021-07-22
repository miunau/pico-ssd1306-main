#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "imgs.h"

#include "ssd1306.h"

#define SLEEPTIME 25

void setup_gpios(void);
void animation(void);

uint8_t SCREEN_WIDTH = 128;
uint8_t SCREEN_HEIGHT = 32;

bool DEEEEBUG = false;

int map(int s, int a1, int a2, int b1, int b2) {
    return b1 + (s - a1) * (b2 - b1) / (a2 - a1);
}

int main() {
    stdio_init_all();

    printf("configuring pins..\n");
    setup_gpios();

    printf("jumping to animation...\n");
    animation();

    return 0;
}


void setup_gpios(void) {
    // i2c screen at i2c0 (gp2/3 - pins 4/5)
    i2c_init(i2c1, 400000);
    gpio_set_function(2, GPIO_FUNC_I2C);
    gpio_set_function(3, GPIO_FUNC_I2C);
    gpio_pull_up(2);
    gpio_pull_up(3);

    // i2c dac control at i2c0 (gp4/5 - pins 6/7)
    i2c_init(i2c0, 100000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(2);
    gpio_pull_up(3);

    // adc pot
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);
    int CLOCK_DIV = 9600;
    adc_set_clkdiv(CLOCK_DIV);
}


void animation(void) {
    char *words[]= {"FOXES", "ARE", "PRETTY", "COOL"};

    ssd1306_t disp;
    disp.external_vcc=false;
    ssd1306_init(&disp, SCREEN_WIDTH, SCREEN_HEIGHT, 0x3C, i2c1);
    ssd1306_clear(&disp);

    //ssd1306_invert(&disp, 1);
    //ssd1306_contrast(&disp, 255);

    int frame = 0;

    for(;;) {

        ++frame;

        uint16_t result = adc_read();
        int frequency = 0.01;
        if (result >= 0 && result <= 0xfff) 
            frequency = map(result, 0, 0xfff, 0, 1000);

        float freq = 1.0-frequency*0.001;

        ssd1306_clear(&disp);

        int lastX = 0;
        int lastY = 0;

        int imgWidth = 128;
        int imgHeight = 32;

        int y = 0;

        if(freq < 0.33) {
            // draw image from array???
            for(int y=0; y < imgHeight; y++) {
                for(int x=0; x < imgWidth; x++) {
                    uint8_t px = img0[x+(y*imgWidth)];
                    if(px > 0) {
                        ssd1306_draw_pixel(&disp, x, y);
                        //printf("printing at %i, %i\n", x, y);
                    }
                }
            }
        }
        else if(freq < 0.66) {
            // draw image from array???
            for(int y=0; y < imgHeight; y++) {
                for(int x=0; x < imgWidth; x++) {
                    uint8_t px = img1[x+(y*imgWidth)];
                    if(px > 0) {
                        ssd1306_draw_pixel(&disp, x, y);
                        //printf("printing at %i, %i\n", x, y);
                    }
                }
            }
        }
        else {
            // draw image from array???
            for(int y=0; y < imgHeight; y++) {
                for(int x=0; x < imgWidth; x++) {
                    uint8_t px = img2[x+(y*imgWidth)];
                    if(px > 0) {
                        ssd1306_draw_pixel(&disp, x, y);
                        //printf("printing at %i, %i\n", x, y);
                    }
                }
            }
        }


/*
        for(int curX=0; curX < SCREEN_WIDTH; ++curX) {
            //ssd1306_draw_line(&disp, 0, x, 127, y);
            //ssd1306_draw_pixel(&disp, curX, SCREEN_HEIGHT/2+sin(curX*freq*sin(frame*0.01))*SCREEN_HEIGHT/2);
            int halfHeight = SCREEN_HEIGHT/2;
            int curY = halfHeight + sin(lastX * freq) * halfHeight;
            //ssd1306_draw_pixel(&disp, x, curY);
            //int fudge = abs(sin(frame*0.1)*(freq*5));
            ssd1306_draw_line(&disp, lastX, lastY, curX, curY);
            //ssd1306_draw_line(&disp, lastX+2, lastY+2, curX+5, curY+5);
            lastY = curY;
            lastX = curX;
        }
*/

        if(DEEEEBUG) {
            char out[32];
            sprintf(out, "%f", freq);
            ssd1306_draw_string(&disp, 1, 1, 1, out);
            
            char out2[32];
            sprintf(out2, "%x", result);
            ssd1306_draw_string(&disp, 1, 11, 1, out2);

            char out3[32];
            sprintf(out3, "%i", frame);
            ssd1306_draw_string(&disp, 1, 22, 1, out3);

        }
        
        ssd1306_show(&disp);
            
    }

    /*
    for(;;) {
        for(int y=0; y<15; ++y) {
            ssd1306_draw_line(&disp, 0, y, 127, y);
            ssd1306_show(&disp);
            sleep_ms(SLEEPTIME);
            ssd1306_clear(&disp);
        }

        for(int y=0, i=1; y>=0; y+=i) {
            ssd1306_draw_line(&disp, 0, 15-y, 127, 15+y);
            ssd1306_draw_line(&disp, 0, 15+y, 127, 15-y);
            ssd1306_show(&disp);
            sleep_ms(SLEEPTIME);
            ssd1306_clear(&disp);
            if(y==16) i=-1;
        }

        for(int i=0; i<sizeof(words)/sizeof(char *); ++i) {
            ssd1306_draw_string(&disp, 8, 12, 2, words[i]);
            ssd1306_show(&disp);
            sleep_ms(800);
            ssd1306_clear(&disp);
        }

        for(int y=15; y<32; ++y) {
            ssd1306_draw_line(&disp, 0, y, 127, y);
            ssd1306_show(&disp);
            sleep_ms(SLEEPTIME);
            ssd1306_clear(&disp);
        }
    }
    */
}
