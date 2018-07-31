
/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "dma2d.h"
#include "ltdc.h"
#include "../std800480/std800480.h"
#include "../../../../Utilities/fonts/font24.c"
#include "../../../../Utilities/fonts/font20.c"
#include "../../../../Utilities/fonts/font16.c"
#include "../../../../Utilities/fonts/font12.c"
#include "../../../../Utilities/fonts/font8.c"


/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup Components
  * @{
  */ 

/** @addtogroup STD800480
  * @brief      This file provides a set of functions needed to drive the
  *             STD800480 lcd.
  * @{
  */

/** @defgroup STD800480_Private_TypesDefinitions
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup STD800480_Private_Defines
  * @{
  */
  
/* Default LCD configuration with LCD Layer 1 */
static uint32_t            ActiveLayer = 0;
static LCD_DrawPropTypeDef DrawProp[MAX_LAYER_NUMBER];

/**
  * @}
  */ 

/** @defgroup STD800480_Private_Macros
  * @{
  */
  
  
#define POLY_X(Z)              ((int32_t)((Points + Z)->X))
#define POLY_Y(Z)              ((int32_t)((Points + Z)->Y)) 

#define ABS(X)  ((X) > 0 ? (X) : -(X))
  
/*根据液晶数据手册的参数配置*/
#define HBP  46		//HSYNC后的无效像素
#define VBP  23		//VSYNC后的无效行数

#define HSW   1		//HSYNC宽度
#define VSW   1		//VSYNC宽度

#define HFP  10		//HSYNC前的无效像素
#define VFP  22		//VSYNC前的无效行数


/**
  * @}
  */  

/** @defgroup STD800480_Private_Variables
  * @{
  */ 

LCD_DrvTypeDef std800480_drv = 
{
  LCD_Init,
  LCD_Test,
  0,
  LCD_DisplayOn,
  0,
  LCD_Clear,
  LCD_SelectLayer,
  LCD_SetTransparency,
  LCD_LayerInit,
};



/**
* @}
*/ 

/** @defgroup STD800480_Private_FunctionPrototypes
  * @{
  */
//static LTDC_HandleTypeDef  hltdc;
static DMA2D_HandleTypeDef hdma2d;


static void DrawChar(uint16_t Xpos, uint16_t Ypos, const uint8_t *c);
static void FillTriangle(uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3);
static void LL_FillBuffer(uint32_t LayerIndex, void *pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex);
static void LL_ConvertLineToARGB8888(void * pSrc, void *pDst, uint32_t xSize, uint32_t ColorMode);
/**
* @}
*/ 

/** @defgroup STD800480_Private_Functions
  * @{
  */

/*用于测试各种液晶的函数*/
void LCD_Test(void)
{
	/*演示显示变量*/
	static uint8_t testCNT = 0;	
	char dispBuff[100];
	
  /* 选择LCD第一层 */
  LCD_SelectLayer(0);
	
	/* 清屏，显示全黑 */
	LCD_Clear(LCD_COLOR_BLACK);	
	/*设置字体颜色及字体的背景颜色(此处的背景不是指LCD的背景层！注意区分)*/
	LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_BLACK);
	/*选择字体*/
	LCD_SetFont(&LCD_DEFAULT_FONT);

	LCD_DisplayStringLine(1,(uint8_t* )"BH 5.0 inch LCD para:");
	LCD_DisplayStringLine(2,(uint8_t* )"Image resolution:800x480 px");
	LCD_DisplayStringLine(3,(uint8_t* )"Touch pad:5 point touch supported");
	LCD_DisplayStringLine(4,(uint8_t* )"Use STM32-LTDC directed driver,");
	LCD_DisplayStringLine(5,(uint8_t* )"no need extern driver,RGB888,24bits data bus");
	LCD_DisplayStringLine(6,(uint8_t* )"Touch pad use IIC to communicate");
	//while(1)
	{		
	testCNT++;
	/*使用c标准库把变量转化成字符串*/
	sprintf(dispBuff,"Display value demo: testCount = %d ",testCNT);
	LCD_ClearLine(7);
	/*设置字体颜色及字体的背景颜色(此处的背景不是指LCD的背景层！注意区分)*/
	LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_BLACK);
	/*然后显示该字符串即可，其它变量也是这样处理*/
	LCD_DisplayStringLine(7,(uint8_t* )dispBuff);


	/* 画直线 */
	LCD_SetTextColor(LCD_COLOR_BLUE);

	LCD_ClearLine(8);
	LCD_DisplayStringLine(8,(uint8_t* )"Draw line:");

	LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_BLACK);
	LCD_DrawLine(50,250,750,250);  
	LCD_DrawLine(50,300,750,300);

	LCD_SetColors(LCD_COLOR_GREEN,LCD_COLOR_BLACK);
	LCD_DrawLine(300,250,400,400);  
	LCD_DrawLine(600,250,600,400);

//	Delay(0xFFFFFF);
  HAL_Delay(3000);

	LCD_SetColors(LCD_COLOR_BLACK,LCD_COLOR_BLACK);
	LCD_FillRect(0,200,LCD_PIXEL_WIDTH,LCD_PIXEL_HEIGHT-200);


	/*画矩形*/
	LCD_SetTextColor(LCD_COLOR_BLUE);
	LCD_ClearLine(8);
	LCD_DisplayStringLine(8,(uint8_t* )"Draw Rect:");

	LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_BLACK);
	LCD_DrawRect(200,250,200,100);

	LCD_SetColors(LCD_COLOR_GREEN,LCD_COLOR_BLACK);
	LCD_DrawRect(350,250,200,50);

	LCD_SetColors(LCD_COLOR_BLUE,LCD_COLOR_BLACK);
	LCD_DrawRect(200,350,50,200);

//	Delay(0xFFFFFF);
  HAL_Delay(3000);


	LCD_SetColors(LCD_COLOR_BLACK,LCD_COLOR_BLACK);
	LCD_FillRect(0,200,LCD_PIXEL_WIDTH,LCD_PIXEL_HEIGHT-200);


	/*填充矩形*/
	LCD_SetTextColor(LCD_COLOR_BLUE);
	LCD_ClearLine(8);
	LCD_DisplayStringLine(8,(uint8_t* )"Draw Full Rect:");

	LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_BLACK);
	LCD_FillRect(200,250,200,100);

	LCD_SetColors(LCD_COLOR_GREEN,LCD_COLOR_BLACK);
	LCD_FillRect(350,250,200,50);

	LCD_SetColors(LCD_COLOR_BLUE,LCD_COLOR_BLACK);
	LCD_FillRect(200,350,50,200);

