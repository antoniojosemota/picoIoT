#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "ssd1306.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"
#include "example_http_client_util.h"
#include "pico/async_context.h"
#include "lwip/altcp_tls.h"

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGTH 64
#define SCREEN_ADDRESS 0x3C
#define I2C_SDA 14
#define I2C_SCL 15
#define ADC_Y 26
#define ADC_X 27
#define ADC_MIC 28
#define BTN 5
#define SSID "SSID_WIFI"
#define PASS "PASSWORD"
#define ADC_MAX 4095
#define HOST "serverjs-production-2bd6.up.railway.app"
#define PORT 443  // HTTPS
ssd1306_t display;

char newvalue[256];
char *direction;

void loc_joystick(uint x, uint y){
    ssd1306_clear(&display);
    ssd1306_draw_pixel(&display, x, y);
    ssd1306_show(&display);
}

void display_txt(char *txt){
    ssd1306_clear(&display);
    ssd1306_draw_string(&display, 30, 30, 1, txt);
    ssd1306_show(&display);
    sleep_ms(1000);
}

void setup(){
    stdio_init_all();

    i2c_init(i2c1, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    adc_init();
    adc_gpio_init(ADC_Y);
    adc_gpio_init(ADC_X);
    adc_gpio_init(ADC_MIC);

    gpio_init(BTN);
    gpio_set_dir(BTN, GPIO_IN);
    gpio_pull_up(BTN);

    sleep_ms(5000);
    
    if(cyw43_arch_init()){
        printf("Falha ao inicializar o Wi-Fi\n");
        return;
    }

    if (!ssd1306_init(&display, DISPLAY_WIDTH, DISPLAY_HEIGTH, SCREEN_ADDRESS, i2c1)) { 
        printf("Falha ao inicializar o display SSD1306\n");
        return;
    }

    display_txt("Iniciando o display");


    
    cyw43_arch_enable_sta_mode();
    
    if(cyw43_arch_wifi_connect_timeout_ms(SSID, PASS, CYW43_AUTH_WPA2_AES_PSK, 20000)){
        display_txt("Conectando ao WI-FI");
        printf("Falha ao conectar ao WI-FI\n");
        return;
    }

    if(netif_default){
        printf("IP do dispositivo: %s\n", ipaddr_ntoa(&netif_default->ip_addr));
    }


}

void http_request(uint16_t buttonState, uint16_t sensor, char *direction){

    snprintf(newvalue, sizeof(newvalue), "/update?direction=%s&buttonState=%d&sensor=%d", direction, buttonState, sensor);
    printf("%s\n", newvalue);

    EXAMPLE_HTTP_REQUEST_T req1 = {0};
    req1.hostname = HOST;
    req1.url = newvalue;
    req1.headers_fn = http_client_header_print_fn;
    req1.recv_fn = http_client_receive_print_fn;
    int result = http_client_request_sync(cyw43_arch_async_context(), &req1);
    result += http_client_request_sync(cyw43_arch_async_context(), &req1);
    printf("result: %d\n", result);

    while(!req1.complete){
        async_context_poll(cyw43_arch_async_context());
        async_context_wait_for_work_ms(cyw43_arch_async_context(), 1000);
    }

    req1.tls_config = altcp_tls_create_config_client(NULL, 0);
    result += http_client_request_sync(cyw43_arch_async_context(), &req1);
    altcp_tls_free_config(req1.tls_config);

    memset(&req1, 0, sizeof req1);

}


char *bussola(uint x, uint y) {
    char *result;

    if (x < 1500 && y < 1500) {
        result = "SUDOESTE";
        printf("SUDOESTE\n");
    } else if (x > 3000 && y < 1500) {
        result = "SUDESTE";
        printf("SUDESTE\n");
    } else if (x < 1500 && y > 3000) {
        result = "NOROESTE";
        printf("NOROESTE\n");
    } else if (x > 3000 && y > 3000) {
        result = "NORDESTE";
        printf("NORDESTE\n");
    } else if (x < 1500) {
        result = "OESTE";
        printf("OESTE\n");
    } else if (x > 3000) {
        result = "LESTE";
        printf("LESTE\n");
    } else if (y < 1500) {
        result = "SUL";
        printf("SUL\n");
    } else if (y > 3000) {
        result = "NORTE";
        printf("NORTE\n");
    }
    else {
        result = "CENTRO";
        printf("CENTRO\n");
    }

    return result;
}

int main()
{
    setup();

    adc_select_input(0);
    uint16_t adc_y = adc_read();
    adc_select_input(1);
    uint16_t adc_x = adc_read();
    adc_select_input(2);
    uint16_t mic = adc_read();

    int button = gpio_get(BTN);
    if (button == 0) {
        button = 1;
    } else {
        button = 0;
    }


    direction = bussola(adc_x, adc_y);

    http_request(button, mic, direction);
    
    sleep_ms(1000);    
    while (true) {
        cyw43_arch_poll();  // Necessário para manter o Wi-Fi ativo
        
        adc_select_input(0);
        adc_y = adc_read();
        adc_select_input(1);
        adc_x = adc_read();
        adc_select_input(2);
        mic = adc_read();

        button = gpio_get(BTN);

        direction = bussola(adc_x, adc_y);
        http_request(button, mic, direction);

        sleep_ms(1000);

        tight_loop_contents();
    }
    return 0;
}
