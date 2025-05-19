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

- Acesse [ThingSpeak](https://thingspeak.mathworks.com/channels/2954999)
- Veja os campos atualizados em tempo real com:
  - Botões A e B (pressionado ou solto)
  - Temperatura em °C
  - Eixos X e Y do joystick

## 💡 Aplicações Reais

Este projeto pode ser usado em:

- 🏭 Monitoramento remoto de máquinas e ambientes
- 🧪 Testes educacionais com sensores e IoT
- 🚀 Prototipagem de sistemas embarcados

# 🧠 Ideia do Projeto

O projeto nasceu com o objetivo de explorar o uso do Raspberry Pi Pico (BitDogLab) em aplicações de IoT. A proposta era:

- Monitorar **botões físicos** e **joystick**
- Medir a **temperatura interna**
- Exibir os dados via **servidor local**
- Como desafio extra: **enviar os dados para a nuvem foi escolhido o (ThingSpeak) por ser grátis e fácil de usar**

Além da leitura bruta dos sensores, foi implementado um sistema de **interpretação de direção (rosa dos ventos)** com o joystick, e visualizações gráficas em tempo real.

O código foi escrito todo em **C**, sem bibliotecas de alto nível, utilizando diretamente o SDK oficial do RP2040 e a pilha TCP/IP LWIP.
# 🌐 Visualização Local com Página HTML

A primeira versão do projeto implementa um servidor local. A cada 1 segundo, os dados dos sensores são atualizados e exibidos em uma página HTML acessível via navegador.

## Sensores Monitorados:
- 🟢 Botões físicos (estado pressionado ou solto)
- 🌡️ Temperatura interna (°C)
- 🎮 Joystick (X, Y e direção interpretada)

## Resultado:



![Captura de tela 2025-05-10 024441](https://github.com/user-attachments/assets/1d2dff7f-5d6c-4439-af52-ae107869bd86)

# ☁️ Integração com o ThingSpeak

A segunda versão envia os dados para a nuvem usando o serviço ThingSpeak. A cada 15 segundos, os sensores são lidos e os dados são enviados via HTTP GET.

## Campos Enviados:
- field1: Botão A
- field2: Botão B
- field3: Temperatura 
- field4: Eixo X do Joystick
- field5: Eixo Y do Joystick

## Resultados:

- 📈 Temperatura em tempo real
- 🧭 Direção do joystick com rosa dos ventos (via Math Visualization)
- 🔴 Estado dos botões em gráfico digital


![Captura de tela 2025-05-10 011456](https://github.com/user-attachments/assets/6da45d66-6e93-49a5-b7d9-95a2ecad4918)
![Captura de tela 2025-05-10 170055](https://github.com/user-attachments/assets/7c11ee36-741f-4861-804b-b1f3104fcc72)
![Captura de tela 2025-05-10 172014](https://github.com/user-attachments/assets/2177eadb-660a-4a8d-8de3-4651e7f7c71a)
![Captura de tela 2025-05-10 011327](https://github.com/user-attachments/assets/98970607-5326-4116-8b22-33a29746dbee)

# 🏭 Aplicação Simulada: Monitoramento Industrial

Este projeto pode simular um sistema de monitoramento remoto de máquinas industriais:

- ⚙️ Botões = estados da máquina (ligado/desligado, emergência)
- 🌡️ Temperatura = sobreaquecimento
- 🎮 Joystick = controle ou ajuste remoto

### Benefícios:
- Visualização em tempo real
- Sem necessidade de hardware adicional
- Possível integração com alertas e dashboards
# ✅ Resultados Finais

O projeto atendeu todos os objetivos e desafios:

✔️ Conexão estável à rede Wi-Fi  
✔️ Visualização dos dados localmente (HTML)  
✔️ Integração com a nuvem (ThingSpeak)  
✔️ Conversão da entrada do joystick para rosa dos ventos  
✔️ Gráficos em tempo real


![Captura de tela 2025-05-10 011355](https://github.com/user-attachments/assets/d870cd97-1b7c-47d2-810d-387f94d5697d)
![Captura de tela 2025-05-10 011429](https://github.com/user-attachments/assets/8c48d8f5-0df5-4c16-a2f0-fea31e8ac44a)
![Captura de tela 2025-05-10 011513](https://github.com/user-attachments/assets/357378e6-25ec-49dd-80d7-bc087182604a)
![Captura de tela 2025-05-10 011529](https://github.com/user-attachments/assets/61cf84f8-69db-45be-9356-7d4b0ccb1793)

> Resultado: um sistema de IoT completo, funcional e com potencial de uso real.


### Contato
📧 **Email**: jonielmendes237@gmail.com
🔗 **LinkedIn**: [linkedin.com/in/joniel-mendes](https://www.linkedin.com/in/jonielmendes/)



