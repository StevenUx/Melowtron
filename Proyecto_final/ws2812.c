#include "ws2812.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"


/// @brief Envia los datos hacia cada pixel, cada color ocupa 8 bits para un total de 24 bits,
/// @param pixel_grb Contiene los 3 canales de color R G B
/// @param index Numero de pixel
void put_pixel(uint32_t pixel_grb,int index) {
    
    for(uint8_t i = 0; i<index; i++){
        pio_sm_put_blocking(pio0, 0, pixel_grb << 8u) ;

    }
   // pio_sm_put_blocking(pio0, 0, pixel_grb << 8u); // Recibe un valor de 32 bits, pero el protocolo solo recibe 24bits, entonces desplaza 8 bits que no son utilizados
}

/// @brief Toma los 8 bits de cada color y les hace un casting a uint32_t
/// Desplaza los bits y luego aplica la operacion OR para concatenar todo en un unico valor de 32 bits.
/// @param r Rojo 8bits
/// @param g Verde 8bits
/// @param b Azul 8bits
/// @return Retorna un uint32 con los 24bits para ser enviados
uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |  ((uint32_t) (g) << 16) |  (uint32_t) (b);
}




/// @brief Inicializa el PIO
void  init_pio_ws2812(){
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);
}