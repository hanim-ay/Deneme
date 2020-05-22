#include "stm32f4xx.h"
#include "stdio.h"
#include "math.h"
//deneme1 Hanim Ay


#define OUT_X_L				0x28           //sayfa 29 https://www.st.com/resource/en/datasheet/lis3dsh.pdf
#define OUT_X_H				0x29
#define OUT_Y_L				0x2A
#define OUT_Y_H				0x2B
#define OUT_Z_L				0x2C
#define OUT_Z_H				0x2D
//#define anglexyz			0.0052

void DelayMs(int);
void SPI_init(void);
uint8_t SPI_Rx(uint8_t);
void SPI_Tx(uint8_t, uint8_t );

void initUART(void);
void USART_Puts(USART_TypeDef* USARTx,volatile char *s) ;
double xyz_Angle_Calculation(int16_t );


int8_t xl,yl,zl,xh,yh,zh;
int16_t x,y,z;
double x_angle,y_angle,z_angle;
char c[20];


int main()
{
	
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
GPIO_Init(GPIOD, &GPIO_InitStruct);
initUART();
SPI_init();



//DelayMs(100);

	//aci_hesabi=(90/(2^15));
SPI_Tx(0x20,0x77); //400 hz
SPI_Tx(0x24,0x00); //2g 

while(1)
 {
  //Rx SPI data
  xh=SPI_Rx(OUT_X_H);
	xl=SPI_Rx(OUT_X_L);
  yl=SPI_Rx(OUT_Y_L);
	yh=SPI_Rx(OUT_Y_H);
	zl=SPI_Rx(OUT_Z_L);
	zh=SPI_Rx(OUT_Z_H);
	 
	 x=(xh<<8)+xl;
	 y=(yh<<8)+yl;
	 z=(zh<<8)+zl;
	 
	// x_angle=(x*anglexyz);
	//y_angle=(y*anglexyz);
	// z_angle=(z*anglexyz);
	 
	 x_angle=xyz_Angle_Calculation(1);
	 y_angle=xyz_Angle_Calculation(2);
	 z_angle=xyz_Angle_Calculation(3);
	 
  //use data
  if (x<-2000) GPIO_SetBits(GPIOD,GPIO_Pin_12);
  else GPIO_ResetBits(GPIOD,GPIO_Pin_12);
  if (x>2000) GPIO_SetBits(GPIOD,GPIO_Pin_14);
  else GPIO_ResetBits(GPIOD,GPIO_Pin_14);
  if (y<-2000) GPIO_SetBits(GPIOD,GPIO_Pin_15);
  else GPIO_ResetBits(GPIOD,GPIO_Pin_15);
  if (y>2000) GPIO_SetBits(GPIOD,GPIO_Pin_13);
  else GPIO_ResetBits(GPIOD,GPIO_Pin_13);
	
	sprintf(c," %d %d %d \n",x,y,z);
	USART_Puts(USART2,c);
	sprintf(c," x y z eksenlerinde yaptigi aci: %.1f ",x_angle);
	USART_Puts(USART2,c);
	sprintf(c,"%.1f ",y_angle);
	USART_Puts(USART2,c);
	sprintf(c,"%.1f \n  ",z_angle);
	USART_Puts(USART2,c);
	DelayMs(500);
 }
}

void DelayMs(int miliseconds) {
	int i = 0, j = 0;
	for (i = 0; i < miliseconds; i++) {
		for (j = 0; j < 8388; j++) {

		}
	}
}

void SPI_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1);

	SPI_InitTypeDef SPI_InitStruct;
	SPI_InitStruct.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_2;
	SPI_InitStruct.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode=SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize=SPI_DataSize_8b;
	SPI_InitStruct.SPI_FirstBit=SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CPOL=SPI_CPOL_High;
	SPI_InitStruct.SPI_CPHA=SPI_CPHA_2Edge;
	SPI_InitStruct.SPI_NSS=SPI_NSS_Soft|SPI_NSSInternalSoft_Set;
	SPI_Init(SPI1,&SPI_InitStruct);
	GPIO_SetBits(GPIOE,GPIO_Pin_3);
	SPI_Cmd(SPI1,ENABLE);
}

