
#include "pico/stdlib.h"
#include <stdio.h>
#include <math.h>
#include "hardware/timer.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/dma.h"
#include "Notas.h"
#include "ws2812.h"
#define TRIG 0
#define ECHO 15
#define NUM_SAMPLES 128
//uint32_t buffer[256];

float notes[] = {
    130.81,   // C3
    138.59,   // C#3 / Db3
    146.83,   // D3
    155.56,   // D#3 / Eb3
    164.81,   // E3
    174.61,   // F3
    185.00,   // F#3 / Gb3
    196.00,   // G3
    207.65,   // G#3 / Ab3
    220.00,   // A3
    233.08,   // A#3 / Bb3
    246.94,   // B3
    261.63,   // C4
    277.18,   // C#4 / Db4
    293.66,   // D4
    311.13,   // D#4 / Eb4
    329.63,   // E4
    349.23,   // F4
    369.99,   // F#4 / Gb4
    392.00,   // G4
    415.30,   // G#4 / Ab4
    440.00,   // A4
    466.16,   // A#4 / Bb4
    493.88,   // B4
    523.25,   // C5
};

float current_scale[25];

/// @brief Selecciona una de las escalas disponibles 1.Sakura 2.Arabe 3.Mayor 4.Penta
/// @param base 0 - 11 cada numero corresponde a cada nota en orden C3 - B4
/// @param num Escalas del 1 al 5 (Ver manual)
void Escala(uint8_t base,uint8_t num){
    uint oct = 0;
    uint aux = 0;
    for(uint i = 0;i<25;i++){


        switch (num)
        {
        case 0:
        current_scale[i] = notes[i];
            break;

        case 1:
            
            if(i%4==0){
                oct+=12;
            }
            aux = sakura[i%5]+oct;
            current_scale[i] = notes[(aux + base)%25];
            
             
            break;
        case 2:
            
            if(i%5==0){
                oct+=12;
            }
            aux = China[i%6]+oct;
            current_scale[i] = notes[(aux + base)%25];
            
            
            break;
        default:
        
            break;
        }
        
        
        
    }
    
}

  
#define PWM_PIN 14
float clk_div_cal(float freq);


int pwm ;
float wrap;
void init_pwm_dma(float milis){
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
    float prescaler = (float)SYS_CLK_KHZ/500;
    pwm = pwm_gpio_to_slice_num(PWM_PIN);
    wrap = 500000*milis/2000;
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config,prescaler);
    pwm_config_set_wrap(&config, wrap);   
    pwm_init(pwm, &config, true);
    pwm_set_gpio_level(PWM_PIN, wrap/2);

    init_pio_ws2812();
    

}

int main(){
    stdio_init_all();

    gpio_init(TRIG);
    gpio_set_dir(TRIG,GPIO_OUT);
    
    gpio_init(ECHO);
    gpio_set_dir(ECHO,GPIO_IN);

   
    Escala(0,2);
    sleep_ms(5000);

  for(uint i = 0;i<25;i++){
    printf("Notes: %f\n",current_scale[i]);
  }
  init_pwm_dma(18);

    while(1){

        for(int i=0;i<11;i++){
         float c = clk_div_cal(current_scale[i]);  
            pwm_set_clkdiv(pwm,c);
            sleep_ms(500);
            put_pixel(urgb_u32(255,255,0));
        }
       
      tight_loop_contents();
       
    }

   
}

float clk_div_cal(float freq){

    float div = 125000000/(wrap*freq);
    return div;

}