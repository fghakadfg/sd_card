#include <SoftwareSerial.h>;
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <SdFat.h>

#include <SPI.h>
#include "buildTime.h"
#define R 10
#define G 9
#define B 8

/*

пины подключения mega:

mosi - 51
miso - 50
sck - 52
cs - 53

интерфейс SD adapter SPI
*/
#define CS_PIN 4

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
byte nuidPICC[4];


String gprsdata;


SdFat sd;






String url = "http://194.226.199.230:26102/api/device?deviceid=2&cardid="; // метод для подсчета очков команды

//String url = "http://194.226.199.230:26102/api/device/add2?cardId="; //отдельный метод для внесения карт в базу данных

String pingurl = "http://194.226.199.230:26102/api/device/ping?id=2"; // метод пинга устройства

SoftwareSerial gprsSerial(3, 2);


void setup(void)
{
  //pinMode(R, OUTPUT);  // используем Pin11 для вывода
  //pinMode(G, OUTPUT); // используем Pin12 для вывода
  //pinMode(B, OUTPUT); 
  pinMode(4, OUTPUT);
 // blueLight();

  Serial.begin(19200);
  gprsSerial.begin(19200);
  Serial.println("System start");

  sd.begin(4);
  Serial.println(sd.exists("DATA.TXT"));

 

      
  File myFile = sd.open("DATA.TXT");
  if (myFile) {
      // считываем все байты из файла и выводим их в COM-порт
      while (myFile.available()) {
          Serial.write(myFile.read());
      }
      // закрываем файл
      myFile.close();
  } else {
      // выводим ошибку если не удалось открыть файл
      Serial.println("error opening DATA.TXT");
  } 
      /*
      volume.init(card);
      Serial.println("Found FS");
      
      Serial.println(volume.fatType(), DEC);
      root.openRoot(volume);
      root.ls(LS_R | LS_DATE | LS_SIZE);
      Serial.println();
      Serial.println();
      Serial.println();
      Serial.println();
     */
      
      
  nfc.begin();
  
  //setupGPRS(); // инициализация GPRS
  
}

void loop()
{ 


  String res = readNFC();


  // в случае проблем заменить None на ""
  if(res != "None"){

    //redLight();
    Serial.println(res);
    saveData(res);

  }
  else {
    Serial.println("Bbb");
    //ShowResultData();
    //pingDevice();
  }
  //greenLight();

  
  //String res = "bbsasd";
  //pingDevice();
  /*
  String res = readNFC();

  
  
  if(res != "None"){

   // redLight();
    
    Serial.println(res);
    saveData(res);
    //sendData(res);

  }
  else {
    Serial.println("No data yet");
  //  pingDevice();
  }
  */
 // Serial.println("abc");
}


void saveData(String id){
 
  
  File myFile = sd.open("DATA.TXT", FILE_WRITE);
    if (myFile) {
        Serial.println("Файл data.txt найден");
        String res = "";

        res.concat(id);

        res.concat(" ");
        res.concat(BUILD_YEAR);
        res.concat(" ");
        res.concat(BUILD_MONTH);
        res.concat(" ");
        res.concat(BUILD_DAY);
        res.concat(" ");
        res.concat(BUILD_HOUR);
        res.concat(" ");
        res.concat(BUILD_MIN);
        res.concat(" ");
        res.concat(BUILD_SEC);

          myFile.println(res);
           myFile.close();
        }
        // закрываем файл
       
     else {
        // выводим ошибку если не удалось открыть файл
        Serial.println("error opening DATA.TXT");
    } 

  
 
}


String readNFC() {
       if (nfc.tagPresent()){
                String tagId;
                String newId;
                NfcTag tag = nfc.read();
                tag.print();
               
                tagId = tag.getUidString();
                
                if (tagId != ""){
                
                  for (int i=0; i<=tagId.length()-1; i++)
                  {
                      if (tagId[i] != ' ')
                      newId += tagId[i];
                  }
                  return newId;
                }
                
        }
       return "None";
       delay(10);
}



