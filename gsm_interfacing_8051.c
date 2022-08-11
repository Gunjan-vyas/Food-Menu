#include<reg52.h>

void lcd_clr(void);
void lcd_cmd(unsigned char a);
void lcd_data(unsigned char b);
void lcd_init(void);
void lcd_str(unsigned char *str);
void send_char(unsigned char b);
void send_str(unsigned char *str);
void sms_send(unsigned char[]);
void sms_rec(void);
void delay_ms(unsigned int i);
void clear(void);
void gsm_link(void);
void UART_init(void);
void led(unsigned int n);
void led_rst(void);
void rcv_sms(void);
void set_nmbr(void);

sbit meal1=P0^0;
sbit meal2=P0^1;
sbit meal3=P0^2;
sbit meal4=P0^3;

sbit rs=P2^0;
sbit en=P2^1;
sfr ldata=0x90;//port1 
sbit L3=P2^7;


sbit A = P3^2;
sbit B1 = P3^3;
sbit C = P3^4;
sbit D = P3^5;
sbit E = P3^6;
sbit F = P2^6;
sbit G = P2^5;

unsigned char number[] = "9717845977";
unsigned char a[90],mesg_rec;
unsigned int i,count,meal;

void ser_int() interrupt 4             
{
	a[i]=SBUF;
	i++;

if(RI==1)
{
L3=0;
RI=0;
mesg_rec=1;
}

if(TI==1)
{
TI=0;
}

}

void main(){

	led_rst();

	lcd_init();
	lcd_str("   WELCOME TO   ");
	lcd_cmd(0xc0);
	lcd_str("   MY PROJECT   ");
	delay_ms(65); 
	lcd_clr();
	lcd_str("   Micro Proj   ");
	delay_ms(65);
	lcd_cmd(0xc0);
	lcd_str("   VIT, Vellore  ");
	delay_ms(65);
	
	meal1=0; meal2 =0; meal3=0; meal4=0;
	
	
	while(1){
	
	lcd_clr();
 if(meal1==1){
		meal=1;
		lcd_str("MORNING");
		//meal[]="Will make breakfast for you!";
		break;
	}	
	else if(meal2==1){
		meal=2;
		lcd_str("LUNCH");
		//meal[]="Will make breakfast for you!";
		break;
	}
	else if(meal3==1){
		meal=3;
		lcd_str("EVENING");
		//meal[]="Will make breakfast for you!";
		break;
	}
	else if(meal4==1){
		meal=4;
		lcd_str("DINNER");
		//meal[]="Will make breakfast for you!";
		break;
	}
	else{
		L3=0;
		lcd_str("WHICH MEAL?");
	}
	delay_ms(65);
}

	UART_init();
	gsm_link();
  sms_send("Hello Student!\n Check out the Menu for today using the format:\n ?menui,\nwhere i = meals(1-4).\nWant to eat?Send Yes as response.\n -VIT, VELLORE");
	
	count=1;
	while(1)
{
	rcv_sms();
	clear();
	mesg_rec=0;
	i=0;
	lcd_clr();
	lcd_str("RECEIVING...");
	L3=1;
	delay_ms(650);
	if(mesg_rec==1)
	{	/*
		lcd_clr();
		lcd_str(a);
		delay_ms(650);*/
		
//+CMT
		if(a[2]=='+'&&a[3]=='C'&&a[4]=='M'&&a[5]=='T')
		{
		lcd_clr();
    lcd_str("MSG RECIEVED");
			delay_ms(650);
			set_nmbr();
			lcd_clr();
			lcd_str(number);
			delay_ms(200);
			if(a[51]=='?'&&a[52]=='m'&&a[53]=='e'&&a[54]=='n'&&a[55]=='u')			
			{							
				if(a[56]=='1')
					sms_send("Morning Meals:\nPlain Dosa,Sambar,paratha,Bread butter,Tea,milk,Coffee.\n -VIT, VELLORE"); 
				else if(a[56]=='2')
					sms_send("Afternoon Meals:\nRice,Pulse,sambar,Roti,sabji.\n -VIT, VELLORE"); 
				else if(a[56]=='3')
					sms_send("Evening Snacks:\nBread Pakoda/Samosa,Tea,Coffee.\n -VIT, VELLORE"); 
				else if(a[56]=='4')
					sms_send("Dinner:\nPuri,Cholle,Rice,Sambar,Pulse,Milk,Coffee.\n -VIT, VELLORE"); 
				else
					sms_send("Invalid!");
			}
			else if(a[51]=='Y'&&a[52]=='e'&&a[53]=='s')
		{
			sms_send("Will make a plate for you!");
			led(count++);
		}
			else
				sms_send("INCORRECT format");
		}
		
	}
	
 }

}
	
