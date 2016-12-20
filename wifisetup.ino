#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <EEPROM.h>

//servidor http para o websetup
ESP8266WebServer httpServer;

//nome do access point para setup
const char * apSSID = "Coisa.01";

//ssid e senha à ser carregada da EEPROM
String ssid = "";
String senha = "";

//página html do setup
String html;

void setup() {

  Serial.begin(115200);
  EEPROM.begin(512);
  delay(10);
  
  Serial.println("\n\n\n\n");
  
  //Lê a EEPROM para a ssid
  for(int i=0;i<32;i++){
    ssid += char(EEPROM.read(i));
  }
  Serial.print("SSID Salva:");
  Serial.println(ssid);
  //Lê a EEPROM para a senha
  for(int i=32;i<96; i++){
    senha += char(EEPROM.read(i));
  }
  Serial.print("Senha salva:");
  Serial.println(senha);
  
  //Conecta e verifica o statusdo Wif=Fi
  verificarWifi();
  //Prossegue caso o wifi conecte
  //Seu setup normal vai aqui :)
  Serial.println("Conectado!");
  Serial.print("IP: ");
  Serial.print(WiFi.localIP());
  //Desliga o AP, fazendo o a rede do setup ser desativada
  WiFi.softAPdisconnect(true);
}

//Método para verificar e conectar o WiFi
void verificarWifi(){
  Serial.print("\nVerificando WiFi:");
    //Inicia o Wifi com SSID e a senha da EEPROM
     WiFi.begin(ssid.c_str(),senha.c_str());
    //loop de 100 vezes, para esperar a conexão
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
    Serial.print("Tempo esgotado");
    //chama método para definir senha 
    definirWifi();
  
  
}

//Método para definir as configurações do WiFi
void definirWifi(){
  Serial.print("\nIniciando setup\n");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  //Pesquisa redes
  int redes = WiFi.scanNetworks();
  Serial.print("Redes disponiveis: ");
  Serial.println(redes);
  //cria uma tag <option> com as redes e o sinal para o form de setup
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
  //cria a rede wifi
  WiFi.softAP(apSSID);
  Serial.print("Modo AP ativado, IP: ");
  Serial.println(WiFi.softAPIP());
  bool setup = true;
  //html da página inicial
  html = "<html><head><title>Configuracao</title></head><body>";
  html+= "<form method='get' action='setup'><label>SSID: </label>";
  html+= htmlLista;
  html+="</br><label>Senha: </label><input name='senha' length=64 type='text' /></br>";
  html+="<input type='submit' value='Definir senha'/>";
  html+="</form></body></html>";
  //define o html para a página de setup
  httpServer.on("/",[](){
    Serial.println("Página de configuração");
    httpServer.send(200,"text/html",html);
  });
  //recebe os dados do setup
  httpServer.on("/setup",[](){
    String setupSSID = httpServer.arg("ssid");
    String setupSenha = httpServer.arg("senha");
    Serial.print("\nSSID e Senha definidos\nSSID: ");
    Serial.println(setupSSID);
    Serial.print("Senha: \n");
    Serial.println(setupSenha);
    //limpa a EEPROM
    for(int i=0;i<96;i++){
      EEPROM.write(i, 0);
    }
    //Escreve o SSID
    for(int i =0;i< setupSSID.length();i++){
      EEPROM.write(i,setupSSID[i]);
    }
    
    //Escreve a senha
    for(int i=0;i<setupSenha.length();i++){
      EEPROM.write(i+32,setupSenha[i]);
    }
    
    //Salva dados na EEPROM
    EEPROM.commit();
    Serial.println("Dados Salvos com sucesso!");
    //Reseta o ESP
    ESP.reset();
  });
  //inicia o servidor http
  httpServer.begin();
  //loop 
  while(setup){
    httpServer.handleClient();
  }
  
}



void loop() {
  
  // put your main code here, to run repeatedly:

}
