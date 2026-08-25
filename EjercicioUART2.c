/*
EJERCICIO 1 – Comunicación Serial Avanzada con UART2 (ESP-IDF)
Tema principal: Comunicación UART
Framework: ESP-IDF (PlatformIO)
Simulación: Permitida en Wokwi (VSC) + Serial Monitor
Hardware real: Opcional
Descripción del ejercicio
Desarrollar una aplicación en ESP32 que utilice el puerto UART2 para implementar un sistema
de comandos seriales.
El sistema deberá:
· Inicializar correctamente el UART2 (baud rate configurable).
· Recibir comandos de texto desde un terminal serial externo.
· Interpretar los comandos recibidos.
· Ejecutar acciones internas según el comando.
· Enviar respuestas estructuradas por el mismo puerto UART.
Ejemplos de comandos obligatorios
· status: devuelve el estado actual del sistema.
· led on / led off: controla un LED virtual o físico.
· info: muestra información del sistema (baud rate, puerto, contador de comandos).
· reset: reinicia variables internas (no el microcontrolador).
*/
