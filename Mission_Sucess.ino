#include <SoftwareSerial.h>//Library esp8266
#include <DHT.h> //Library DHT11

//inisialisasi pin digital
#define RX 2 //pin arduino D2 sebagai RX
#define TX 3 //pin arduino D3 sebagai TX

String AP = "********"; //pada string AP silahkan isikan nama WiFi yang adakan digunakan
String PASS = "********"; //pada string PASS silahkan isikan password WiFi
String API = "CD9A8TNHK4MTVII6";   //Api key yang disediakan oleh thingspeak
String HOST = "api.thingspeak.com"; //Website Tingspeak
String PORT = "80"; //PORT thingspeak

//inisialisasi pengiriman data
String field1 = "field1";  //alamat pengiriman data Suhu DH11
String field2 = "field2"; //alamat pengiriman data kelembaban DH11
String field3 = "field3"; //alamat pengiriman data getaran
String field4 = "field4"; //alamat pengiriman data magnetik
String field5 = "field5"; //alamat pengiriman data MMA 7361 (accelerometer)


int countTrueCommand;
int countTimeCommand; 
boolean found = false; 

//deklarasi variabel suatu nilai
int valSensor1 = 1;
int valSensor2 = 1;
int valSensor3 = 1;
int valSensor4 = 1;
int valSensor5 = 1;

SoftwareSerial esp8266(RX,TX); //pemanggilan pin RX dan TX

#define getar 4  //input sensor getaran
#define magnet 5 //imput sensor magnet
#define DHTPIN 6 //input sensor DHT
#define DHTTYPE DHT11 //jenis tipe DHT yang digunakan
DHT dht(DHTPIN, DHTTYPE); //pemanggilan dht (pin 6,tipe DHT)

//MMA7361
const int turu = 7; // sleep
const int sense_select = 8; // gs

const int numbaca = 8;
int bacaX[numbaca], bacaY[numbaca], bacaZ[numbaca];
int j = 0;
int totalX = 0, totalY = 0, totalZ = 0;
int ax = 0, ay = 0, az = 0;
int i = 1;
int MMI;
float vx, vy, vz, gx, gy, gz, jumlahX, jumlahY, jumlahZ, meanX, 
meanY, meanZ, PGA, R, g;

void setup() 
{
  Serial.begin(9600);     //9600 merupakan alamat serial untuk Arduino Nano
  esp8266.begin(115200);  //115200 merupakan alamat serial untuk ESP8266-01
  
  sendCommand("AT",5,"OK"); //printah AT untuk komunikasi arduino dengan ESP8266
  sendCommand("AT+CWMODE=1",5,"OK"); //printah AT+CWMODE = 1 pada ESP berfungsi sebagai penerima WiFi
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK"); //AT+CWJAP = "AP","PASS" agar modul ESP terhubung dengan jaringan WiFi yang ada
  //Jika ingin mengetahu jaringan WiFi yang dapat di deteksi oleh ESP
  //cukup dengan memberikan komentar AT+CWLAP

  //penggunaan pin digital arduino sebagai INPUTAN
  pinMode (getar, INPUT);
  pinMode (magnet, INPUT);

  //MMA
  for(int thisReading = 0; thisReading<numbaca; thisReading++)
  {
  bacaX[thisReading] = 0;
  bacaY[thisReading] = 0;
  bacaZ[thisReading] = 0;
  }
  // Atur mode pin yang digunakan
  pinMode(turu,OUTPUT);
  pinMode(sense_select,OUTPUT);
  digitalWrite(turu,HIGH);
  digitalWrite(sense_select,LOW);
}

void loop() 
{
  //program upload setiap data
  valSensor1 = getSensorData1(); //pengiriman suhu DHT
  valSensor2 = getSensorData2(); //pengiriman kembaban DHT
  valSensor3 = getSensorData3(); //pengiriman sensor getaran
  valSensor4 = getSensorData4(); //pengiriman sensor magnetik
  valSensor5 = getSensorData5(); //pengiriman sensor accelerometer
  
  //perintah pengiriman ke akun website thingspeak
  String getData = "GET /update?api_key="+ API +
  "&"+ field1 +"="+String(valSensor1)+
  "&"+ field2 +"="+String(valSensor2)+
  "&"+ field3 +"="+String(valSensor3)+
  "&"+ field4 +"="+String(valSensor4)+
  "&"+ field5 +"="+String(valSensor5);
   
  sendCommand("AT+CIPMUX=1",5,"OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK"); //AT+CIPSTAR = 0 merupakan target data yang akan dikirimkan ke website Thingspeak
  sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">"); //AT+CIPSEND = 0 merupakan perintah pengiriman data pada website Thingseak
  esp8266.println(getData);
  delay(1500);
  countTrueCommand++;
  sendCommand("AT+CIPCLOSE=0",5,"OK"); 
}
//suhu DHT11
int getSensorData1()
{
  // Read sensor values
   float t = dht.readTemperature();
   //float h = dht.readHumidity();
   Serial.print(t);
   return (t);
}
//kelembaban DHT11
int getSensorData2()
{
   // Read sensor values
   //float t = dht.readTemperature();
   float h = dht.readHumidity();
   Serial.print(h); 
   return (h);
}
//tranduser Getaran
int getSensorData3()
{
  int v = digitalRead(getar); // v = vibration (getaran)
  Serial.print(v);
  return (v);
}
//tranduser magnetik
int getSensorData4()
{
  int m = digitalRead(magnet); // m = magnet
  Serial.print(m);
  return (m);
}

int getSensorData5()
{
 /* totalX = totalX - bacaX[j];
totalY = totalY - bacaY[j];
totalZ = totalZ - bacaZ[j];

bacaX[j] = analogRead(A0);
bacaY[j] = analogRead(A1);
bacaZ[j] = analogRead(A2);

totalX = totalX + bacaX[j];
totalY = totalY + bacaY[j];
totalZ = totalZ + bacaZ[j];

j = j+1;
if(j>=numbaca){
  j = 0;
}

ax = totalX/numbaca;
ay = totalY/numbaca;
az = totalZ/numbaca;
*/

ax = analogRead(A0);
ay = analogRead(A1);
az = analogRead(A2);

vx = (float)ax*5/1023;
vy = (float)ay*5/1023;
vz = (float)az*5/1023;

i = i+1;
if(i<=100){  
  jumlahX += vx;
  meanX = jumlahX/i;
  jumlahY += vy;
  meanY = jumlahY/i;
  jumlahZ += vz;
  meanZ = jumlahZ/i;
}
else if(i>100){
  i = 1;
  jumlahX = 0;
  jumlahY = 0;
  jumlahZ = 0;
  
  jumlahX += vx;
  meanX = jumlahX/i;
  jumlahY += vy;
  meanY = jumlahY/i;
  jumlahZ += vz;
  meanZ = jumlahZ/i;
}

gx = (vx-meanX)/0.8;
gy = (vy-meanY)/0.8;
gz = (vz-meanZ)/0.8;

Serial.print(gx,4);
Serial.print("\t");
Serial.print(gy,4);
Serial.print("\t");
Serial.print(gz,4);
Serial.print("\t");

R = sqrt((gx*gx)+(gy*gy)+(gz*gz));//vektor
g = abs(R);//mutlak
PGA = g*981;
if(PGA>= 0.833350281020331){
  MMI = 0.287+3.625*log10(PGA);
}
else{
  MMI = 0;
}
Serial.println(MMI);
delay(10);
  return (MMI);
}

void sendCommand(String command, int maxTime, char readReplay[]) 
{
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("BERHASIL");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("GAGAL");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
}
