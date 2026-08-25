# Comunicacion serial de UART en Sistemas Embebidos 
UART es asíncrono, lo que significa que no utiliza una señal de reloj compartida. Usa transmisión (TX) y recepción (RX) del ESP32.
La sincronización se realiza mediante una velocidad de transmisión previamente acordada, conocida como baud rate. Esto hace
que UART sea ideal para depuración, configuración de dispositivos y comunicación con terminales o módulos externos como GPS, Bluetooth o módulos de audio.
## Interfaces UART en el ESP32
El ESP32 contiene internamente tres módulos de hardware UART independientes, nombrados UART0, UART1 y UART2. Tener tres interfaces permite asignar funciones 
distintas a cada puerto para evitar interferencias en el sistema.\
### UART0
Se utiliza por defecto para la programación y depuración del chip. Está conectado físicamente al convertidor USB-a-Serie de la placa de desarrollo, por lo que se usa para cargar el código desde la computadora y para imprimir mensajes en el Monitor Serial mediante printf o ESP_LOG. Sus pines por defecto son GPIO1 (TX0) y GPIO3 (RX0).
### UART1
Rara vez se utiliza en su configuración por defecto porque sus pines estándar (GPIO9 y GPIO10) están conectados internamente a la memoria Flash del ESP32, donde reside el código. 
Si intentas usar este puerto sin remapear sus pines a otros GPIO libres mediante software, la memoria Flash colisiona y el microcontrolador se reinicia.
### UART2
Es la interfaz serie completamente libre para las aplicaciones del usuario. Al no estar atada al USB de depuración ni a la memoria Flash, se emplea para conectar módulos externos como GPS, 
Bluetooth, sensores seriales o para implementar un puerto de comandos externo (usando los pines GPIO17 para TX y GPIO16 para RX).
<img width="575" height="507" alt="image" src="https://github.com/user-attachments/assets/b030cf74-c6c9-469e-82f5-9d30396ba747" />