//	Delay(0xFFFFFF);
  HAL_Delay(3000);

	LCD_SetColors(LCD_COLOR_BLACK,LCD_COLOR_BLACK);
	LCD_FillRect(0,200,LCD_PIXEL_WIDTH,LCD_PIXEL_HEIGHT-200);

	/* 画圆 */
	LCD_SetTextColor(LCD_COLOR_BLUE);
	LCD_ClearLine(8);
	LCD_DisplayStringLine(8,(uint8_t* )"Draw circle:");

	LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_RED);
	LCD_DrawCircle(200,350,50);

	LCD_SetColors(LCD_COLOR_GREEN,LCD_COLOR_GREEN);
	LCD_DrawCircle(350,350,75);

//	Delay(0xFFFFFF);
  HAL_Delay(3000);

	LCD_SetColors(LCD_COLOR_BLACK,LCD_COLOR_BLACK);
	LCD_FillRect(0,200,LCD_PIXEL_WIDTH,LCD_PIXEL_HEIGHT-200);


	/*填充圆*/
	LCD_SetTextColor(LCD_COLOR_BLUE);
	LCD_ClearLine(8);
	LCD_DisplayStringLine(8,(uint8_t* )"Draw full circle:");

	LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_BLACK);
	LCD_FillCircle(300,350,50);

	LCD_SetColors(LCD_COLOR_GREEN,LCD_COLOR_BLACK);
	LCD_FillCircle(450,350,75);

//	Delay(0xFFFFFF);
  HAL_Delay(3000);

	LCD_SetColors(LCD_COLOR_BLACK,LCD_COLOR_BLACK);
	LCD_FillRect(0,200,LCD_PIXEL_WIDTH,LCD_PIXEL_HEIGHT-200);

	LCD_ClearLine(8);
	
	/*透明效果 前景层操作*/
	LCD_SetTextColor(LCD_COLOR_BLUE);
	LCD_ClearLine(8);
	LCD_DisplayStringLine(8,(uint8_t* )"Transparency effect:");
	
	/*设置前景层不透明度*/
	LCD_SetTransparency(1, 128);

    /* 选择LCD第一层 */
    LCD_SelectLayer(1);
	
	/* 清屏，显示全黑 */
	LCD_Clear(LCD_COLOR_BLACK);	
	/*在前景画一个红色圆*/
	LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_TRANSPARENT);
	LCD_FillCircle(400,350,75);
	
//	Delay(0xFFFFFF);
  HAL_Delay(3000);
	
	/*透明效果 背景层操作*/

	/* 选择LCD背景层 */
	LCD_SelectLayer(0);	
	LCD_Clear(LCD_COLOR_BLACK);		
	/*设置背景层不透明*/
	LCD_SetTransparency(1, 0);
	

	/*在背景画一个绿色圆*/
	LCD_SetColors(LCD_COLOR_GREEN,LCD_COLOR_BLACK);
	LCD_FillCircle(450,350,75);
	
	/*在背景画一个蓝色圆*/
	LCD_SetColors(LCD_COLOR_BLUE,LCD_COLOR_BLACK);
	LCD_FillCircle(350,350,75);
	
//	Delay(0xFFFFFF);
  HAL_Delay(3000);
	LCD_SetColors(LCD_COLOR_BLACK,LCD_COLOR_BLACK);
	LCD_FillRect(0,200,LCD_PIXEL_WIDTH,LCD_PIXEL_HEIGHT-200);
	}

}

void LCD_Init(void)
{

    /* 初始化字体 */
    LCD_SetFont(&LCD_DEFAULT_FONT);
}

/**
  * @brief  获取LCD当前有效层X轴的大小
  * @retval X轴的大小
  */
uint32_t LCD_GetXSize(void)
{
  return hltdc.LayerCfg[ActiveLayer].ImageWidth;
}

/**
  * @brief  获取LCD当前有效层Y轴的大小
  * @retval Y轴的大小
  */
uint32_t LCD_GetYSize(void)
{
  return hltdc.LayerCfg[ActiveLayer].ImageHeight;
}

/**
  * @brief  设置LCD当前有效层Y轴的大小
  * @param  imageWidthPixels：图像宽度像素个数
  * @retval 无
  */
void LCD_SetXSize(uint32_t imageWidthPixels)
{
  hltdc.LayerCfg[ActiveLayer].ImageWidth = imageWidthPixels;
}

/**
  * @brief  设置LCD当前有效层Y轴的大小
  * @param  imageHeightPixels：图像高度像素个数
  * @retval None
  */
void LCD_SetYSize(uint32_t imageHeightPixels)
{
  hltdc.LayerCfg[ActiveLayer].ImageHeight = imageHeightPixels;
}

/**
  * @brief  初始化LCD层 
  * @param  LayerIndex:  前景层(层1)或者背景层(层0)
  * @param  FB_Address:  每一层显存的首地址
  * @param  PixelFormat: 层的像素格式
  * @retval 无
  */
void LCD_LayerInit(uint16_t LayerIndex, uint32_t FB_Address,uint32_t PixelFormat)
{     
  LTDC_LayerCfgTypeDef  layer_cfg;

  /* 层初始化 */
  layer_cfg.WindowX0 = 0;				//窗口起始位置X坐标
  layer_cfg.WindowX1 = LCD_GetXSize();	//窗口结束位置X坐标
  layer_cfg.WindowY0 = 0;				//窗口起始位置Y坐标
  layer_cfg.WindowY1 = LCD_GetYSize();  //窗口结束位置Y坐标
  layer_cfg.PixelFormat = PixelFormat;	//像素格式
  layer_cfg.FBStartAdress = FB_Address; //层显存首地址
  layer_cfg.Alpha = 255;				//用于混合的透明度常量，范围（0—255）0为完全透明
  layer_cfg.Alpha0 = 0;					//默认透明度常量，范围（0—255）0为完全透明
  layer_cfg.Backcolor.Blue = 0;			//层背景颜色蓝色分量
  layer_cfg.Backcolor.Green = 0;		//层背景颜色绿色分量
  layer_cfg.Backcolor.Red = 0;			//层背景颜色红色分量
  layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;//层混合系数1
  layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;//层混合系数2
  layer_cfg.ImageWidth = LCD_GetXSize();//设置图像宽度
  layer_cfg.ImageHeight = LCD_GetYSize();//设置图像高度
  
  HAL_LTDC_ConfigLayer(&hltdc, &layer_cfg, LayerIndex); //设置选中的层参数

  DrawProp[LayerIndex].BackColor = LCD_COLOR_WHITE;//设置层的字体颜色
  DrawProp[LayerIndex].pFont     = &LCD_DEFAULT_FONT;//设置层的字体类型
  DrawProp[LayerIndex].TextColor = LCD_COLOR_BLACK; //设置层的字体背景颜色
  
  __HAL_LTDC_RELOAD_CONFIG(&hltdc);//重载层的配置参数
}
/**
  * @brief  选择LCD层
  * @param  LayerIndex: 前景层(层1)或者背景层(层0)
  * @retval 无
  */
