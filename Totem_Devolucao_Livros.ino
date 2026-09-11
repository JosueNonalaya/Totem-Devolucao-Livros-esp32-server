#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#define SPIFFS LittleFS
#include <WebServer.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 5
#define RST_PIN 27
#define LED_VERDE 2
#define LED_VERMELHO 15

MFRC522 mfrc522(SS_PIN, RST_PIN);
WebServer server(80);

const char* ssid = "NOME_DA_REDE_WIFI";
const char* password = "SENHA_DA_REDE";

String currentUser = "";
String pendingLivro = "";
unsigned long ultimoCheckWiFi = 0;
const unsigned long intervaloCheckWiFi = 10000; // 10 segundos

/* ================= WIFI ================= */

void conectarWiFi() {
  WiFi.mode(WIFI_STA);  
  WiFi.begin(ssid, password);
  Serial.println("Conectando ao Wi-Fi...");

  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
    delay(500);
    Serial.print(".");
    tentativas++;
  }

  Serial.print("Status: ");
  Serial.println(WiFi.status());

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n SUCESSO Wi-Fi conectado: " + WiFi.localIP().toString());
    digitalWrite(2, HIGH);
  } else {
    Serial.println("\n FALHA ao conectar. Tentando novamente em 10s...");
  }
}
void verificarWiFi() {
  unsigned long agora = millis();
  if (agora - ultimoCheckWiFi >= intervaloCheckWiFi) {
    ultimoCheckWiFi = agora;

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println(" ! Wi-Fi desconectado ! Tentando reconectar...");
      digitalWrite(LED_VERMELHO, HIGH);
      conectarWiFi();
      if (WiFi.status() == WL_CONNECTED) {
        digitalWrite(LED_VERMELHO, LOW);
        server.begin();
        Serial.println("Servidor reiniciado após reconexão Wi-Fi.");
      }
    }
  }
}

/* ================= UTILIDADES ================= */

void sendFile(String path, String contentType) {
  File f = SPIFFS.open(path, "r");
  if (!f) {
    server.send(404, "text/plain", "Arquivo nao encontrado");
    return;
  }
  server.streamFile(f, contentType);
  f.close();
}
bool isUsuario(String id) {
  File f = SPIFFS.open("/banco/usuarios.json", "r"); // <-- trocar caminho
  if(!f) return false; // caso arquivo não exista
  DynamicJsonDocument doc(8192); // aumenta para caber todos os usuários
  deserializeJson(doc, f);
  f.close();
  return doc.containsKey(id);
}


/* ================= ROTAS ================= */
void handleRoot() { sendFile("/inicio.html", "text/html"); }
void handleLogin() { sendFile("/inicio.html", "text/html"); }
void handleDevolucao() { sendFile("/emprestimos/lista_livros.html", "text/html"); } 
void handleLer() { sendFile("/lendolivro/lerlivro.html", "text/html"); }
void handleSucesso() { sendFile("/retorno/livro_devolvido.html", "text/html"); }
void handleErro() { sendFile("/retorno/livro_nao_devolvido.html", "text/html"); }

void handleUsuario() {
  String id = server.arg("id");
  if (id == "") {
    server.send(400, "application/json", "{\"erro\":\"ID não informado\"}");
    return;
  }

  File f = SPIFFS.open("/banco/usuarios.json", "r");
  DynamicJsonDocument doc(8192);
  deserializeJson(doc, f);
  f.close();

  if (!doc.containsKey(id)) {
    server.send(404, "application/json", "{\"erro\":\"Usuário não encontrado\"}");
    return;
  }

  String res;
  serializeJson(doc[id], res);
  server.send(200, "application/json", res);
}

void iniciarDevolucao() {
  if (currentUser == "") {
    server.send(400, "application/json", "{\"erro\":\"Nenhum usuário ativo\"}");
    digitalWrite(LED_VERMELHO, HIGH);
    delay(1000);
    digitalWrite(LED_VERMELHO, LOW);
    return;
  }

  pendingLivro = "aguardando";
  server.send(200, "application/json", "{\"mensagem\":\"Passe o livro no leitor para devolução\"}");
}

