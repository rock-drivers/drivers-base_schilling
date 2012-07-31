#include "Driver.hpp"
#include "SchillingRaw.hpp"
#include <act_schilling/Error.hpp>
#include <base/logging.h>
#include <iostream>
#include <map>
#include <string.h>

using namespace base_schilling;
using namespace std;
using namespace oro_marum;




Driver::Driver(int max_packet_size, bool extract_last)
  : iodrivers_base::Driver(max_packet_size,extract_last)
{
    mAddress = 0x00;
}

void Driver::open(std::string const& uri)
{
  openURI(uri);
}
 
void Driver::setAddress(uint const& address)
{
  mAddress = address;
}

void Driver::setRegs(std::vector<uint8_t> const& regs)
{
  mRegMap.clear();
  for(int i=0; i < regs.size(); i++)
  {
    mRegMap[regs[i]] = 0;
  }
}

uint8_t Driver::getReg(uint8_t reg)
{
  return mRegMap[reg];
}

void Driver::sendReadMsg()
{
  int msgLen = mRegMap.size() + 1;
  std::vector<uint8_t> msg(SCHILL_LEN_HEADER + msgLen);
  schilling_raw::MsgHeader *header = (schilling_raw::MsgHeader*)msg.data();
  header->type = SCHILL_CMD_MSG;
  header->address = mAddress;
  header->length = msgLen;
  header->cmd = SCHILL_CMD_READ;
  
  map<uint8_t,uint8_t>::iterator it;
  int i = 0;
  for( it = mRegMap.begin(); it != mRegMap.end(); it++ ) {
    msg[SCHILL_LEN_HEADER+i] = it->first;
    i++;
  }
  setCS((char*)msg.data());
  writePacket(msg.data(),msg.size());
}
 
void Driver::read(int timeout)
{
  char buffer[MAX_PACKET_SIZE];
  size_t packet_size = readPacket(reinterpret_cast<uint8_t *>( buffer), MAX_PACKET_SIZE, timeout);
  if(packet_size<SCHILL_LEN_HEADER){
    return;
  }
  checkCS(buffer);
  int len = 0;
  schilling_raw::MsgHeader *header = (schilling_raw::MsgHeader*)buffer;
  if(SCHILL_REPL_UNCHG_MSG==header->type || 
    SCHILL_REPL_CHG_MSG==header->type){
    len = header->length;
  }
  if(mRegMap.size() != len){
    return;
  }
  map<uint8_t,uint8_t>::iterator it;
  int i = 0;
  char szOut[128];
  sprintf(szOut,"got msg, len %i :",len);
  for( it = mRegMap.begin(); it != mRegMap.end(); it++ ) {
    it->second = (uint8_t)(unsigned char)buffer[3+i]; 
    i++;
    sprintf(szOut+strlen(szOut),"%02x | ",(int8_t)buffer[3+i]);    
  }
  LOG_DEBUG("%s",szOut);
  return;
}

int Driver::extractPacket (uint8_t const *buffer, size_t buffer_size) const
{
  for (size_t i = 0; i < buffer_size; i++) {
    if (buffer[i] == SCHILL_REPL_UNCHG_MSG ||
	buffer[i] == SCHILL_REPL_CHG_MSG)
    {
      if(i){
	return -i;
      }
      if(buffer_size<3){
	return 0;
      }
      size_t len = ((schilling_raw::MsgHeader*)buffer)->length + SCHILL_LEN_HEADER;
      if(buffer_size >= len){
	return len;
      }
      return 0;
    }
  }
  return -buffer_size;
}

void Driver::setCS(char *cData)
{
  if (!cData){
    return;
  }
  schilling_raw::MsgHeader *header = (schilling_raw::MsgHeader*)cData;
  int length = header->length + SCHILL_LEN_HEADER;
  cData[length-1] = 0;
  for (int i=0;i<length-1;i++){
    cData[length-1] += cData[i];
  }
  return;
}

void Driver::checkCS(const char *cData)
{
  if (!cData){
        throw MarError(MARSTR_PARAMINV,MARERROR_PARAMINV);
  }
  schilling_raw::MsgHeader  *header = (schilling_raw::MsgHeader*)cData;
  int length = header->length + SCHILL_LEN_HEADER;
  char cCs = 0;
  for (int i=0;i<length-1;i++){
    cCs += cData[i];
  }
  if (cCs != cData[length-1]){
    throw MarError(MARSTR_CHECKSUM,MARERROR_CHECKSUM);
  }
  return;
}