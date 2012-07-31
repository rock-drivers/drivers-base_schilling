#ifndef BASE_SCHILLING_SCHILLINGRAW_HPP
#define BASE_SCHILLING_SCHILLINGRAW_HPP

namespace schilling_raw
{
  #define SCHILL_LEN_HEADER         	4
  #define SCHILL_CMD_READ    		0x72
  #define SCHILL_CMD_WRITE   		0x77
  #define SCHILL_CMD_SET     		0x73
  #define SCHILL_CMD_RESET   		0x74
  
  #define SCHILL_CMD_MSG 		0xC4
  #define SCHILL_STAT_UNCHG_MSG 	0x06
  #define SCHILL_STAT_CHG_MSG 		0x16
  #define SCHILL_REPL_UNCHG_MSG 	0xF2
  #define SCHILL_REPL_CHG_MSG 		0xD2
  #define SCHILL_COMM_STAT_MSG 		0xE2
  
  struct MsgHeader
  {
    unsigned char type;
    unsigned char address;
    unsigned char length;
    unsigned char cmd;
  };

  struct Msg
  {
    MsgHeader header;
    char*  msg_body;
  }; 
  
}

#endif
