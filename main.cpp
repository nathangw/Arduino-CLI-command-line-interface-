//
 //Run the CLI commands                  
 //                                        
 //     input:    string from serial port   COMMAND    NAME    VALUE
 //                                         ^          ^       ^
 //                                       word1      word2    word3
 //    output:    none
 //   returns:    none
 //      uses:    none
 //
void ExecCLI(char *cmdline) {

  uint16_t word1 = 0;  //location of first word
  uint16_t word2 = 0;  //location of second word
  uint16_t word3 = 0;  //location of third word

  boolean spcfnd = true;
  boolean word1fnd = false;
  boolean charfnd = false;  
  for (uint16_t i = 0; i < strlen(cmdline); i++) {  //convet to upper case
    char ch = cmdline[i];
    if ((ch >= 97) and (ch <= 122)) cmdline[i] = ch - 32;
    if ((ch >= 33) and (ch <= 126)) charfnd = true;   //check if any characters are found
    if (ch == 32) { 
      spcfnd = true;    
    } else {
      if (!spcfnd) {             // find the start of all 3 words
        //do nothing
      }
      else if (!word1 and !word1fnd) {
        word1 = i;
        word1fnd = true;
      }
      else if (!word2) {
        word2 = i;
      }
      else if (!word3) {
        word3 = i;
      }
      spcfnd = false;
    }
  }

  char *s1 = &cmdline[word1];  //copy the word text to a new char array
  char *s2 = &cmdline[word2];
  char *s3 = &cmdline[word3];
  if (!word2) s2 = NULL;      //delete empty words
  if (!word3) s3 = NULL;
  char *command = strsep(&s1, " ");  //parse the first word in the line
  char *name = strsep(&s2, " ");  //parse the second word in the line
  char *value = strsep(&s3, " ");  //parse the third word in the line
  uint8_t key[16];
  uint32_t x;
  uint16_t i;


  if (do_echo) Serial.write("\r\n");    // output CRLF
  if (!charfnd) return;

  if (strcmp_P(command, PSTR("HELP")) == 0) {
    Serial.println(F(
      "The following commands should be followed by the return key.\r\n"
      "dump all\r\n"
      "dump EEPROM\r\n"
      "echo <value>: set echo off <0> or on <1>\r\n"
      "get [AppSKey] [Bandwidth] [CodingRate] [DevAddr] [Freq] [HealthmS] [NwkSKey] [SyncWord] [SFactor] [TXDelay] [TXPower]"
    ));
  } 
  else if (strcmp_P(command, PSTR("GET")) == 0) {
    if (strcmp_P(name, PSTR("DEVADDR")) == 0) {
      Serial.print("DevAddr=0x");
      LORA_HEX_PRINTLN(Serial,LoRaWanPacket.DevAddr,4);
    } 
    else if (strcmp_P(name, PSTR("NWKSKEY")) == 0) {
      Serial.print("NwkSKey=0x");
      LORA_HEX_PRINTLN(Serial,LoRaWanPacket.NwkSKey,16);
    } 
    else if (strcmp_P(name, PSTR("APPSKEY")) == 0) {
      Serial.print("AppSKey=0x");
      LORA_HEX_PRINTLN(Serial,LoRaWanPacket.AppSKey,16);
    }
    else if (strcmp_P(name, PSTR("FREQ")) == 0) {
      Serial.print("Frequency=");
      Serial.print(freq);
      Serial.println(" Hz");
    }
    else if (strcmp_P(name, PSTR("SYNCWORD")) == 0) {
      uint8_t H = syncword >> 8;
      uint8_t L = syncword - (H << 8);
      Serial.print("SyncWord=0x");
      LORA_HEX_PRINT(Serial,&H,1);
      LORA_HEX_PRINTLN(Serial,&L,1);
    }
    else if (strcmp_P(name, PSTR("TXPOWER")) == 0) {
      Serial.print("TXPower=");
      Serial.print(txpower);
      Serial.println("dBm");
    }
    else if (strcmp_P(name, PSTR("SFACTOR")) == 0) {
      Serial.print("SpreadingFactor=");
      Serial.print(sfactor);
    }
    else if (strcmp_P(name, PSTR("CODINGRATE")) == 0) {
      Serial.print("CodingRate=");
      Serial.print(codingrate);
    }
    else if (strcmp_P(name, PSTR("BANDWIDTH")) == 0) {
      Serial.print("Bandwidth=");
      Serial.print(bandwidth);
      Serial.println(" Hz");
    }
    else if (strcmp_P(name, PSTR("TXDELAY")) == 0) {
      Serial.print("TXDelay=");
      Serial.print(txdelay);
      Serial.println(" mS");
    }
    else if (strcmp_P(name, PSTR("HEALTHMS")) == 0) {
      Serial.print("HealthmS=");
      Serial.print(healthms);
      Serial.println(" mS");
    }
    else {
      Serial.print("Error: Unknown GET Name: ");
      Serial.println(name);
    }
  } 
  
  else if (strcmp_P(command, PSTR("SET")) == 0) {
    if (strcmp_P(name, PSTR("DEVADDR")) == 0) {
      if (ValidHex(value) == 8) {
        LORA_HEX_TO_BYTE((char *)key, (char *)value, 4);  //convert hex array to byte array
        EEPROM_write_Block(key,DevAddrEE,4);      //save the array
        EEPROM.commit();
        ReadInfo();                                       //get all the vars from flash
        Serial.print("DevAddr=0x");
        LORA_HEX_PRINTLN(Serial,LoRaWanPacket.DevAddr,4);
      } else {
        Serial.println("Error: Value must be 8 Hex Characters");
      }
    } 
    else if (strcmp_P(name, PSTR("NWKSKEY")) == 0) {
      if (ValidHex(value) == 32) {
        LORA_HEX_TO_BYTE((char *)key, (char *)value, 16);  //convert hex array to byte array
        EEPROM_write_Block(key,NwkSKeyEE,16);      //save the array
        EEPROM.commit();
        ReadInfo();                                       //get all the vars from flash
        Serial.print("NwkSKey=0x");
        LORA_HEX_PRINTLN(Serial,LoRaWanPacket.NwkSKey,16);
      } else {
        Serial.println("Error: Value must be 32 Hex Characters");
      }
    } 
    else if (strcmp_P(name, PSTR("APPSKEY")) == 0) {
      if (ValidHex(value) == 32) {
        LORA_HEX_TO_BYTE((char *)key, (char *)value, 16);  //convert hex array to byte array
        EEPROM_write_Block(key,AppSKeyEE,16);      //save the array
        EEPROM.commit();
        ReadInfo();                                       //get all the vars from flash
        Serial.print("AppSKey=0x");
        LORA_HEX_PRINTLN(Serial,LoRaWanPacket.AppSKey,16);
      } else {
        Serial.println("Error: Value must be 32 Hex Characters");
      }
    }
    else if (strcmp_P(name, PSTR("FREQ")) == 0) {
      x = atol(value);
      if ((x >= 430000000) and (x <= 930000000)) {
        EEPROM_write_uint32(FreqEE,x);
        EEPROM.commit();
        ReadInfo();                                       //get all the vars from flash
        SetupLoRa();
        Serial.print("Frequency=");
        Serial.print(freq);
        Serial.println(" Hz");
      } else {
        Serial.println("Error: Value must be between 430000000 and 930000000 Hz (no commas)");
      }
    }
    else if (strcmp_P(name, PSTR("SYNCWORD")) == 0) {
      if (ValidHex(value) == 4) {
        LORA_HEX_TO_BYTE((char *)key, (char *)value, 2);  //convert hex array to byte array
        i = key[0]<<8;
        i = i + key[1];
        EEPROM_write_uint16(SyncWordEE,i);      //save the array
        EEPROM.commit();
        ReadInfo();                                       //get all the vars from flash
        SetupLoRa();
        uint8_t H = syncword >> 8;
        uint8_t L = syncword - (H << 8);
        Serial.print("SyncWord=0x");
        LORA_HEX_PRINT(Serial,&H,1);
        LORA_HEX_PRINTLN(Serial,&L,1);
      } else {
        Serial.println("Error: Value must be 4 Hex Characters");
      }
    }
    else if (strcmp_P(name, PSTR("TXPOWER")) == 0) {
      i = atoi(value);
      if (i <= 20) {
        EEPROM.write(TXPowerEE,i);
        EEPROM.commit();
        ReadInfo();                                       //get all the vars from flash
        SetupLoRa();
        Serial.print("TXPower=");
        Serial.print(txpower);
        Serial.println("dBm");
      } else {
        Serial.println("Error: Max Value is 20");
      }
    }
    else if (strcmp_P(name, PSTR("SFACTOR")) == 0) {
      i = atoi(value);
      if ((i >= 7) and (i <= 12)) {
        EEPROM.write(SFactorEE,i);
        EEPROM.commit();
        ReadInfo();                                       //get all the vars from flash
        SetupLoRa();
        Serial.print("SpreadingFactor=");
        Serial.println(sfactor);
      } else {
        Serial.println("Error: Value should be from 7 to 12");
      }
    }
    else if (strcmp_P(name, PSTR("CODINGRATE")) == 0) {
      i = atoi(value);
      if ((i >= 1) and (i <= 4)) {
        EEPROM.write(CodingRateEE,i);
        EEPROM.commit();
        ReadInfo();                                       //get all the vars from flash
        SetupLoRa();
        Serial.print("CodingRate=");
        Serial.println(codingrate);
      } else {
        Serial.println("Error: Value should be from 1 to 4");
      }
    }
    else if (strcmp_P(name, PSTR("BANDWIDTH")) == 0) {
      x = atol(value);
      if ((x == 125000) or (x == 250000) or (x == 500000)) {
        EEPROM_write_uint32(BandwidthEE,x);
        EEPROM.commit();
        ReadInfo();                                       //get all the vars from flash
        SetupLoRa();
        Serial.print("Bandwidth=");
        Serial.print(bandwidth);
        Serial.println(" Hz");
      } else {
        Serial.println("Error: Value must be 125000, 250000 or 500000 Hz (no commas)");
      }
    }
    else if (strcmp_P(name, PSTR("TXDELAY")) == 0) {
      x = atol(value);
      if (x <= 30000) {
        EEPROM_write_uint16(TXDelayEE,x);
        EEPROM.commit();
        ReadInfo();                                       //get all the vars from flash
        Serial.print("TXDelay=");
        Serial.print(txdelay);
        Serial.println(" mS");
      } else {
        Serial.println("Error: Value must be between 0 AND 30000");
      }
    }
    else if (strcmp_P(name, PSTR("HEALTHMS")) == 0) {
      x = atol(value);
      if ((x >= 30000) & (x <= 604800)) {
        EEPROM_write_uint32(HealthmSEE,x);
        EEPROM.commit();
        ReadInfo();                                       //get all the vars from flash
        Serial.print("HealthmS=");
        Serial.print(healthms);
        Serial.println(" mS");
      } else {
        Serial.println("Error: Value must be between 30000 AND 604800");
      }
    }
    else {
      Serial.print("Error: Unknown SET Name: ");
      Serial.println(name);
    }
  } 

  else if (strcmp_P(command, PSTR("ECHO")) == 0) {
    do_echo = atoi(name);
    Serial.print("Echo set to ");
    if (do_echo) {
      Serial.println("ON");
    } else {
      Serial.println("OFF");
    }
  } 

  else if (strcmp_P(command, PSTR("DUMP")) == 0) {
    if (strcmp_P(name, PSTR("EEPROM")) == 0) {
      PrintEEProm();
    } 
    else if (strcmp_P(name, PSTR("ALL")) == 0) {
      DumpAll();
    }
    else {
      Serial.print("Error: Unknown DUMP Name: ");
      Serial.println(name);
    }
  }

  else {
    Serial.print(F("Error: Unknown Command: "));
    Serial.println(command);
  }

}