void LCD_SelectLayer(uint32_t LayerIndex)
{
  ActiveLayer = LayerIndex;
} 

/**
  * @brief  设置LCD层的可视化
  * @param  LayerIndex: 前景层(层1)或者背景层(层0)
  * @param  State: 禁能或者使能
  * @retval 无
  */
void LCD_SetLayerVisible(uint32_t LayerIndex, FunctionalState State)
{
  if(State == ENABLE)
  {
    __HAL_LTDC_LAYER_ENABLE(&hltdc, LayerIndex);
  }
  else
  {
    __HAL_LTDC_LAYER_DISABLE(&hltdc, LayerIndex);
  }
  __HAL_LTDC_RELOAD_CONFIG(&hltdc);
} 

/**
  * @brief  设置LCD的透明度常量
  * @param  LayerIndex: 前景层(层1)或者背景层(层0)
  * @param  Transparency: 透明度，范围（0—255）0为完全透明
  * @retval 无
  */
void LCD_SetTransparency(uint32_t LayerIndex, uint8_t Transparency)
{    
  HAL_LTDC_SetAlpha(&hltdc, Transparency, LayerIndex);
}

/**
  * @brief  设置LCD缓冲帧的首地址 
  * @param  LayerIndex: 前景层(层1)或者背景层(层0)
  * @param  Address: LCD缓冲帧的首地址     
  * @retval 无
  */
void LCD_SetLayerAddress(uint32_t LayerIndex, uint32_t Address)
{
  HAL_LTDC_SetAddress(&hltdc, Address, LayerIndex);
}

/**
  * @brief  设置显示窗口
  * @param  LayerIndex: 前景层(层1)或者背景层(层0)
  * @param  Xpos: LCD 	X轴起始位置
  * @param  Ypos: LCD 	Y轴起始位置
  * @param  Width: LCD  窗口大小
  * @param  Height: LCD 窗口大小  
  * @retval None
  */
void LCD_SetLayerWindow(uint16_t LayerIndex, uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  /* 重新设置窗口大小 */
  HAL_LTDC_SetWindowSize(&hltdc, Width, Height, LayerIndex);
  
  /* 重新设置窗口的起始位置 */
  HAL_LTDC_SetWindowPosition(&hltdc, Xpos, Ypos, LayerIndex); 
}


/**
  * @brief  设置LCD当前层文字颜色
  * @param  Color: 文字颜色
  * @retval 无
  */
void LCD_SetTextColor(uint32_t Color)
{
  DrawProp[ActiveLayer].TextColor = Color;
}

/**
  * @brief  获取LCD当前层文字颜色
  * @retval 文字颜色
  */
uint32_t LCD_GetTextColor(void)
{
  return DrawProp[ActiveLayer].TextColor;
}

/**
  * @brief  设置LCD当前层的文字背景颜色
  * @param  Color: 文字背景颜色
  * @retval 无
  */
void LCD_SetBackColor(uint32_t Color)
{
  DrawProp[ActiveLayer].BackColor = Color;
}

/**
  * @brief  获取LCD当前层的文字背景颜色
  * @retval 文字背景颜色
  */
uint32_t LCD_GetBackColor(void)
{
  return DrawProp[ActiveLayer].BackColor;
}

/**
 * @brief  设置LCD文字的颜色和背景的颜色
 * @param  TextColor: 指定文字颜色
 * @param  BackColor: 指定背景颜色
 * @retval 无
 */
void LCD_SetColors(uint32_t TextColor, uint32_t BackColor)
{
     LCD_SetTextColor (TextColor);
     LCD_SetBackColor (BackColor);
}
/**
  * @brief  设置LCD当前层显示的字体
  * @param  fonts: 字体类型
  * @retval None
  */
void LCD_SetFont(sFONT *fonts)
{
  DrawProp[ActiveLayer].pFont = fonts;
}

/**
  * @brief  获取LCD当前层显示的字体
  * @retval 字体类型
  */
sFONT *LCD_GetFont(void)
{
  return DrawProp[ActiveLayer].pFont;
}

/**
  * @brief  读LCD的像素值
  * @param  Xpos: X 轴坐标位置
  * @param  Ypos: Y 轴坐标位置
  * @retval RGB像素值
  */
uint32_t LCD_ReadPixel(uint16_t Xpos, uint16_t Ypos)
{
  uint32_t ret = 0;
  
  if(hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    /* 从SDRAM显存中读取颜色数据 */
    ret = *(__IO uint32_t*) (hltdc.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*LCD_GetXSize() + Xpos)));
  }
  else if(hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  {
    /* 从SDRAM显存中读取颜色数据 */
    ret  = (*(__IO uint8_t*) (hltdc.LayerCfg[ActiveLayer].FBStartAdress + (3*(Ypos*LCD_GetXSize() + Xpos))+2) & 0x00FFFFFF);
	ret |= (*(__IO uint8_t*) (hltdc.LayerCfg[ActiveLayer].FBStartAdress + (3*(Ypos*LCD_GetXSize() + Xpos))+1) & 0x00FFFFFF);
	ret |= (*(__IO uint8_t*) (hltdc.LayerCfg[ActiveLayer].FBStartAdress + (3*(Ypos*LCD_GetXSize() + Xpos))) & 0x00FFFFFF);
  }
  else if((hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565) || \
          (hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
          (hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_AL88))  
  {
    /* 从SDRAM显存中读取颜色数据 */
    ret = *(__IO uint16_t*) (hltdc.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*LCD_GetXSize() + Xpos)));    
  }
  else
  {
    /* 从SDRAM显存中读取颜色数据 */
    ret = *(__IO uint8_t*) (hltdc.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*LCD_GetXSize() + Xpos)));    
  }
  
  return ret;
}

/**
  * @brief  LCD当前层清屏
  * @param  Color: 背景颜色
  * @retval None
  */
void LCD_Clear(uint32_t Color)
{ 
  /* 清屏 */ 
  LL_FillBuffer(ActiveLayer, (uint32_t *)(hltdc.LayerCfg[ActiveLayer].FBStartAdress), LCD_GetXSize(), LCD_GetYSize(), 0, Color);
}

/**
  * @brief  清除一行
  * @param  Line: 行
  * @retval None
  */
