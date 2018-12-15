
void checkBTcmd()  
 { 
    if (BT1.available()) 
    { 
      Serial.print(BT1.read());
      movement(BT1.read());
      BT1.flush();
    }
 }
 
