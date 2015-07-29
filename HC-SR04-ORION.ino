#include <WiFi.h> 
// Uso del Sensor Ultrasónico HC-SR04
// Declaro variables para el manejo de los pines Trigger y Echo del sensor
// Conectar el USB pero sin el pin de energía conectado, cargar el programa, ejecutarlo y luego conectar el pin de energía
 
int trigger = 8;
int echo = 9;
long distancia;
long tiempo;

// Wifi Settings
char ssid[] = "SSID"; //  your network SSID (name) 
char pass[] = "password";    // your network password (use for WPA, or use as key for WEP) 
WiFiClient client;
int status = WL_IDLE_STATUS;
// Orion IP
IPAddress server(10,38,15,96);

void setup() {
  Serial.begin(9600); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }   
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    // don't continue:
    while(true);
  }  
  
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid, pass);
  
    // wait 10 seconds for connection:
    delay(10000);
  }  
 
  // Configuro los pines; trigger de salida y echo de entrada 
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  Serial.begin(9600);    // inicializo el puerto USB a 9600 baudios
}
 
void loop() {
  // Enviamos el pulso inicial al Sensor a través del pin Trigger
  digitalWrite(trigger,LOW); // apago el pin
  delayMicroseconds(5);      // estabilizo
  digitalWrite(trigger,HIGH);
  delayMicroseconds(3);      //se envía el pulso de 10 micro segundos
 
  // debo esperar un tiempo para que la ráfaga rebote y vuelva
 
  tiempo = pulseIn(echo,HIGH); //pendemos el pin echo
  distancia = int(tiempo / 58); //calculo distancia en cm
                                //se trasforma el cálculo a entero
  String valor = String(distancia);                                
  Serial.print(distancia);
  Serial.println(" cm");
  delay(1000);


  printWifiStatus();
  
  // if you get a connection, report back via serial:
  if (client.connect(server, 1026)) {
    Serial.println("\nConnected...");
    //String PostData = "";
    String PostData = "{ \"value\": " + valor + "}";
    Serial.println(PostData);
    Serial.println(sizeof(PostData));    
    
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("PUT /v1/contextEntities/type/distancia/id/HC-SR04/attributes/distancia HTTP/1.1");
    client.println("Host: 192.168.1.95:1026");
    client.println("User-Agent: Arduino/1.0");
    client.println("Accept: application/json");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(sizeof(PostData));
    
    Serial.print("Content-Length: ");
    Serial.println(sizeof(PostData));    
    client.println();
    client.println(PostData);  

    //client.println("Connection: close");    
    
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    } 

    //client.println();
    delay(3000);
    client.stop();
  }  
  
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}