void LCD_ClearLine(uint32_t Line)
{
  uint32_t color_backup = DrawProp[ActiveLayer].TextColor;
  DrawProp[ActiveLayer].TextColor = DrawProp[ActiveLayer].BackColor;
  
  /* 画一个跟背景色一致的矩形完成清除 */
  LCD_FillRect(0, (Line * DrawProp[ActiveLayer].pFont->Height), LCD_GetXSize(), DrawProp[ActiveLayer].pFont->Height);
  
  DrawProp[ActiveLayer].TextColor = color_backup;
  LCD_SetTextColor(DrawProp[ActiveLayer].TextColor);  
}

/**
  * @brief  显示一个字符
  * @param  Xpos: X轴起始坐标
  * @param  Ypos: Y轴起始坐标
  * @param  Ascii: 字符 ascii 码,范围（ 0x20 —0x7E ）
  * @retval 无
  */
void LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii)
{
    DrawChar(Xpos, Ypos, &DrawProp[ActiveLayer].pFont->table[(Ascii-' ') *\
    DrawProp[ActiveLayer].pFont->Height * ((DrawProp[ActiveLayer].pFont->Width + 7) / 8)]);
}

/**
  * @brief  显示字符串
  * @param  Xpos: X轴起始坐标
  * @param  Ypos: Y轴起始坐标 
  * @param  Text: 字符串指针
  * @param  Mode: 显示对齐方式，可以是CENTER_MODE、RIGHT_MODE、LEFT_MODE
  * @retval None
  */
void LCD_DisplayStringAt(uint16_t Xpos, uint16_t Ypos, uint8_t *Text, Text_AlignModeTypdef Mode)
{
  uint16_t ref_column = 1, i = 0;
  uint32_t size = 0, xsize = 0; 
  uint8_t  *ptr = Text;
  
  /* 获取字符串大小 */
  while (*ptr++) size ++ ;
  
  /* 每一行可以显示字符的数量 */
  xsize = (LCD_GetXSize()/DrawProp[ActiveLayer].pFont->Width);
  
  switch (Mode)
  {
  case CENTER_MODE:
    {
      ref_column = Xpos + ((xsize - size)* DrawProp[ActiveLayer].pFont->Width) / 2;
      break;
    }
  case LEFT_MODE:
    {
      ref_column = Xpos;
      break;
    }
  case RIGHT_MODE:
    {
      ref_column = - Xpos + ((xsize - size)*DrawProp[ActiveLayer].pFont->Width);
      break;
    }    
  default:
    {
      ref_column = Xpos;
      break;
    }
  }
  
  /*检查起始行是否在显示范围内 */
  if ((ref_column < 1) || (ref_column >= 0x8000))
  {
    ref_column = 1;
  }

  /* 使用字符显示函数显示每一个字符*/
  while ((*Text != 0) & (((LCD_GetXSize() - (i*DrawProp[ActiveLayer].pFont->Width)) & 0xFFFF)\
			>= DrawProp[ActiveLayer].pFont->Width))
  {
    /* 显示一个字符 */
    LCD_DisplayChar(ref_column, Ypos, *Text);
    /* 根据字体大小计算下一个偏移位置 */
    ref_column += DrawProp[ActiveLayer].pFont->Width;
    /* 指针指向下一个字符 */
    Text++;
    i++;
  }  
}

/**
  * @brief  在指定行显示字符串(最多60个)
  * @param  Line: 显示的行
  * @param  ptr: 字符串指针
  * @retval 无
  */
void LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr)
{  
  LCD_DisplayStringAt(0, LINE(Line), ptr, LEFT_MODE);
}

/**
  * @brief  绘制水平线
  * @param  Xpos: X轴起始坐标
  * @param  Ypos: Y轴起始坐标
  * @param  Length: 线的长度
  * @retval 无
  */
void LCD_DrawHLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint32_t  Xaddress = 0;

  if(hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    Xaddress = (hltdc.LayerCfg[ActiveLayer].FBStartAdress) + 4*(LCD_GetXSize()*Ypos + Xpos);
  }
  else if(hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  {
    Xaddress = (hltdc.LayerCfg[ActiveLayer].FBStartAdress) + 3*(LCD_GetXSize()*Ypos + Xpos);
  }
  else if((hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565) || \
          (hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
          (hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_AL88))  
  {
    Xaddress = (hltdc.LayerCfg[ActiveLayer].FBStartAdress) + 2*(LCD_GetXSize()*Ypos + Xpos);   
  }
  else
  {
    Xaddress = (hltdc.LayerCfg[ActiveLayer].FBStartAdress) + 2*(LCD_GetXSize()*Ypos + Xpos);   
  }	
  /* 填充数据 */
  LL_FillBuffer(ActiveLayer, (uint32_t *)Xaddress, Length, 1, 0, DrawProp[ActiveLayer].TextColor);
}

/**
  * @brief  绘制垂直线
  * @param  Xpos: X轴起始坐标
  * @param  Ypos: Y轴起始坐标
  * @param  Length: 线的长度
  * @retval 无
  */
void LCD_DrawVLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint32_t  Xaddress = 0;
  
  if(hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    Xaddress = (hltdc.LayerCfg[ActiveLayer].FBStartAdress) + 4*(LCD_GetXSize()*Ypos + Xpos);
  }
  else if(hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  {
    Xaddress = (hltdc.LayerCfg[ActiveLayer].FBStartAdress) + 3*(LCD_GetXSize()*Ypos + Xpos);
  }
  else if((hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565) || \
          (hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
          (hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_AL88))  
  {
    Xaddress = (hltdc.LayerCfg[ActiveLayer].FBStartAdress) + 2*(LCD_GetXSize()*Ypos + Xpos);   
  }
  else
  {
    Xaddress = (hltdc.LayerCfg[ActiveLayer].FBStartAdress) + 2*(LCD_GetXSize()*Ypos + Xpos);   
  }	
  
  /* 填充数据 */
  LL_FillBuffer(ActiveLayer, (uint32_t *)Xaddress, 1, Length, (LCD_GetXSize() - 1), DrawProp[ActiveLayer].TextColor);
}

/**
  * @brief  指定两点画一条线
  * @param  x1: 第一点X轴坐标
  * @param  y1: 第一点Y轴坐标
  * @param  x2: 第二点X轴坐标
  * @param  y2: 第二点Y轴坐标
  * @retval 无
  */
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, num_add = 0, num_pixels = 0, 
  curpixel = 0;
  
  deltax = ABS(x2 - x1);        /* 求x轴的绝对值 */
  deltay = ABS(y2 - y1);        /* 求y轴的绝对值 */
  x = x1;                       /* 第一个像素的x坐标起始值 */
  y = y1;                       /* 第一个像素的y坐标起始值 */
  
  if (x2 >= x1)                 /* x坐标值为递增 */
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          /* x坐标值为递减 */
  {
    xinc1 = -1;
    xinc2 = -1;
  }
  
  if (y2 >= y1)                 /* y坐标值为递增 */
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          /* y坐标值为递减 */
  {
    yinc1 = -1;
    yinc2 = -1;
  }
  
  if (deltax >= deltay)         /* 每个 y 坐标值至少有一个x坐标值*/
  {
    xinc1 = 0;                  /* 当分子大于或等于分母时不要改变 x */
    yinc2 = 0;                  /* 不要为每次迭代更改 y */
    den = deltax;
    num = deltax / 2;
    num_add = deltay;
    num_pixels = deltax;         /* x比y多的值 */
  }
  else                          /* 每个 x 坐标值至少有一个y坐标值 */
  {
    xinc2 = 0;                  /* 不要为每次迭代更改 x */
    yinc1 = 0;                  /* 当分子大于或等于分母时不要改变 y */
    den = deltay;
    num = deltay / 2;
    num_add = deltax;
    num_pixels = deltay;         /* y比x多的值 */
  }
  
  for (curpixel = 0; curpixel <= num_pixels; curpixel++)
  {
    LCD_DrawPixel(x, y, DrawProp[ActiveLayer].TextColor);   /* 绘制当前像素点 */
    num += num_add;                            /* 在分数的基础上增加分子 */
    if (num >= den)                           /* 检查分子大于或等于分母 */
    {
      num -= den;                             /* 计算新的分子值 */
      x += xinc1;                             /* x值递增 */
      y += yinc1;                             /* y值递增 */
    }
    x += xinc2;                               /* y值递增 */
    y += yinc2;                               /* y值递增 */
  }
}

/**
  * @brief  绘制一个矩形
  * @param  Xpos:   X轴坐标
  * @param  Ypos:   Y轴坐标
  * @param  Width:  矩形宽度  
  * @param  Height: 矩形高度
  * @retval 无
  */
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  /* 绘制水平线 */
  LCD_DrawHLine(Xpos, Ypos, Width);
  LCD_DrawHLine(Xpos, (Ypos+ Height), Width);
  
  /* 绘制垂直线 */
  LCD_DrawVLine(Xpos, Ypos, Height);
  LCD_DrawVLine((Xpos + Width), Ypos, Height);
}

