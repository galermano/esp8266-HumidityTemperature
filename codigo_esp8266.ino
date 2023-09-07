#include <ESP8266WiFi.h>
#include <SimpleDHT.h>

// WiFi - Coloque aqui suas configurações de WI-FI
const char ssid[] = "Rede2004";
const char psw[] = "victin17";

// Site remoto - Coloque aqui os dados do site que vai receber a requisição GET
const char http_site[] = "192.168.201.228";
const int http_port = 8081;
const char http_path[] = "/cadastrar";

// Variáveis globais
WiFiClient client;
IPAddress server(192, 168, 201, 228);  // Endereço IP do servidor - http_site
int pinDHT11 = D0;
SimpleDHT11 dht11;

void setup() {
  delay(5000);
  Serial.begin(9600);
  Serial.println("NodeMCU - Gravando dados no BD via GET");
  Serial.println("Aguardando conexão");

  // Tenta conexão com Wi-Fi
  WiFi.begin(ssid, psw);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.print("\nWi-Fi conectado com sucesso: ");
  Serial.println(ssid);
}

void loop() {

  // Leitura do sensor DHT11
  delay(3000);  // delay entre as leituras
  byte temp = 0;
  byte humid = 0;
  if (dht11.read(pinDHT11, &temp, &humid, NULL)) {
    Serial.print("Falha na leitura do sensor.");
    return;
  }

  Serial.println("Gravando dados no BD: ");
  Serial.print((int)temp);
  Serial.print(" *C, ");
  Serial.print((int)humid);
  Serial.println(" %");

  // Envio dos dados do sensor para o servidor via GET
  if (!getPage((int)temp, (int)humid)) {
    Serial.println("GET request failed");
  }
}

// Executa o HTTP GET request no site remoto
bool getPage(int temp, int humid) {
  if (!client.connect(server, http_port)) {
    Serial.println("Falha na conexão com o site");
    return false;
  }
  String http_path =+ "/" + String(temp) + "/" + String(humid);  // Parâmetros com as leituras

  client.print(String("GET /cadastrar/") + (temp) +"/"+(humid)+

                      " HTTP/1.1\r\n" + "Host: " + http_site + "\r\n" + "Connection: close\r\n\r\n");

  // Informações de retorno do servidor para debug
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  return true;
}
