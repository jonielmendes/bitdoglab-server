# Projeto IoT com Raspberry Pi Pico e ThingSpeak

Este projeto utiliza um Raspberry Pi Pico para monitorar dois botões, a posição de um joystick e a temperatura interna do microcontrolador. Os dados são enviados periodicamente para a nuvem (ThingSpeak), permitindo visualização em tempo real por meio de gráficos e painéis personalizados.

## 🧩 Funcionalidades

- **Leitura dos botões (GPIO 5 e 6)**: Captura o estado (pressionado/solto).
- **Leitura do joystick (ADC 0 e 1)**: Interpretação de direção (rosa dos ventos).
- **Leitura da temperatura interna (ADC 4)**: Conversão para °C.
- **Envio de dados para o ThingSpeak(versão nuvem)**: Via requisição HTTP GET a cada 15 segundos.
- **Servidor Web local (versão local)**: Exibição dos dados em HTML via rede local a cada 1 segundo.
- **Exemplo de aplicação real**: Simulação de monitoramento industrial.

## 🔌 Pinos Utilizados

| Função                | Pino do Pico     |
|-----------------------|------------------|
| Botão A               | GPIO 5           |
| Botão B               | GPIO 6           |
| Joystick X            | ADC 1 (GPIO 27)  |
| Joystick Y            | ADC 0 (GPIO 26)  |
| Sensor de Temperatura | ADC 4            |
| LED Indicador         | GPIO 20          |

## 🌐 Configuração de Rede

> ⚠️ **Importante:** Antes de compilar, edite o código e preencha suas informações de Wi-Fi e chave da API do ThingSpeak:

```c
#define WIFI_SSID           "SEU_SSID_AQUI"
#define WIFI_PASSWORD       "SUA_SENHA_AQUI"
#define THINGSPEAK_API_KEY  "SUA_API_KEY_AQUI"
#define THINGSPEAK_HOST     "api.thingspeak.com"
```

## 🧭 Mapeamento da Rosa dos Ventos com o Joystick

O joystick retorna valores analógicos nos eixos **X** e **Y**, de 0 a 4095. Esses valores são mapeados para direções de uma rosa dos ventos, com base em limites definidos no código.

### 🎯 Direções possíveis

```
         Norte
           ↑
  Noroeste ↑↑↑ Nordeste
           ↑
Oeste ←── Centro ──→ Leste
           ↓
  Sudoeste ↓↓↓ Sudeste
           ↓
         Sul
```

### 📊 Faixas de valores (exemplo)

| Direção     | X         | Y         |
|-------------|-----------|-----------|
| Norte       | ~2000     | >3500     |
| Sul         | ~2000     | <1000     |
| Leste       | >3500     | ~2000     |
| Oeste       | <1000     | ~2000     |
| Nordeste    | >3000     | >3000     |
| Noroeste    | <1000     | >3000     |
| Sudeste     | >3000     | <1000     |
| Sudoeste    | <1000     | <1000     |
| Centro      | 1800–2300 | 1800–2300 |

> ⚙️ Ajuste os limites conforme o comportamento real do seu joystick.

## ☁️ Envio para ThingSpeak

Os dados dos sensores são enviados a cada 15 segundos (limite da conta gratuita no ThingSpeak) usando uma requisição HTTP GET.

### 🧾 Exemplo de requisição:

```http
GET /update?api_key=SUA_API_KEY&field1=BOTAO_A&field2=BOTAO_B&field3=TEMP&field4=JOY_X&field5=JOY_Y HTTP/1.1
Host: api.thingspeak.com
Connection: close
```

## 🖥️ Como Rodar o Projeto

### ✅ Pré-requisitos

- SDK do Raspberry Pi Pico configurado
- Pilha LWIP funcional
- Acesso à internet via Wi-Fi

### ⚙️ Configuração

Edite no código:

```c
#define WIFI_SSID "SEU_SSID"
#define WIFI_PASSWORD "SUA_SENHA"
#define THINGSPEAK_API_KEY "SUA_CHAVE_THINGSPEAK"
```

### 🧪 Compilação

```bash
mkdir build
cd build
cmake ..
make
```

### 🚀 Upload para o Pico

- Use o `picotool`, ou
- Copie o arquivo `.uf2` gerado para o Raspberry Pi Pico via USB

### 📡 Visualização

- Acesse [ThingSpeak](https://thingspeak.com/)
- Veja os campos atualizados em tempo real com:
  - Botões A e B (pressionado ou solto)
  - Temperatura em °C
  - Eixos X e Y do joystick

## 💡 Aplicações Reais

Este projeto pode ser usado em:

- 🏭 Monitoramento remoto de máquinas e ambientes
- 🧪 Testes educacionais com sensores e IoT
- 🚀 Prototipagem de sistemas embarcados