/**
  * @brief  绘制一个圆形
  * @param  Xpos:   X轴坐标
  * @param  Ypos:   Y轴坐标
  * @param  Radius: 圆的半径
  * @retval 无
  */
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t   decision;    /* 决策变量 */ 
  uint32_t  current_x;   /* 当前x坐标值 */
  uint32_t  current_y;   /* 当前y坐标值 */
  
  decision = 3 - (Radius << 1);
  current_x = 0;
  current_y = Radius;
  
  while (current_x <= current_y)
  {
    LCD_DrawPixel((Xpos + current_x), (Ypos - current_y), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos - current_x), (Ypos - current_y), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos + current_y), (Ypos - current_x), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos - current_y), (Ypos - current_x), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos + current_x), (Ypos + current_y), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos - current_x), (Ypos + current_y), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos + current_y), (Ypos + current_x), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos - current_y), (Ypos + current_x), DrawProp[ActiveLayer].TextColor);
    
    if (decision < 0)
    { 
      decision += (current_x << 2) + 6;
    }
    else
    {
      decision += ((current_x - current_y) << 2) + 10;
      current_y--;
    }
    current_x++;
  } 
}

/**
  * @brief  绘制一条折线
  * @param  Points: 指向点数组的指针
  * @param  PointCount: 点数
  * @retval 无
  */
void LCD_DrawPolygon(pPoint Points, uint16_t PointCount)
{
  int16_t x = 0, y = 0;
  
  if(PointCount < 2)
  {
    return;
  }
  
  LCD_DrawLine(Points->X, Points->Y, (Points+PointCount-1)->X, (Points+PointCount-1)->Y);
  
  while(--PointCount)
  {
    x = Points->X;
    y = Points->Y;
    Points++;
    LCD_DrawLine(x, y, Points->X, Points->Y);
  }
}

/**
  * @brief  绘制一个椭圆
  * @param  Xpos:   X轴坐标
  * @param  Ypos:   Y轴坐标
  * @param  XRadius: 椭圆X轴半径
  * @param  YRadius: 椭圆Y轴半径
  * @retval 无
  */
void LCD_DrawEllipse(int Xpos, int Ypos, int XRadius, int YRadius)
{
  int x = 0, y = -YRadius, err = 2-2*XRadius, e2;
  float k = 0, rad1 = 0, rad2 = 0;
  
  rad1 = XRadius;
  rad2 = YRadius;
  
  k = (float)(rad2/rad1);  
  
  do { 
    LCD_DrawPixel((Xpos-(uint16_t)(x/k)), (Ypos+y), DrawProp[ActiveLayer].TextColor);
    LCD_DrawPixel((Xpos+(uint16_t)(x/k)), (Ypos+y), DrawProp[ActiveLayer].TextColor);
    LCD_DrawPixel((Xpos+(uint16_t)(x/k)), (Ypos-y), DrawProp[ActiveLayer].TextColor);
    LCD_DrawPixel((Xpos-(uint16_t)(x/k)), (Ypos-y), DrawProp[ActiveLayer].TextColor);      
    
    e2 = err;
    if (e2 <= x) {
      err += ++x*2+1;
      if (-y == x && e2 <= y) e2 = 0;
    }
    if (e2 > y) err += ++y*2+1;     
  }
  while (y <= 0);
}

/**
  * @brief  绘制一个点
  * @param  Xpos:   X轴坐标
  * @param  Ypos:   Y轴坐标
  * @param  RGB_Code: 像素颜色值
  * @retval 无
  */
void LCD_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint32_t RGB_Code)
{

  if(hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    *(__IO uint32_t*) (hltdc.LayerCfg[ActiveLayer].FBStartAdress + (4*(Ypos*LCD_GetXSize() + Xpos))) = RGB_Code;
  }
  else if(hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  {
    *(__IO uint8_t*) (hltdc.LayerCfg[ActiveLayer].FBStartAdress + (3*(Ypos*LCD_GetXSize() + Xpos))+2) = 0xFF&(RGB_Code>>16);
	*(__IO uint8_t*) (hltdc.LayerCfg[ActiveLayer].FBStartAdress + (3*(Ypos*LCD_GetXSize() + Xpos))+1) = 0xFF&(RGB_Code>>8);
	*(__IO uint8_t*) (hltdc.LayerCfg[ActiveLayer].FBStartAdress + (3*(Ypos*LCD_GetXSize() + Xpos))) = 0xFF&RGB_Code;
  }
  else if((hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565) || \
          (hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
          (hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_AL88))  
  {
    *(__IO uint16_t*) (hltdc.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*LCD_GetXSize() + Xpos))) = (uint16_t)RGB_Code;   
  }
  else
  {
    *(__IO uint16_t*) (hltdc.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*LCD_GetXSize() + Xpos))) = (uint16_t)RGB_Code;   
  }

}

