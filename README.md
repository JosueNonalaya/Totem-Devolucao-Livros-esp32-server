# Totem de Devolução de Livros

Sistema de **devolução de livros utilizando ESP32 como servidor web local**, desenvolvido como um protótipo de IoT para automação do processo de devolução em uma biblioteca.

O ESP32 gerencia a comunicação com o leitor RFID, disponibiliza a interface web e mantém os dados dos usuários e livros armazenados localmente no sistema de arquivos **LittleFS**.

O sistema tem como requisitos os seguintes critérios:
  * Identificar usuários através de RFID;
  * Identificar livros através de RFID;
  * Disponibilizar uma interface web para interação com o usuário;
  * Processar a devolução do livro;
  * Atualizar o estado do livro e do usuário;
  * Informar visualmente o resultado da operação através de LEDs.

---
## Tecnologias Usadas

* **Hardware:** ESP32, Leitor RFID MFRC522, Tags/cartões RFID, Led VERMELHO e VERDE
* **Front:** HTML, CSS, JavaScript
* **Backend:** C++, JSON
* **Bibliotecas:** ArduinoJson.h, LittleFS.h, Wifi.h, WebServer.h, SPI.h, MRFC522.h
* **IDE:** Arduino, VSCODE

---
## Arquitetura

O ESP32 atua como o servidor da aplicação.

```text
                    +----------------------+
                    |       Usuário        |
                    +----------+-----------+
                               |
                               v
                    +----------------------+
                    |    Interface Web     |
                    |     HTML / JS        |
                    +----------+-----------+
                               |
                              HTTP
                               |
                               v
+---------------+      +----------------------+
|  Leitor RFID  |----->|        ESP32         |
|    MFRC522    |      |     Web Server       |
+---------------+      +----------+-----------+
                                  |
                    +-------------+-------------+
                    |                           |
                    v                           v
             +-------------+             +-------------+
             |   LittleFS  |             |    LEDs     |
             | JSON / Web  |             | Verde/Verm. |
             +-------------+             +-------------+
```

## Fluxo do Sistema

1. O usuário acessa a interface disponibilizada pelo ESP32.
2. O usuário é identificado através de seu cartão/tag RFID.
3. O usuário visualiza seus emprestimos e selecina livro a devolver.
4. O sistema solicita a aproximação do livro.
5. O livro é identificado pelo leitor RFID.
6. O ESP32 verifica o livro e atualiza seus dados.
7. O sistema registra o livro como devolvido.
8. O LED verde indica uma devolução realizada com sucesso.

## Estrutura do Projeto

```text
Totem_Devolucao_Livros/
│
├── Totem_Devolucao_Livros.ino
│
└── data/
    │
    ├── inicio.html
    │
    ├── banco/
    │   ├── alunos.json
    │   └── livros.json
    │
    ├── emprestimos/
    │   ├── lista_livros.html
    │   └── lista_livros.js
    │
    ├── js/
    │   └── index.js
    │
    ├── lendolivro/
    │   ├── lerlivro.html
    │   └── lerlivro.js
    │
    └── retorno/
        ├── livro_devolvido.html
        ├── livro_devolvido.js
        ├── livro_nao_devolvido.html
        └── livro_nao_devolvido.js
```
---
## Execução

1. Instale o **Arduino IDE**.
2. Configure o suporte à placa ESP32.
3. Instale as bibliotecas necessárias.
4. Configure a rede Wi-Fi no código.
5. Conecte o ESP32 ao computador.
6. Faça o upload do sketch.
7. Faça o upload da pasta `data/` para o LittleFS.
8. Conecte-se à mesma rede Wi-Fi do ESP32.
9. Acesse o endereço IP exibido no monitor serial.



**Protótipo funcional**
O projeto foi desenvolvido com foco na integração entre **hardware, software e comunicação em rede**, utilizando o ESP32 como servidor da aplicação.