void delay_ms(unsigned int t)
{
unsigned int j,k;
	for(j=0;j<t;j++)
	for(k=0;k<1275;k++);  //1ms delay
}

void UART_init()
{
	SCON = 0x50;
	TMOD = 0x20;
	TH1 = 0xFD;   //baud rate = 9600
	TR1 = 1;
}

void gsm_link(){
		
	send_str("ATE0");
	send_char(0x0D);
	delay_ms(100);
	
	send_str("AT+IPR=9600");		//Set the baud rate
	send_char(0x0D);
	delay_ms(100);
	
	send_str("AT");
	send_char(0x0D);
	delay_ms(100);
	
	clear();
}

void send_char(unsigned char chr){
	
	SBUF=chr;
	while(TI==0);
	TI=0;
}

void send_str(unsigned char *str){

	while(*str)
	{
		send_char(*str++);
	}
}

void sms_send(unsigned char mssg[])
{
	IE = 0x00; /// disable interrupt
	lcd_clr();
  lcd_str("MSG SENDING...");
	
	send_str("AT+CMGF=1");
	send_char(0x0D);
	delay_ms(100);
	
	send_str("AT+CMGS=");
	send_char(0x22);
	send_str(number);
	send_char(0x22);
	send_char(0x0D);
	delay_ms(1000);
	send_str(mssg);
	send_char(0x1A);   // ctrl+z
	delay_ms(500);
	clear();
	
	lcd_clr();
	lcd_str("   SMS SENT   ");
	
	IE = 0x90;	///Enable interrupt - serial
}

void lcd_init()
{
	
	lcd_cmd(0x38);
	lcd_cmd(0x0c);
	lcd_cmd(0x01);
	lcd_cmd(0x80);
}

void lcd_cmd(unsigned char a)
{
  rs=0;				//cmd 
	ldata=a;
	en=1;
	delay_ms(5);
	en=0;
	delay_ms(5);
}
void lcd_data(unsigned char b)
{
  rs=1;				//data 
	ldata=b;
	en=1;
	delay_ms(5);
	en=0;
	delay_ms(5);
}

void lcd_str(unsigned char *str)
{
while(*str)
{
	lcd_data(*str++);
}
}

void lcd_clr(){
	lcd_cmd(0x01);
	lcd_cmd(0x80);
}

void rcv_sms(){
	IE=0x00;
	send_str("AT+CNMI=1,2,0,0,0");
	send_char(0x0D);
	delay_ms(100);
	IE=0x90;
}


void set_nmbr(){
	unsigned int num_pointer,k=12;
	for(num_pointer=0;num_pointer<10;num_pointer++){
		if(k<=21){
		number[num_pointer]=a[k];
		k++;
		}
		else 
			break;
	}
}


void clear(void)
{
	for(i=0;i<90;i++)
		a[i]=0;
}

void led(unsigned int n){
	led_rst();
	delay_ms(50);
	switch(n){
		case 1: B1=1;
						C=1;
						break;
		case 2: A=1;
						B1=1;
						G=1;
						D=1;
						E=1;
						break;
		case 3: A=1;
						B1=1;
						G=1;
						C=1;
						D=1;
						break;
		case 4: B1=1;
						F=1;
						C=1;
						G=1;
						break;
		case 5: A=1;
						F=1;
						G=1;
						C=1;
						D=1;
						break;
		case 6: A=1;
						E=1;
						F=1;
						G=1;
						C=1;
						D=1;
						break;
		case 7: A=1;
						F=1;
						B1=1;
						C=1;
						break;
		case 8: A=1;
						B1=1;
						E=1;
						F=1;
						G=1;
						C=1;
						D=1;
						break;
		case 9: A=1;
						B1=1;
						F=1;
						G=1;
						C=1;
						D=1;
						break;
		default: led_rst();
						break;
	}
}

void led_rst(){
	A=0;
	B1=0;
	C=0;
	D=0;
	E=0;
	F=0;
	G=0;
}