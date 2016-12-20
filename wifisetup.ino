#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <EEPROM.h>

ESP8266WebServer httpServer;

const char * apSSID = "Coisa.01";
String ssid = "";
String senha = "";
String html;

void setup() {

  Serial.begin(115200);
  EEPROM.begin(512);
  delay(10);
  Serial.println("\n\n\n\n");
  for(int i=0;i<32;i++){
    ssid += char(EEPROM.read(i));
  }
  Serial.print("SSID Salva:");
  Serial.println(ssid);
  for(int i=32;i<96; i++){
    senha += char(EEPROM.read(i));
  }
  Serial.print("Senha salva:");
  Serial.println(senha);
  verificarWifi();
  Serial.println("Conectado!");
  Serial.print("IP: ");
  Serial.print(WiFi.localIP());
  WiFi.softAPdisconnect(true);
}

void verificarWifi(){
  Serial.print("\nVerificando WiFi:");
  if(ssid.length() > 1){
     WiFi.begin(ssid.c_str(),senha.c_str());
     int c = 0;
     while(c < 100){
      if(WiFi.status() != WL_CONNECTED){
        Serial.print("|");
        delay(100);
        c ++;
      }
      else{
        return;
      }
     }
     Serial.println();
     if(WiFi.status() != WL_CONNECTED){
      Serial.print("Tempo esgotado");
      definirWifi();
     }
     else{
      return;
     }
  }
  
}

void definirWifi(){
  Serial.print("\nIniciando setup\n");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int redes = WiFi.scanNetworks();
  Serial.print("Redes disponiveis: ");
  Serial.println(redes);
  String htmlLista = "<select name='ssid'>";
  for(int i = 0;i<redes;i++){
    String scanSSID = WiFi.SSID(i);
    int scanRSSI = WiFi.RSSI(i);
    Serial.print("Rede: ");
    Serial.println(scanSSID);
    Serial.print("Sinal: ");
    Serial.println(scanRSSI);
    htmlLista +="<option value='"+scanSSID+"'>"+scanSSID + " : "+scanRSSI+"</option>";
  }
  htmlLista+="</select>";
  Serial.println("Fim da busca");
  delay(100);
  WiFi.softAP(apSSID);
  Serial.print("Modo AP ativado, IP: ");
  Serial.println(WiFi.softAPIP());
  bool setup = true;
  html = "<html><head><title>Configuracao</title></head><body>";
  html+= "<form method='get' action='setup'><label>SSID: </label>";
  html+= htmlLista;
  html+="</br><label>Senha: </label><input name='senha' length=64 type='text' /></br>";
  html+="<input type='submit' value='Definir senha'/>";
  html+="</form></body></html>";
  httpServer.on("/",[](){
    Serial.println("Página de configuração");
    httpServer.send(200,"text/html",html);
  });
  httpServer.on("/setup",[](){
    String setupSSID = httpServer.arg("ssid");
    String setupSenha = httpServer.arg("senha");
    Serial.print("\nSSID e Senha definidos\nSSID: ");
    Serial.print(setupSSID);
    Serial.print("Senha: \n");
    Serial.println(setupSenha);
    for(int i=0;i<96;i++){
      EEPROM.write(i, 0);
    }
    for(int i =0;i< setupSSID.length();i++){
      EEPROM.write(i,setupSSID[i]);
    }
    
    for(int i=0;i<setupSenha.length();i++){
      EEPROM.write(i+32,setupSenha[i]);
    }
    EEPROM.commit();
    Serial.println("Dados Salvos com sucesso!");
    ESP.reset();
  });
  httpServer.begin();
  while(setup){
    httpServer.handleClient();
  }
  
}



void loop() {
  
  // put your main code here, to run repeatedly:

}
