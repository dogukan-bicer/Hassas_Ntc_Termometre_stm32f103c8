#include "stm32f10x.h"                  
#include "math.h"
/*
kurulum

PA8 -> RS
PA9 -> RW
PA10-> E

PA4 -> DB4
PA5 -> DB5
PA6 -> DB6
PA7 -> DB7
PA2 -> 10k ntc
3.3v -> 10k ntc
Gnd-> 10k res

*/
float sicaklik = 0;
char analog[10];
char analog2[10];
int isi;
int32_t deger;
int a=0;
int rms[30];
void int2char(int num, char str[])
{
  char lstr[30];
   int cnt = 0;
  int div = 10;
  int j = 0;

	
	
while( num >= div)
{
	lstr[cnt] = num % div + 0x30;
	num /= 10;
	cnt++;
}
	lstr[cnt] = num + 0x30;
for(j= cnt ; j >=0;j--)
{
	str[cnt-j] = lstr[j];
}
   
   
}

void DelayUs(unsigned long t)
{
	for(;t>0;t--)
		{
		 SysTick->LOAD = 72;
	   SysTick->VAL = 0;
	   while((SysTick->CTRL & 0x00010000) == 0);
		}
}


void lcd_cmd(unsigned char data)
{
	//pin_output(0,11);
	GPIOA->ODR &=~0x100;//lcd_rs(LOW);
	GPIOA->ODR &=~0x200;//lcd_rw(LOW);
	DelayUs(10);
	GPIOA->ODR |=0x400;//lcd_e(HIGH);
	DelayUs(5);
	GPIOA->ODR &= 0xff0f;
	GPIOA->ODR |= (data & 0x00f0);
	DelayUs(10);
	GPIOA->ODR &=~0x400;//lcd_e(LOW);
	
	DelayUs(20);
	
	GPIOA->ODR |=0x400;//lcd_e(HIGH);
	DelayUs(5);
	GPIOA->ODR &= 0xff0f;
	GPIOA->ODR |= ((data << 4) & 0x00f0);
	DelayUs(10);
	GPIOA->ODR &=~0x400;//lcd_e(LOW);
}

void lcd_data(unsigned char data)
{
	//pin_output(4,7);
	GPIOA->ODR |=0x100;//lcd_rs(HIGH);
	GPIOA->ODR &=~0x200;//lcd_rw(LOW);
	DelayUs(10);
	GPIOA->ODR |=0x400;//lcd_e(HIGH);
	DelayUs(5);
	GPIOA->ODR &= 0xff0f;
	GPIOA->ODR |= (data & 0x00f0);
	DelayUs(10);
	GPIOA->ODR &=~0x400;//lcd_e(LOW);
	
	DelayUs(20);
	
	GPIOA->ODR |=0x400;//lcd_e(HIGH)
	DelayUs(5);
	GPIOA->ODR &= 0xff0f;
	GPIOA->ODR |= ((data << 4) & 0x00f0);
	DelayUs(10);
	GPIOA->ODR &=~0x400;//lcd_e(LOW)
}

void lcd_send( char str[])
{
	int i = 0;
		while(str[i])
		{
			lcd_data(str[i]);
			i++;
			DelayUs(100);
		}

}


void lcd_msg(unsigned char line_1_2, unsigned char pos_0_16, char msg[])
{
	short pos = 0;
	if(line_1_2==1)
	{
		pos = 0;
	}
	else if(line_1_2==2)
	{
		pos = 0x40;
	}
	lcd_cmd(0x80 +pos + pos_0_16);
	DelayUs(100);
	lcd_send(msg);
}



