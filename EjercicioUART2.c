/*
 * EJERCICIO 1: Sistemas Embebidos (ESP32 / ESP-IDF)
 * Tema: Comunicación Serial Avanzada con UART2 e intérprete de comandos.
 * Descripción: Inicializa el puerto UART2 a 115200 baudios (TX=GPIO17, RX=GPIO16),
 * procesa comandos seriales entrantes y ejecuta acciones sobre variables internas y un LED.
 */
// OJO: No usamos el Framework de ARDUINO 
#include <stdio.h>             // Funciones estándar de E/S de C como snprintf() para formatear cadenas[cite: 6].
#include <string.h>            // Funciones de manipulación de cadenas como strcmp(), strlen() y strcspn()[cite: 6].
#include "freertos/FreeRTOS.h" // Cabecera principal del sistema operativo en tiempo real FreeRTOS.
#include "freertos/task.h"     // API de FreeRTOS para la creación y gestión de tareas (xTaskCreate, vTaskDelay)[cite: 4, 6].
#include "driver/uart.h"       // Driver nativo de ESP-IDF para la configuración del hardware UART[cite: 6].
#include "driver/gpio.h"       // Driver nativo de ESP-IDF para el control digital de pines GPIO[cite: 6].
#include "esp_log.h"           // Macros de depuración e impresión por consola (ESP_LOGI)[cite: 6].
#include "esp_rom_sys.h"       // Funciones del sistema ROM del ESP32.

#define UART_PORT_NUM      2       // Indicamos que usaremos el puerto hardware UART2 del ESP32.
#define UART_BAUD_RATE     115200  // Acordamos la velocidad de transmisión (115200 bits/segundo).
#define UART_TX_PIN        17      // Asignamos el pin GPIO 17 para la salida de datos (TX2).
#define UART_RX_PIN        16      // Asignamos el pin GPIO 16 para la entrada de datos (RX2).
#define UART_BUFFER_SIZE   1024    // Reservamos un tamaño base de 1024 bytes en RAM para el búfer[cite: 6].
#define LED_PIN            2       // Definimos el pin GPIO 2 para controlar el LED.

// Configuracion de la UART2 con parámetros básicos
void init_hardware(){
    // Llenamos los parametros para la configuracion serial
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,           // Establece los 115200 baudios
        .data_bits = UART_DATA_8_BITS,          // Usa tramas de 8 bits de datos
        .parity    = UART_PARITY_DISABLE,       // Sin bit de paridad
        .stop_bits = UART_STOP_BITS_1,          // 1 bit de parada
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,  // Sin control de flujo por hardware[cite: 6]
        .source_clk = UART_SCLK_APB             // Fuente de reloj interna del chip
    };
    // Aplicamos la configuración al puerto UART2
    uart_param_config(UART_PORT_NUM, &uart_config);
    // 2. Conectamos la UART2 a los pines físicos (TX = GPIO17, RX = GPIO16)
    uart_set_pin(UART_PORT_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // 3. Reservamos memoria RAM para el búfer de recepción (2048 bytes)
    uart_driver_install(UART_PORT_NUM, UART_BUFFER_SIZE * 2, 0, 0, NULL, 0);
    // 4. Configurar el pin GPIO 2 para controlar el LED
    gpio_reset_pin(LED_PIN);                       // Restablece el pin a su estado inicial limpio
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT); // Lo establece como SALIDA digital
    gpio_set_level(LED_PIN, 0);                    // Garantiza que el LED inicie apagado (0 lógico)

};
// Función auxiliar para transmitir una cadena de caracteres por la UART2
void uart_send_string(const char* str) {
    // Envía los bytes del texto a través del puerto UART2
    uart_write_bytes(UART_PORT_NUM, str, strlen(str));
}

// Interprete de comandos a través de la UART2
// Variables de estado del sistema
static uint32_t command_count = 0; // Cuenta cuántos comandos válidos hemos procesado
static bool led_state = false;       // Guarda si el LED está encendido (true) o apagado (false)

void process_command(char* cmd) {
    // 1. Quitar caracteres invisibles de salto de línea (\r y \n)
    cmd[strcspn(cmd, "\r\n")] = 0;
    // Si la cadena quedó vacía (el usuario solo presionó Enter), la ignoramos
    if (strlen(cmd) == 0) return;

    // Contamos este comando como procesado
    command_count++;
    // 2. Evaluar cada comando permitido
    if (strcmp(cmd, "status") == 0) {
        char response[64];
        snprintf(response, sizeof(response), "STATUS: OK | LED: %s\r\n", led_state ? "ON" : "OFF");
        uart_send_string(response);
    } 
    else if (strcmp(cmd, "led on") == 0) {
        led_state = true;
        gpio_set_level(LED_PIN, 1); // Enciende el LED
        uart_send_string("RESPONSE: LED Encendido\r\n");
    } 
    else if (strcmp(cmd, "led off") == 0) {
        led_state = false;
        gpio_set_level(LED_PIN, 0); // Apaga el LED
        uart_send_string("RESPONSE: LED Apagado\r\n");
    } 
    else if (strcmp(cmd, "info") == 0) {
        char response[128];
        snprintf(response, sizeof(response), "INFO: Puerto UART2 | Baudrate: %d | Comandos recibidos: %lu\r\n", 
                 UART_BAUD_RATE, command_count);
        uart_send_string(response);
    } 
    else if (strcmp(cmd, "reset") == 0) {
        command_count = 0;
        led_state = false;
        gpio_set_level(LED_PIN, 0); // Reinicia variables y apaga el LED
        uart_send_string("RESPONSE: Variables de sistema reiniciadas\r\n");
    } 
    else {
        // Respuesta por defecto si el usuario escribió cualquier otra cosa
        uart_send_string("ERROR: Comando no reconocido\r\n");
    }
}
// La Tarea de Lectura Continuada (uart_rx_task) y el Arranque (app_main)

// Tarea de FreeRTOS que escucha el puerto UART2 en segundo plano
void uart_rx_task(void *pvParameters) {
    uint8_t rx_buffer[UART_BUFFER_SIZE]; // Arreglo para almacenar temporalmente los bytes leídos

    while (1) {
        // Lee los bytes disponibles en la UART2
        int rx_bytes = uart_read_bytes(UART_PORT_NUM, rx_buffer, UART_BUFFER_SIZE - 1, 20 / portTICK_PERIOD_MS);

        if (rx_bytes > 0) {
            rx_buffer[rx_bytes] = '\0';        // Agrega el carácter nulo al final para convertir los bytes en una cadena válida de C
            process_command((char*)rx_buffer); // Le pasa la cadena de texto al intérprete de comandos
        }
    }
}
void app_main(void) {
    // 1. Inicializa el hardware (puerto UART2 y pin del LED)
    init_hardware();

    // 2. Transmite un mensaje de bienvenida por la UART2 para confirmar que el sistema arrancó
    uart_send_string("\r\n--- Sistema de Comandos UART2 Listo ---\r\n");

    // 3. Crea la tarea de FreeRTOS para atender la recepción de comandos
    xTaskCreate(uart_rx_task, "uart_rx_task", 4096, NULL, 5, NULL);
}
