#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "stm32f10x.h"
#include <string.h>
#include "main.h"
#include "rtc.h"
#include "timer.h"
#include "esp_at.h"
//#include "st7735.h"
//#include "stfonts.h"
#include "weather.h"
#include "mpu6050.h"
//#include "stimage.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "Picture.h"



static const char *wifi_ssid = "cxy0714";
static const char *wifi_password = "33333333";
static const char *weather_uri = "https://api.seniverse.com/v3/weather/now.json?key=Sx-4zGIkd11Zo5s7_&location=chongqing&language=en&unit=c";

static  uint32_t runms;
static  uint32_t height;

static void timer_elapsed_callback (void)
{
	runms++;
	if(runms>=24*60*60*1000)
	{
		runms=0;
	}
}
static void wifi_init(void)
{
	Gui_DrawString_8x16(0,height,BLACK,WHITE,"Init ESP32...");
	height +=16;
	if(!esp_at_init())
	{
		Gui_DrawString_8x16(0,height,RED,WHITE,"Failed!!!");
		height +=16;
		while(1);
	}
	
	Gui_DrawString_8x16(0,height,BLACK,WHITE,"Init WIFI...");
	height +=16;
	if(!esp_at_wifi_init())
	{
		Gui_DrawString_8x16(0,height,RED,WHITE,"Failed!!!");
		height +=16;
		while(1);
	}
	
	Gui_DrawString_8x16(0,height,BLACK,WHITE,"connect WIFI...");
	height +=16;
	if(!esp_at_wifi_connect(wifi_ssid,wifi_password))
	{
		Gui_DrawString_8x16(0,height,RED,WHITE,"Failed!!!");
		height +=16;
		while(1);
	}

	Gui_DrawString_8x16(0,height,BLACK,WHITE,"sync time...");
	height +=16;
	if(!esp_at_sntp_init())
	{
		Gui_DrawString_8x16(0,height,RED,WHITE,"Failed!!!");
		height +=16;
		while(1);
	}
}


int main(void)
{
	board_lowlevel_init();
	rtc_init();
	
	//每一毫秒进入一次中断
	timer_init(1000);
	timer_elapsed_register(timer_elapsed_callback);
    timer_start();
	
	Lcd_Init();
	LCD_LED_SET;
	Lcd_Clear(WHITE);

	//显示开机内容
	Gui_DrawString_8x16(0,height,BLACK,WHITE,"Initializing...");
	height +=16; 
	delay_ms(1000); 

	Gui_DrawString_8x16(0,height,BLACK,WHITE,"Waiting ESP32...");
	height +=16; 
	delay_ms(1000); 
	
	wifi_init();
	
	Gui_DrawString_8x16(0,height,GREEN,WHITE,"READY...");
	height +=16; 
	delay_ms(1000); 
	
	Lcd_Clear(WHITE);
	
	//显示位置
	Gui_showimage(location_map, 32, 32, 0,100);
	Gui_DrawFont_GBK24(40,100,RED,WHITE,"重庆");
	
	//岁岁平，岁岁安
	Gui_DrawFont_GBK16(8,140,BLUE,WHITE,"岁岁平，岁岁安");
	
	char str[64];
	bool weather_ok=false;
	bool sntp_ok=false;
	
	runms=0;
	uint32_t last_runms=runms;
	while(1)
	{
		// 防卡死、防空跑
		if(runms==last_runms)
		{
			continue;
		}
		//把当前时间 runms 存到 last_runms
		last_runms=runms;
		//更新时间(100ms)
		if(last_runms%100==0)
		{
			rtc_date_t date;//定义日期结构体变量
			rtc_get_date(&date);//提取当前RTC时间戳并转化为日期，存储在变量date中
			snprintf(str,sizeof(str),"%04d-%02d-%02d",date.year,date.month,date.day);
			Gui_DrawString_8x16(0,0,BLACK,WHITE,str);
			
			snprintf(str,sizeof(str),"%02d%s%02d",date.hour,date.second %2 ? " ":":",date.minute);//当秒数为0，2，4，6，8时显示“：”
			Gui_DrawFont_Num24x48(0,64,GREEN,WHITE,str);
		}
		
		//联网同步时间(1小时)
		if(!sntp_ok || last_runms%(60*60*1000)==0)
		{
			uint32_t ts=0;
			sntp_ok=esp_at_get_time(&ts);//联网获取当前时间戳
			rtc_set_timestamp(ts+(8*60*60));//将时间戳（转化为东八区）写入RTC
		}

		//更新天气信息(10分钟)
		if(!weather_ok || last_runms%(10*60*1000)==0)
		{
			const char *rsp;
			weather_ok=esp_at_get_http(weather_uri,&rsp,NULL,10000);//将联网同步的天气信息写入rsp
			weather_t weather;//定义天气结构体变量
			weather_parse(rsp,&weather);//将天气数据存入结构体变量
			if(strcmp(weather.weather, "Cloudy") == 0)
			{
                Gui_showimage(duoyun_map, 48, 48, 0,16);
            }
			else if (strcmp(weather.weather, "Wind") == 0)
			{
                Gui_showimage(feng_map, 48, 48, 0,16);
            }
			else if (strcmp(weather.weather, "Clear") == 0 || strcmp(weather.weather, "Sunny") == 0)
			{
                Gui_showimage(qing_map, 48, 48, 0,16);
            }
			else if (strcmp(weather.weather, "Snow") == 0)
			{
                Gui_showimage(xue_map, 48, 48, 0,16);
            }
			else if (strcmp(weather.weather, "Overcast") == 0)
			{
                Gui_showimage(yin_map, 48, 48, 0,16);
            }
			else if (strcmp(weather.weather, "Rain") == 0)
			{
                Gui_showimage(yu_map, 48, 48, 0,16);
            }
			
			snprintf(str,sizeof(str),"%s",weather.temperature);
			Gui_DrawString_8x16(90,16,BLUE,WHITE,str);
			Gui_DrawFont_GBK16(106,16,BLUE,WHITE,"℃");
		}
		//更新环境温度
		if(last_runms%(1*1000)==0)
		{
			float temper=mpu6050_read_temper();
			snprintf(str,sizeof(str),"%5.1f",temper);//%5.1f = 保留 1 位小数，总宽度 5 个字符
			Gui_DrawString_8x16(66,32,GRAY1,WHITE,str);
			Gui_DrawFont_GBK16(106,32,GRAY1,WHITE,"℃");

		}
		
	}
}