int main(void)
{
	// initialize the delay function (Must initialize)
	SysTick->CTRL = 0;
	SysTick->LOAD = 0x00FFFFFF;
	SysTick->VAL = 0;
	SysTick->CTRL = 5;
	
	
	//pin_output(4,11);
	RCC->APB2ENR = 0x04; /// port a aktif
	GPIOA->CRL   = 0x33330000; /// pin 3 ü çikis olarak ayarla
	GPIOA->CRH   = 0x00003333; /// pin 3 ü çikis olarak ayarla
	GPIOA->CRL &= 0xFFFFF0FF; /// A3 pini resetlendi
	GPIOA->CRL |= 0x00000000; //a2 sifir
	DelayUs(20000);
	GPIOA->ODR |=0x100;//lcd_rs(LOW);
	GPIOA->ODR |=0x200;//lcd_rw(LOW);
	DelayUs(10);
	GPIOA->ODR |=0x400;//lcd_e(HIGH);
	DelayUs(5);
	GPIOA->ODR &= 0xff0f;
	GPIOA->ODR |= 0x30; // 8 bit communication mode 
	DelayUs(10);
	GPIOA->ODR &=~0x400;//lcd_e(LOW);
	
	DelayUs(20000);
	
	
	GPIOA->ODR &=~0x100;//lcd_rs(LOW);
	GPIOA->ODR &=~0x200;//lcd_rw(LOW);
	DelayUs(10);
	GPIOA->ODR |=0x400;//lcd_e(HIGH);
	DelayUs(5);
	GPIOA->ODR &= 0xff0f;
	GPIOA->ODR |= 0x20; // 4 bit communication mode 
	DelayUs(10);
	GPIOA->ODR &=~0x400;//lcd_e(LOW);
	
	
	lcd_cmd(0x2C); // 4 bit communication mode / 2 lines
	DelayUs(5000);
	lcd_cmd(0x0C); // Display ON
	DelayUs(5000);
	lcd_cmd(0x01); // Clear Display
	DelayUs(5000);
	lcd_cmd(0x02); // Get back to initial address
	DelayUs(5000);
	
		lcd_msg(1, 0,"Sicaklik(C)");
		RCC->APB2ENR |= 0x201; //ADC 1 interface clock enable
		ADC1->CR2 = 0;    //adc kapali
		ADC1->SQR3 = 2;  //düzenli sirayla ilk dönüsüm  
		ADC1->SQR1 &= ~(0xf << 20);//adc kanal dizisi uzunlugu
    ADC1->SQR1 |= (0x1 << 20);			
    ADC1->SMPR2 &= ~(0x7);
    ADC1->SMPR2 |= (0x1);	//7.5 döngü
    //12mhz: 7.5+12.5 =20 hz ->3.3us(mikro Saniye) 
    ADC1->CR2 &= ~(1 << 11); // right alignment		
    ADC1->CR2 |= 2|1; //adc açik ve surekli
    ADC1->CR2 |= (1<<2);// kalibrasyon	
    while(ADC1->CR2 & (1<<2));
    ADC1->CR2 |=1; //adc açik
	while(1)
	{
		isi=0;
			for(int c= 29; c>=0;c--)
{
  sicaklik = log(((40950000 / ADC1->DR) - 10000));
  sicaklik = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * sicaklik * sicaklik)) * sicaklik);
  rms[c] = (sicaklik - 274.15)*100; 
	  DelayUs(4000);	
	isi=rms[c]+isi;
}	  
   deger=isi/30;
if(deger<0)
{
		 lcd_msg(2, 1,"-");
     deger=deger*-1;
}
else
{
lcd_msg(2, 1," ");
}

		if(deger<1000)
		{
     a=3; 
		}
    else if(deger<10000)
		{
     a=4;		 
		}
    else if(deger<100000)
		{
     a=5;		 
		}
		if(deger<100)
		{
		 lcd_msg(2, 2,"0");				
		}
		else
		{
	 int2char(deger,analog);
	 lcd_msg(2, 2,analog);		
	 deger=deger%100;
		}
	 int2char(deger,analog2);
	 lcd_msg(2, a,".");
	 lcd_msg(2, a+1,analog2);
	 if(deger<10)
		{
		  lcd_msg(2, a+1,"0");
      lcd_msg(2, a+2,analog2);			
		}
   lcd_msg(2, a+3,"     ");
	}
	
}

