#define MODULE_ID 0
#define TOTAL_MODULES 4

/*
 "@"+"ADDRESS_ID"+":"+"SENDER_ID"+"#"+"MSG_STRING"
 
*MSG_STRINGs*
1. STARTGAME
Controller broadcasts to all modules that game starts
2. NEUTRALISEMODULE
Module broadcasts to controller it is neutralised (non-needy modules only)
3. BOOM
Module broadcasts to controller it has been triggered (needy modules)
4. STRIKE
Module broadcasts to controller a strike (non-needy modules only)
5. ENDGAME
Controller broadcasts to modules that game has ended.

*Modules_ID*
Controller module: 0
Zhenyuan's core module: 1
Y3's core module: 2
Tom's core module: 3
 */

//Globally declare these
bool module_status[TOTAL_MODULES];
bool module_status_template[TOTAL_MODULES];
int num_strikes=0;
bool continueTimer=true;
bool isExplode=true;

void setup() {
  //Set baud rate appropriately
  Serial.begin(115200);
  //module_status stores whether each module is still active
  //controller module is set to false
  memset(module_status,true,TOTAL_MODULES);
  memset(module_status_template,false,TOTAL_MODULES);
  module_status[0]=false;
}

void sendMessage(String MSG_STRING, int ADDRESS_ID, int SENDER_ID=MODULE_ID){
  //Function for sending a message
  Serial.write("@");
  Serial.write(ADDRESS_ID);
  Serial.write(":");
  Serial.write(MODULE_ID);
  Serial.write("#");
  Serial.print(MSG_STRING);
}

void checkMessages(){
  //Check for message or relay it on
  while(Serial.available()>0){
    digitalWrite(13,HIGH);
    char incomingChar=Serial.read();
    if(incomingChar=='@'){
      //legit sequence
      int address=Serial.parseInt();
      int sender=Serial.parseInt();
      String message=Serial.readString().substring(1);
      if(address==MODULE_ID){
        
        if(message=="NEUTRALISEMODULE"){
          module_status[sender]=false;
          if(module_status==module_status_template){
            isExplode=false;
            continueTimer=false;
          }
          break;
        }
        
        else if(message=="STRIKE"){
          num_strikes++;
          if(num_strikes>=3){
            continueTimer=false;
          }
          break;
        }
        
        else if(message=="BOOM"){
          break;
        }
        
        else{break;}
      }
      else{
        sendMessage(message,address,sender);
      }
    }
    digitalWrite(13,LOW);
  }
}

int tickClock(int TIME_LEFT){
  //display the number on screen
  TIME_LEFT-=1;
  return TIME_LEFT;
}

int main() {
  int TIME_LEFT=300;
  while(continueTimer){
    checkMessages();
    TIME_LEFT=tickClock(TIME_LEFT);
    if(TIME_LEFT<=0){
      continueTimer=false;
      break;
    }
  }
  for(int i=1;i<TOTAL_MODULES;++i){
    sendMessage("ENDGAME",i);
  }
  if(isExplode){
    //play audio
  }
  return 0;
}
