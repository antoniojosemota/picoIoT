# 📡 Projeto IoT com Raspberry Pi Pico W e Comunicação HTTPS

Este projeto utiliza o Raspberry Pi Pico W para ler dados de sensores (joystick, microfone e botão), exibir informações em um display OLED (SSD1306) via I2C e enviar essas informações a um servidor web via HTTPS usando Wi-Fi.

## Clone o repositório:
``` bash
git clone https://github.com/antoniojosemota/picoIoT.git
```

## 🔧 Componentes Utilizados
- Raspberry Pi Pico W
- Display OLED SSD1306 (128x64, comunicação I2C)
- Joystick analógico (2 eixos conectados aos ADCs)
- Microfone analógico (conectado ao ADC)
- Botão (com pull-up interno)
- Wi-Fi (via módulo integrado CYW43 do Pico W)

## 📌 Funcionalidades
- Conexão à rede Wi-Fi via WPA2.
- Leitura de:
  - Joystick (eixos X e Y) e conversão em uma direção tipo bússola (ex: NORTE, SUL, LESTE...).
  - Microfone (nível de ruído captado via ADC).
  - Botão (pressionado ou não).
- Exibição de informações no display OLED.
- Envio periódico (a cada 1 segundo) dos dados via requisição HTTPS GET para um [API Node.js](https://github.com/antoniojosemota/serverjs.git)
- Visualização dos dados enviados para a API, usamos a [Dashboard Pico](https://github.com/antoniojosemota/htmlpico).

## 🔩 Pinagem
| Função         | Pino Pico W | Observação          |
| -------------- | ----------- | ------------------- |
| I2C SDA        | GP14        | Display SSD1306     |
| I2C SCL        | GP15        | Display SSD1306     |
| ADC Joystick Y | GP26 (ADC0) | Eixo vertical       |
| ADC Joystick X | GP27 (ADC1) | Eixo horizontal     |
| ADC Microfone  | GP28 (ADC2) | Entrada analógica   |
| Botão          | GP5         | Com pull-up interno |

## ⚙️ Compilação com o Pico SDK
Estrutura de pastas esperada:
```
picoIoT/
├── CMakeLists.txt
├── pico_sdk_import.cmake
├── lwipopts.h
├── lwipopts_examples_common.h
├── mbedtls_config_examples_common.h
├── senddata.c
├── utils/
│   ├── ssd1306.c
│   └── ssd1306.h
└── .vscode/
    └── settings.json
```
Usando o Pico SDK, faça a compilação.
- Conecte o Pico W via USB com o botão BOOTSEL pressionado.
- 🌐 Conecte ao Wi-Fi:
```
#define SSID "SSID_WIFI"
#define PASS "PASSWORD"
```
- Copie o arquivo .uf2 gerado na pasta build para o dispositivo que aparecer.

## Projetado por Antonio Jose Mota

