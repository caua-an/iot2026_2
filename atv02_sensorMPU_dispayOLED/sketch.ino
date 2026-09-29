#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// define dos pinos I2c (sensor e o display)
#define I2C_SDA 8
#define I2C_SCL 9

// define dos pinos do SD
#define SPI_CS 10
#define SPI_DI 11
#define SPI_SCK 12
#define SPI_DO 13

// config do display 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// instancia do mpu
Adafruit_MPU6050 mpu;

unsigned long tempoUltTela = 0;
unsigned long tempoUltSD = 0;



void setup() {
  Serial.begin(115200);
  while(!Serial) delay(10);
  Serial.println("Sistema IOT iniciado");

  Wire.begin(I2C_SDA, I2C_SCL);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    Serial.println("Falha ao inicializar o display");
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Iniciando sensores...");
  display.display();


  // teste para iniciar o sensor mpu
  if(!mpu.begin()){
    Serial.println("Falha ao rodar o MPU6050");
  } else {
    Serial.println("Sensor MPU6050 inicializado com sucesso");
  }

  // inicializar o sd card
  SPI.begin(SPI_SCK, SPI_DO, SPI_DI, SPI_CS);

  if(!SD.begin(SPI_CS, SPI)){
    Serial.println("Erro ao inicializar o cartao SD");
  } else {
    Serial.println("Sucesso ao inicializar o cartao SD");
  }

}

void loop() {

  unsigned long tempoAtual = millis();
  // struct para guardar os eventos do sensor mpu
  sensors_event_t a, g, temp;

  // bloco para leitura do MPU6050 e exibicao no display a cad 500ms
  if(tempoAtual - tempoUltTela >= 500){
    tempoUltTela = tempoAtual;


    if (mpu.getEvent(&a, &g, &temp)){
      Serial.println("Leitura de sensor atualizada com sucesso");

      display.clearDisplay();
      display.setCursor(0,0);
      display.println("Aceleracao m/s²:");
      display.print("x: "); display.println(a.acceleration.x);
      display.print("y: "); display.println(a.acceleration.y);
      display.print("z: "); display.println(a.acceleration.z);
      display.display();
    } else{
      Serial.println("Falha na leitura do MPU6050");
    }

  }
  // bloco para gravacao dos dados do sensor no cartao SD
  if(tempoAtual - tempoUltSD >= 1000){
    tempoUltSD = tempoAtual;

    mpu.getEvent(&a, &g, &temp);

    // gravar os logs no SD
    File arq_dados = SD.open("/log_iot.txt", FILE_APPEND);

    if(arq_dados){
      arq_dados.print("Tempo(ms): "); arq_dados.print(tempoAtual);
      arq_dados.print("Eixo X: "); arq_dados.print(a.acceleration.x);
      arq_dados.print("Eixo Y: "); arq_dados.print(a.acceleration.y);
      arq_dados.print("Eixo Z: "); arq_dados.println(a.acceleration.z);
      arq_dados.close(); 
      Serial.println("Sucesso em gravar os dados no cartao SD");
    } else {
      Serial.println("Falha ao tentar gravar os dados no cartao SD");
    }
  }

}
