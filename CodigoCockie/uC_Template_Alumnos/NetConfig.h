#include "COBS_Kirale.h"

uint8_t channel = 0x0B;
uint8_t PANID[] = {0x12, 0x34}; // PAN ID = 0x1234
uint8_t NetName[] = {0x4d, 0x79, 0x4e, 0x65, 0x74,0x77, 0x6f, 0x72, 0x6b};
uint8_t MeshLocPrefix[] = {0xfd, 0x12, 0x34, 0x00,0x38, 0x00, 0x00, 0x00, 0x00};
uint8_t MasterKey[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
uint8_t ExtPANID[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
uint8_t ComCred[] = {0x4d, 0x79,0x50, 0x61, 0x73, 0x73,0x77, 0x6f, 0x72, 0x64};
uint8_t JoinCred[] = {0x38, 0x34, 0x30, 0x34, 0x44, 0x32, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x35, 0x46, 0x43};


static void InicioLeader(void);
static void InicioFed(void);
//static uint8_t XOR_CKS(uint8_t *frame, size_t size);



/*static  void InicioFed(void)
{
	send(ComClear, (sizeof(ComClear)/sizeof(ComClear[0])));
	HAL_Delay(3000);
	
	send(OOB, sizeof(OOB)/sizeof(OOB[0]));
	receive();
	HAL_Delay(1000);
	
	//Role
	HAL_Delay(1000);
	//printf("Role\n\r");		
	size_t size1 = sizeof(WriteRole);
	size_t size2 = sizeof(med);
	uint8_t cmdRole[size1+size2];
	for(int i = 0; i<size1;i++)
	{
		cmdRole[i] = WriteRole[i];
	}
	for(int i = size1; i<(size1+size2); i++)
	{
		cmdRole[i] = med;
	}
	//unite(WriteRole, &leader, cmdRole, size1, doble);
  send(cmdRole,(sizeof(cmdRole)/sizeof(cmdRole[0])));
	receive();
	HAL_Delay(1000);
	
	//Channel	
	//printf("Channel\n\r");
	
	size1 = sizeof(WriteChannel);
	size2 = sizeof(channel);
	uint8_t cmdChannel[size1+size2];
	for(int i = 0; i<size1;i++)
	{
		cmdChannel[i] = WriteChannel[i];
	}
	for(int i = size1; i<(size1+size2); i++)
	{
		cmdChannel[i] = channel;
	}
	send(cmdChannel,(sizeof(cmdChannel)/sizeof(cmdChannel[0])));
	receive();
	HAL_Delay(1000);
	
	
	//WritePANID
	size1 = sizeof(WritePANID);
	size2 = sizeof(PANID);
	uint8_t cmdWPANID[size1+size2];
	for(int i = 0; i<size1;i++)
	{
		cmdWPANID[i] = WritePANID[i];
	}
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdWPANID[i] = PANID[p];
	}	
	//unite(WritePANID, PANID, cmdWPANID);
	send(cmdWPANID, sizeof(cmdWPANID)/sizeof(cmdWPANID[0]));
	receive();
	HAL_Delay(1000);
	
	
	size1 = sizeof(WriteNetName);
	size2 = sizeof(NetName);
	uint8_t cmdWNetName[size1+size2];
	//unite(WriteNetName, NetName, cmdWNetName);
	for(int i = 0; i<size1;i++)
	{
		cmdWNetName[i] = WriteNetName[i];
	}
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdWNetName[i] = NetName[p];
	}	
	send(cmdWNetName, sizeof(cmdWNetName)/sizeof(cmdWNetName[0]));
	receive();
	HAL_Delay(1000);
	

	size1 = sizeof(WriteMLocPref);
	size2 = sizeof(MeshLocPrefix);
	uint8_t cmdWMLP[size1 + size2];
	//unite(WriteMLocPref, MeshLocPrefix, cmdWMLP);
	for(int i = 0; i<size1;i++)
	{
		cmdWMLP[i] = WriteMLocPref[i];
	}
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdWMLP[i] = MeshLocPrefix[p];
	}	

	send(cmdWMLP, sizeof(cmdWMLP)/sizeof(cmdWMLP[0]));
	receive();
	HAL_Delay(1000);
	
	
	size1 = sizeof(WriteMK);
	size2 = sizeof(MasterKey);
	uint8_t cmdMK[size1 + size2];
	//unite(WriteMK, MasterKey, cmdMK);
	for(int i = 0; i<size1;i++)
	{
		cmdMK[i] = WriteMK[i];
	}
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdMK[i] = MasterKey[p];
	}	
	send(cmdMK, sizeof(cmdMK)/sizeof(cmdMK[0]));
	receive();
	HAL_Delay(1000);
	
	////
	
	size1 = sizeof(WriteExtPID);
	size2 = sizeof(ExtPANID);
	uint8_t cmdExtPID[size1 + size2];
	//unite(WriteExtPID, ExtPANID, cmdExtPID);
	for(int i = 0; i<size1;i++)
	{
		cmdExtPID[i] = WriteExtPID[i];
	}
	HAL_Delay(100);
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdExtPID[i] = ExtPANID[p];
	}	
	
	send(cmdExtPID, sizeof(cmdExtPID)/sizeof(cmdExtPID[0]));
	receive();
	HAL_Delay(1000);
	
	//Join Credential

	size1 = sizeof(WriteComCred);
	size2 = sizeof(ComCred);
	uint8_t cmdComCred[size1 + size2];
	//unite(WriteComCred, ComCred, cmdComCred);
	for(int i = 0; i<size1;i++)
	{
		cmdComCred[i] = WriteComCred[i];
	}
	HAL_Delay(100);
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdComCred[i] = ComCred[p];
	}	
	
	send(cmdComCred,(sizeof(cmdComCred)/sizeof(cmdComCred[0])));
	receive();
	HAL_Delay(1000);
	
	
	
	size1 = sizeof(WriteJoinCred);
	size2 = sizeof(JoinCred);
	uint8_t cmdJoinCred[size1 + size2];
	//unite(WriteJoinCred, JoinCred, cmdJoinCred);
	for(int i = 0; i<size1;i++)
	{
		cmdJoinCred[i] = WriteJoinCred[i];
	}
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdJoinCred[i] = JoinCred[p];
	}	
	
	send(cmdJoinCred,(sizeof(cmdJoinCred)/sizeof(cmdJoinCred[0])));
	receive();	
	HAL_Delay(1000);

	
	//IFUP
	send(ifup,(sizeof(ifup)/sizeof(ifup[0])));
	receive();
	HAL_Delay(5000);
	
	//uint8_t *SetIP;
	//unite(WriteIP,IPFed, SetIP);
	send(WriteIP, sizeof(WriteIP)/sizeof(WriteIP[0]));
	receive();
	HAL_Delay(1000);	
	
}*/


/*static  void InicioLeader(void)
{
	//Clear
	send(ComClear, (sizeof(ComClear)/sizeof(ComClear[0])));
	HAL_Delay(3000);
	
	send(OOB, sizeof(OOB)/sizeof(OOB[0]));
	receive();
	HAL_Delay(1000);
	
	//Role
	HAL_Delay(1000);
	//printf("Role\n\r");		
	size_t size1 = sizeof(WriteRole);
	size_t size2 = sizeof(fed);
	uint8_t cmdRole[size1+size2];
	for(int i = 0; i<size1;i++)
	{
		cmdRole[i] = WriteRole[i];
	}
	for(int i = size1; i<(size1+size2); i++)
	{
		cmdRole[i] = fed
		;
	}
	//unite(WriteRole, &leader, cmdRole, size1, doble);
  send(cmdRole,(sizeof(cmdRole)/sizeof(cmdRole[0])));
	receive();
	HAL_Delay(1000);
	
	//Channel	
	//printf("Channel\n\r");
	
	size1 = sizeof(WriteChannel);
	size2 = sizeof(channel);
	uint8_t cmdChannel[size1+size2];
	for(int i = 0; i<size1;i++)
	{
		cmdChannel[i] = WriteChannel[i];
	}
	for(int i = size1; i<(size1+size2); i++)
	{
		cmdChannel[i] = channel;
	}
	send(cmdChannel,(sizeof(cmdChannel)/sizeof(cmdChannel[0])));
	receive();
	HAL_Delay(1000);
	
	
	//WritePANID
	size1 = sizeof(WritePANID);
	size2 = sizeof(PANID);
	uint8_t cmdWPANID[size1+size2];
	for(int i = 0; i<size1;i++)
	{
		cmdWPANID[i] = WritePANID[i];
	}
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdWPANID[i] = PANID[p];
	}	
	//unite(WritePANID, PANID, cmdWPANID);
	send(cmdWPANID, sizeof(cmdWPANID)/sizeof(cmdWPANID[0]));
	receive();
	HAL_Delay(1000);
	
	
	size1 = sizeof(WriteNetName);
	size2 = sizeof(NetName);
	uint8_t cmdWNetName[size1+size2];
	//unite(WriteNetName, NetName, cmdWNetName);
	for(int i = 0; i<size1;i++)
	{
		cmdWNetName[i] = WriteNetName[i];
	}
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdWNetName[i] = NetName[p];
	}	
	send(cmdWNetName, sizeof(cmdWNetName)/sizeof(cmdWNetName[0]));
	receive();
	HAL_Delay(1000);
	

	size1 = sizeof(WriteMLocPref);
	size2 = sizeof(MeshLocPrefix);
	uint8_t cmdWMLP[size1 + size2];
	//unite(WriteMLocPref, MeshLocPrefix, cmdWMLP);
	for(int i = 0; i<size1;i++)
	{
		cmdWMLP[i] = WriteMLocPref[i];
	}
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdWMLP[i] = MeshLocPrefix[p];
	}	

	send(cmdWMLP, sizeof(cmdWMLP)/sizeof(cmdWMLP[0]) );
	receive();
	HAL_Delay(1000);
	
	
	size1 = sizeof(WriteMK);
	size2 = sizeof(MasterKey);
	uint8_t cmdMK[size1 + size2];
	//unite(WriteMK, MasterKey, cmdMK);
	for(int i = 0; i<size1;i++)
	{
		cmdMK[i] = WriteMK[i];
	}
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdMK[i] = MasterKey[p];
	}	
	send(cmdMK, sizeof(cmdMK)/sizeof(cmdMK[0]));
	receive();
	HAL_Delay(1000);
	
	////
	
	size1 = sizeof(WriteExtPID);
	size2 = sizeof(ExtPANID);
	uint8_t cmdExtPID[size1 + size2];
	//unite(WriteExtPID, ExtPANID, cmdExtPID);
	for(int i = 0; i<size1;i++)
	{
		cmdExtPID[i] = WriteExtPID[i];
	}
	HAL_Delay(100);
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdExtPID[i] = ExtPANID[p];
	}	
	
	send(cmdExtPID, sizeof(cmdExtPID)/sizeof(cmdExtPID[0]));
	receive();
	HAL_Delay(1000);
	
	//Join Credential

	/////
	
	
	size1 = sizeof(WriteJoinCred);
	size2 = sizeof(JoinCred);
	uint8_t cmdJoinCred[size1 + size2];
	//unite(WriteJoinCred, JoinCred, cmdJoinCred);
	for(int i = 0; i<size1;i++)
	{
		cmdJoinCred[i] = WriteJoinCred[i];
	}
	HAL_Delay(100);
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdJoinCred[i] = JoinCred[p];
	}	
	
	send(cmdJoinCred,(sizeof(cmdJoinCred)/sizeof(cmdJoinCred[0])));
	receive();	
	HAL_Delay(1000);
	
	size1 = sizeof(WriteComCred);
	size2 = sizeof(ComCred);
	uint8_t cmdComCred[size1 + size2];
	//unite(WriteComCred, ComCred, cmdComCred);
	for(int i = 0; i<size1;i++)
	{
		cmdComCred[i] = WriteComCred[i];
	}
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdComCred[i] = ComCred[p];
	}	
	
	send(cmdComCred,(sizeof(cmdComCred)/sizeof(cmdComCred[0])));
	receive();
	HAL_Delay(1000);
	
	
	//IFUP
	send(ifup,(sizeof(ifup)/sizeof(ifup[0])), );
	receive();
	HAL_Delay(7000);
	
	send(WriteIPLeader, sizeof(WriteIPLeader)/sizeof(WriteIPLeader[0]));
	receive();
	HAL_Delay(1000);
	
	/*send(CommissionerOn,sizeof(CommissionerOn)/sizeof(CommissionerOn[0]),);
	receive();
	HAL_Delay(1000);
	
	send(AddBR,sizeof(AddBR)/sizeof(AddBR[0]),);
	receive();
	HAL_Delay(1000);
	
	send(AddJoiner,sizeof(AddJoiner)/sizeof(AddJoiner[0]),);
	receive();
	HAL_Delay(1000);
}*/