#include <inttypes.h>

#define CMD_WRITE 0x10
#define CMD_READ 0x11
#define CMD_DELETE 0x12

#define CKS_POS 4

uint8_t PacketMarker = 0;

uint8_t ifup[] = {0x00, 0x00, CMD_WRITE, 0x08, 0x18};
uint8_t ifdown[] = {0x00, 0x00, CMD_WRITE, 0x07, 0x17};
uint8_t ComClear[] = {0x00, 0x00, CMD_WRITE, 0x00, 0x10};
uint8_t Reset[] = {0x00, 0x00, CMD_WRITE, 0x03, 0x13};
uint8_t Status[] = {0x00, 0x00, CMD_READ, 0x05, 0x14}; //Look the answer in the KBI pdf
uint8_t Uptime[] = {0x00, 0x00, CMD_READ, 0x02, 0x13};
uint8_t ThreadVersion[] = {0x00, 0x00, CMD_READ, 0x01, 0x10};
uint8_t Ping[] = {0x00, 0x12, CMD_WRITE, 0x06}; //A�adir CKS + direccion ipv6 (16 bytes)+ PayloadRandom
uint8_t OpenSocket[] = {0x00, 0x02, CMD_WRITE, 0x09, 0x012, 0x30, 0x39};
uint8_t OpenSocket2[] = {0x00, 0x02, CMD_WRITE, 0x09, 0x012, 0x30, 0x30};//abrir socket en puerto 12345
uint8_t CloseSocket[] =  {0x00, 0x02, CMD_DELETE, 0x09, 0x012, 0x30, 0x39};
uint8_t ReadEui64Address[] = {0x00, 0x00, CMD_READ, 0x0E, 0x0F};
uint8_t ReadPowerMode[] ={0x00, 0x00, CMD_READ, 0x0f, 0x1e};
uint8_t PowerModeOn[] ={0x00, 0x00, CMD_WRITE, 0x0f, 0x1f};
uint8_t PowerModeOff[] ={0x00, 0x00, CMD_DELETE, 0x0f, 0x1d};
uint8_t ReadTxPower[] = {0x00, 0x00, CMD_READ, 0x10, 0x01};
uint8_t WriteTxPower[] = {0x00, 0x01, CMD_WRITE,0x10, 0x0a, 0x0b}; //-4dbm
uint8_t ReadChannel[] = {0x00, 0x00, CMD_READ, 0x12, 0x03};
uint8_t WriteChannel[] = {0x00, 0x01, CMD_WRITE, 0x12, 0x08}; //Channel 15 predefined, change last uint8_t to change channell between 11-26
uint8_t ReadRole[] = {0x00, 0x00, CMD_READ, 0x19, 0x08};
uint8_t SendHello[] = {0x00, 0x1a, CMD_WRITE, 0x2f, 0x06, 0x30, 0x39, 0x30, 0x30, 0xab, 0xcd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x21};
uint8_t ReadIp[] = {0x00, 0x00, CMD_READ, 0x20, 0x31};
uint8_t WriteIPLeader[] = {0x00, 0x10, CMD_WRITE, 0x20, 0x60, 0xab, 0xcd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34};
uint8_t WriteIP[] = {0x00, 0x10, CMD_WRITE, 0x20, 0x60, 0xab, 0xcd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x12};
uint8_t JoinPort[] = {0x00, 0x02, CMD_WRITE,0x21, 0x00, 0x30, 0x39};

uint8_t OOB[] = {0x00, 0x00, CMD_WRITE, 0x24,0x34}; 
uint8_t WritePANID[] = {0x00, 0x02, CMD_WRITE, 0x11, 0x25};
uint8_t WriteNetName[] = {0x00,0x09, CMD_WRITE, 0x14, 0x67};
uint8_t WriteMLocPref[] = {0x00, 0x08, CMD_WRITE, 0x1c, 0x89};
uint8_t WriteMK[] = {0x00, 0x10, CMD_WRITE, 0x15, 0x15};
uint8_t WriteExtPID[] = {0x00, 0x08, CMD_WRITE, 0x13, 0x83};
uint8_t WriteComCred[] = {0x00, 0x0a, CMD_WRITE, 0x16, 0x07};


uint8_t route[] = {0x00, 0x13, CMD_WRITE, 0x27, 0x00, 0xab, 0xcd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x12, 0x60, 0x00, 0x01};
/////////////////////
// ROLE CMDS ////////
/////////////////////

uint8_t WriteRole[] = {0x00, 0x01, CMD_WRITE, 0x19, 0x0B}; //FED, look the table below and change the last uint8_t

uint8_t router = 0x01;
uint8_t reed = 0x02;
uint8_t fed = 0x03;
uint8_t med = 0x04;
uint8_t sed = 0x05;
uint8_t leader = 0x06;

/* //CKS for WriteRole
  0 = Not Configured
  1 = Router -- CKS = 09
  2 = REED (Router Elegible End Device) -- CKS = 0A
  3 = FED (Full End Device) -- CKS = OB
  4 = MED (Minimal End Device) -- CKS = 0C
  5 = SED (Sleepy End Device) CKS = 0D
  6 = Leader -- CKS = 0E

  Modifications conditions
  Status must be none, except none - saved configuration. Also allowed when status is joined to trigger some
  role transition mechanisms (SED ? MED, SED ? FED, MED? FED).
*/

uint8_t ReadJoinCred[] = {0x00, 0x00, CMD_READ, 0x17, 0x06};
uint8_t WriteJoinCred[] = {0x00, 0x10, CMD_WRITE, 0x17, 0x69};
uint8_t CommissionerOn[] = {0x00, 0x00, CMD_WRITE, 0x1b, 0x0b}; 
uint8_t CommissionerOff[] = {0x00, 0x00, CMD_DELETE, 0x1b, 0x09};
uint8_t AddJoiner[] = {0x00, 0x18 ,0x10, 0x18, 0xC5,0x84, 0x04,0xD2, 0x00,0x00, 0x00,0x05,0xFC,0x38, 0x34, 0x30, 0x34, 0x44, 0x32, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x35, 0x46, 0x43}; 
	