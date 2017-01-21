/*
 * --RING AND OPEN--
 * 
 * Nodo miembro del servidor IoT de la oficina de ImagineXYZ
 * Nombre en el diagrama de arquitectura: "RING IMAGINE - OPEN XYZ"
 *
 * Funciona como:
 *  -"Botón" para abrir desde abajo
 *  -Timbre
 *
 * @author Fabian Montero - Adrian Sanchez
 */

/* Bibliotecas para wifi y mqtt */
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/* Biblioteca para el sensor ultrasonico */
#include <Ultrasonic.h>

/* Valores de las redes inalámbrica y del servidor. */
const char* ssid = "placeholder_of_secret_information";
const char* password = "placeholder_of_secret_information";
const char* mqtt_server = "placeholder_of_secret_information";

/* Instancia la bilbioteca de wifi y declara variables para leer mensajes de mqtt */
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];

/* Variable donde se guarda la distancia de sensor ultrasonico */
int distancia;

Ultrasonic ultrasonic(12, 13); // Inicializa los pines del sensor ultrasonico (Trig PIN,Echo PIN)

/* Variables para el timbre */
bool ultrasonicListen = false;

String message = "";

void setup() {
  
  Serial.begin(115200); //Inicia comunicacion por el puerto serial

  Serial.println("Starting");
  setup_wifi(); //Inicia conexion con wifi

  client.setServer(mqtt_server, placeholder_of_secret_information); //Función para conectarse con el servidor MQTT (direccion , puerto)
  client.setCallback(callback); //Función para escuchar publicaciones del servidor
}




/*##------------------------FUNCIONES MQTT Y WiFi------------------------##*/

/* Funcion para conectarse a wifi */
void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

/* Función para escuchar publicaciones del servidor */
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }


  onMessage(message);
  
}

/* Ciclo para reconexión */
void reconnect() {
 
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    //Intenta conectarse con el servidor
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Si se conecta publica un mensaje
      client.publish("placeholder_of_secret_information" , "RING AND OPEN CONNECTED (ESP8266-ABAJO)");
      // Y se subscribe a los topics relevantes a el
      client.subscribe("placeholder_of_secret_information");
      client.subscribe("placeholder_of_secret_information");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void onMessage(String pMessage){


  if(pMessage == "placeholder_of_secret_information"){
    ultrasonicListen = true;

    while(ultrasonicListen){
        client.loop();

        distancia = ultrasonic.Ranging(CM);
        Serial.println(distancia);

        if(distancia < 20){
          client.publish("placeholder_of_secret_information" , "placeholder_of_secret_information");
          Serial.println("MENSAJE PARA QUE ABRA ENVIADO");

          ultrasonicListen = false;
        }
    }
  }
}




/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {

  // Si no está conectado se intenta conectar
  if (!client.connected()) {
    delay(5000);
    reconnect();
  }
  
  // Se mantiene enciclado para seguir recibiendo datos
  client.loop();
 
}