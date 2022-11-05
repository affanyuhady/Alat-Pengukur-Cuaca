#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal_I2C.h> 

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)

Adafruit_BMP280 bmp;

volatile byte half_revolutions; 
unsigned int rpmku; 
unsigned long timeold; 

unsigned int kalibrasi;

const byte buzzer = 6;
const byte ledr = 3;
const byte ledg = 4;
const byte ledb = 5;

int cahaya;
int rain;

char value;

void setup() 
{
  Serial.begin(9600);
  lcd.init();  
  lcd.backlight();
  lcd.clear();
  lcd.noCursor();

  lcd.setCursor(0, 0);
           //012345678901234567890
  lcd.print(F("    AFFAN YUHADY     "));
  lcd.setCursor(0, 1);
           //012345678901234567890
  lcd.print(F("   17101152620090    "));
  lcd.setCursor(0, 2);
           //012345678901234567890
  lcd.print(F(" ALAT PENGUKUR CUACA "));
  lcd.setCursor(0, 3);
           //012345678901234567890 
  lcd.print(F(" MENGGUNAKAN ANDROID "));
  delay(5000);
   
  attachInterrupt(0, rpm_fun, RISING); //mengambil sinyal high pada pin 2
  half_revolutions = 0; 
  rpmku = 0;
  timeold = 0;
  kalibrasi = 0;
  
  if (!bmp.begin()) 
  {
    while (1);
  }

  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
                  
  pinMode(buzzer, OUTPUT);
  pinMode(ledr, OUTPUT);
  pinMode(ledg, OUTPUT);
  pinMode(ledb, OUTPUT);
  digitalWrite(buzzer, LOW);
  digitalWrite(ledr, LOW);
  digitalWrite(ledg, LOW);
  digitalWrite(ledb, LOW);

  
}

void loop() 
{
  angin();
  baro();
  hari();
  hujan();

  String datakirim = String(kalibrasi) + "|" + String(bmp.readTemperature()) + "|" + String(bmp.readPressure()) + "|";
  
  Serial.print(datakirim);
  
  if (cahaya >= 700)
  {
    Serial.print(F("Gelap"));
    Serial.print("|");
  }
  else
  {
    Serial.print(F("Terang"));
    Serial.print("|");
  }

  if (rain <= 700)
  {
    Serial.print(F("Dan Hujan"));
    Serial.print("|");
    Serial.println(" ");
  }
  else
  {
    Serial.print(F("Dan Tidak Hujan"));
    Serial.print("|");
    Serial.println(" ");
  }
  delay(1000);
}

void rpm_fun()
{
   half_revolutions++; //counter interupt
}

void angin()
{
  
  rpmku = 30*1000/(millis() - timeold)*half_revolutions; //mengaktifkan counter millis
  timeold = millis(); //hasil counter dimasukkan ke variabel timeold
  half_revolutions = 0; //reset variabel

  kalibrasi = (rpmku - 150)/109;  //rumus kalibrasi
 
  if((kalibrasi > 590)&&(kalibrasi < 605)){
    kalibrasi = 0;
  }

  if(kalibrasi >= 20)
  {
    digitalWrite(buzzer, HIGH);
    delay(200);
    digitalWrite(buzzer, LOW);
    delay(500);
  }
  else
  {
   digitalWrite(buzzer, LOW); 
  }
  
  lcd.setCursor(0, 0);
  lcd.print(F("V= "));
  lcd.print(kalibrasi);
  lcd.print(F(" m/s "));
  //delay(250);
}

void baro()
{
    lcd.setCursor(10, 0);
    lcd.print(F("T= "));
    lcd.print(bmp.readTemperature());
    lcd.print(F("*C"));

    if (bmp.readTemperature() >= 34)
    {
      digitalWrite(ledr, HIGH);
    }
    else
    {
      digitalWrite(ledr, LOW);
    }

    lcd.setCursor(0, 1);
    lcd.print(F("Tek.atm="));
    lcd.print(bmp.readPressure());
    lcd.print(F(" Pa"));

    //delay(250);
}

void hari()
{
  cahaya = analogRead(A1);

  if (cahaya >= 700)
  {
    digitalWrite(ledg, HIGH);
    lcd.setCursor(0, 2);
             //012345678901234567890
    lcd.print(F("     Hari Gelap      "));
  }
  else
  {
    digitalWrite(ledg, LOW);    
    lcd.setCursor(0, 2);
             //012345678901234567890
    lcd.print(F("     Hari Terang     "));
  }

  //delay(250);

}

void hujan() 
{
  rain = analogRead(A0);

  if (rain <= 700)
  {
    digitalWrite(ledb, HIGH);
    lcd.setCursor(0, 3);
             //012345678901234567890
    lcd.print(F("     Dan Hujan      "));
  }
  else
  {
    digitalWrite(ledb, LOW);
    lcd.setCursor(0, 3);
             //012345678901234567890
    lcd.print(F("  Dan Tidak Hujan   "));
  }

  //delay(250);
}
