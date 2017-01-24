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

/* Notes for the buzzer */

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
 
#define melodyPin 4

/* Bibliotecas para wifi y mqtt */
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/* Biblioteca para el sensor ultrasonico */
#include <Ultrasonic.h>

/* Valores de las redes inalámbrica y del servidor. */
const char* ssid = "IoT-XYZ";
const char* password = "imagineRocks";
const char* mqtt_server = "192.168.1.98";

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

  client.setServer(mqtt_server, 1883); //Función para conectarse con el servidor MQTT (direccion , puerto)
  client.setCallback(callback); //Función para escuchar publicaciones del servidor


  pinMode(4, OUTPUT);//buzzer
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
      client.publish("imaginexyz/iotserver/status" , "RING AND OPEN CONNECTED (ESP8266-ABAJO)");
      // Y se subscribe a los topics relevantes a el
      client.subscribe("imaginexyz/iotserver/dwarf/entrance_status");
      client.subscribe("imaginexyz/iotserver/dwarf/ring");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void onMessage(String pMessage){


  if(pMessage == "100"){
    ultrasonicListen = true;

    while(ultrasonicListen){
        client.loop();

        distancia = ultrasonic.Ranging(CM);
        Serial.println(distancia);

        if(distancia < 20){
          client.publish("imaginexyz/iotserver/dwarf/entrance_execute" , "111");
          Serial.println("MENSAJE PARA QUE ABRA ENVIADO");

          ultrasonicListen = false;
        }
    }
  }
}




/*##------------------------FUNCIONES PARA EL BUZZER------------------------##*/

/* Mario main theme melody */
int melody[] = {
  NOTE_E7, NOTE_E7, 1, NOTE_E7,
  1, NOTE_C7, NOTE_E7, 1,
  NOTE_G7, 1, 1,  1,
  NOTE_G6, 1, 1, 1,
 
  NOTE_C7, 1, 1, NOTE_G6,
  1, 1, NOTE_E6, 1,
  1, NOTE_A6, 1, NOTE_B6,
  1, NOTE_AS6, NOTE_A6, 1,
 
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 1, NOTE_F7, NOTE_G7,
  1, NOTE_E7, 1, NOTE_C7,
  NOTE_D7, NOTE_B6, 1, 1,
 
  NOTE_C7, 1, 1, NOTE_G6,
  1, 1, NOTE_E6, 1,
  1, NOTE_A6, 1, NOTE_B6,
  1, NOTE_AS6, NOTE_A6, 1,
 
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 1, NOTE_F7, NOTE_G7,
  1, NOTE_E7, 1, NOTE_C7,
  NOTE_D7, NOTE_B6, 1, 1
};

/* Mario main theme tempo */
int tempo[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};

/* Underwolrd melody */
int underworld_melody[] = {
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 1,
  1,
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 1,
  1,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 1,
  1,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 1,
  1, NOTE_DS4, NOTE_CS4, NOTE_D4,
  NOTE_CS4, NOTE_DS4,
  NOTE_DS4, NOTE_GS3,
  NOTE_G3, NOTE_CS4,
  NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
  NOTE_GS4, NOTE_DS4, NOTE_B3,
  NOTE_AS3, NOTE_A3, NOTE_GS3,
  1, 1, 1
};

/* Underwolrd tempo */
int underworld_tempo[] = {
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  6, 18, 18, 18,
  6, 6,
  6, 6,
  6, 6,
  18, 18, 18, 18, 18, 18,
  10, 10, 10,
  10, 10, 10,
  3, 3, 3
};

int song = 0;
 
void sing(int s) {
  // iterate over the notes of the melody:
  song = s;
  if (song == 2) {
    Serial.println(" 'Underworld Theme'");
    int size = sizeof(underworld_melody) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++) {
 
      // to calculate the note duration, take one second
      // divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / underworld_tempo[thisNote];
 
      buzz(melodyPin, underworld_melody[thisNote], noteDuration);
 
      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
 
      // stop the tone playing:
      buzz(melodyPin, 1, noteDuration);
 
    }
 
  } else {
 
    Serial.println(" 'Mario Theme'");
    int size = sizeof(melody) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++) {
 
      // to calculate the note duration, take one second
      // divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / tempo[thisNote];
 
      buzz(melodyPin, melody[thisNote], noteDuration);
 
      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
 
      // stop the tone playing:
      buzz(melodyPin, 1, noteDuration);
 
    }
  }
}
 
void buzz(int targetPin, long frequency, long length) {
  long delayValue = 1000000 / frequency / 2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  long numCycles = frequency * length / 1000; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to
  //// get the total number of cycles to produce
  for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
    digitalWrite(targetPin, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin, LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait again or the calculated delay value
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