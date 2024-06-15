
#include "pico/stdlib.h"
#include <stdio.h>
#include <math.h>
#include "hardware/timer.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/dma.h"
#include "Notas.h"
#include "ws2812.h"

#define TRIG 1
#define ECHO 15

//uint32_t buffer[256];
int read_distance();
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
             if(i%5 == 0){
                oct+=12;
            }
            aux = sakura[i%5]+oct;
            current_scale[i] = notes[(aux + base)%25];
                        
            break;
        case 2:
            if(i%6==0){
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

uint c = 0;
#define dnc_t 300
 absolute_time_t last_irq;
 void GPIO_CallBack(uint GPIO, uint32_t Events)
{
   absolute_time_t now = get_absolute_time();
    gpio_acknowledge_irq(GPIO,Events);
 /* Debounce button pres. */
   if (absolute_time_diff_us(last_irq,now)<dnc_t * 1000){    
        return;   
   }
    Escala(0,c%2);
    c++;
}
  
#define PWM_PIN 14
float clk_div_cal(float freq);
int pwm ;
float wrap;
/// @brief Inicializa el PWM con valor de wrap predeterminado para generar los tonos correctos
void init_pwm_tone(){
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
    float prescaler = (float)SYS_CLK_KHZ/500;
    pwm = pwm_gpio_to_slice_num(PWM_PIN);
    wrap = 500000*18/2000;
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config,prescaler);
    pwm_config_set_wrap(&config, wrap);   
    pwm_init(pwm, &config, true);
    pwm_set_gpio_level(PWM_PIN, wrap/2);
}

int main(){
    stdio_init_all();
//Boton
    gpio_init(16);
    gpio_pull_up(16);
    gpio_set_irq_enabled_with_callback(16, GPIO_IRQ_EDGE_FALL, true, &GPIO_CallBack);
//Sensor
    gpio_init(TRIG);
    gpio_set_dir(TRIG,GPIO_OUT); 
    gpio_init(ECHO);
    gpio_set_dir(ECHO,GPIO_IN);
//Tira led
    init_pio_ws2812();

    init_pwm_tone();
    Escala(0,1);
    sleep_ms(5000);


  for(uint i = 0;i<25;i++){
    printf("Notes: %f\n",current_scale[i]);
  }

// Main Loop
    while(1){
        
        int d = read_distance();

        if((d > 60) || (d<10)){
            pwm_set_enabled(pwm,false);
        }
        else{
            
                pwm_set_clkdiv(pwm,clk_div_cal(current_scale[d/2]));
                pwm_set_enabled(pwm,true);
                put_pixel(urgb_u32(128,0,128),12);
            
            
        }
        
        
        sleep_ms(100);
       
    }

   
}


float clk_div_cal(float freq){

    float div = 125000000/(wrap*freq);
    return div;

}

/// @brief Calcula la distancia, envia un pulso hacia el trigger del sensor
/// @return Distancia en Centimetros
int read_distance() {
    // Envía un pulso de trigger
    gpio_put(TRIG, 0);
    sleep_us(2);
    gpio_put(TRIG, 1);
    sleep_us(10);
    gpio_put(TRIG, 0);
    // Espera a que el pin de echo se active
    while (gpio_get(ECHO) == 0);
    absolute_time_t start_time = get_absolute_time();
    // Espera a que el pin de echo se desactive
    while (gpio_get(ECHO) == 1);
    absolute_time_t end_time = get_absolute_time();
    // Calcula el tiempo de viaje del pulso
    int64_t pulse_duration = absolute_time_diff_us(start_time, end_time);
    // Calcula la distancia en centímetros
    int distance = (pulse_duration * 0.0343) / 2;
    return distance;
}

