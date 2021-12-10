// Programma Lego Sorteer Machine (LSM)  project Mechatronisch ontwerpen en realiseren
// Mitchel Oosterkamp
// EMT2V.A  Groep 1

#include "mbed.h"
#include "TextLCD.h"
#include "Servo.h"
 #include "TCS3200.h"
 
// Host PC communicatie setup
Serial pc(USBTX, USBRX, 9600);   // Seriële port for communication with your PC
FileHandle *mbed::mbed_override_console(int fd){
    return &pc; }
// Declaratie PWM pinnen
PwmOut SP_Trans1 (D2);                  // Snelheid motor Transportband 1, range is beperkt tot 0~1
PwmOut SP_Trans2 (D3);                  // Snelheid motor Transportband 2, range is beperkt tot 0~1
PwmOut SP_Lift (D4);                    // Snelheid motor Lift, range is beperkt tot 0~1
PwmOut SP_Geleiding (D5);               // Snelheid motoren Geleiding, range is beperkt tot 0~1
// Declaratie digitale input pinnen
DigitalIn Voorraad1(PC_9);              // IR nabijheidsensor
DigitalIn Voorraad2(PC_8);              // IR nabijheidsensor
DigitalIn Voorraad3(D10);               // IR nabijheidsensor
DigitalIn Voorraad4(D11);               // IR nabijheidsensor
DigitalIn Voorraad5(D12);               // IR nabijheidsensor
DigitalIn Voorraad6(D13);               // IR nabijheidsensor 
DigitalIn Start_Knop(PB_2);             // Start knop groen (Eaton M22-DL-G)
DigitalIn Stop_Knop(PA_11);             // Stop knop rood (Eaton M22-DL-R)
DigitalIn Reset_Knop(PC_5);             // Reset knop blauw (Eaton M22-DL-B)
// Declaratie digitale output pinnen
DigitalOut Lamp_Start(PB_12);           // Lamp in Start knop   (Eaton M22-LEDC-G LED)
DigitalOut Lamp_Stop(PA_12);            // Lamp in Stop knop    (Eaton M22-LEDC-R LED)
DigitalOut Lamp_Reset(PC_6);            // Lamp in Reset knop   (Eaton M22-LEDC-B LED)
// Declaratie TCS230-TCS3200 RGB kleursensor
 TCS3200 color(PB_15, PB_14, PB_1);  //Create a TCS3200 object
 //              S2    S3    OUT
// Declaratie Servo output pinnen
Servo ServoRood(D6), ServoGroen(D7), ServoBlauw(D8), ServoGeel(D9);     // MG90S Servo motoren (SG90 met metalen gears)
// I2C communicatie pins
I2C i2c_lcd(PB_9,PB_8); // SDA, SCL         2004 LCD met blauw backlight 20x4 met PCF8574 I2C module
// LCD initialisatie
TextLCD_I2C lcd(&i2c_lcd, 0x4E, TextLCD::LCD20x4); // I2C exp: I2C bus, PCF8574 Slaveaddress, LCD Type

//==============================================================================
//                 Declaraties voor gebruik binnen programma
//==============================================================================
int Voorraad3LangRood = 0 ;
int Voorraad3LangGroen = 0 ;
int Voorraad3LangBlauw = 0 ;
int Voorraad3LangGeel = 0 ;
int Voorraad5Lang = 0 ;
int Voorraad7Lang = 0 ;
int Voorraad9Lang = 0 ;
int Voorraad11Lang = 0 ;
int Voorraad13Lang = 0 ;
int Voorraad15Lang = 0 ;

int T_LCD = 4000;           // Wait volgende scherm LCD in ms
int ServoDelay = 500;       // Duratie activering Servo in ms

Timer Kleur;                // Timer
int Rood [200] = {};        // Array activatie tijden Servo Rood
int R = 1;                  // Array Servo Rood locatie integer
int Blauw [200] = {};       // Array activatie tijden Servo Blauw
int B = 1;                  // Array Servo Blauw locatie integer
int Groen [200] = {};       // Array activatie tijden Servo Groen
int Gr = 1;                 // Array Servo Groen locatie integer
int Geel [200] = {};        // Array activatie tijden Servo Geel
int Ge = 1;                 // Array Servo Geel locatie integer