void setupGPRS() {
  
  gprsSerial.println("AT+CBC");//проверяем электричество
  delay(800);// ждем ответ от модуля
  Serial.println("проверка электрички");
  ShowSerialData();
  delay(500);// ждем ответ от модуля
  Serial.println("если меньше 0,75,4005 - то питания не хватает, страшна, вырубай");
  delay(500);// ждем ответ от модуля

  gprsSerial.println("AT");//пишем в серийный порт
  delay(500);// ждем ответ от модуля
  ShowSerialData();
  delay(500);// ждем ответ от модуля

  gprsSerial.println("AT+CPIN?");//проверяем статус SIM карты
  delay(800);// ждем ответ от модуля
  Serial.println("проверяем статус SIM карты");
  ShowSerialData();
  delay(800);// ждем ответ от модуля

  gprsSerial.println("AT+CSQ");//проверяем статус SIM карты
  delay(800);// ждем ответ от модуля
  Serial.println("проверяем статус сигнала");
  ShowSerialData();
  delay(800);

  gprsSerial.println("AT+COPS=?");
  Serial.println("Принудительный запрос на поиск сети...");
  delay(20000);// ждем ответ от модуля
  ShowSerialData();

  gprsSerial.println("AT+COPS?");
  delay(800);// ждем ответ от модуля
  Serial.println("Запрос информации об операторе");
  ShowSerialData();

  gprsSerial.println("AT+SAPBR=1,1");
  gprsSerial.println("AT+SAPBR=2,1");
  delay(1000);
  ShowSerialData();

  Serial.println("GPRS initialized");
}




void setupHTTP() // функция для настройки HTTP соединения
{
  
  Serial.println("Setup HTTP");
  ShowSerialData();
  gprsSerial.println("AT+HTTPINIT"); //инициализация
  delay(300);
  ShowSerialData();
  gprsSerial.println("AT+HTTPPARA=\"CID\",1"); // настройка http соединения
  delay(300);
  ShowSerialData();
  
}



void pingDevice() {

  gprsSerial.println("AT+HTTPINIT"); //инициализация
  delay(200);
  ShowResultData();
  
  gprsSerial.println("AT+HTTPPARA=\"URL\",\"" + pingurl + "\""); // соединяет устройство по IP с сервером, не отключать
  delay(200);//
  ShowSerialData();
  
  gprsSerial.println("AT+HTTPACTION=0"); // выводит код ответа с сервера, не отключать
  delay(200);//если ставить меньше не будет успевать получить
  ShowSerialData();

}



void sendData(String data_string) {

  String str = data_string;

  setupHTTP();

  gprsSerial.println("AT+HTTPPARA=\"URL\",\"" + url + str + "\""); // соединяет устройство по IP с сервером, не отключать
  delay(300);//
  ShowSerialData();
  
  gprsSerial.println("AT+HTTPACTION=0"); // выводит код ответа с сервера, не отключать
  delay(300);//если ставить меньше не будет успевать получить
  ShowSerialData();

  gprsSerial.println("AT+HTTPREAD"); // если отключить ломается питание почему-то
  delay(300);//
  ShowSerialData();
  
  gprsSerial.println("AT+HTTPTERM"); //если отключить не закрывается сессия HTTP
  delay(1500);//
  ShowSerialData();
  
  blinkLight();
  
}

void redLight()
{
  digitalWrite(R, HIGH);
  digitalWrite(G, LOW);
  digitalWrite(B, LOW);
}

void blueLight()
{
  digitalWrite(R, LOW);
  digitalWrite(G, LOW);
  digitalWrite(B, HIGH);
}

void greenLight()
{
  digitalWrite(R, LOW);
  digitalWrite(G, HIGH);
  digitalWrite(B, LOW);
}

void blinkLight()
{
  digitalWrite(R, LOW);
  digitalWrite(G, LOW);
  digitalWrite(B, LOW);

  for(int i=0; i<4; i++){
    digitalWrite(G, HIGH);
    delay(100);
    digitalWrite(G, LOW);
    delay(100);
  }
}

void blinkRLight()
{
  digitalWrite(R, LOW);
  digitalWrite(G, LOW);
  digitalWrite(B, LOW);

  for(int i=0; i<4; i++){
    digitalWrite(R, HIGH);
    delay(100);
    digitalWrite(R, LOW);
    delay(100);
  }
}


void ShowSerialData()//цикл обработки и показа передачи данных на сервер
{
  String v;
  
  while (gprsSerial.available()) {
    char c = gprsSerial.read();
    
    v += c;
  }
  
  Serial.println(v);  

}



void ShowResultData( )//цикл обработки и показа передачи данных на сервер
{
  String v;
  
  while (gprsSerial.available()) {
    char c = gprsSerial.read();
    
    v += c;
    
  }
  if(v.indexOf("200") != -1){
    Serial.println("Статус код 200!!!");
    blinkLight();
  }
  else if(v.indexOf("401") != -1){
    Serial.println("Статус код 401!!!");
    blinkRLight();
  }
  Serial.println(v);
  
}