/* ================= PROCESSAMENTO ================= */

void processarDevolucao(String livroTag) {
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_VERMELHO, LOW);

  File f1 = SPIFFS.open("/banco/livros.json", "r");
  File f2 = SPIFFS.open("/banco/usuarios.json", "r");
  DynamicJsonDocument livros(4096), banco(8192);
  deserializeJson(livros, f1);
  deserializeJson(banco, f2);
  f1.close(); f2.close();

  if (!livros.containsKey(livroTag)) {
    Serial.println(" X Livro não encontrado!");
    digitalWrite(LED_VERMELHO, HIGH);
    delay(2000);
    digitalWrite(LED_VERMELHO, LOW);
    pendingLivro = "";
    sendFile("/retorno/livro_nao_devolvido.html", "text/html");
    return;
  }

  livros[livroTag]["emprestado"] = false;
  livros[livroTag]["pessoa"] = nullptr;

  JsonArray arr = banco[currentUser]["livros"].as<JsonArray>();
  for (JsonObject l : arr) {
    if (l["id"] == livroTag) l["emprestado"] = false;
  }

  File out1 = SPIFFS.open("/banco/livros.json", "w");
  serializeJsonPretty(livros, out1);
  out1.close();
  File out2 = SPIFFS.open("/banco/usuarios.json", "w");
  serializeJsonPretty(banco, out2);
  out2.close();

  Serial.println("!Livro devolvido com sucesso!");
  digitalWrite(LED_VERDE, HIGH);
  delay(2000);
  digitalWrite(LED_VERDE, LOW);
  pendingLivro = "";

  sendFile("/retorno/livro_devolvido.html", "text/html");
}

/* ================= SETUP ================= */
String lastTag = "";   // última TAG lida
bool novaTag = false;  // controle para o JSP

void setup() {
  Serial.begin(115200);
  SPIFFS.begin(true);
  SPI.begin();
  mfrc522.PCD_Init();
  
  pinMode(2, OUTPUT); 

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);

  conectarWiFi();

  // Rotas HTML  
  server.on("/", handleRoot);                                   // envia /inicio.html
  server.on("/inicio.html", handleLogin);                       // envia /inicio.html
  server.on("/emprestimos/lista_livros.html", handleDevolucao); // envia /emprestimos/lista_livros.html
  server.on("/lendolivro/lerlivro.html", handleLer);
  server.on("/retorno/livro_devolvido.html", handleSucesso);
  server.on("/retorno/livro_nao_devolvido.html", handleErro);
  server.on("/banco/alunos.json", HTTP_GET, []() {
    sendFile("/banco/alunos.json", "application/json");
});



  // Arquivo JS
  server.on("/js/index.js", []() { sendFile("/js/index.js", "application/javascript"); });

  server.on("/rfid", HTTP_GET, []() {
    if (novaTag) {
        novaTag = false;
        server.send(200, "application/json", "{\"tag\":\"" + lastTag + "\"}");
    } else {
        server.send(204); // sem conteúdo
    }
  });

  server.serveStatic("/", LittleFS, "/");

  server.begin();
  Serial.println("OK Servidor iniciado!");
}


/* ================= LOOP ================= */
void loop() {
  server.handleClient();
  verificarWiFi();

  // Leitura RFID
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return;

  String tag = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) tag += String(mfrc522.uid.uidByte[i], HEX);
  tag.toUpperCase();

  Serial.println("Tag detectada: " + tag);

  lastTag = tag;
  novaTag = true;
  if (pendingLivro != "") {
    processarDevolucao(tag);
  } else if (isUsuario(tag)) {
    currentUser = tag;
    Serial.println("Usuário identificado: " + currentUser);
  }

  delay(1000);
}