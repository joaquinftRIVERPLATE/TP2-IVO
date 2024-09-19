#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <HTTPClient.h>

const char* nombreRed = "IoTB";
const char* claveRed = "inventaronelVAR";
const char* apiKey = "16dac840d6e5389d39e868bd59a67ef8
qq  aa"; // Reemplaza con tu API Key de OpenWeatherMap
const char* ciudad = "Buenos Aires";

WebServer servidor(80);
const int pinRele = 5;
const int pinDHT = 4; // Pin donde está conectado el DHT
DHT dht(pinDHT, DHT22); // Usa DHT11 o DHT22 según tu sensor

void paginaPrincipal() {
  String pagina = "<html>\
  <head><title>Control del Relé</title></head>\
  <body>\
  <h1>Control del Relé</h1>\
  <button onclick=\"window.location.href='/encender'\">Encender</button>\
  <button onclick=\"window.location.href='/apagar'\">Apagar</button>\
  <h2>Temperatura Ambiente: " + String(dht.readTemperature()) + " °C</h2>\
  <h2>Temperatura en Buenos Aires: " + obtenerTemperaturaBuenosAires() + " °C</h2>\
  </body>\
  </html>";
  servidor.send(200, "text/html", pagina);
}

void encenderRele() {
  digitalWrite(pinRele, LOW);
  servidor.send(200, "text/html", "<h1>Relé Encendido</h1><a href='/'>Volver</a>");
}

void apagarRele() {
  digitalWrite(pinRele, HIGH);
  servidor.send(200, "text/html", "<h1>Relé Apagado</h1><a href='/'>Volver</a>");
}

String obtenerTemperaturaBuenosAires() {
  HTTPClient http;
  String temperatura = "No disponible";

  http.begin("http://api.openweathermap.org/data/2.5/weather?q=" + String(ciudad) + "&appid=" + String(apiKey) + "&units=metric");
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    int startIndex = payload.indexOf("\"temp\":") + 7;
    int endIndex = payload.indexOf(",", startIndex);
    temperatura = payload.substring(startIndex, endIndex);
  }
  
  http.end();
  return temperatura;
}

void setup() {
  Serial.begin(115200);
  pinMode(pinRele, OUTPUT);
  digitalWrite(pinRele, HIGH);
  dht.begin();

  WiFi.begin(nombreRed, claveRed);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a la red WiFi...");
  }

  Serial.println("Conectado a WiFi");
  Serial.println(WiFi.localIP());

  servidor.on("/", paginaPrincipal);
  servidor.on("/encender", encenderRele);
  servidor.on("/apagar", apagarRele);

  servidor.begin();
  Serial.println("Servidor web en funcionamiento");
}

void loop() {
  servidor.handleClient();
}
