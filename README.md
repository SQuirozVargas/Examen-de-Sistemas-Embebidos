# Comunicacion serial de UART en Sistemas Embebidos 
UART es asíncrono, lo que significa que no utiliza una señal de reloj compartida. Usa transmisión (TX) y recepción (RX) del ESP32.
La sincronización se realiza mediante una velocidad de transmisión previamente acordada, conocida como baud rate. Esto hace
que UART sea ideal para depuración, configuración de dispositivos y comunicación con terminales o módulos externos como GPS, Bluetooth o módulos de audio.
## Interfaces UART en el ESP32
El ESP32 contiene internamente tres módulos de hardware UART independientes, nombrados UART0, UART1 y UART2. Tener tres interfaces permite asignar funciones 
distintas a cada puerto para evitar interferencias en el sistema.\
### UART0
Se utiliza por defecto para la programación y depuración del chip. Está conectado físicamente al convertidor USB-a-Serie de la placa de desarrollo, por lo que se usa para cargar el código desde la computadora y para imprimir mensajes en el Monitor Serial mediante printf o ESP_LOG. Sus pines por defecto son GPIO1 (TX) y GPIO3 (RX).