/**
  * @brief  绘制一幅从内部flash加载的ARGB888(32 bits per pixel)格式的图片
  * @param  Xpos: Bmp在液晶上X 轴坐标
  * @param  Ypos: Bmp在液晶上Y 轴坐标
  * @param  pbmp: 指针指向存在内部flash的Bmp图片的首地址
  * @retval 无
  */
void LCD_DrawBitmap(uint32_t Xpos, uint32_t Ypos, uint8_t *pbmp)
{
  uint32_t index = 0, width = 0, height = 0, bit_pixel = 0;
  uint32_t address;
  uint32_t input_color_mode = 0;
  
  /* 获取位图数据的基地址 */
  index = *(__IO uint16_t *) (pbmp + 10);
  index |= (*(__IO uint16_t *) (pbmp + 12)) << 16;
  
  /* 获取位图宽度 */
  width = *(uint16_t *) (pbmp + 18);
  width |= (*(uint16_t *) (pbmp + 20)) << 16;
  
  /* 获取位图高度 */
  height = *(uint16_t *) (pbmp + 22);
  height |= (*(uint16_t *) (pbmp + 24)) << 16; 
  
  /* 获取像素个数 */
  bit_pixel = *(uint16_t *) (pbmp + 28);   
  
  /* 设定地址 */
  address = hltdc.LayerCfg[ActiveLayer].FBStartAdress + (((LCD_GetXSize()*Ypos) + Xpos)*(4));
  
  /*判断层输入像素格式 */    
  if ((bit_pixel/8) == 4)
  {
    input_color_mode = CM_ARGB8888;
  }
  else if ((bit_pixel/8) == 2)
  {
    input_color_mode = CM_RGB565;   
  }
  else 
  {
    input_color_mode = CM_RGB888;
  }
  
  /* 绕过位图的帧头 */
  pbmp += (index + (width * (height - 1) * (bit_pixel/8)));  
  
  /* 将图片转换为 ARGB8888 像素格式 */
  for(index=0; index < height; index++)
  {
    /* 像素格式转换 */
    LL_ConvertLineToARGB8888((uint32_t *)pbmp, (uint32_t *)address, width, input_color_mode);
    
    /* 递增源和目标缓冲区 */
    address+=  (LCD_GetXSize()*4);
    pbmp -= width*(bit_pixel/8);
  } 
}

/**
  * @brief  填充一个实心矩形
  * @param  Xpos: X坐标值
  * @param  Ypos: Y坐标值
  * @param  Width:  矩形宽度 
  * @param  Height: 矩形高度
  * @retval 无
  */
void LCD_FillRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  uint32_t  x_address = 0;
  
  /* 设置文字颜色 */
  LCD_SetTextColor(DrawProp[ActiveLayer].TextColor);
  
  /* 设置矩形开始地址 */
    if(hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    x_address = (hltdc.LayerCfg[ActiveLayer].FBStartAdress) + 4*(LCD_GetXSize()*Ypos + Xpos);
  }
  else if(hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  {
    x_address = (hltdc.LayerCfg[ActiveLayer].FBStartAdress) + 3*(LCD_GetXSize()*Ypos + Xpos);
  }
  else if((hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565) || \
          (hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
          (hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_AL88))  
  {
    x_address = (hltdc.LayerCfg[ActiveLayer].FBStartAdress) + 2*(LCD_GetXSize()*Ypos + Xpos);   
  }
  else
  {
    x_address = (hltdc.LayerCfg[ActiveLayer].FBStartAdress) + 2*(LCD_GetXSize()*Ypos + Xpos);
  }	
  /* 填充矩形 */
  LL_FillBuffer(ActiveLayer, (uint32_t *)x_address, Width, Height, (LCD_GetXSize() - Width), DrawProp[ActiveLayer].TextColor);
}

/**
  * @brief  填充一个实心圆
  * @param  Xpos: X坐标值
  * @param  Ypos: Y坐标值
  * @param  Radius: 圆的半径
  * @retval 无
  */
void LCD_FillCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t   decision;    /* 决策变量 */ 
  uint32_t  current_x;   /* 当前x坐标值 */
  uint32_t  current_y;   /* 当前y坐标值 */
  
  decision = 3 - (Radius << 1);
  
  current_x = 0;
  current_y = Radius;
  
  LCD_SetTextColor(DrawProp[ActiveLayer].TextColor);
  
  while (current_x <= current_y)
  {
    if(current_y > 0) 
    {
      LCD_DrawHLine(Xpos - current_y, Ypos + current_x, 2*current_y);
      LCD_DrawHLine(Xpos - current_y, Ypos - current_x, 2*current_y);
    }
    
    if(current_x > 0) 
    {
      LCD_DrawHLine(Xpos - current_x, Ypos - current_y, 2*current_x);
      LCD_DrawHLine(Xpos - current_x, Ypos + current_y, 2*current_x);
    }
    if (decision < 0)
    { 
      decision += (current_x << 2) + 6;
    }
    else
    {
      decision += ((current_x - current_y) << 2) + 10;
      current_y--;
    }
    current_x++;
  }
  
  LCD_SetTextColor(DrawProp[ActiveLayer].TextColor);
  LCD_DrawCircle(Xpos, Ypos, Radius);
}

/**
  * @brief  绘制多边形
  * @param  Points: 指向点数组的指针
  * @param  PointCount: 点数
  * @retval 无
  */
