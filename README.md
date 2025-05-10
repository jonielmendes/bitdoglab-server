# Projeto IoT com Raspberry Pi Pico e ThingSpeak

Este projeto utiliza um Raspberry Pi Pico para monitorar os status de botões e a posição do joystick da placa Bitdoglab. Os dados dos botões e do joystick são enviados para um servidor a cada 1 segundo. Como um desafio extra, um sensor foi adicionado para enviar seus dados para o servidor. Além disso, uma rosa dos ventos imaginária foi criada para representar a posição selecionada no joystick.

## Funcionalidades

### 1. Leitura de Status dos Botões
O programa monitora os estados dos botões da placa Bitdoglab e envia, a cada 1 segundo, os status para um servidor. 

### 2. Leitura da Posição do Joystick
A posição X e Y do joystick da placa Bitdoglab é lida e enviada para um servidor via Wi-Fi. Além disso, a posição do joystick é mapeada para uma rosa dos ventos imaginária, que inclui as direções: 
- Norte
- Sul
- Leste
- Oeste
- Nordeste
- Sudeste
- Noroeste
- Sudoeste

### 3. Desafio Extra: Envio de Dados de Sensor
Um sensor adicional foi integrado ao sistema, e seus dados são enviados ao servidor juntamente com os status dos botões e as posições do joystick.

### 4. Desafio Adicional: Servidor na Nuvem
O código foi refeito para enviar os dados para um servidor na nuvem. O ThingSpeak foi utilizado para monitorar os dados dos sensores, mas também foi feito um exemplo de integração com outros servidores na nuvem como AWS ou Google.

## Estrutura do Projeto

### Pinos Utilizados:

- **Pino 5 e 6:** Botões de entrada (BUTTON1_PIN, BUTTON2_PIN)
- **Pino 20:** LED de saída (LED_PIN)
- **Pino 27 e 26:** Joystick (JOYSTICK_X, JOYSTICK_Y)
- **Pino 4:** Sensor (desafio extra)

### Configuração de Rede:

- **Wi-Fi SSID:** `TESTE2`
- **Wi-Fi Senha:** `#Akatsuki`
- **ThingSpeak API Key:** `4G8XNIL18230JBI5`
- **ThingSpeak Host:** `api.thingspeak.com`

### Bibliotecas Utilizadas:

- **pico/cyw43_arch.h:** Para interação com o Wi-Fi do Raspberry Pi Pico.
- **pico/stdlib.h:** Funções padrão para controle do hardware e entrada/saída.
- **hardware/adc.h:** Para ler os valores dos sensores analógicos.
- **lwip/tcp.h:** Para comunicação TCP.
- **lwip/dns.h:** Para resolução DNS ao enviar dados para o servidor na nuvem (ThingSpeak ou outros).

## Funcionalidade Principal

### 1. Monitoramento dos Sensores:
- O status dos botões e os valores do joystick são lidos constantemente.
- Um sensor adicional foi integrado, e seus dados também são enviados para o servidor.

### 2. Envio de Dados para o Servidor:
- Os dados coletados dos sensores são formatados e enviados para um servidor a cada 1 segundo.
- O servidor na nuvem (ThingSpeak ou outro) exibe os dados em tempo real.

### 3. Mapeamento da Rosa dos Ventos:
- A posição do joystick é mapeada para uma rosa dos ventos imaginária e enviada para o servidor.
  
### 4. Servidor na Nuvem:
- O projeto foi adaptado para enviar dados para um servidor na nuvem, como ThingSpeak, mas a estrutura do código pode ser facilmente modificada para outros servidores, como AWS ou Google Cloud.

## Como Rodar o Projeto

### 1. Preparar o Ambiente:
- Certifique-se de que o Raspberry Pi Pico esteja configurado corretamente com o SDK do Raspberry Pi Pico.
- Conecte os sensores e os botões aos pinos apropriados do Raspberry Pi Pico.

### 2. Compilar e Carregar:
- Compile o código utilizando o ambiente de desenvolvimento do Raspberry Pi Pico.
- Carregue o código no Raspberry Pi Pico.

### 3. Conectar ao Wi-Fi:
- O projeto se conecta automaticamente à rede Wi-Fi configurada (`TESTE2` e `#Akatsuki`).

### 4. Monitorar os Dados:
- O Raspberry Pi Pico enviará os dados para o servidor a cada 1 segundo.
- Você pode acessar o servidor (ThingSpeak ou outro) para visualizar os dados em tempo real.

### 5. Servidor na Nuvem:
- Para visualizar os dados na nuvem, configure o ThingSpeak ou outro servidor de sua preferência para receber os dados. A chave API de ThingSpeak já está configurada no código.

## Desafios Realizados

- **Desafio 1:** Envio dos dados dos botões e joystick para o servidor.
- **Desafio 2:** Adição de um sensor extra e envio de seus dados para o servidor.
- **Desafio 3:** Criação de uma rosa dos ventos imaginária e envio da direção selecionada pelo joystick.
- **Desafio 4:** Migração do servidor local para um servidor na nuvem (ThingSpeak e servidores como AWS ou Google).

## Contribuindo

Se você deseja contribuir para este projeto, sinta-se à vontade para abrir issues e pull requests. Você pode melhorar a leitura de sensores, adicionar novos sensores ou modificar a visualização na nuvem.
