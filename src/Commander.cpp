#include "Commander.h"


Commander::Commander(Stream& serial, char eol, bool echo){
  com_port = &serial;
  this->eol = eol;
  this->echo = echo;
}
Commander::Commander(char eol, bool echo){
  this->eol = eol;
  this->echo = echo;
}


void Commander::add(char id, CommandCallback onCommand, char* label ){
  call_list[call_count] = onCommand;
  call_ids[call_count] = id;
  call_label[call_count] = label;
  call_count++;
}


void Commander::run(){
  if(!com_port) return;
  run(*com_port, eol);
}

void Commander::run(Stream& serial, char eol){
  Stream* tmp = com_port; // save the serial instance
  char eol_tmp = this->eol;
  this->eol = eol;
  com_port = &serial;

  // a string to hold incoming data
  while (serial.available()) {
    // get the new byte:
    int ch = serial.read();
    received_chars[rec_cnt++] = (char)ch;
    // end of user input
    if(echo)
      print((char)ch);
    if (isSentinel(ch)) {
      // execute the user command
      run(received_chars);

      // reset the command buffer
      received_chars[0] = 0;
      rec_cnt=0;
    }
    if (rec_cnt>=MAX_COMMAND_LENGTH) { // prevent buffer overrun if message is too long
        received_chars[0] = 0;
        rec_cnt=0;
    }
  }

  com_port = tmp; // reset the instance to the internal value
  this->eol = eol_tmp;
}

void Commander::run(char* user_input){
  // execute the user command
  char id = user_input[0];
  switch(id){
    case CMD_SCAN:
      for(int i=0; i < call_count; i++){
          print(call_ids[i]);
          print(":");
          if(call_label[i]) println(call_label[i]);
          else println("");
      }
      break;
    case CMD_VERBOSE:
      if(!isSentinel(user_input[1])) verbose = (VerboseMode)atoi(&user_input[1]);
      printVerbose(F("Verb:"));
      switch (verbose){
      case VerboseMode::nothing:
        println(F("off!"));
        break;
      case VerboseMode::on_request:
      case VerboseMode::user_friendly:
        println(F("on!"));
        break;
      }
      break;
    case CMD_DECIMAL:
      if(!isSentinel(user_input[1])) decimal_places = atoi(&user_input[1]);
      printVerbose(F("Decimal:"));
      println(decimal_places);
      break;
    default:
      for(int i=0; i < call_count; i++){
        if(id == call_ids[i]){
          call_list[i](&user_input[1]);
          break;
        }
      }
      break;
  }
}


void Commander::scalar(float* value,  char* user_cmd){
  bool GET  = isSentinel(user_cmd[0]);
  if(!GET) *value = atof(user_cmd);
  println(*value);
}

bool Commander::isSentinel(char ch)
{
  if(ch == eol)
    return true;
  else if (ch == '\r')
  {
      printVerbose(F("Warn: \\r detected! \n"));
  }
  return false;
}

void Commander::print(const int number){
  if( !com_port || verbose == VerboseMode::nothing ) return;
  com_port->print(number);
}
void Commander::print(const float number){
  if(!com_port || verbose == VerboseMode::nothing ) return;
  com_port->print((float)number,(int)decimal_places);
}
void Commander::print(const char* message){
  if(!com_port || verbose == VerboseMode::nothing ) return;
  com_port->print(message);
}
void Commander::print(const __FlashStringHelper *message){
  if(!com_port || verbose == VerboseMode::nothing ) return;
  com_port->print(message);
}
void Commander::print(const char message){
  if(!com_port || verbose == VerboseMode::nothing ) return;
  com_port->print(message);
}

void Commander::println(const int number){
  if(!com_port || verbose == VerboseMode::nothing ) return;
  com_port->println(number);
}
void Commander::println(const float number){
  if(!com_port || verbose == VerboseMode::nothing ) return;
  com_port->println((float)number, (int)decimal_places);
}
void Commander::println(const char* message){
  if(!com_port || verbose == VerboseMode::nothing ) return;
  com_port->println(message);
}
void Commander::println(const __FlashStringHelper *message){
  if(!com_port || verbose == VerboseMode::nothing ) return;
  com_port->println(message);
}
void Commander::println(const char message){
  if(!com_port || verbose == VerboseMode::nothing ) return;
  com_port->println(message);
}


void Commander::printVerbose(const char* message){
  if(verbose == VerboseMode::user_friendly) print(message);
}
void Commander::printVerbose(const __FlashStringHelper *message){
  if(verbose == VerboseMode::user_friendly) print(message);
}
void Commander::printError(){
 println(F("err"));
}