bool Start = true;
bool Stop = false;
bool MachineAan = false;
bool KleurError = false;

//==============================================================================
//                 Aparte thread voor aansturen LCD scherm
//==============================================================================
Thread thread;
void LCD_thread() {
    while (true) {
        lcd.printf("Lego Sorteer Machine  \n");                              // Naam project
        if (MachineAan == true){lcd.printf("\n Status:   Actief \n"); }      // Status machine: Actief (machine loopt) 
        else {lcd.printf("\n Status:  Inactief \n"); }
        ThisThread::sleep_for(T_LCD);                                       // Wacht x seconden met wisselen naar volgende scherm
        lcd.cls();                                                          // Clear huidige scherm
        lcd.printf("3 lang Rood: %d\n", Voorraad3LangRood);                 // print voorraad op LCD
        lcd.printf("3 lang Blauw: %d\n", Voorraad3LangBlauw);
        lcd.printf("3 lang Groen: %d\n", Voorraad3LangGroen);
        lcd.printf("3 lang Geel: %d\n", Voorraad3LangGeel);
        ThisThread::sleep_for(T_LCD);
        lcd.cls();
        lcd.printf("Blok 5 lang: %d\n", Voorraad5Lang);
        lcd.printf("Blok 7 lang: %d\n", Voorraad7Lang);
        lcd.printf("Blok 9 lang: %d\n", Voorraad9Lang);
        lcd.printf("Blok 11 lang: %d\n", Voorraad11Lang);
        ThisThread::sleep_for(T_LCD);
        lcd.cls();
        lcd.printf("\n Blok 13 lang: %d\n", Voorraad13Lang);
        lcd.printf("Blok 15 lang: %d\n", Voorraad15Lang);
        ThisThread::sleep_for(T_LCD);
        lcd.cls();
    }
}
//==============================================================================
//             Aparte thread voor activatie van de Servo motoren
//==============================================================================
Thread thread2;
void Servo_thread() {   // Als tijd Servo Array gelijk is aan de actuele timer tijd, activeer Servo voor x seconden
    while(true){
        if (Rood[R] = duration_cast<milliseconds>(Kleur.elapsed_time()).count()){
            ServoRood.write(45);
            ThisThread::sleep_for(ServoDelay); 
            ServoRood.write(0);
            R++;
        }if (Blauw[B] = duration_cast<milliseconds>(Kleur.elapsed_time()).count()){
            ServoBlauw.write(45);
            ThisThread::sleep_for(ServoDelay); 
            ServoBlauw.write(0);
            B++;
        }if (Groen[Gr] = duration_cast<milliseconds>(Kleur.elapsed_time()).count()){
            ServoGroen.write(45);
            ThisThread::sleep_for(ServoDelay); 
            ServoGroen.write(0);
            Gr++;
        }if (Geel[Ge] = duration_cast<milliseconds>(Kleur.elapsed_time()).count()){
            ServoGeel.write(45);
            ThisThread::sleep_for(ServoDelay); 
            ServoGeel.write(0);
            Ge++;
        }
        ThisThread::sleep_for(50); 
    }
}
//==============================================================================
//               Aparte thread voor bijhouden actuele voorraad
//==============================================================================
Thread thread3;
void Voorraad_thread() {
    Voorraad5Lang = Voorraad1 - Voorraad2;
    Voorraad7Lang = Voorraad2 - Voorraad3;
    Voorraad9Lang = Voorraad3 - Voorraad4;
    Voorraad11Lang = Voorraad4 - Voorraad5;
    Voorraad13Lang = Voorraad5 - Voorraad6;
    Voorraad15Lang = Voorraad6;
    
    ThisThread::sleep_for(1000); 
}
//==============================================================================
//            Als de machine gestopt word voer volgende commands uit:
//==============================================================================
void StopProgramma (void){
    Lamp_Stop = 1;
    SP_Trans1 = 0;
    SP_Trans2 = 0;
    SP_Lift = 0;
    SP_Geleiding = 0;
    Kleur.stop();
}
//==============================================================================
//            Als de machine gereset word voer volgende commands uit:
//==============================================================================
void Reset (void){
    Voorraad3LangRood = 0 ;
    Voorraad3LangGroen = 0 ;
    Voorraad3LangBlauw = 0 ;
    Voorraad3LangGeel = 0 ;
    Voorraad5Lang = 0 ;
    Voorraad7Lang = 0 ;
    Voorraad9Lang = 0 ;
    Voorraad11Lang = 0 ;
    Voorraad13Lang = 0 ;
    Voorraad15Lang = 0 ;
    ServoRood.write(0);
    ServoGroen.write(0);
    ServoBlauw.write(0);
    ServoGeel.write(0);
    Kleur.reset();
}
//==============================================================================
//                              Start hoofd programma
//==============================================================================
int main() {
    thread.start(LCD_thread);
    thread2.start(Servo_thread);
    thread3.start(Voorraad_thread);
    long red, green, blue, clear;
    Reset();
    
    if (Start_Knop == true && Stop_Knop == false){
        Start = true;
        Stop = false;
    } else if (Stop_Knop == true && Start_Knop == false){
        Start = false;
        Stop = true;
    }
    
    if (Start == true){
        Lamp_Start = true;
        Kleur.start();
        // Start motoren motorcontrollers en zet snelheid (range PWM is 0-1)
        SP_Trans1 = 0.5;        // PWM snelheid transportband boven lift, range: 0~1
        SP_Trans2 = 0.5;        // PWM snelheid transportband 3 lang gekleurd, range: 0~1
        SP_Lift = 0.5;          // PWM snelheid blokken lift, range: 0~1
        SP_Geleiding = 0.5;     // PWM snelheid motoren geleiding, range: 0~1
        
        while (Stop != true){
            if (Voorraad1 == 0 || KleurError == true){
                KleurError = false;
                //Read the HIGH pulse width in nS for each color.
                //The lower the value, the more of that color is detected 
                red = color.ReadRed();
                green = color.ReadGreen() - 5;
                blue = color.ReadBlue();
                clear = color.ReadClear();
         
                if (red < green && red < blue && green >= 30){
                    printf ("RED \r\n");
                    Voorraad3LangRood++;
                    Rood[Voorraad3LangRood] = duration_cast<milliseconds>(Kleur.elapsed_time()).count() + 1000;
                }else if (blue < red && blue < green && red >= 30){
                    printf ("BLUE \r\n");
                    Voorraad3LangBlauw++;
                    Blauw[Voorraad3LangRood] = duration_cast<milliseconds>(Kleur.elapsed_time()).count() + 2000;
                }else if (green < red && green < blue && red >= 30){
                    printf ("GREEN \r\n");
                    Voorraad3LangGroen++;
                    Groen[Voorraad3LangRood] = duration_cast<milliseconds>(Kleur.elapsed_time()).count() + 3000;
                }else if (red < 25 && green < 25 && clear <= 9){
                    printf ("YELLOW \r\n");
                    Voorraad3LangGeel++;
                    Geel[Voorraad3LangRood] = duration_cast<milliseconds>(Kleur.elapsed_time()).count() + 4000;
                }else {
                    printf ("error \r\n");
                    KleurError = true;
                }
                
                // Printf's voor debugging
                
                // Print servo status (bepaald welke servo geactiveerd word
                //printf("Servo status: %d\r\n", Servo);
                // Print waardes kleur sensor
                //printf("RED: %10d     GREEN: %10d     BLUE: %10d     CLEAR: %10d \r\n", red, green, blue, clear);  // Waardes voor debuggen en finetunen
                ThisThread::sleep_for(50); 
            }
        }
        StopProgramma();
    }
    if (Reset_Knop == true){
        Reset();
    }
}