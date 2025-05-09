#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>
#include "hardware/adc.h"

// Definições de pinos
#define BUTTON1_PIN 5
#define BUTTON2_PIN 6
#define JOYSTICK_X 27
#define JOYSTICK_Y 26

#define WIFI_SSID "TESTE2"
#define WIFI_PASS "#Akatsuki"

// Estado dos sensores
char button1_message[50] = "Nenhum evento no botão 1";
char button2_message[50] = "Nenhum evento no botão 2";
char temperature_message[50] = "Temperatura: N/A";
char joystick_direction[50] = "Posição meutra";

char http_response[2048];

// Protótipos
void monitor_sensors();

const char* map_joystick_to_direction(int x, int y) {
    const int threshold_low = 1500;
    const int threshold_high = 2500;

    if (x < threshold_low && y > threshold_high) return "Noroeste";
    if (x > threshold_high && y > threshold_high) return "Nordeste";
    if (x < threshold_low && y < threshold_low) return "Sudoeste";
    if (x > threshold_high && y < threshold_low) return "Sudeste";
    if (x < threshold_low) return "Oeste";
    if (x > threshold_high) return "Leste";
    if (y > threshold_high) return "Norte";
    if (y < threshold_low) return "Sul";
    return "Posição neutra";
}

float read_temperature_celsius() {
    adc_select_input(4);
    uint16_t raw = adc_read();
    const float conversion_factor = 3.3f / (1 << 12);
    float voltage = raw * conversion_factor;
    return 27.0f - (voltage - 0.706f) / 0.001721f;
}

void create_http_response() {
    snprintf(http_response, sizeof(http_response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=UTF-8\r\n"
            "Cache-Control: no-cache, no-store, must-revalidate\r\n"
            "Pragma: no-cache\r\n"
            "Expires: 0\r\n\r\n"
            "<!DOCTYPE html>"
            "<html>"
            "<head>"
                "<meta charset=\"UTF-8\">"
                "<title>Status do Sistema</title>"
                "<meta http-equiv=\"refresh\" content=\"1\">"
                "<style>"
                    "body { font-family: Arial, sans-serif; margin: 20px; }"
                    "h1 { color: #444; }"
                    ".sensor-box { border: 1px solid #ddd; padding: 15px; margin-bottom: 15px; border-radius: 5px; }"
                "</style>"
            "</head>"
            "<body>"
            "<h1>Monitoramento em Tempo Real</h1>"
            "<div class=\"sensor-box\">"
                "<h2>Botões</h2>"
                "<p><strong>Botão 1:</strong> %s</p>"
                "<p><strong>Botão 2:</strong> %s</p>"
            "</div>"
            "<div class=\"sensor-box\">"
                "<h2>Temperatura</h2>"
                "<p>%s</p>"
            "</div>"
            "<div class=\"sensor-box\">"
                "<h2>Joystick</h2>"
                "<p><strong>Direção:</strong> %s</p>"
            "</div>"
            "<p><small>Atualizando automaticamente...</small></p>"
            "</body></html>\r\n",
            button1_message, button2_message, temperature_message, joystick_direction);
}

static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    monitor_sensors();
    create_http_response();

    tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);
    pbuf_free(p);
    return ERR_OK;
}

static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, http_callback);
    return ERR_OK;
}

static void start_http_server() {
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        printf("Erro ao criar PCB\n");
        return;
    }
    
    if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Erro ao vincular porta 80\n");
        return;
    }
    
    pcb = tcp_listen(pcb);
    if (!pcb) {
        printf("Erro ao colocar PCB em modo listen\n");
        return;
    }
    
    tcp_accept(pcb, connection_callback);
    printf("Servidor HTTP iniciado na porta 80\n");
}

void monitor_sensors() {
    static bool b1_last = false, b2_last = false;
    bool b1 = !gpio_get(BUTTON1_PIN);
    bool b2 = !gpio_get(BUTTON2_PIN);

    if (b1 != b1_last) {
        b1_last = b1;
        snprintf(button1_message, sizeof(button1_message), b1 ? "Pressionado!" : "Solto");
    }

    if (b2 != b2_last) {
        b2_last = b2;
        snprintf(button2_message, sizeof(button2_message), b2 ? "Pressionado!" : "Solto");
    }

    adc_select_input(1);
    int joy_x = adc_read();
    adc_select_input(0);
    int joy_y = adc_read();
    snprintf(joystick_direction, sizeof(joystick_direction), "%s (X: %d, Y: %d)", 
             map_joystick_to_direction(joy_x, joy_y), joy_x, joy_y);

    float temp = read_temperature_celsius();
    snprintf(temperature_message, sizeof(temperature_message), "%.2f °C", temp);
}

int main() {
    stdio_init_all();
    
    // Aguarda a conexão serial estar pronta (especialmente importante para USB)
    sleep_ms(2000);
    printf("\n=== Iniciando Sistema ===\n");
    printf("Aguardando inicialização...\n");
    sleep_ms(3000);

    // Inicializa Wi-Fi
    printf("Inicializando Wi-Fi...\n");
    if (cyw43_arch_init()) {
        printf("Falha na inicialização do Wi-Fi\n");
        return 1;
    }

    // Conecta ao Wi-Fi
    printf("Conectando a rede '%s'...\n", WIFI_SSID);
    cyw43_arch_enable_sta_mode();
    
    int tentativas = 0;
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        tentativas++;
        printf("Falha na conexão (tentativa %d). Tentando novamente...\n", tentativas);
        sleep_ms(5000);
        
        if (tentativas >= 3) {
            printf("Não foi possível conectar após %d tentativas. Reinicie o sistema.\n", tentativas);
            return 1;
        }
    }

    // Mostra informações de conexão
    printf("\n=== Conexão Estabelecida ===\n");
    printf("Wi-Fi conectado com sucesso!\n");
    
    uint8_t *ip = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
    printf("Endereço IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
    printf("Acesse no navegador: http://%d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
    printf("Monitorando sensores...\n\n");

    // Configura GPIOs
    gpio_init(BUTTON1_PIN); 
    gpio_set_dir(BUTTON1_PIN, GPIO_IN); 
    gpio_pull_up(BUTTON1_PIN);
    
    gpio_init(BUTTON2_PIN); 
    gpio_set_dir(BUTTON2_PIN, GPIO_IN); 
    gpio_pull_up(BUTTON2_PIN);

    // Configura ADC
    adc_init();
    adc_gpio_init(JOYSTICK_X);
    adc_gpio_init(JOYSTICK_Y);
    adc_set_temp_sensor_enabled(true);

    // Inicia servidor
    start_http_server();

    // Loop principal
    while (true) {
        monitor_sensors();
        sleep_ms(1000);
    }

    return 0;
}