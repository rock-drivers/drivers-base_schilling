#include "Driver.hpp"
#include "SchillingRaw.hpp"
#include <iostream>
#include <map>

using namespace base_schilling;
using namespace std;


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
  //TODO: handle stat msg
  if(mRegMap.size() != len){
    return;
  }
  map<uint8_t,uint8_t>::iterator it;
  int i = 0;
  for( it = mRegMap.begin(); it != mRegMap.end(); it++ ) {
    it->second = (uint8_t)(unsigned char)buffer[3+i]; 
    i++;
    //cout <<"length " <<len <<" got value: " <<(int8_t)buffer[3+i] <<" "<<it->second <<endl; 
    
  }
  //char szOut[24];
  //sprintf(szOut,"%02x %02x ",(int8_t)buffer[3],(int8_t)buffer[4]);
  //cout <<"length " <<len <<" got values: " <<szOut <<endl; 
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
      size_t len = ((schilling_raw::MsgHeader*)buffer)->length + SCHILL_LEN_HEADER;
      if(buffer_size >= len){
	return len;
      }
      return 0;
    }
  }
  return -buffer_size;
  //TODO: handle stat msg
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
    throw std::runtime_error("empty Data");
  }
  schilling_raw::MsgHeader  *header = (schilling_raw::MsgHeader*)cData;
  int length = header->length + SCHILL_LEN_HEADER;
  char cCs = 0;
  for (int i=0;i<length-1;i++){
    cCs += cData[i];
  }
  if (cCs != cData[length-1]){
    throw std::runtime_error("invalid checksum");
  }
  return;
}