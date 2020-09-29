/**
 * Arduino CNC lathe/tornio CNC
 * @author Igor Nociaro
 * @version 1.8
 * -aggiunta di affilatura orizzontale e minor bug fix
 */ 
float velocita = 1500.0;
float accelerazione = 3200.0;
char c;
float gradiassoluti = 0;

float motrice1 = 15.0; //motore 1
float motrice2 = 18.0; //motore 2
float condotta1 = 60.0;
float condotta2 = 60.0;
float riduzione1 = 3.0; //slitta lineare
float riduzione2 = 1.0; 
//////////////////////////////
#define passigiro 3200.0 //reali del motore1 con 1/16(200*16)
#define passigiro2 1600.0 //motore2 con 1/8

float passimm = passigiro*(condotta1/motrice1)*(1/riduzione1); //passi per mm motore1
float passiridotti = passigiro2*(condotta2/motrice2)*riduzione2; //passi per giro motore2

//float passiinch = passimm*25.4; //pollice
//float passithou = passimm*0.0254;//millesimo di pollice
/////////////////////////////////////////////////////////////////dichiarazione DISPLAY
#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

byte grad[8] = {
  0b11100,
  0b10100,
  0b11100,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
//////////////////////////////////////////////////////////////////////////dichiarazione TASTIERA
#include <Keypad.h>
char tasti[][4] =
{
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte Colonne[] = {10,11,12,13};
byte Righe[] = {14,15,16,17};

Keypad tastiera = Keypad(makeKeymap(tasti),Righe,Colonne,4,4);
//////////////////////////////////////////////////////////////////////////dichiarazione MOTORI
#include <AccelStepper.h>

#define DIR 8
#define STEP 9
AccelStepper stepper1(AccelStepper::DRIVER,STEP,DIR); //step e direzione
///
#define DIR2 18  //A4
#define STEP2 19 //A5
AccelStepper stepper2(AccelStepper::DRIVER,STEP2,DIR2);


//////////////////////////////////////////////////////////////////////////prototipi funzioni numero
/*
 * NON TOCCARE - utilizza solo inseriscinumero():
 * restituisce un numero intero di 4 cifre o un numero di (max)3 cifre con (max)2 cifre decimali
 */
int cifra(char ch);
int inseriscicifra();
float inseriscinumero();
float numerotot(float num1,float num2,float num3,float num4);
float numerotot(float num1,float num2,float num3);
float numerotot(float num1,float num2);
///////////////////////////////////////////////////////////////////////////prototipi funzioni
//int inserisciRPM();
//int inserisciACC();
///////////////////////////////////////////////////////////////////////////prototipi funzioni grafica
void menu1();
void menu2();
void interfaccia(float angolo);
void menurotcontinua();
void impostazioni();
////////////////////////////////////////////////////////////////////////////prototipi modalità
void modA();
void modB();
void modC();
void modA2();
void modB2();
void modC2();
void modD();
///////////////////////////////////////////////////////////////////////////////////////////setup
void setup() {
  Serial.begin(9600); 
  Serial.println("inizio programma");
  lcd.begin(20,4);
  lcd.createChar(0, grad);

  stepper1.setMaxSpeed(velocita);
  stepper1.setAcceleration(accelerazione);
  stepper2.setMaxSpeed(velocita);
  stepper2.setAcceleration(accelerazione);

}
/////////////////////////////////////////////////////////////////////////////////////loop
void loop() {
  menu1();
  c = tastiera.waitForKey();
  if(c != '*'){
  switch(c){
    case 'A':
       modA();
       break;
    case 'B':
       modB();
       break;
    case 'C':
       modC();
       break;
    case 'D':
       modD();
       break;
    default:
    //fai una scelta
       break;
  }
  }else{
    do{
    menu2();
    c = tastiera.waitForKey();
  switch(c){
    case 'A':
       modA2();
       break;
    case 'B':
       modB2();
       break;
    case 'C':
       modC2();
       break;
    case 'D':
       modD();
       break;
    default:
    //fai una scelta
       break;
  }
    }while(c != '*');
  }

}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////funzioni 
//da ottimizzare
float inserisciRPM(){
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("inserisci RPM:");
  lcd.setCursor(0,3);
  lcd.print("A: OK        D: <-");
  float var = inseriscinumero();
  if(var == 0) return 0; //ritorna al menu
  return var;
}
float inserisciACC(){
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("inserisci ACC:");
  lcd.setCursor(0,3);
  lcd.print("A: OK        D: <-");
  float var = inseriscinumero();
  if(var == 0) return 0; //ritorna al menu
  return var;
}
float inserisciLEN(){
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("lunghezza :");
  lcd.setCursor(12,2);
  lcd.print("mm");
  lcd.setCursor(0,3);
  lcd.print("A: OK        D: <-");
  float var = inseriscinumero();
  if(var == 0) return 0; //ritorna al menu
  return var;
}
float inserisciPAS(String a){
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("passo del filetto:");
  lcd.setCursor(13,2);
  lcd.print(a);
  lcd.setCursor(0,3);
  lcd.print("A: OK        D: <-");
  float var = inseriscinumero();
  if(var == 0) return 0; //ritorna al menu
  return var;
}
float inseriscidenti(){
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("numero di denti:");
  lcd.setCursor(0,3);
  lcd.print("A: OK        D: <-");
  float var = inseriscinumero();
  if(var == 0) return 0; //ritorna al menu
  return var;
}
float inserisciriduzione(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("rapporto");
  lcd.setCursor(0,1);
  lcd.print("di riduzione:");
  lcd.setCursor(0,3);
  lcd.print("A: OK        D: <-");
  float var = inseriscinumero();
  if(var == 0) return 0; //ritorna al menu
  return var;
}
float inserisciVEL(){
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("velocita':");
 // lcd.setCursor(13,2);
 // lcd.print("(max 1000)");
  lcd.setCursor(0,3);
  lcd.print("A: OK        D: <-");
  float var = inseriscinumero();
  if(var == 0) return 0; //ritorna al menu
  return var;
}
float inserisciD(){
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("distanza:");
  lcd.setCursor(0,3);
  lcd.print("A: OK        D: <-");
  float var = inseriscinumero();
  if(var == 0) return 0; //ritorna al menu
  return var;
}
int inserisciFlauti(){
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("numero di taglienti:");
  lcd.setCursor(0,3);
  lcd.print("A: OK        D: <-");
  float var = inseriscinumero();
  if(var == 0) return 0; //ritorna al menu
  return var;
}
float inserisciAngoloAffilatura(){  
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("angolo affilatura:");
  lcd.setCursor(11,2);
  lcd.write(byte(0));
  lcd.setCursor(0,3);
  lcd.print("A: OK        D: <-");
  float var = inseriscinumero();
  if(var == 0) return 0; //ritorna al menu
  return var;
}
float inserisciDiam(){
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("diametro utensile:");
  lcd.setCursor(0,3);
  lcd.print("A: OK        D: <-");
  float var = inseriscinumero();
  if(var == 0) return 0; //ritorna al menu
  return var;
}

///////////////////////////////////////////////////////////////////////////////////////////modalità
void modA(){
  short int cont = 0;
  float gradirelativi=0; //togliere se si vuole la posizione assoluta
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("inserisci gradi:");
  lcd.setCursor(0,3);
  lcd.print("A: OK        D: <-");
  float gradi = inseriscinumero();
  if(gradi != 0){  
    do{
      interfaccia(gradi);
      lcd.setCursor(4,0);
      lcd.print(gradiassoluti);
      lcd.setCursor(10,0);
      lcd.write(byte(0));
      lcd.setCursor(4,1);
      lcd.print(gradirelativi);
      lcd.setCursor(10,1);
      lcd.write(byte(0));
      lcd.setCursor(15,0);
      lcd.print("n:");
      lcd.setCursor(17,0);
      lcd.print(cont);
      //scelta
      c = tastiera.waitForKey();
      if(c == 'A'){
        gradirelativi = gradirelativi+gradi;
        gradiassoluti = gradiassoluti+gradi;
        cont++;
        stepper1.move(passimm*gradi/360.0); 
        stepper2.move(passiridotti*gradi/360.0);
      }
      if(c == 'B') {
        gradirelativi = gradirelativi-gradi; 
        gradiassoluti = gradiassoluti-gradi;
        cont--;
        stepper1.move(-passimm*gradi/360.0);
        stepper2.move(-passiridotti*gradi/360.0);//moveTo per posizione assoluta
      }
      //rotazione+  

      while(!((stepper1.distanceToGo()==0)&&(stepper2.distanceToGo()==0))){
        stepper1.run();
        stepper2.run();
      }

    }while(c != 'C');
  }
}
void modB(){
  short int cont = 0;
  float gradirelativi = 0;
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("inserisci divisione:");
  lcd.setCursor(0,3);
  lcd.print("A: OK        D: <-");
  int divisione = inseriscinumero();
  if(divisione != 0){
    float gradi = 360.0 / divisione;
    do{
      interfaccia(gradi);
      lcd.setCursor(4,0);
      lcd.print(gradiassoluti);
      lcd.setCursor(10,0);
      lcd.write(byte(0));
      lcd.setCursor(4,1);
      lcd.print(gradirelativi);
      lcd.setCursor(10,1);
      lcd.write(byte(0));
      lcd.setCursor(15,0); //visualizzazione avanzamento
      lcd.print("n:");
      lcd.setCursor(17,0);
      lcd.print(cont);
      c = tastiera.waitForKey();
      if(c == 'A'){
        gradirelativi = gradirelativi+gradi;
        gradiassoluti = gradiassoluti+gradi;
        cont++;
        stepper1.move(passimm*gradi/360.0); 
        stepper2.move(passiridotti*gradi/360.0);
      }
      if(c == 'B'){
        gradirelativi = gradirelativi-gradi;
        gradiassoluti = gradiassoluti-gradi;
        cont--;
        stepper1.move(-passimm*gradi/360.0); 
        stepper2.move(-passiridotti*gradi/360.0);
      }
      while(!((stepper1.distanceToGo()==0)&&(stepper2.distanceToGo()==0))){
      stepper1.run();
      stepper2.run();
      }
    }while(c != 'C');
    
  }
}
void modC(){
  menurotcontinua();
  c = tastiera.waitForKey();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Rotazione continua");
  lcd.setCursor(0,3);
  lcd.print("C: <-");
  if(c == 'A'){
    stepper2.move(100000000);
    lcd.setCursor(0,1);
    lcd.print("muovendo avanti");
  }
  if(c == 'B'){
    stepper2.move(-100000000);
    lcd.setCursor(0,1);
    lcd.print("muovendo indietro");
  }
  
  while((c != 'C')&&(c != 'D')){
      c = tastiera.getKey();
      stepper1.run();
      stepper2.run();
  }
  stepper1.stop();
  stepper2.stop();
  while(!((stepper1.distanceToGo()==0)&&(stepper2.distanceToGo()==0))){
      stepper1.run();
      stepper2.run();
      }
}
void modD(){
  /*ritorno al menù principale -> '#'
  *                   indietro -> 'D'
  */
  do{
  impostazioni();
  c = tastiera.waitForKey();
  switch(c){
    case 'A':
       velocita = inserisciRPM();//per interfaccia grafica altrimenti bastava inserisci numero
       stepper1.setMaxSpeed(velocita);
       stepper2.setMaxSpeed(velocita);
       break;
    case 'B':
       accelerazione = inserisciACC();
       stepper1.setAcceleration(accelerazione);
       stepper2.setAcceleration(accelerazione);
       break;
    case 'C':
       // modC2();
       do{
       menumotori();
       c = tastiera.waitForKey();
       switch(c){
        case 'A':
          do{
          menucambioriduzione(motrice1,condotta1,riduzione1);
          lcd.setCursor(12,3);
          lcd.print("mm/giro");
          c = tastiera.waitForKey();
          switch(c){
            case 'A':
              motrice1 = inseriscidenti();
              passimm = passigiro*(condotta1/motrice1)*(1/riduzione1);
              break;
            case 'B':
              condotta1 = inseriscidenti();
              passimm = passigiro*(condotta1/motrice1)*(1/riduzione1);
              break;
            case 'C':
              riduzione1 = inserisciriduzione();
              passimm = passigiro*(condotta1/motrice1)*(1/riduzione1);
              break;
            default:
              break;
          }
          
          }while((c != 'D')&&(c != '#'));
          if(c != '#') //ritorno al menu principale
            c = 'E'; //prevenzione ritorno menu principale,qualsiasi lettera che non sia abcd
          break;  //fine caso 'A' switch 1
        case 'B':
          do{
          menucambioriduzione(motrice2,condotta2,riduzione2);
          c = tastiera.waitForKey();
          switch(c){
            case 'A':
              motrice2 = inseriscidenti();
              passiridotti = passigiro2*(condotta2/motrice2)*riduzione2;
              break;
            case 'B':
              condotta2 = inseriscidenti();
              passiridotti = passigiro2*(condotta2/motrice2)*riduzione2;
              break;
            case 'C':
              riduzione2 = inserisciriduzione();
              passiridotti = passigiro2*(condotta2/motrice2)*riduzione2;
              break;
            default:
              break;
          } //switch 3o livello
          }while((c != 'D')&&(c != '#'));
          if(c != '#')
            c = 'E'; //prevenzione ritorno menu principale,qualsiasi lettera che non sia abcd
          break; //fine caso 'B' switch 1
        default:
          break;
       }//switch 2o livello
       }while((c != 'D')&&(c != '#'));//while all'interno di caso 'C'
       if(c != '#')
        c = 'E';
       break;
    default:
    //fai una scelta
       break;
  }//fine switch 1
  }while((c != 'D')&&(c != '#')); //aggiungere lettere per altre impostazioni
}
void modA2(){
  //la velocità di stepper 1 dipende da stepper 2
  float unitamisura; //unità di misura utilizzata
  String temp = "";
  unitamisura = passimm;//da aggiustare
  menuunitamisura();
  c = tastiera.waitForKey();
  switch(c){
    case 'A':
     // unitamisura = passimm;
      temp = "mm";
      break;
    case 'B':
      //unitamisura = passiinch;
      //unitamisura = passimm;
      temp = "TPI";
      break;
    default :
      //unitamisura = passimm;
      temp = "mm";
      break;
  }
  
  float lunghezza = inserisciLEN();
  if(lunghezza == 0) return;
  float passo = inserisciPAS(temp);
  if(passo == 0) return;
  if(temp == "TPI"){
    passo = 25.40000 / passo ; //thread per inch
  }
  float numpassi = (passiridotti*lunghezza)/passo; //numero di passi per compiere un giro, cast implicito float->int
  //float t = numpassi / velocita;
  //float velocita2 = (unitamisura*lunghezza)/t;
  float t = (unitamisura*lunghezza)/ velocita;
  float velocita2 = numpassi / t;
  float mv = 3.0; //moltiplicatore velocità per ritorno veloce
 // int numpassi = (passiridotti*lunghezza)/passo; //numero di passi per compiere un giro, cast implicito float->int
  bool isteresi = true;
  bool dxsx;
  destrasinistra();
  c = tastiera.waitForKey(); 
  if(c == 'A'){  //vite destra
    dxsx = true;
  do{
    menufiletto(lunghezza,passo,t,temp,dxsx);
    c = tastiera.waitForKey();
    if((c == 'A') && isteresi){
      isteresi = !isteresi;
      stepper1.move(unitamisura*lunghezza);
      //stepper2.move(-(passiridotti*lunghezza)/passo); 
      stepper2.move(numpassi);    
      stepper1.setSpeed(velocita);//move potrebbe cambiare la velocità
      stepper2.setSpeed(velocita2); //anche mot2 non ha accelerazioni ora
      
      while(!((stepper1.distanceToGo()==0)&&(stepper2.distanceToGo()==0))){
        stepper1.runSpeedToPosition();
        stepper2.runSpeedToPosition();       
      }
    }
    if((c == 'B') && !isteresi){     //indietro veloce
      isteresi = !isteresi; 
      float divisore = ((passiridotti*lunghezza)/passo)/passiridotti;
      float passizero = ((passiridotti*lunghezza)/passo)-(passiridotti*(int)divisore);
      if(passizero <= passiridotti/2.0){ //se ha superato metà giro
        stepper2.move(-passizero);
      }else{
        stepper2.move(-(passizero-passiridotti));  //-(passiridotti-passizero)
      }
      stepper1.move(-unitamisura*lunghezza);
      stepper1.setSpeed(velocita*mv*4);
      stepper2.setSpeed(velocita*mv); 
      while(!((stepper1.distanceToGo()==0)&&(stepper2.distanceToGo()==0))){
        stepper1.runSpeedToPosition();
        stepper2.runSpeedToPosition(); 
      }                                  

    }
  }while(c != 'C');
  }
  if(c == 'B'){    //vite sinistra
    dxsx = false;
    do{
    menufiletto(lunghezza,passo,t,temp,dxsx);
    c = tastiera.waitForKey();
    if((c == 'A') && !isteresi){   //avanti veloce
      isteresi = !isteresi;
      float divisore = ((passiridotti*lunghezza)/passo)/passiridotti;
      float passizero = ((passiridotti*lunghezza)/passo)-(passiridotti*(int)divisore);
      
      if(passizero <= passiridotti/2.0){ //se ha superato metà giro
        stepper2.move(-passizero);
      }else{
        stepper2.move(-(passizero-passiridotti));  
      }
      stepper1.move(unitamisura*lunghezza);
      stepper1.setSpeed(velocita*mv*4);
      stepper2.setSpeed(velocita*mv); 
      
    }
    if((c == 'B') && isteresi){
      isteresi = !isteresi; 

      stepper1.move(-unitamisura*lunghezza);
      stepper2.move(numpassi);    
      stepper1.setSpeed(velocita);//move potrebbe cambiare la velocità
      stepper2.setSpeed(velocita2); //anche mot2 non ha accelerazioni ora
    }
    while(!((stepper1.distanceToGo()==0)&&(stepper2.distanceToGo()==0))){
        stepper1.runSpeedToPosition();
        stepper2.runSpeedToPosition();
      }
  }while(c != 'C');
  }
}
void modB2(){
  float lunghezza = inserisciD();
  if(lunghezza == 0) return;
  float vel = inserisciVEL();
  if(vel == 0) return;
  stepper1.setMaxSpeed(vel);
 
  do{
    menusolomot1(lunghezza,vel);
    c = tastiera.waitForKey();
    if(c == 'A'){
 
      stepper1.move(passimm*lunghezza);
      while(stepper1.distanceToGo()!=0){
        stepper1.run();
      }
    }
    if(c == 'B'){
 
      stepper1.move(-passimm*lunghezza);
      while(stepper1.distanceToGo()!=0){
        stepper1.run();
      }
    }
  }while(c != 'C');
  stepper1.setMaxSpeed(velocita);
}
void modC2(){  //affilatura orizzontale-rettifica
  int pos = 1; //posizione fresa su tagliente
  bool isteresi = true;
  float lunghezza = inserisciLEN();
  if(lunghezza == 0) return;
  float diametro = inserisciDiam();
  if(diametro == 0) return;
  float alpha = inserisciAngoloAffilatura();
  if((alpha == 0)||(alpha > 60)) return;
  int flauti = inserisciFlauti();
  if((flauti == 0)||(flauti > 8)) return;
  
/*  Serial.print("lunghezza: ");
  Serial.println(lunghezza);
  Serial.print("alpha: ");
  Serial.println(alpha);
  Serial.print("tanalpha: ");
  Serial.println(tan(alpha));*/
  float rad = (alpha * 71) / 4068; //conversione gradi->radianti
/*  Serial.print("rad: ");
  Serial.println(rad);*/
  float len2 = lunghezza*tan(rad); //tan vuole radianti come angolo
 /* Serial.print("arco: ");
  Serial.println(len2);*/
  float beta = (360.0*len2)/(PI*diametro); //angolo di rotazione tornio
 /* Serial.print("angolo: ");
  Serial.println(beta);*/
  float numpassi = (passiridotti*beta)/360.0; //numero di passi per fare un determinato angolo
 /* Serial.print("numpassi angolo: ");
  Serial.println(numpassi);*/

  //float t = numpassi / velocita;
  //float velocita2 = (lunghezza*passimm)/t;

  float t = (lunghezza*passimm)/ velocita;
  float velocita2 = numpassi/t;
 
  /*Serial.print("numpassi prossimo tagliente: ");
  Serial.println( passiridotti*(360.0/flauti)/360.0);
  Serial.print("t: ");
  Serial.println(t);*/
  
  do{
   // int pos = flauti; //provv posizione n flauto
    menurettifica(lunghezza,alpha,t,pos);
    c = tastiera.waitForKey();
    if (c == 'A'){
      //prima dalla base verso la punta poi riprendi la posizione
      if(isteresi){
        isteresi = !isteresi;
        //
        stepper1.move(-passimm*lunghezza);
        stepper2.move(-numpassi);    
        stepper1.setSpeed(velocita);//move potrebbe cambiare la velocità
        stepper2.setSpeed(velocita2); //anche mot2 non ha accelerazioni ora
        
        while(!((stepper1.distanceToGo()==0)&&(stepper2.distanceToGo()==0))){
        stepper1.runSpeedToPosition();
        stepper2.runSpeedToPosition();       
      }
      }else{  //riprendi la posizione
        isteresi = !isteresi;
        //
        stepper1.move(passimm*lunghezza);
        stepper2.move(numpassi);    
        stepper1.setSpeed(velocita);//move potrebbe cambiare la velocità
        stepper2.setSpeed(velocita2); //anche mot2 non ha accelerazioni ora
        
        while(!((stepper1.distanceToGo()==0)&&(stepper2.distanceToGo()==0))){
        stepper1.runSpeedToPosition();
        stepper2.runSpeedToPosition();       
      }
      }
    }
    if (c == 'B'){
      pos++;
      if(pos > flauti) pos = 1;
      
      //motore tornio + 360°/flauti
      stepper2.move(passiridotti*(360.0/flauti)/360.0);
    
      while(stepper2.distanceToGo()!=0){
        stepper2.run();
      }
    }
  }while(c != 'C');
}
/////////////////////////////////////////////////////////////////////////////funzioni numero
int cifra(char ch){
  switch(ch){
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case '0': return 0;
    default : return 10;
  }
}

int inseriscicifra(){
 // Serial.println("all'interno di inserisci cifra");
  int num;
  do{
  c = tastiera.waitForKey();
 // Serial.println(c);
  if (c == 'D') return 10; //annulla
  if (c == 'A') return 11; //conferma
  if (c == '*') return 12; //virgola
  num = cifra(c);
  if(num == 10)
     Serial.println("Non è un numero!");
  }while(num==10);
  return num;
}

float inseriscinumero(){
  byte primo,secondo,terzo,quarto,decimi,centesimi,cursore = 6; //non può essere negativo
  
  primo = inseriscicifra();
  if((primo == 11)||(primo ==10)){ //conferma o annulla
    Serial.println("ritorno al menu'");
    return 0; //errore
  }
  lcd.setCursor(cursore,2);
  if(primo == 12){
    lcd.print('0');cursore++;
    lcd.setCursor(cursore,2);
    lcd.print('.');cursore++;
    do{
    decimi = inseriscicifra();
    }while(decimi == 12); //prevenzione 2a virgola
    if((decimi == 11)||(decimi == 10)) return 0; //conferma o annulla
    lcd.setCursor(cursore,2);
    lcd.print(decimi);cursore++;
    do{
    centesimi = inseriscicifra();
    }while(centesimi == 12); 
    if(centesimi == 11) return decimi/10.0;
    if(centesimi == 10) return 0; //errore
    lcd.setCursor(cursore,2);
    lcd.print(centesimi);cursore++;
    return  decimi/10.0 + centesimi/100.0;
  }
  lcd.print(primo);cursore++;

  secondo = inseriscicifra();
  if(secondo == 11) return primo;
  if(secondo == 10) return 0; //ritorno al menu
  lcd.setCursor(cursore,2);
  if(secondo == 12){
    lcd.print('.');cursore++;
    do{
    decimi = inseriscicifra();
    }while(decimi == 12); //prevenzione 2a virgola
    if(centesimi == 11) return primo;
    if(centesimi == 10) return 0; //errore
    lcd.setCursor(cursore,2);
    lcd.print(decimi);cursore++;
    do{
    centesimi = inseriscicifra();
    }while(centesimi == 12); 
    if(centesimi == 11) return primo + decimi/10.0;
    if(centesimi == 10) return 0; //errore
    lcd.setCursor(cursore,2);
    lcd.print(centesimi);cursore++;
    return  primo + decimi/10.0 + centesimi/100.0;
  }
  lcd.print(secondo);cursore++;

  
  terzo = inseriscicifra();
  if(terzo == 11) return numerotot(primo,secondo);
  if(terzo == 10) return 0;
  lcd.setCursor(cursore,2);
  if(terzo == 12){
    lcd.print('.');cursore++;
    do{
    decimi = inseriscicifra();
    }while(decimi == 12); //prevenzione 2a virgola
    if(centesimi == 11) return numerotot(primo,secondo);
    if(centesimi == 10) return 0; //errore
    lcd.setCursor(cursore,2);
    lcd.print(decimi);cursore++;
    do{
    centesimi = inseriscicifra();
    }while(centesimi == 12); 
    if(centesimi == 11) return numerotot(primo,secondo) + decimi/10.0;
    if(centesimi == 10) return 0; //errore
    lcd.setCursor(cursore,2);
    lcd.print(centesimi);cursore++;
    return  numerotot(primo,secondo) + decimi/10.0 + centesimi/100.0;
  }
  lcd.print(terzo);cursore++;

  quarto = inseriscicifra();
  if(quarto == 11) return numerotot(primo,secondo,terzo);
  if(quarto == 10) return 0; 
  lcd.setCursor(cursore,2);
  if(quarto == 12){
    lcd.print('.');cursore++;
    do{
    decimi = inseriscicifra();
    }while(decimi == 12); //prevenzione 2a virgola
    if(centesimi == 11) return numerotot(primo,secondo,terzo);
    if(centesimi == 10) return 0; //errore
    lcd.setCursor(cursore,2);
    lcd.print(decimi);cursore++;
    do{
    centesimi = inseriscicifra();
    }while(centesimi == 12); 
    if(centesimi == 11) return numerotot(primo,secondo,terzo) + decimi/10.0;
    if(centesimi == 10) return 0; //errore
    lcd.setCursor(cursore,2);
    lcd.print(centesimi);cursore++;
    return  numerotot(primo,secondo,terzo) + decimi/10.0 + centesimi/100.0;
  }
  lcd.print(quarto);

  return numerotot(primo,secondo,terzo,quarto);  //togli se aggiungi il quarto numero
}

float numerotot(float num1,float num2,float num3,float num4){
  float numerocompleto;
  num1 = num1*1000;
  num2 = num2*100;
  num3 = num3*10;
  numerocompleto = num1 + num2 + num3 + num4;
  return numerocompleto;
}
float numerotot(float num1,float num2,float num3){
  float numerocompleto;
  num1 = num1*100;
  num2 = num2*10;
  numerocompleto = num1 + num2 + num3;
  return numerocompleto;
}
float numerotot(float num1,float num2){
  float numerocompleto;
  num1 = num1*10;
  numerocompleto = num1 + num2;
  return numerocompleto;
}

/////////////////////////////////////////////////////////////////////////////////////////grafica
void menu1(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("A:gradi         *:->");
  lcd.setCursor(0,1);
  lcd.print("B:divisioni");
  lcd.setCursor(0,2);
  lcd.print("C:rotazione continua");
  lcd.setCursor(0,3);
  lcd.print("D:impostazioni");
}
void menu2(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("A:filettatura   *:<-");
  lcd.setCursor(0,1);
  lcd.print("B:solo motore 1");
  lcd.setCursor(0,2);
  lcd.print("C:rettifica");
  lcd.setCursor(0,3);
  lcd.print("D:impostazioni");
}
void interfaccia(float angolo){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Abs:");
  lcd.setCursor(0,1);
  lcd.print("Rel:");
  lcd.setCursor(0,2);
  lcd.print("Deg:");
  lcd.setCursor(4,2);
  lcd.print(angolo);
  lcd.setCursor(10,2);
  lcd.write(byte(0));
  lcd.setCursor(17,2);
  lcd.print("C:X");
  lcd.setCursor(0,3);
  lcd.print("A:avanti  B:indietro");
}
void menurotcontinua(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Rotazione continua");
  lcd.setCursor(0,1);
  lcd.print("A:avanti");
  lcd.setCursor(0,2);
  lcd.print("B:indietro");
  lcd.setCursor(0,3);
  lcd.print("C: <-");
}
void impostazioni(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("A:Velocita'");
  lcd.setCursor(14,0);
  lcd.print(velocita);
  lcd.setCursor(0,1);
  lcd.print("B:Acceleraz");
  lcd.setCursor(14,1);
  lcd.print(accelerazione);
  lcd.setCursor(0,2);
  lcd.print("C:Motori");
  lcd.setCursor(0,3);
  lcd.print("D: <-");
}
void menumotori(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("A:motore 1");
  lcd.setCursor(0,1);
  lcd.print("B:motore 2");
  lcd.setCursor(0,2);
  lcd.print("C:");
  lcd.setCursor(0,3);
  lcd.print("D: <-");
}
void menucambioriduzione(float p1,float p2,float p3){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("A:motrice");
  lcd.setCursor(13,0);
  lcd.print(p1);
  lcd.setCursor(0,1);
  lcd.print("B:condotta");
  lcd.setCursor(13,1);
  lcd.print(p2);
  lcd.setCursor(0,2);
  lcd.print("C:riduzione");
  lcd.setCursor(13,2);
  lcd.print(p3);
  lcd.setCursor(0,3);
  lcd.print("D: <-");
}
 void menuunitamisura(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("A:mm");
  lcd.setCursor(0,1);
  lcd.print("B:TPI");
  lcd.setCursor(0,2);
  lcd.print("C:");
  lcd.setCursor(0,3);
  lcd.print("D: <-");
 }
 void menufiletto(float lunghezza,float passo,float tempo,String unitamisura,bool dxsx){
  int minuti = tempo/60.0;
  int secondi = (int)tempo%60;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("lunghezza:       mm");
  lcd.setCursor(11,0);
  lcd.print(lunghezza);
  lcd.setCursor(0,1);
  lcd.print("passo:");
  lcd.setCursor(7,1);
  lcd.print(passo,3);
  lcd.setCursor(13,1);
  lcd.print(unitamisura);
  lcd.setCursor(0,2);
  lcd.print("tempo:");
  lcd.setCursor(7,2);
  lcd.print(minuti);
  lcd.setCursor(9,2);
  lcd.print(":");
  lcd.setCursor(10,2);
  lcd.print(secondi);
  lcd.setCursor(13,2);
  lcd.print("m");
  lcd.setCursor(18,1);
  if(dxsx){  
  lcd.print("DX");
  }else{
  lcd.print("SX");
  }
  lcd.setCursor(17,2);
  lcd.print("C:X");
  lcd.setCursor(0,3);
  lcd.print("A:avanti  B:indietro"); //se sta già avanti non puoi premerlo nuovamente
 }
 void menurettifica(float lunghezza,float angolo,float tempo,int ntagliente){
  int minuti = tempo/60.0;
  int secondi = (int)tempo%60;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("lunghezza:      n : ");
  lcd.setCursor(17,0);
  lcd.write(byte(0));
  lcd.setCursor(11,0);
  lcd.print(lunghezza);
  lcd.setCursor(19,0);
  lcd.print(ntagliente);
  lcd.setCursor(0,1);
  lcd.print("angolo:");
  lcd.setCursor(8,1);
  lcd.print(angolo);
  lcd.setCursor(12,1);
  lcd.write(byte(0));
  lcd.setCursor(0,2);
  lcd.print("tempo:");
  lcd.setCursor(7,2);
  lcd.print(minuti);
  lcd.setCursor(9,2);
  lcd.print(":");
  lcd.setCursor(10,2);
  lcd.print(secondi);
  lcd.setCursor(13,2);
  lcd.print("m");
  lcd.setCursor(17,2);
  lcd.print("C:X");
  lcd.setCursor(0,3);
  lcd.print("A:avanti  B:cambia ");
 }
 void menusolomot1(float lunghezza,float velocita){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("lunghezza:       mm");
  lcd.setCursor(11,0);
  lcd.print(lunghezza);
  lcd.setCursor(0,1);
  lcd.print("velocita':");
  lcd.setCursor(11,1);
  lcd.print(velocita);
  lcd.setCursor(17,2);
  lcd.print("C:X");
  lcd.setCursor(0,3);
  lcd.print("A:avanti  B:indietro");
 }
 void destrasinistra(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("A:destra");
  lcd.setCursor(0,1);
  lcd.print("B:sinistra");
  lcd.setCursor(0,2);
  lcd.print("C:");
  lcd.setCursor(0,3);
  lcd.print("D: <-");
 }
