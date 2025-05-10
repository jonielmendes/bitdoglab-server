#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"

// Definições de pinos
#define BUTTON1_PIN 5
#define BUTTON2_PIN 6
#define LED_PIN 20
#define JOYSTICK_X 27
#define JOYSTICK_Y 26

#define WIFI_SSID "TESTE2"
#define WIFI_PASSWORD "#Akatsuki"

#define THINGSPEAK_API_KEY "4G8XNIL18230JBI5"
#define THINGSPEAK_HOST "api.thingspeak.com"

// Função para ler a temperatura do sensor interno
float read_temperature() {
    adc_select_input(4);  // Canal 4 é o sensor de temperatura interno
    uint16_t raw = adc_read(); // Leitura do ADC (valor bruto)
    const float conversion_factor = 3.3f / (1 << 12); // Conversão para tensão
    float voltage = raw * conversion_factor;
    float temperature = 27.0f - (voltage - 0.706f) / 0.001721f; // Fórmula da documentação
    return temperature;
}

typedef struct {
    struct tcp_pcb *pcb;
    char request[256];
} thingspeak_request_t;

err_t thingspeak_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    printf("Dados enviados ao ThingSpeak com sucesso!\n");
    tcp_close(tpcb);
    free(arg);
    return ERR_OK;
}

err_t thingspeak_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err != ERR_OK) {
        printf("Erro na conexão TCP: %d\n", err);
        tcp_close(tpcb);
        free(arg);
        return err;
    }

    thingspeak_request_t *req = (thingspeak_request_t *)arg;
    tcp_write(tpcb, req->request, strlen(req->request), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);
    tcp_sent(tpcb, thingspeak_sent);

    return ERR_OK;
}

void thingspeak_dns_callback(const char *name, const ip_addr_t *ipaddr, void *arg) {
    if (!ipaddr) {
        printf("Falha na resolução de DNS para %s\n", name);
        free(arg);
        return;
    }

    thingspeak_request_t *req = (thingspeak_request_t *)arg;
    if (tcp_connect(req->pcb, ipaddr, 80, thingspeak_connected) != ERR_OK) {
        printf("Erro ao conectar após DNS\n");
        tcp_close(req->pcb);
        free(req);
    }
}

void send_to_thingspeak() {
    static bool b1_last = false, b2_last = false;

    // Lê os estados dos botões
    bool b1 = !gpio_get(BUTTON1_PIN); // 1 se pressionado, 0 se solto
    bool b2 = !gpio_get(BUTTON2_PIN); // 1 se pressionado, 0 se solto

    // Depuração dos botões
    printf("Estado dos botões - B1: %d, B2: %d\n", b1, b2);

    // Lê a temperatura
    float temp = read_temperature(); // Lê a temperatura da placa

    // Lê os valores do joystick
    adc_select_input(1); // Canal do eixo X
    int joy_x = adc_read();
    adc_select_input(0); // Canal do eixo Y
    int joy_y = adc_read();

    // Depuração do joystick
    printf("Joystick X: %d, Joystick Y: %d\n", joy_x, joy_y);

    // Prepara a estrutura para o envio
    thingspeak_request_t *req = malloc(sizeof(thingspeak_request_t));
    if (!req) return;

    req->pcb = tcp_new();
    if (!req->pcb) {
        free(req);
        return;
    }

    // Cria a requisição para o ThingSpeak
    snprintf(req->request, sizeof(req->request),
        "GET /update?api_key=%s&field1=%d&field2=%d&field3=%.2f&field4=%d&field5=%d HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n\r\n",
        THINGSPEAK_API_KEY, b1, b2, temp, joy_x, joy_y, THINGSPEAK_HOST);

    // Depuração do URL enviado
    printf("Enviando para ThingSpeak: %s\n", req->request);

    ip_addr_t ipaddr;
    err_t dns_err = dns_gethostbyname(THINGSPEAK_HOST, &ipaddr, thingspeak_dns_callback, req);

    if (dns_err == ERR_OK) {
        if (tcp_connect(req->pcb, &ipaddr, 80, thingspeak_connected) == ERR_OK) {
            tcp_write(req->pcb, req->request, strlen(req->request), TCP_WRITE_FLAG_COPY);
            tcp_output(req->pcb);
            tcp_sent(req->pcb, thingspeak_sent);
        } else {
            printf("Erro ao conectar (resolução cache)\n");
            tcp_close(req->pcb);
            free(req);
        }
    } else if (dns_err != ERR_INPROGRESS) {
        printf("Erro ao iniciar resolução de DNS: %d\n", dns_err);
        tcp_close(req->pcb);
        free(req);
    }
}

int main() {
    stdio_init_all();
    adc_init();

    // Configura GPIOs
    gpio_init(BUTTON1_PIN);
    gpio_set_dir(BUTTON1_PIN, GPIO_IN);
    gpio_pull_up(BUTTON1_PIN);

    gpio_init(BUTTON2_PIN);
    gpio_set_dir(BUTTON2_PIN, GPIO_IN);
    gpio_pull_up(BUTTON2_PIN);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Configura ADC para os pinos do joystick
    adc_gpio_init(JOYSTICK_X);
    adc_gpio_init(JOYSTICK_Y);

    if (cyw43_arch_init()) {
        printf("Erro ao inicializar Wi-Fi\n");
        return -1;
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Falha ao conectar ao Wi-Fi\n");
        return -1;
    }

    printf("Conectado ao Wi-Fi!\n");

    while (true) {
        send_to_thingspeak();
        gpio_put(LED_PIN, 1);
        sleep_ms(1000);
        gpio_put(LED_PIN, 0);
        sleep_ms(15000); // Aguarda 15s entre envios
    }

    return 0;
}