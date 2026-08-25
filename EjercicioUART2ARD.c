/*
 * EJERCICIO 1: Sistemas Embebidos (ESP32 / Arduino Framework)
 * Tema: Comunicación Serial e intérprete de comandos.
 * Descripción: Configura el puerto Serial a 115200 baudios, procesa
 * comandos entrantes por consola y ejecuta acciones sobre un LED y variables internas.
 */

#include <Arduino.h>

// Definición de pines y parámetros
#define UART_BAUD_RATE 115200
#define LED_PIN        2 // GPIO 13 (LED integrado en Adafruit Feather) o GPIO 2 para DevKit

// Variables de estado del sistema
static uint32_t command_count = 0; // Cuenta los comandos válidos procesados
static bool led_state = false;      // Guarda el estado del LED (true = ON, false = OFF)

// Función para transmitir mensajes por el puerto serie
void uart_send_string(const String& str) {
    Serial.print(str);
}

// Intérprete de comandos (Parser)
void process_command(String cmd) {
    // 1. Limpiar espacios, saltos de línea (\r, \n) del inicio y final
    cmd.trim();

    // Ignorar si la línea está vacía
    if (cmd.length() == 0) return;

    // Incrementar contador de comandos recibidos
    command_count++;

    // 2. Evaluación de comandos
    if (cmd.equalsIgnoreCase("status")) {
        char response[64];
        snprintf(response, sizeof(response), "STATUS: OK | LED: %s\r\n", led_state ? "ON" : "OFF");
        uart_send_string(response);
    } 
    else if (cmd.equalsIgnoreCase("led on")) {
        led_state = true;
        digitalWrite(LED_PIN, HIGH); // Enciende el LED
        uart_send_string("RESPONSE: LED Encendido\r\n");
    } 
    else if (cmd.equalsIgnoreCase("led off")) {
        led_state = false;
        digitalWrite(LED_PIN, LOW); // Apaga el LED
        uart_send_string("RESPONSE: LED Apagado\r\n");
    } 
    else if (cmd.equalsIgnoreCase("info")) {
        char response[128];
        snprintf(response, sizeof(response), "INFO: Puerto Serial0 | Baudrate: %d | Comandos recibidos: %lu\r\n", 
                 UART_BAUD_RATE, command_count);
        uart_send_string(response);
    } 
    else if (cmd.equalsIgnoreCase("reset")) {
        command_count = 0;
        led_state = false;
        digitalWrite(LED_PIN, LOW); // Reinicia variables y apaga el LED
        uart_send_string("RESPONSE: Variables de sistema reiniciadas\r\n");
    } 
    else {
        uart_send_string("ERROR: Comando no reconocido\r\n");
    }
}

// Configuración inicial de hardware
void init_hardware() {
    // 1. Inicializar la comunicación serial a 115200 baudios
    Serial.begin(UART_BAUD_RATE);

    // 2. Configurar el pin del LED como SALIDA digital
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW); // Inicia en estado apagado
}

void setup() {
    // Inicialización de perifericos
    init_hardware();

    // Pequeño retardo para estabilizar la conexión serial
    delay(1000);

    // Mensaje de bienvenida inicial
    uart_send_string("\r\n--- Sistema de Comandos UART (Arduino ESP32) Listo ---\r\n");
}

void loop() {
    // Reemplazo de la tarea de lectura continua (uart_rx_task)
    if (Serial.available() > 0) {
        // Lee la cadena completa hasta encontrar el salto de línea al presionar Enter
        String incomingCommand = Serial.readStringUntil('\n');
        process_command(incomingCommand);
    }
}
