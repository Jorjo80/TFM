

static  void InicioFed(void)
{
	//Clear
	HAL_Delay(1000);
	send(ComClear, (sizeof(ComClear)/sizeof(ComClear[0])));
	HAL_Delay(3000);
	
	
	
	//Channel	
	//printf("Channel\n\r");
	send(WriteChannel,(sizeof(WriteChannel)/sizeof(WriteChannel[0])));
	receive();
	
	//Join Credential
	HAL_Delay(1000);
	send(WriteJoinCred,(sizeof(WriteJoinCred)/sizeof(WriteJoinCred[0])));
	receive();
	
	//Role
	HAL_Delay(1000);
	//printf("Role\n\r");
  send(WriteRole,(sizeof(WriteRole)/sizeof(WriteRole[0])));
	receive();
	
	
	

	HAL_Delay(1000);
	//IFUP
	send(ifup,(sizeof(ifup)/sizeof(ifup[0])));
	receive();
	HAL_Delay(5000);
}