void SPI_Tx(uint8_t adress, uint8_t data)
{
  GPIO_ResetBits(GPIOE,GPIO_Pin_3);
  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
  SPI_I2S_SendData(SPI1,adress);
  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
  SPI_I2S_ReceiveData(SPI1);
  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
  SPI_I2S_SendData(SPI1,data);
  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
  SPI_I2S_ReceiveData(SPI1);
  GPIO_SetBits(GPIOE,GPIO_Pin_3);
  
}


 uint8_t SPI_Rx(uint8_t address)
{
  GPIO_ResetBits(GPIOE,GPIO_Pin_3);
  address=0x80|address;
  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
  SPI_I2S_SendData(SPI1,address);
  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
  SPI_I2S_ReceiveData(SPI1);
  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
  SPI_I2S_SendData(SPI1,0x01);
  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
  
  GPIO_SetBits(GPIOE,GPIO_Pin_3);
  return SPI_I2S_ReceiveData(SPI1);
}
void initUART(void)
{
	/*Initialize clocks*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);

	/*Initialize UART Peripheral*/
	USART_InitTypeDef UsartStruct;
	UsartStruct.USART_BaudRate=9600;
	UsartStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	UsartStruct.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;
	UsartStruct.USART_Parity=USART_Parity_No;
	UsartStruct.USART_StopBits=USART_StopBits_1;
	UsartStruct.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART2,&UsartStruct);
	USART_Cmd(USART2,ENABLE);

	/*Initialize TX and RX Pins*/
	GPIO_InitTypeDef GPIOStruct;
	GPIOStruct.GPIO_Mode=GPIO_Mode_AF;
	GPIOStruct.GPIO_OType=GPIO_OType_PP;
	GPIOStruct.GPIO_Pin=GPIO_Pin_5 | GPIO_Pin_6;
	GPIOStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD,&GPIOStruct);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);
	

	/*Initialize NVIC to interrupt on RX*/
	NVIC_InitTypeDef NVICStruct;
	NVICStruct.NVIC_IRQChannel=USART2_IRQn;
	NVICStruct.NVIC_IRQChannelCmd=ENABLE;
	NVICStruct.NVIC_IRQChannelPreemptionPriority=1;
	NVICStruct.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVICStruct);
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
}

 void USART_Puts(USART_TypeDef* USARTx,volatile char *s)    //https://stm32f4-discovery.net/2014/04/library-04-connect-stm32f429-discovery-to-computer-with-usart/
{
 while(*s)
 {
  while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	 USART_SendData(USARTx,*s);
	 *s++;
 }	 
}

double xyz_Angle_Calculation(int16_t a)
{

	//anglex=atan(xaxis/sqrt(y^2+z^2))*180
	double x2,y2,z2,result;
	double accel_angle_x, accel_angle_y,accel_angle_z;
	 // Work out the squares
   x2 = (x*x);
   y2 = (y*y);
   z2 = (z*z);

  if(a==1) //X Axis
	{
   result=sqrt(y2+z2);
   result=x/result;
   accel_angle_x = atan(result);
	 accel_angle_x=accel_angle_x*180;
	 return accel_angle_x;
	}

   if(a==2)//Y Axis
	 {
   result=sqrt(x2+z2);
   result=y/result;
   accel_angle_y = atan(result);
	 accel_angle_y=accel_angle_y*180;
	 return accel_angle_y;
	 }
	if(a==3)//Z Axis
	{
   result=sqrt(x2+y2);
   result=z/result;
   accel_angle_z = atan(result);
	 accel_angle_z=accel_angle_z*180;
	 return accel_angle_z;
	}
	
}