void LCD_FillPolygon(pPoint Points, uint16_t PointCount)
{
  int16_t X = 0, Y = 0, X2 = 0, Y2 = 0, X_center = 0, Y_center = 0, X_first = 0, Y_first = 0, pixelX = 0, pixelY = 0, counter = 0;
  uint16_t  image_left = 0, image_right = 0, image_top = 0, image_bottom = 0;
  
  image_left = image_right = Points->X;
  image_top= image_bottom = Points->Y;
  
  for(counter = 1; counter < PointCount; counter++)
  {
    pixelX = POLY_X(counter);
    if(pixelX < image_left)
    {
      image_left = pixelX;
    }
    if(pixelX > image_right)
    {
      image_right = pixelX;
    }
    
    pixelY = POLY_Y(counter);
    if(pixelY < image_top)
    { 
      image_top = pixelY;
    }
    if(pixelY > image_bottom)
    {
      image_bottom = pixelY;
    }
  }  
  
  if(PointCount < 2)
  {
    return;
  }
  
  X_center = (image_left + image_right)/2;
  Y_center = (image_bottom + image_top)/2;
  
  X_first = Points->X;
  Y_first = Points->Y;
  
  while(--PointCount)
  {
    X = Points->X;
    Y = Points->Y;
    Points++;
    X2 = Points->X;
    Y2 = Points->Y;    
    
    FillTriangle(X, X2, X_center, Y, Y2, Y_center);
    FillTriangle(X, X_center, X2, Y, Y_center, Y2);
    FillTriangle(X_center, X2, X, Y_center, Y2, Y);   
  }
  
  FillTriangle(X_first, X2, X_center, Y_first, Y2, Y_center);
  FillTriangle(X_first, X_center, X2, Y_first, Y_center, Y2);
  FillTriangle(X_center, X2, X_first, Y_center, Y2, Y_first);   
}

/**
  * @brief  填充一个实心椭圆
  * @param  Xpos:   X轴坐标
  * @param  Ypos:   Y轴坐标
  * @param  XRadius: 椭圆X轴半径
  * @param  YRadius: 椭圆Y轴半径
  * @retval 无
  */
void LCD_FillEllipse(int Xpos, int Ypos, int XRadius, int YRadius)
{
  int x = 0, y = -YRadius, err = 2-2*XRadius, e2;
  float k = 0, rad1 = 0, rad2 = 0;
  
  rad1 = XRadius;
  rad2 = YRadius;
  
  k = (float)(rad2/rad1);
  
  do 
  {       
    LCD_DrawHLine((Xpos-(uint16_t)(x/k)), (Ypos+y), (2*(uint16_t)(x/k) + 1));
    LCD_DrawHLine((Xpos-(uint16_t)(x/k)), (Ypos-y), (2*(uint16_t)(x/k) + 1));
    
    e2 = err;
    if (e2 <= x) 
    {
      err += ++x*2+1;
      if (-y == x && e2 <= y) e2 = 0;
    }
    if (e2 > y) err += ++y*2+1;
  }
  while (y <= 0);
}

/**
  * @brief  使能显示
  * @retval 无
  */
void LCD_DisplayOn(void)
{
  /* 开显示 */
  __HAL_LTDC_ENABLE(&hltdc);
  HAL_GPIO_WritePin(LTDC_DISP_GPIO_Port, LTDC_DISP_Pin, GPIO_PIN_SET);/* LCD_DISP使能*/
  HAL_GPIO_WritePin(LTDC_BL_GPIO_Port, LTDC_BL_Pin, GPIO_PIN_SET);  /* 开背光*/
}

/**
  * @brief  禁能显示
  * @retval 无
  */
void LCD_DisplayOff(void)
{
  /* 关显示 */
  __HAL_LTDC_DISABLE(&hltdc);
  HAL_GPIO_WritePin(LTDC_DISP_GPIO_Port, LTDC_DISP_Pin, GPIO_PIN_RESET); /* LCD_DISP禁能*/
  HAL_GPIO_WritePin(LTDC_BL_GPIO_Port, LTDC_BL_Pin, GPIO_PIN_RESET);/*关背光*/
}

/**
  * @brief  时钟配置
  * @param  hltdc: LTDC句柄
  * @param  Params
  * @retval 无
  */
void LCD_ClockConfig(LTDC_HandleTypeDef *hltdc, void *Params)
{
  static RCC_PeriphCLKInitTypeDef  periph_clk_init_struct;

  /* LCD时钟配置 */
  /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
  /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 192 Mhz */
  /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 192/5 = 38.4 Mhz */
  /* LTDC时钟频率 = PLLLCDCLK / LTDC_PLLSAI_DIVR_4 = 38.4/4 = 9.6Mhz */
  periph_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  periph_clk_init_struct.PLLSAI.PLLSAIN = 400;
  periph_clk_init_struct.PLLSAI.PLLSAIR = 4;
  periph_clk_init_struct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
  HAL_RCCEx_PeriphCLKConfig(&periph_clk_init_struct);
}


/*******************************************************************************
                            局部函数
*******************************************************************************/

/**
  * @brief  显示一个字符
  * @param  Xpos: 显示字符的行位置
  * @param  Ypos: 列起始位置
  * @param  c: 指向字体数据的指针
  * @retval 无
  */
static void DrawChar(uint16_t Xpos, uint16_t Ypos, const uint8_t *c)
{
  uint32_t i = 0, j = 0;
  uint16_t height, width;
  uint8_t  offset;
  uint8_t  *pchar;
  uint32_t line;
  
  height = DrawProp[ActiveLayer].pFont->Height;//获取正在使用字体高度
  width  = DrawProp[ActiveLayer].pFont->Width; //获取正在使用字体宽度
  
  offset =  8 *((width + 7)/8) -  width ;//计算字符的每一行像素的偏移值，实际存储大小-字体宽度
  
  for(i = 0; i < height; i++)//遍历字体高度绘点
  {
    pchar = ((uint8_t *)c + (width + 7)/8 * i);//计算字符的每一行像素的偏移地址
    
    switch(((width + 7)/8))//根据字体宽度来提取不同字体的实际像素值
    {
      
    case 1:
      line =  pchar[0];      //提取字体宽度小于8的字符的像素值
      break;
      
    case 2:
      line =  (pchar[0]<< 8) | pchar[1]; //提取字体宽度大于8小于16的字符的像素值     
      break;
      
    case 3:
    default:
      line =  (pchar[0]<< 16) | (pchar[1]<< 8) | pchar[2]; //提取字体宽度大于16小于24的字符的像素值     
      break;
    } 
    
    for (j = 0; j < width; j++)//遍历字体宽度绘点
    {
      if(line & (1 << (width- j + offset- 1))) //根据每一行的像素值及偏移位置按照当前字体颜色进行绘点
      {
        LCD_DrawPixel((Xpos + j), Ypos, DrawProp[ActiveLayer].TextColor);
      }
      else//如果这一行没有字体像素则按照背景颜色绘点
      {
        LCD_DrawPixel((Xpos + j), Ypos, DrawProp[ActiveLayer].BackColor);
      } 
    }
    Ypos++;
  }
}

/**
  * @brief  填充三角形（基于三点）
  * @param  x1: 第一点的X坐标值
  * @param  y1: 第一点的Y坐标值
  * @param  x2: 第二点的X坐标值
  * @param  y2: 第二点的Y坐标值
  * @param  x3: 第三点的X坐标值
  * @param  y3: 第三点的Y坐标值
  * @retval 无
  */
