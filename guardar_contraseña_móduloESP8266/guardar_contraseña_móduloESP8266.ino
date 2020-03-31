#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

String pral = "<html>"
"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'/>"
"<title>WIFI CONFIG</title> <style type='text/css'> body,td,th { color: #036; } body { background-color: #999; } </style> </head>"
"<body> "
"<h1>WIFI CONF</h1><br>"
"<form action='config' method='get' target='pantalla'>"
"<fieldset align='left' style='border-style:solid; border-color:#336666; width:200px; height:180px; padding:10px; margin: 5px;'>"
"<legend><strong>Configurar WI-FI</strong></legend>"
"SSID: <br> <input name='ssid' type='text' size='15'/> <br><br>"
"PASSWORD: <br> <input name='pass' type='password' size='15'/> <br><br>"
"<input type='submit' value='Comprobar conexion' />"
"</fieldset>"
"</form>"
"<iframe id='pantalla' name='pantalla' src='' width=900px height=400px frameborder='0' scrolling='no'></iframe>"
"</body>"
"</html>";

ESP8266WebServer server(80);

char ssid[20];
char pass[20];
String ssid_leido;
String pass_leido;
int ssid_tamano = 0;
int pass_tamano = 0;

String arregla_simbolos(String a) {
 a.replace("%C3%A1", "á");
 a.replace("%C3%A9", "é");
 a.replace("%C3%A", "i");
 a.replace("%C3%B3", "ó");
 a.replace("%C3%BA", "ú");
 a.replace("%21", "!");
 a.replace("%23", "#");
 a.replace("%24", "$");
 a.replace("%25", "%");
 a.replace("%26", "&");
 a.replace("%27", "/");
 a.replace("%28", "(");
 a.replace("%29", ")");
 a.replace("%3D", "=");
 a.replace("%3F", "?");
 a.replace("%27", "'");
 a.replace("%C2%BF", "¿");
 a.replace("%C2%A1", "¡");
 a.replace("%C3%B1", "ñ");
 a.replace("%C3%91", "Ñ");
 a.replace("+", " ");
 a.replace("%2B", "+");
 a.replace("%22", "\"");
 return a;
}

//**** CONFIGURACION WIFI  *******
void wifi_conf() {
 int cuenta = 0;

 String getssid = server.arg("ssid"); //Recibimos los valores que envia por GET el formulario web
 String getpass = server.arg("pass");

 getssid = arregla_simbolos(getssid); //Reemplazamos los simbolos que aparecen cun UTF8 por el simbolo correcto
 getpass = arregla_simbolos(getpass);

 ssid_tamano = getssid.length() + 1;  //Calculamos la cantidad de caracteres que tiene el ssid y la clave
 pass_tamano = getpass.length() + 1;

 getssid.toCharArray(ssid, ssid_tamano); //Transformamos el string en un char array ya que es lo que nos pide WIFI.begin()
 getpass.toCharArray(pass, pass_tamano);

 Serial.println(ssid);     //para depuracion
 Serial.println(pass);

 WiFi.begin(ssid, pass);     //Intentamos conectar
 while (WiFi.status() != WL_CONNECTED)
 {
  delay(500);
  Serial.print(".");
  cuenta++;
  if (cuenta > 20) {
   graba(70, "noconfigurado");
   server.send(200, "text/html", String("<h2>No se pudo realizar la conexion<br>no se guardaron los datos.</h2>"));
   return;
  }
 }
 Serial.print(WiFi.localIP());
 graba(70, "configurado");
 graba(1, getssid);
 graba(30, getpass);
 server.send(200, "text/html", String("<h2>Conexion exitosa a: "
  + getssid + "<br> El pass ingresado es: " + getpass + "<br>Datos correctamente guardados."));

}


//*******  G R A B A R  EN LA  E E P R O M  ***********
void graba(int addr, String a) {
 int tamano = (a.length() + 1);
 Serial.print(tamano);
 char inchar[30];    //'30' Tamaño maximo del string
 a.toCharArray(inchar, tamano);
 EEPROM.write(addr, tamano);
 for (int i = 0; i < tamano; i++) {
  addr++;
  EEPROM.write(addr, inchar[i]);
 }
 EEPROM.commit();
}

//*******  L E E R   EN LA  E E P R O M    **************
String lee(int addr) {
 String nuevoString;
 int valor;
 int tamano = EEPROM.read(addr);
 for (int i = 0;i < tamano; i++) {
  addr++;
  valor = EEPROM.read(addr);
  nuevoString += (char)valor;
 }
 return nuevoString;
}

//*********  INTENTO DE CONEXION   *********************
void intento_conexion() {
 if (lee(70).equals("configurado")) {
  ssid_leido = lee(1);      //leemos ssid y password
  pass_leido = lee(30);

  Serial.println(ssid_leido);  //Para depuracion
  Serial.println(pass_leido);

  ssid_tamano = ssid_leido.length() + 1;  //Calculamos la cantidad de caracteres que tiene el ssid y la clave
  pass_tamano = pass_leido.length() + 1;

  ssid_leido.toCharArray(ssid, ssid_tamano); //Transf. el String en un char array ya que es lo que nos pide WiFi.begin()
  pass_leido.toCharArray(pass, pass_tamano);

  int cuenta = 0;
  WiFi.begin(ssid, pass);      //Intentamos conectar
  while (WiFi.status() != WL_CONNECTED) {
   delay(500);
   cuenta++;
   if (cuenta > 20) {
    Serial.println("Fallo al conectar");
    return;
   }
  }
 }
 if (WiFi.status() == WL_CONNECTED) {
  Serial.print("Conexion exitosa a: ");
  Serial.println(ssid);
  Serial.println(WiFi.localIP());
 }
}


//*****  S E T U P  **************
void setup() {
 Serial.begin(115200);
 EEPROM.begin(4096);
 WiFi.softAP("xxxxxxx");      //Nombre que se mostrara en las redes wifi

 server.on("/", []() {
  server.send(200, "text/html", pral);
 });
 server.on("/config", wifi_conf);
 server.begin();
 Serial.println("Webserver iniciado...");

 Serial.println(lee(70));
 Serial.println(lee(1));
 Serial.println(lee(30));
 intento_conexion();
}


//*****   L O O P   **************
void loop() {
 server.handleClient();
 delay(2000);
}﻿
