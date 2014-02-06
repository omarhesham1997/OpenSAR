
// ==================================== INCLUDEs =========================
#include "Gui.h"
#include <math.h>
#include <assert.h>

// ==================================== MACROs   =========================

// ==================================== TYPEs    =========================

// ==================================== DATAs    =========================
static const GuiConfig_Type* pConfig = NULL;
// ==================================== FUNCTIONs=========================
static void Calc(int *pX,int *pY,const GuiPosition_Type* center,const GuiPosition_Type* center2,uint32 degree)
{ // This is difficult, I think I need a lib to do this job
	if(degree == 0)
	{
		// output by default
	}
	else
	{
		double O = (double)M_PI*degree / (double)180;
		int X = pX[0];
		int Y = pY[0];

		pX[0]= (int)(center->x + (double)(X-center->x)*cos(O) - (double)(Y-center->y)*sin(O));
		pY[0]= (int)(center->y + (double)(X-center->x)*sin(O) + (double)(Y-center->y)*cos(O));

		pX[0] += center2->x*(1-cos(O)) + center2->y*sin(O);
		pY[0] -= center2->x*sin(O)     - center2->y*(1-cos(O));


	}
}
static void Draw(uint32 Widget)
{
	GuiWidgetContext_Type* pContext = pConfig->widgets[Widget].pContext;
	const GuiImage_Type*     image = pConfig->widgets[Widget].image;
	uint32 degree = (pContext->degree%360);
	if(0u == degree)
	{
		LCDD_DrawGIMPImage(pContext->area.top_left.x,
							pContext->area.top_left.y,
							image);
	}
	else
	{	// rotate draw
		uint8* pImage= image->pucPixel_data;

		for(uint32_t Y=pContext->area.top_left.y;Y<(pContext->area.top_left.y+image->dwHeight);Y++)
		{
			for(uint32_t X=pContext->area.top_left.x;X<(pContext->area.top_left.x+image->dwWidth);X++)
			{
				int tX,tY;
				tX=X;tY=Y;
				Calc(&tX,&tY,&(pContext->area.top_left),&(pConfig->widgets[Widget].center),degree);
				uint32 dwColor = ((uint32_t)pImage[0]<<16) + ((uint32_t)pImage[1]<<8) + (pImage[2]);
				LCDD_DrawPixel(tX,tY,dwColor);
				pImage = pImage+3;
			}
		}
	}
}
void Gui_Init(const GuiConfig_Type* config)
{
	pConfig = config;
	for(uint32 W=0;W<(pConfig->number);W++)
	{
		GuiWidget_Type* pWidget = &(pConfig->widgets[W]);
		memcpy(pWidget->pContext,&(pWidget->defaultContext),sizeof(GuiWidgetContext_Type));
	}
}

Std_ReturnType Gui_SetWidgetArea(uint32 id,uint32 x,uint32 y,uint32 width,uint32 height)
{
	Std_ReturnType ercd = E_OK;
	if((NULL != pConfig) && (id<pConfig->number))
	{
		GuiWidgetContext_Type* pContext = pConfig->widgets[id].pContext;
		pContext->area.top_left.x=x;
		pContext->area.top_left.y=y;
		pContext->area.width=width;
		pContext->area.height=height;
	}
	else
	{
		ercd = E_NOT_OK;
	}
	return ercd;
}

Std_ReturnType Gui_SetWidgetDegree(uint32 id,uint32 degree)
{
	Std_ReturnType ercd = E_OK;
	if((NULL != pConfig) && (id<pConfig->number))
	{
		GuiWidgetContext_Type* pContext = pConfig->widgets[id].pContext;
		pContext->degree = degree;
	}
	else
	{
		ercd = E_NOT_OK;
	}
	return ercd;
}
Std_ReturnType Gui_SetWidgetLayer(uint32 id,uint8 layer)
{
	Std_ReturnType ercd = E_OK;
	if((NULL != pConfig) && (id<pConfig->number))
	{
		GuiWidgetContext_Type* pContext = pConfig->widgets[id].pContext;
		pContext->layer = layer;
	}
	else
	{
		ercd = E_NOT_OK;
	}
	return ercd;
}
void Gui_MainFunction(void)
{
	LCDD_On();
	if(NULL != pConfig)
	{
		for(uint8 layer=0;layer<GUI_MAX_LAYERS;layer++)
		{
			for(uint32 W=0;W<pConfig->number;W++)
			{
				GuiWidgetContext_Type* pContext = pConfig->widgets[W].pContext;
				if(layer == pContext->layer)
				{
					Draw(W);
				}
			}
		}
	}
}