static void FillTriangle(uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3)
{ 
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, num_add = 0, num_pixels = 0,
  curpixel = 0;
  
  deltax = ABS(x2 - x1);        /* 求x轴的绝对值 */
  deltay = ABS(y2 - y1);        /* 求y轴的绝对值 */
  x = x1;                       /* 第一个像素的x坐标起始值 */
  y = y1;                       /* 第一个像素的y坐标起始值 */
  
  if (x2 >= x1)                 /* x坐标值为递增*/
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          /* x坐标值为递减 */
  {
    xinc1 = -1;
    xinc2 = -1;
  }
  
  if (y2 >= y1)                 /* y坐标值为递增*/
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          /* y坐标值为递减 */
  {
    yinc1 = -1;
    yinc2 = -1;
  }
  
  if (deltax >= deltay)         /* 每个 y 坐标值至少有一个x坐标值*/
  {
    xinc1 = 0;                  /* 当分子大于或等于分母时不要改变 x */
    yinc2 = 0;                  /* 不要为每次迭代更改 y */
    den = deltax;
    num = deltax / 2;
    num_add = deltay;
    num_pixels = deltax;         /* x比y多的值 */
  }
  else                          /* 每个 x 坐标值至少有一个y坐标值 */
  {
    xinc2 = 0;                  /* 不要为每次迭代更改 x */
    yinc1 = 0;                  /* 当分子大于或等于分母时不要改变 y */
    den = deltay;
    num = deltay / 2;
    num_add = deltax;
    num_pixels = deltay;         /* y比x多的值 */
  }
  
  for (curpixel = 0; curpixel <= num_pixels; curpixel++)
  {
    LCD_DrawLine(x, y, x3, y3);
    
    num += num_add;              /* 在分数的基础上增加分子 */
    if (num >= den)             /* 判断分子是否大于或等于分母 */
    {
      num -= den;               /* 计算新的分子值 */
      x += xinc1;               /* x值递增 */
      y += yinc1;               /* y值递增 */
    }
    x += xinc2;                 /* x值递增 */
    y += yinc2;                 /* y值递增 */
  } 
}

/**
  * @brief  填充一个缓冲区
  * @param  LayerIndex: 当前层
  * @param  pDst: 指向目标缓冲区指针
  * @param  xSize: 缓冲区宽度
  * @param  ySize: 缓冲区高度
  * @param  OffLine: 偏移量
  * @param  ColorIndex: 当前颜色
  * @retval None
  */
static void LL_FillBuffer(uint32_t LayerIndex, void *pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex) 
{

  hdma2d.Init.Mode         = DMA2D_R2M;
  if(hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
  { 
    hdma2d.Init.ColorMode    = DMA2D_RGB565;
  }
  else if(hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  { 
    hdma2d.Init.ColorMode    = DMA2D_ARGB8888;
  }
  else if(hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  { 
    hdma2d.Init.ColorMode    = DMA2D_RGB888;
  }
  else if(hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB1555)
  { 
    hdma2d.Init.ColorMode    = DMA2D_ARGB1555;
  }
  else if(hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444)
  { 
    hdma2d.Init.ColorMode    = DMA2D_ARGB4444;
  }
  hdma2d.Init.OutputOffset = OffLine;      
  
  hdma2d.Instance = DMA2D;
  
  /* DMA2D 初始化 */
  if(HAL_DMA2D_Init(&hdma2d) == HAL_OK) 
  {
    if(HAL_DMA2D_ConfigLayer(&hdma2d, LayerIndex) == HAL_OK) 
    {
      if (HAL_DMA2D_Start(&hdma2d, ColorIndex, (uint32_t)pDst, xSize, ySize) == HAL_OK)
      {
        /* DMA轮询传输 */  
        HAL_DMA2D_PollForTransfer(&hdma2d, 100);
      }
    }
  } 
  
//  if(HAL_DMA2D_Init(&hdma2d) == HAL_OK) 
//  {
//    if(HAL_DMA2D_ConfigLayer(&hdma2d, LayerIndex) == HAL_OK) 
//    {
//      if (HAL_DMA2D_Start_IT(&hdma2d, ColorIndex, (uint32_t)pDst, xSize, ySize) != HAL_OK)
//      {
////        /* DMA轮询传输 */  
////        HAL_DMA2D_PollForTransfer(&hdma2d, 100);
//      }
//    }
//  } 
  
}

/**
  * @brief  转换一行为ARGB8888像素格式
  * @param  pSrc: 指向源缓冲区的指针
  * @param  pDst: 输出颜色
  * @param  xSize: 缓冲区宽度
  * @param  ColorMode: 输入颜色模式   
  * @retval 无
  */
static void LL_ConvertLineToARGB8888(void *pSrc, void *pDst, uint32_t xSize, uint32_t ColorMode)
{    
  /* 配置DMA2D模式,颜色模式和输出偏移 */
  hdma2d.Init.Mode         = DMA2D_M2M_PFC;
  hdma2d.Init.ColorMode    = DMA2D_ARGB8888;
  hdma2d.Init.OutputOffset = 0;     
  
  /* Foreground Configuration */
  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hdma2d.LayerCfg[1].InputAlpha = 0xFF;
  hdma2d.LayerCfg[1].InputColorMode = ColorMode;
  hdma2d.LayerCfg[1].InputOffset = 0;
  
  hdma2d.Instance = DMA2D; 
  
  /* DMA2D 初始化 */
  if(HAL_DMA2D_Init(&hdma2d) == HAL_OK) 
  {
    if(HAL_DMA2D_ConfigLayer(&hdma2d, 1) == HAL_OK) 
    {
      if (HAL_DMA2D_Start(&hdma2d, (uint32_t)pSrc, (uint32_t)pDst, xSize, 1) == HAL_OK)
      {
        /* DMA轮询传输*/  
        HAL_DMA2D_PollForTransfer(&hdma2d, 10);
      }
    }
  }

//  if(HAL_DMA2D_Init(&hdma2d) == HAL_OK) 
//  {
//    if(HAL_DMA2D_ConfigLayer(&hdma2d, 1) == HAL_OK) 
//    {
//      if (HAL_DMA2D_Start_IT(&hdma2d, (uint32_t)pSrc, (uint32_t)pDst, xSize, 1) != HAL_OK)
//      {
////        /* DMA轮询传输*/  
////        HAL_DMA2D_PollForTransfer(&hdma2d, 10);
//      }
//    }
//  }
}
  


/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/

/************************ (C) COPYRIGHT greedyhao *****END OF FILE****/

