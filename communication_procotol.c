
//file name: communication_procotol.c

#define __COMMUNICATION_PROTOCOL_C
#include "communication_wrport.h"
#include "bmpfile.h"

#define BRIGHTNESS_SPECIAL_TEST   //强减亮度 5% 15% 30%

ST_CONTEXT_TAG *stgContextp;

static unsigned char  *ucGrcvUpdataSoft_p;
static unsigned int    uiGRcvUpdataSoftLen;

int GetSelfFontLib_crc(ST_FONTIMAGE_CRC_TAG *fontCrcp);
int GetSelfImageLib_crc(ST_FONTIMAGE_CRC_TAG *pDest);
void CreatCmsBitMap(ST_CONTEXT_TAG *stContextp);
unsigned char GetCmsReady(ST_CONTEXT_TAG *stContextp);

const static unsigned int uigTimeReport[]={0,1,2,5,60,300};//定时回报时间 单位秒

const static ST_SYSCONFIG_TAG sysconfig_init=
{
	//.ST_CIRCULATION_DISPLAY={0},	  //循环显示参数设置
	//.ucCurrShow_MsgId={0},		  // 1 - 127:text,128-254:binmap 
	//.CurrZoneGroupId_t={0},
	//.ViewMode={0},
	.stOffLine={1,0}, 				  //脱机显示模式之设定
	.stBreakLine={1,0},
	//.ST_BRIGHTNESS={0}, 			  //设备亮度控制之设定
	.ST_SET_LOCK_UNLOCK={0},		  //锁定或解除现场操作
	//.ST_RET_DEVICE_NUM={0}, 		  //设备编号  
	//.ST_RET_SOFTVER={0},			  //软件版本
	//.ST_PASSWORD={0},				  //资料库密码	  
		
	.address={0xff,0xff},				  //编码地址	
		
	.ipConfig=
	{
        .ipaddr={192,168,1,6},
		.gateway={192,168,1,1}, 
		.netmask={255,255,255,0},
		.portaddr={0x1f,0x90},	
	},
	.ST_BRIGHTNESS_DEF =
	{
		.adc = {1500,2000,2400},
		.rate= {40,30,20,10},
	},
	.packetsize_upload = LOAD_PACKETSIZE_DEF,
	.tMisc =
	{
	    .xinbei = 0,
		.Mfail_LedNum = 8,
	}
};


const static unsigned short ucgProtolCode_Cmd[]=
{
    AF10H_SET_CIRCULATION_DISPLAY,//循环显示参数设置
    AF40H_GET_CIRCULATION_DISPLAY,//循环显示参数查询	
    AFC0H_RET_CIRCULATION_DISPLAY,//循环显示参数回报

    AF11H_SET_PRESET_TEXT,//预设全文管理之设定
    AF41H_GET_PRESET_TEXT,//预设全文管理之查询
    AFC1H_RET_PRESET_TEXT,//预设全文管理之回报
	
    AF12H_SET_PRESET_TEXT_COLOR,//预设全文彩色参数管理之设定
    AF42H_GET_PRESET_TEXT_COLOR,//预设全文彩色参数管理之查询
    AFC2H_RET_PRESET_TEXT_COLOR,//预设全文彩色参数管理之回报

    AF13H_SET_CURRENT_TEXT,//当前全文显示管理之设定
    AF43H_GET_CURRENT_TEXT,//当前全文显示管理之查询
    AFC3H_RET_CURRENT_TEXT,//当前全文显示管理之回报
    AF14H_CLR_CURRENT_TEXT,//当前显示消除处理

    AF44H_GET_CURRENT_TEXT_COLOR,//当前全文彩色参数管理之查询
    AFC4H_RET_CURRENT_TEXT_COLOR,//当前全文彩色参数管理之回报

    AF15H_SET_MARQUEE,//跑马灯设定
    AF45H_GET_MARQUEE,//跑马灯查询	
    AFC5H_RET_MARQUEE,//跑马灯回报

    AF16H_SET_CHINESE_FONT,//中文字形处理设置
    AF46H_GET_CHINESE_FONT,//中文字形处理查询	
    AFC6H_RET_CHINESE_FONT,//中文字形处理回报
    AF17H_CLR_CHINESE_FONT,//中文字形处理清除

    AF47H_GET_DISPLAY_MODULE_STATUS,//显示模组状态查询
    AFC7H_RET_DISPLAY_MODULE_STATUS,//显示模组状态回报

    AF18H_SET_COLOR_SINGLE_TEXT,//彩色字窗图形管理之设定
    AF48H_GET_COLOR_SINGLE_TEXT,//彩色字窗图形管理之查询	
    AFC8H_RET_COLOR_SINGLE_TEXT,//彩色字窗图形管理之回报

    AF31H_SET_OFFLINE_MODE,//脱机显示模式之设定
    AF61H_GET_OFFLINE_MODE,//脱机显示模式之查询	
    AFE1H_RET_OFFLINE_MODE,//脱机显示模式之回报

    AF3EH_SET_BRIGHTNESS,//设备亮度控制之设定
    AF6EH_GET_BRIGHTNESS,//设备亮度控制之查询回报
    AFEEH_RET_BRIGHTNESS_SLAVE,//设备亮度控制之查询回报
    AF0EH_RET_BRIGHTNESS_MASTER ,//设备亮度控制之主动回报
	
//----------------------扩展----------------------------
    AE10H_UPDATASOF_SET,////软件下载
    AE40H_UPDATASOF_GETCRC,
    AEC0H_UPDATASOF_RETCRC,
    AEC1H_UPDATASOF_SETEND,
    AE20H_CLEAN_USERFILE,//清除用户图片

    AE43H_GET_CHECKFONT,
    AEC3H_RET_CHECKFONT,
    AE44H_GET_CHECKIMAGE,
    AEC4H_RET_CHECKIMAGE,
	
    AE05H_SET_NET_CONF,//IP及MAC设置
    AE45H_GET_NET_CONF,//IP及MAC查询
    AEC5H_REG_NET_CONF,//IP及MAC回复
	
	AE06H_SET_ADDRESSCODE,
	AE46H_GET_ADDRESSCODE,
	AEC6H_RET_ADDRESSCODE,

    AE07H_SET_CURRBRIGHT,//设置一半亮度
    AE47H_GET_CURRBRIGHT,//查询当前亮度值
    AEC7H_RET_CURRBRIGHT,//回报当前亮度值    
	
    AE08H_SET_BRIGHTTAB,
    AE48H_GET_BRIGHTTAB,
    AEC8H_RET_BRIGHTTAB,

    AE23H_GET_CONTRMODE,//控制方式 远程及现场
    AE53H_REG_CONTRMODE,

    AF1AH_SET_INSERT,//設定插入區域及文字參數
    AF4AH_GET_INSERT,//查詢插入區域及文字參數
	AFCAH_RET_INSERT,//回報插入區域及文字參數


	AF51H_SET_RGIMAGE,
	AF54H_GET_RGIMAGE,
	AF56H_RET_RGIMAGE,

    AE67H_GET_MISC,//读取其它杂项参数
    AE68H_RET_MISC,
	AE69H_SET_MISC,

	//-----------------------通用-----------------------------
	_0F80H_RET_SETOK,//回报设定信息正确
	_0F81H_RET_SETERR,//回报设定或查询指令无效
	_0F10H_SET_REBOOT,//重启现场设备
	_0F90H_RET_REBOOT,//回报现场设备重设置
	_0F40H_GET_DEVICE_NUM,//查询现场设备编号
	_0FC0H_RET_DEVICE_NUM,//回报现场设备编号
	_0F00H_RET_POWERBREAK,//回报前次电源中断
	_0F11H_SET_RESET,//重设定现场设备
	_0F91H_RET_RESET,//回报通讯重设定
	_0F41H_GET_DEVICE_STATUS,//查询现场设备状态
	_0FC1H_RET_DEVICE_STATUS,//回报查询现场设备状态
	_0F12H_SET_DATETIME,//设定现场设备日期时间
	_0F92H_DATETIME_SYNCH,//要求中心对时
	_0F42H_GET_DATETIME,//查询现场设备日期时间
	_0FC2H_RET_DATETIME,//回报现场设备日期时间
	_0F13H_SET_SOFTVER,//设定现场设备信息等级
	_0F43H_GET_SOFTVER,//查询软件版本
	_0FC3H_RET_SOFTVER,//回报软件版本
	_0F14H_SET_HARDSTATUS_REPORTCLY,//设定现场设备回报硬件状态之周期
	_0F4FH_GET_HARDSTATUS_REPORTCLY,//查询现场设备回报硬件状态之周期
	_0FC4H_RET_HARDSTATUS_REPORTCLY,//查询后现场设备回报硬件状态之周期
	_0F04H_RET_HARDSTATUE_REPORT,//现场设备回报硬件状态

	_0F15H_SET_DATA_PASSWORD,//设定资料库操作的密码
	_0F45H_GET_DATA_PASSWORD,//查询资料库操作的密码
	_0FC5H_RET_DATA_PASSWORD,//回复资料库操作的密码

	_0F16H_SET_LOCK_UNLOCK,//锁定或解除现场操作
	_0F46H_GET_LOCK_UNLOCK,//查询锁定或解除现场操作
	_0FC6H_RET_LOCK_UNLOCK,//回报查询锁定或解除现场操作

	_0F47H_GET_PROTOL_CODE,//查询通信协定 
	_0FC7H_RET_PROTOL_CODE,//回复查询通信协定 

	_0F02H_REBOOT_REP,    //重启报告
};

void msleep(unsigned int m);

const ST_CMDFUN_TAG stProtoCmdFun[]=
{
	{AF10H_SET_CIRCULATION_DISPLAY,AF10H_SET_CIRCULATION_DISPLAY_FUN}, //循环显示参数设置    
	{AF40H_GET_CIRCULATION_DISPLAY,AF40H_GET_CIRCULATION_DISPLAY_FUN},//循环显示参数查询	

	{AF11H_SET_PRESET_TEXT,AF11H_SET_PRESET_TEXT_FUN},//预设全文管理之设定
	{AF41H_GET_PRESET_TEXT,AF41H_GET_PRESET_TEXT_FUN},//预设全文管理之查询
	
	{AF12H_SET_PRESET_TEXT_COLOR,AF12H_SET_PRESET_TEXT_COLOR_FUN},//预设全文彩色参数管理之设定
	{AF42H_GET_PRESET_TEXT_COLOR,AF42H_GET_PRESET_TEXT_COLOR_FUN},//预设全文彩色参数管理之查询

	{AF14H_CLR_CURRENT_TEXT,AF14H_CLR_CURRENT_TEXT_FUN},//当前显示消除处理
	{AF13H_SET_CURRENT_TEXT,AF13H_SET_CURRENT_TEXT_FUN},//当前全文显示管理之设定
	
#if(PARK_CMS_ENB)
	{AF43H_GET_CURR_PARK_CONTEXT,AF43H_GET_CURR_PARK_CONTEXT_FUN},
    {AF44H_GET_CURRENT_TEXT_COLOR,AF44H_GET_CURR_PARK_TEXT_COLOR_FUN},
#else
	{AF43H_GET_CURRENT_TEXT,AF43H_GET_CURRENT_TEXT_FUN},//当前全文显示管理之查询
	{AF44H_GET_CURRENT_TEXT_COLOR,AF44H_GET_CURRENT_TEXT_COLOR_FUN},//当前全文彩色参数管理之查询
#endif

	{AF15H_SET_MARQUEE,AF15H_SET_MARQUEE_FUN},//跑马灯设定
	{AF45H_GET_MARQUEE,AF45H_GET_MARQUEE_FUN},//跑马灯查询 

	{AF16H_SET_CHINESE_FONT,AF16H_SET_CHINESE_FONT_FUN},//中文字形处理设置
	{AF46H_GET_CHINESE_FONT,AF46H_GET_CHINESE_FONT_FUN},//中文字形处理查询	
	{AF17H_CLR_CHINESE_FONT,AF17H_CLR_CHINESE_FONT_FUN},//中文字形处理清除

	{AF47H_GET_DISPLAY_MODULE_STATUS,AF47H_GET_DISPLAY_MODULE_STATUS_FUN},//显示模组状态查询

	{AF18H_SET_COLOR_SINGLE_TEXT,AF18H_SET_COLOR_SINGLE_TEXT_FUN},//彩色字窗图形管理之设定
	{AF48H_GET_COLOR_SINGLE_TEXT,AF48H_GET_COLOR_SINGLE_TEXT_FUN},//彩色字窗图形管理之查询	

	{AF31H_SET_OFFLINE_MODE,AF31H_SET_OFFLINE_MODE_FUN},//脱机显示模式之设定
	{AF61H_GET_OFFLINE_MODE,AF61H_GET_OFFLINE_MODE_FUN},//脱机显示模式之查询	

	{AF3EH_SET_BRIGHTNESS,AF3EH_SET_BRIGHTNESS_FUN},//设备亮度控制之设定
	{AF6EH_GET_BRIGHTNESS,AF6EH_GET_BRIGHTNESS_FUN},//设备亮度控制之查询回报

	{AF52H_GET_BITMAP_JUNYE,AF52H_GET_BITMAP_JUNYE_FUN},

	
	{AF32H_SET_PRESET_MSG,AF32H_SET_PRESET_MSG_FUN},//预设信息设定
	{AF62H_GET_PRESET_MSG,AF62H_GET_PRESET_MSG_FUN},//预设信息查询

	{AF33H_SET_CURR_MSG,AF33H_SET_CURR_MSG_FUN},//选择预设信息
	//{AF63H_GET_CURR_MSG,AF63H_GET_CURR_MSG_FUN},//查询当前信息

    {AF1AH_SET_INSERT,AF1AH_SET_INSERT_FUN},
    {AF4AH_GET_INSERT,AF4AH_GET_INSERT_FUN},

//---------------------------------------------------------------------------------------------------------
	{_0F10H_SET_REBOOT,_0F10H_SET_REBOOT_FUN},//重启现场设备
	{_0F40H_GET_DEVICE_NUM,_0F40H_GET_DEVICE_NUM_FUN},//查询现场设备编号
	{_0F11H_SET_RESET,_0F11H_SET_RESET_FUN},//重设定现场设备
	{_0F41H_GET_DEVICE_STATUS,_0F41H_GET_DEVICE_STATUS_FUN},//查询现场设备状态
	{_0F12H_SET_DATETIME,_0F12H_SET_DATETIME_FUN},//设定现场设备日期时间
	{_0F42H_GET_DATETIME,_0F42H_GET_DATETIME_FUN},//查询现场设备日期时间
	{_0F13H_SET_SOFTVER,_0F13H_SET_SOFTVER_FUN},//设定现场设备信息等级
	{_0F43H_GET_SOFTVER,_0F43H_GET_SOFTVER_FUN},//查询软件版本
	{_0F14H_SET_HARDSTATUS_REPORTCLY,_0F14H_SET_HARDSTATUS_REPORTCLY_FUN},//设定现场设备回报硬件状态之周期
	{_0F4FH_GET_HARDSTATUS_REPORTCLY,_0F4FH_GET_HARDSTATUS_REPORTCLY_FUN},//查询现场设备回报硬件状态之周期

	{_0F15H_SET_DATA_PASSWORD,_0F15H_SET_DATA_PASSWORD_FUN}, //设定资料库操作的密码
	{_0F45H_GET_DATA_PASSWORD,_0F45H_GET_DATA_PASSWORD_FUN}, //查询资料库操作的密码
	
	{_0F16H_SET_LOCK_UNLOCK,_0F16H_SET_LOCK_UNLOCK_FUN},//锁定或解除现场操作
	{_0F46H_GET_LOCK_UNLOCK,_0F46H_GET_LOCK_UNLOCK_FUN},//查询锁定或解除现场操作

	{_0F47H_GET_PROTOL_CODE,_0F47H_GET_PROTOL_CODE_FUN},//查询通信协定 

	{_1008H_TEST_WDT,_0F10H_SET_REBOOT_FUN},
    {AE33H_SET_CMSERRCHECK,AE33H_SET_CMSERRCHECK_FUN},
	{AE10H_UPDATASOF_SET,AE10H_UPDATASOF_SET_FUN}, //软件下载
	{AE40H_UPDATASOF_GETCRC,AE40H_UPDATASOF_GETCRC_FUN},
	{AEC1H_UPDATASOF_SETEND,AEC1H_UPDATASOF_SETEND_FUN},
	{AE20H_CLEAN_USERFILE,AE20H_CLEAN_USERFILE_FUN},//清除用户图片
	{AE05H_SET_NET_CONF,AE05H_SET_NET_CONF_FUN},
	{AE45H_GET_NET_CONF,AE45H_GET_NET_CONF_FUN},
	{AE06H_SET_ADDRESSCODE,AE06H_SET_ADDRESSCODE_FUN},  //
	{AE46H_GET_ADDRESSCODE,AE46H_GET_ADDRESSCODE_FUN},  //
	{AE07H_SET_CURRBRIGHT,AE07H_SET_CURRBRIGHT_FUN},
	{AE47H_GET_CURRBRIGHT,AE47H_GET_CURRBRIGHT_FUN},	
	{AE08H_SET_BRIGHTTAB,AE08H_SET_BRIGHTTAB_FUN},	
	{AE48H_GET_BRIGHTTAB,AE48H_GET_BRIGHTTAB_FUN},
	{AE23H_GET_CONTRMODE,AE23H_GET_CONTRMODE_FUN},
	{AE43H_GET_CHECKFONT,AE43H_GET_CHECKFONT_FUN},
	{AE44H_GET_CHECKIMAGE,AE44H_GET_CHECKIMAGE_FUN},
	{AE11H_SET_BREAKLINE,AE11H_SET_BREAKLINE_FUN},
	{AE12H_GET_BREAKLINE,AE12H_GET_BREAKLINE_FUN},
	{AE60H_SET_PACKSIZE_UPLOAD,AE60H_SET_PACKSIZE_UPLOAD_FUN},
	{AE61H_GET_PACKSIZE_UPLOAD,AE61H_GET_PACKSIZE_UPLOAD_FUN},
	{AE18H_SET_CENTER_PORT,AE18H_SET_CENTER_PORT_FUN},
	{AE19H_GET_CENTER_PORT,AE19H_GET_CENTER_PORT_FUN},   
	{AE65H_GET_CMSTYPE,AE65H_GET_CMSTYPE_FUN},
    {AE67H_GET_MISC,AE67H_GET_MISC_FUN},
	{AE69H_SET_MISC,AE69H_SET_MISC_FUN},

    {AE6A_WDT_TEST,AE6A_WDT_TEST_FUN},

    {A005H_RET_CMS_ERRMAP,A005H_RET_CMS_ERRMAP_FUN},
    {A007H_RET_CMS_SHOWD,A007H_RET_CMS_SHOWD_FUN},
	{A002H_RET_CMS_TICK,A002H_RET_CMS_TICK_FUN},
		
#if(TC_PANEL_ENB)
//PANEL
	{A009H_GET_PANEL_STATE,A009H_GET_PANEL_STATE_FUN},
	{A00BH_GET_PANEL_IN,A00BH_GET_PANEL_IN_FUN},		
#endif

    //{AF51H_SET_RGIMAGE,AF51H_SET_RGIMAGE_FUN},

#if (PARK_CMS_ENB)
	{AF19H_SET_PARK_CONTEXT,AF19H_SET_PARK_CONTEXT_FUN},
	//{AF43H_GET_CURR_PARK_CONTEXT,AF43H_GET_CURR_PARK_CONTEXT_FUN},
	{AF32H_SET_PARK_CONTEXT_RGCOLOR,AF32H_SET_PARK_CONTEXT_RGCOLOR_FUN},
	{AF23H_ADD_PARK_INFO,AF23H_ADD_PARK_INFO_FUN},
	{AF53H_GET_PARK_INFO,AF53H_GET_PARK_INFO_FUN},
	{AF24H_DEL_PARK_INFO,AF24H_DEL_PARK_INFO_FUN},
	{AF55H_GET_PARK_INFO_ERR,AF55H_GET_PARK_INFO_ERR_FUN},
	{AF62H_SET_PARK_ICON,AF62H_SET_PARK_ICON_FUN},
	{AF72H_GET_PARK_ICON,AF72H_GET_PARK_ICON_FUN},
	{AF63H_SET_PARK_INDICATE,AF63H_SET_PARK_INDICATE_FUN},
	{AF73H_GET_PARK_INDICATE,AF73H_GET_PARK_INDICATE_FUN},
#endif	
};

//-------------------------------------------

const unsigned char CRC8Table[]={
  0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
  157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
  35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
  190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
  70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
  219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
  101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
  248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
  140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
  17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
  175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
  50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
  202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
  87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
  233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
  116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};

static void led_ctl(unsigned char num,unsigned char on);
static void switch_ctr();
void sysreset(ST_CONTEXT_TAG *stContextp);
static int AddInfoToSendList(ST_PORTRCVTXD_TAG *stPortp,unsigned char type);
unsigned short GetColor(unsigned char rg);

unsigned char CRC8(unsigned char *p, int len)
{
    unsigned char crc8 = 0;

    for( ; len > 0; len--){
        crc8 = CRC8Table[crc8 ^ *p];
        p++;
    }
    return(crc8);

}

int GetIpport(void)
{
	int ipport;

	ipport = stgContextp->stSysConfig.ipConfig.portaddr[0];
	ipport <<= 8;
	ipport |=stgContextp->stSysConfig.ipConfig.portaddr[1];
    return ipport;
}


//****************************************************************************
// function: check if any alone ASCII character in the buffer
//proto : unsigned char Check,AF11(unsigned char *buf,unsigned char len)
//parameters:
//       unsigned char *buf: message buffer;
//       unsigned char len: word length
//return value:    0-->none;  
//			   not 0-->there is alone ASCII character in the buff
//****************************************************************************
int Check_AF11(unsigned char *buf,unsigned char len)
{
	unsigned char i,blen;

//	if(len == 0)
	//	return -1;
	if(buf == NULL)
		return -1;
	
	i = 0;
	blen = (len <<1);
	blen -= 1; // ?
	while(i < blen)
	{
		if((buf[i] < 0x80) &&(buf[i+1] > 0x80))	// one ascii character
			return -1;
		i += 2;
	}
	return 0;
}




//-------------------------------------------

/*********************************************************************************************************
** Phototype:	  INT16U Save_FrameToFont(INT8U*sbuf,INT8U*dbuf)
** Descriptions:	    save one frame to  font lib data
** input parameters:   INT8U*sbuf : ST_SET_CHINESE_FONT_TAG struct address
**				      INT8U*dbuf : one font lib data address
** Returned value:      int16u the len of the ucBitMap
*********************************************************************************************************/
unsigned int Save_FrameToWordWin(unsigned char *sbuf,unsigned char *dbuf, unsigned char pix)
{
	unsigned char  framecolumn,framenum,i;
	unsigned int  m,tempint;
	ST_USER_CHINESE_FONT_TAG * rcpt;
	
	rcpt = (ST_USER_CHINESE_FONT_TAG *)sbuf;
	if(!rcpt->ucFrameColumn)
	{
		rcpt->ucFrameColumn = CMS_FONT_VIEW_V;
	}
	if(!rcpt->ucFrameNumber)
	{
		rcpt->ucFrameNumber = 1;
	}
	if(!rcpt->ucFrameRow)
	{
		rcpt->ucFrameRow = CMS_FONT_VIEW_W;
	}
    if(!rcpt->ucFrameTotal)
    {
		rcpt->ucFrameTotal = 1;
	}
	
	//如果是第一Frame,将整个缓存清除
	if(rcpt->ucFrameNumber == 1)
	{
       memset(dbuf,0x00,CMS_FONT_VIEW_V * CMS_FONT_VIEW_W / 8 * pix);
	}

	framecolumn = CMS_FONT_VIEW_V / (rcpt->ucFrameColumn);
	framenum = rcpt->ucFrameNumber-1;			//  1 .. 255;
	m = ((framenum / framecolumn) * rcpt->ucFrameRow / 8) * pix;	// row starting
	
	tempint = (framenum % framecolumn);
	tempint *= rcpt->ucFrameColumn;
	tempint *= (CMS_FONT_VIEW_W / 8) * pix;
	tempint += m;
	m = 0;
	for(i=0;i<rcpt->ucFrameColumn;i++)
	{
		for(framenum = 0;framenum<((rcpt->ucFrameRow/8)*pix);framenum++)
		{
			dbuf[tempint+framenum] = ~(rcpt->ucBitMap[m]);		// exclusive OR
			m++;
		}
		tempint += (CMS_FONT_VIEW_W / 8) * pix;
	}
	return m;
}
/*********************************************************************************************************
** Phototype:	   INT16U Read_FrameFromFont(INT8U*sbuf,INT8U*dbuf)
** Descriptions:	    Read frame fromt font
** input parameters:   INT8U*sbuf : one font lib data address
**				      INT8U*dbuf : ST_SET_CHINESE_FONT_TAG struct address
** Returned value:      int16u the len of the ucBitMap
*********************************************************************************************************/
unsigned int Read_FrameFromWordWin(unsigned char  *sbuf,unsigned char *dbuf, unsigned char pix)
{
	unsigned char framecolumn,framenum,i;
	unsigned int m,tempint;
	ST_USER_CHINESE_FONT_TAG* rcpt;
		
	rcpt = (ST_USER_CHINESE_FONT_TAG*)dbuf;

	if(!rcpt->ucFrameColumn)
	{
		rcpt->ucFrameColumn = CMS_FONT_VIEW_W;
	}
    if(!rcpt->ucFrameNumber)
    {
		rcpt->ucFrameNumber = 1;
	}
	if(!rcpt->ucFrameRow)
	{
		rcpt->ucFrameRow = CMS_FONT_VIEW_V;
	}
    if(!rcpt->ucFrameTotal)
    {
		rcpt->ucFrameTotal = 1;
	}
	
	framecolumn = CMS_FONT_VIEW_V / (rcpt->ucFrameColumn);
	framenum = rcpt->ucFrameNumber-1;			//  1 .. 255;
	m = ((framenum / framecolumn) * rcpt->ucFrameRow / 8) * pix;// row starting
	
	tempint = (framenum % framecolumn);
	tempint *= rcpt->ucFrameColumn;
	tempint *= (CMS_FONT_VIEW_W / 8) * pix;
	tempint += m;
	m = 0;
	for(i=0;i<rcpt->ucFrameColumn;i++)
	{
		for(framenum = 0;framenum<(rcpt->ucFrameRow/8)*pix;framenum++)
		{
			//rcpt->ucBitMap[m] = ~(sbuf[tempint+framenum]);		// exclusive OR
			rcpt->ucBitMap[m] = (sbuf[tempint+framenum]);	
			m++;
		}
		tempint += (CMS_FONT_VIEW_W / 8) * pix;
	}
	return m;
}


/*att:0增加 1查找 -1删除
  ret:>=0 OK -1 err
*/
int mem_bigfound(unsigned char *sbuf,unsigned char *dbuf,unsigned int len)
{
	int ret=-1;
    int i;
	unsigned short *usbuf=(unsigned short *)sbuf;
	unsigned short *udbuf=(unsigned short *)dbuf;

	for(i=0;i<len/2;i++)
	{
        if(usbuf[i]==udbuf[0])
        {
           ret = i*2;
	//	   printf("mem_bigfound:ret i=%d,dbuf=%x%x\n",i,dbuf[0],dbuf[1]);
		   break;	
		}
	}

	return ret;
}

int mem_bigdel(unsigned char *sbuf,unsigned char *dbuf,unsigned int len)
{
	int ret=-1;
    
	ret = mem_bigfound(sbuf,dbuf,len);
	if(ret!=-1)
	{
		sbuf[ret]=0;
		sbuf[ret+1]=0;
	}

	return ret;
}


int mem_bigadd(unsigned char *sbuf,unsigned char *dbuf,unsigned int len)
{
	int ret;
    unsigned char bank[3];
	
	ret = mem_bigfound(sbuf,dbuf,len);
    if(ret!=-1)
    {
        sbuf[ret]=dbuf[0];
		sbuf[ret+1]=dbuf[1];
	}
	else
	{
        memset(bank,0,sizeof(bank));
		ret = mem_bigfound(sbuf, bank, len);
		if(ret!=-1)
		{
			sbuf[ret]=dbuf[0];
			sbuf[ret+1]=dbuf[1];
		}
		else
		{
			sbuf[0]=dbuf[0];
			sbuf[1]=dbuf[1];
			ret = 0;
		}
	}

	return ret;
}

int GetSelfFontLib(ST_USER_CHINESE_FONT_TAG *rcp)
{
	unsigned char *uubuf;
	int fp,i;
	int len;
		
	fp = open("/tc/fontlib",O_RDWR|O_CREAT,777);
	if(fp < 0)
	{
	   printf("Err: open font file index.\n");
	}
    uubuf = malloc(500);
	memset(uubuf,0x00,500);
	lseek(fp,0,SEEK_SET);
    len = read(fp,uubuf,256);
	if(len != 256)
	{
		lseek(fp,0,SEEK_SET);
        i= write(fp,uubuf,256);
		i= truncate("/tc/fontlib",256);//设置文件长度
	}

    i=mem_bigfound(uubuf,rcp->ucBig5Code,len);
	if(i== -1)
	{
		printf("GetSelfFontLib:not found font:%x%x\n",rcp->ucBig5Code[0],rcp->ucBig5Code[1]);
	    memset(rcp,0x00,sizeof(ST_USER_CHINESE_FONT_TAG));
	}
	else
	{
	    int seek;
		seek = (i/2) * (CMS_FONT_VIEW_V * CMS_FONT_VIEW_W / 8) + 256;
		lseek(fp,seek,SEEK_SET);
		len=read(fp,uubuf,CMS_FONT_VIEW_V * CMS_FONT_VIEW_W / 8);
	//	printf("GetSelfFontLib:read data len=%d,seek=%d,p=%i\n",len,seek,i);
		if(len== (CMS_FONT_VIEW_V * CMS_FONT_VIEW_W / 8))
		{
            Read_FrameFromWordWin(uubuf, (unsigned char *)rcp,1);
	//		printf("GetSelfFontLib:font big5=%x%x,col=%d,row=%d,framenum=%d,total=%d\n",rcp->ucBig5Code[0],rcp->ucBig5Code[1],rcp->ucFrameColumn,rcp->ucFrameRow,rcp->ucFrameNumber,rcp->ucFrameTotal);
		}
		else
		{
			memset(rcp,0x00,sizeof(ST_USER_CHINESE_FONT_TAG));
			printf("GetSelfFontLib: file data err\n");
		}
		i=0;
	}
	
	free(uubuf);
	close(fp);

	return i;
}

int DelSelfFontLib(unsigned char *sbuf,ST_CONTEXT_TAG *stContextp)
{
	int ret;
	unsigned char *uubuf;
	int fp;
	int len;

	ST_GET_CHINESE_FONT_TAG *rcp=(ST_GET_CHINESE_FONT_TAG *)sbuf;

	//pthread_mutex_lock(&stContextp->ST_PTHREAD_USERFONT.stMutexFontFile);
	
	fp = open("/tc/fontlib",O_RDWR|O_CREAT,777);
	if(fp < 0)
	{
	   printf("Err: open font file index.\n");
//	   return -1;
	}
	uubuf = malloc(500);
	memset(uubuf,0x00,500);
	lseek(fp,0,SEEK_SET);
	len = read(fp,uubuf,256);
	if(len != 256)
	{
		lseek(fp,0,SEEK_SET);
		ret = write(fp,uubuf,256);
		ret=truncate("/tc/fontindex",256);//设置文件长度
	}
	ret = mem_bigdel(uubuf,rcp->ucBig5Code,len);
    if(ret != -1)
    {
		printf("DelSelfFontLib OK: Big5=%x%x\n",rcp->ucBig5Code[0],rcp->ucBig5Code[1]);
		lseek(fp,0,SEEK_SET);
		ret=write(fp,uubuf,256);
	}
	else
	{
		printf("DelSelfFontLib Err: Big5=%x%x\n",rcp->ucBig5Code[0],rcp->ucBig5Code[1]);
	}
	free(uubuf);
	close(fp);
	
	//pthread_mutex_unlock(&stContextp->ST_PTHREAD_USERFONT.stMutexFontFile);

	
	return ret;
}

int AddSelfFontLib(unsigned char *sbuf,ST_CONTEXT_TAG *stContextp)
{
	unsigned char *uubuf;//[500];
	int fp,i,n;
	int len,seek;

	ST_USER_CHINESE_FONT_TAG *rcp=(ST_USER_CHINESE_FONT_TAG *)sbuf;
	
	fp = open("/tc/fontlib",O_RDWR|O_CREAT,777);
	if(fp < 0)
	{
	   printf("Err: open font file index.\n");
	   return -1;
	}
    uubuf = malloc(500);
	memset(uubuf,0x00,500);
	lseek(fp,0,SEEK_SET);
    len = read(fp,uubuf,256);
	if(len != 256)
	{
		lseek(fp,0,SEEK_SET);
        i=write(fp,uubuf,256);
		i=truncate("/tc/fontlib",256);//设置文件长度
	}

    i=mem_bigadd(uubuf,rcp->ucBig5Code,256);//返回AT表的位置	
	lseek(fp,0,SEEK_SET);
	n=write(fp,uubuf,256);
	printf("AddSelfFontLib:add post=%d\n",i);
	
	seek = (i/2)*(CMS_FONT_VIEW_V * CMS_FONT_VIEW_W / 8) + 256; // 48 X 48 / 8 = 48 X 6 ,(i/2)at表是双字节，计算数据地址时以at表地址要/2
	lseek(fp,seek,SEEK_SET);
	
	i=read(fp,uubuf,(CMS_FONT_VIEW_V * CMS_FONT_VIEW_W / 8));
	Save_FrameToWordWin(sbuf,uubuf,1);
    lseek(fp,seek,SEEK_SET);  
	i=write(fp,uubuf,(CMS_FONT_VIEW_V * CMS_FONT_VIEW_W / 8));

    printf("AddSelfFontLib:Big5=%x%x\n",rcp->ucBig5Code[0],rcp->ucBig5Code[1]);
	
	close(fp);

	free(uubuf);

	return i;
}

/*用户自造图形保存线程
每个图片一个文件，最多保存64个文件
--------------------------------------------------*/
int GetSelfImageLib(ST_COLOR_SINGLE_TEXT_TAG *pImageFrame)
{	
	int fp,ret,len,i;
	unsigned  char *pFileName,*pfile;
    unsigned char *pColorMap;
    unsigned char *pt;
    unsigned char row,col;

    if(access("/tc/downimage",F_OK))
    {
   	   mkdir("/tc/downimage",0x0777);
       printf("create downimage.\n");
    }
	
	pFileName = pImageFrame->ucPatternCode;
    pfile = (unsigned char *)malloc(32);	
	memset(pfile,0x00,32);
	sprintf((char *)pfile,"/tc/downimage/%x%x",pFileName[0],pFileName[1]);
	printf("read Image Big5 :%s\n",pfile);

    if(access((char *)pfile,F_OK))
    {
		printf("Err pthread_userImage: file not found.\n");
		return -1;
	}	
	
	ret=-1;

    fp = open((char *)pfile,O_RDWR|O_CREAT,777);
	lseek(fp,0,SEEK_SET);		
    len = sizeof(ST_COLOR_SINGLE_TEXT_TAG);
	pColorMap = malloc(len);
	len = read(fp,pColorMap,len);
	if(len < 6)//文件数据错误
	{
		printf("Err pthread_userImage: file no data.\n");
	}
    else		
    {
		if((pImageFrame->ucFrameRow == 0) || (pImageFrame->ucFrameColumn == 0))
		{
			pImageFrame->ucFrameColumn = CMS_FONT_VIEW_V;
			pImageFrame->ucFrameRow = CMS_FONT_VIEW_W;
			pImageFrame->ucFrameTotal = 1;
			pImageFrame->ucFrameNumber = 1;
            memcpy(pImageFrame->ucColorMap,pColorMap,sizeof(pImageFrame->ucColorMap));
		}
		else
		{
			row = pImageFrame->ucFrameRow / 4	 * pImageFrame->ucFrameTotal;	
			col = pImageFrame->ucFrameColumn * pImageFrame->ucFrameTotal;
			for(i=0;i < pImageFrame->ucFrameColumn;i++)//垂直方向
			{
				pt = &pColorMap[row * i + pImageFrame->ucFrameRow / 4 * (pImageFrame->ucFrameNumber -1)];
				memcpy(&pImageFrame->ucColorMap[pImageFrame->ucFrameRow / 4 * i],pt,pImageFrame->ucFrameRow / 4);
			}
		}
    }
	close(fp);
	
	free(pfile);
	return len;
}

int AddSelfImageLib(unsigned char *sbuf,ST_CONTEXT_TAG *stContextp)
{	
	int fp,ret,len;
	int i;
	unsigned  char *pFileName,*pfile;
//	struct stat  filestat;
    unsigned char *pColorMap;
    unsigned char *pt;
    unsigned char row,col;
	
	ST_COLOR_SINGLE_TEXT_TAG *pImageFrame=(ST_COLOR_SINGLE_TEXT_TAG *)sbuf;

    pfile = (unsigned char *)malloc(32);
	
    if(access("/tc/downimage",F_OK))
    {
   	   mkdir("/tc/downimage",0x0777);
       printf("create downimage.\n");
    }
		
	pFileName = pImageFrame->ucPatternCode;
	memset(pfile,0x00,32);
	sprintf((char *)pfile,"/tc/downimage/%x%x",pFileName[0],pFileName[1]);
	printf("write Image Big5 :%s\n",pfile);

	fp = open((char *)pfile,O_RDWR|O_CREAT,777);
    
    len = sizeof(ST_COLOR_SINGLE_TEXT_TAG);
	pColorMap = malloc(len);
	if(pImageFrame->ucFrameNumber == 1)
	{
		memset(pColorMap,0x00,len);
	}
	else
	{
	    read(fp,pColorMap,len);
	}

    row = pImageFrame->ucFrameRow / 4    * pImageFrame->ucFrameTotal;	
    col = pImageFrame->ucFrameColumn * pImageFrame->ucFrameTotal;
	for(i=0;i < pImageFrame->ucFrameColumn;i++)//垂直方向
	{
		pt = &pColorMap[row * i + pImageFrame->ucFrameRow / 4 * (pImageFrame->ucFrameNumber -1)];
		memcpy(pt,&pImageFrame->ucColorMap[pImageFrame->ucFrameRow / 4 * i],pImageFrame->ucFrameRow / 4);
	}
	lseek(fp,0,SEEK_SET);
	if((ret = write(fp,pColorMap,len))<0)
	{
        printf("Err pthread_userImage write.\n");
	}
	i=truncate((char *)pfile,len);
	
	close(fp);
	
    free(pColorMap);
    free(pfile);
	return ret;
}

#if 0
int AddImageMsg(ST_SET_MSG_AF32H_TAG *pImageFrame)
{	
	int fp,ret,len,i;
	char *pfile;
	
    pfile = (char *)malloc(32);
	
    if(access("/tc/downmsg",F_OK))
    {
   	   mkdir("/tc/downmsg",0x0777);
       printf("create downmsg.\n");
    }

	memset(pfile,0x00,32);
	sprintf((char *)pfile,"/tc/downmsg/msg%d",pImageFrame->MsgId);
	printf("write Image msg :%s\n",pfile);

//	ret = stat((char *)pfile,&filestat);
//    printf("PackNo:%d\n",pImageFrame->PackNo);

	fp = open((char *)pfile,O_RDWR|O_CREAT,777);
	len = pImageFrame->MsgSizeH << 8 | pImageFrame->MsgSizeL;
    if(pImageFrame->PackNo == 1)
    {
		lseek(fp,0,SEEK_SET);
		i=truncate((char *)pfile,len);
    }
	else
	{
		lseek(fp,0,SEEK_END);
	}
	
	if((ret = write(fp,pImageFrame->MsgBuf,len))<0)
	{
        printf("Err pthread_userImage write.\n");
	}
	//i=truncate((char *)pfile,len);
	
	close(fp);

    free(pfile);
	return ret;
}

int DelImageMsg(unsigned char MsgId)
{	
	int ret;
	char *pfile;
	struct stat  filestat;
	
    pfile = (char *)malloc(32);
    
	memset(pfile,0x00,32);
	sprintf((char *)pfile,"/tc/downmsg/msg%d",MsgId);
	ret = stat((char *)pfile,&filestat);
	if(filestat.st_size)
	{
        remove(pfile);
		printf("Del msg file OK: %s.\n",pfile);
	}
    else
    {
		printf("Del msg file Err: %s.\n",pfile);
	}

    free(pfile);
	return ret;
}

int GetImageMsg(ST_GET_MSG_AF62H_TAG *pST_GET_MSG,ST_SET_MSG_AFE2H_TAG *dbuf,unsigned short maxlen)
{	
	int fp,ret,len;
	int seek;
	unsigned  char *pfile;
	struct stat  filestat;

    pfile = (unsigned char *)malloc(32);	
	
	memset(pfile,0x00,32);
	sprintf((char *)pfile,"/tc/downmsg/msg%d",pST_GET_MSG->MsgId);

    if(access((char *)pfile,F_OK))
    {
		printf("Err Image msg: file not found.%s\n",pfile);
		return -1;
	}	
	printf("read msg Image :%s\n",pfile);
	
	fp = open((char *)pfile,O_RDWR|O_CREAT,777);
	maxlen = ((maxlen < LOAD_PACKETSIZE_MIN)||(maxlen > LOAD_PACKETSIZE_MAX)) ? LOAD_PACKETSIZE_DEF : maxlen;

    printf("total:=%d,no=%d\n",pST_GET_MSG->TotalPacket,pST_GET_MSG->PackNo);//debug
	
	ret = stat((char *)pfile,&filestat);
    if((!pST_GET_MSG->PackNo)&&(!pST_GET_MSG->TotalPacket))
    {
        dbuf->MsgId = pST_GET_MSG->MsgId;
		//dbuf->TotalPacket = (filestat.st_size / sizeof(dbuf->MsgBuf)) + ((filestat.st_size % sizeof(dbuf->MsgBuf)) > 0);
		dbuf->TotalPacket = (filestat.st_size / maxlen) + ((filestat.st_size % maxlen) > 0);
		dbuf->PackNo = 1;
		
		lseek(fp,0,SEEK_SET);		
		//len = read(fp,dbuf->MsgBuf,sizeof(dbuf->MsgBuf));
		len = read(fp,dbuf->MsgBuf,maxlen);
	}
	else
	{
        dbuf->MsgId = pST_GET_MSG->MsgId;
		dbuf->TotalPacket = pST_GET_MSG->TotalPacket;
		dbuf->PackNo = pST_GET_MSG->PackNo;
		
        //seek = (dbuf->PackNo-1) * sizeof(dbuf->MsgBuf);
        seek = (dbuf->PackNo-1) * maxlen;
		if(seek >= filestat.st_size)
			len = 0;
		else
		{
			lseek(fp,seek,SEEK_SET);		
			//len = read(fp,dbuf->MsgBuf,sizeof(dbuf->MsgBuf));
			len = read(fp,dbuf->MsgBuf,maxlen);
		}
	}
	dbuf->MsgSizeH = len >> 8;
	dbuf->MsgSizeL = len;
    		
	close(fp);
	
	free(pfile);
	return 0;
}
#endif

int AddPresetText(ST_PRESET_TEXT_TAG *pFrame)
{	
	int fp,ret,len,i;
	unsigned  char *pfile;
	
    pfile = (unsigned char *)malloc(32);
	
    if(access("/tc/downtext",F_OK))
    {
   	   mkdir("/tc/downtext",0x0777);
       printf("create downtext.\n");
    }

	memset(pfile,0x00,32);
	sprintf((char *)pfile,"/tc/downtext/text%d",pFrame->ucTextID);
	printf("write preset text:%s\n",pfile);

	fp = open((char *)pfile,O_RDWR|O_CREAT,777);
	lseek(fp,0,SEEK_SET);

	len = pFrame->ucTextLength * 2 + 3;
	if((ret = write(fp,(unsigned char *)pFrame,len))<0)
	{
        printf("Err pthread_userImage write.\n");
	}
	i=truncate((char *)pfile,len);
	
	close(fp);

    free(pfile);
	return ret;
}

int DelPresetText(unsigned char textid)
{	
	int ret;
	char *pfile;
	struct stat  filestat;
	
    pfile = (char *)malloc(32);
    
	memset(pfile,0x00,32);
	sprintf((char *)pfile,"/tc/downtext/text%d",textid);
	ret = stat((char *)pfile,&filestat);
	if(filestat.st_size)
	{
        remove(pfile);
		printf("Del text file OK: %s.\n",pfile);
	}
    else
    {
		printf("Del text file Err: %s.\n",pfile);
	}

    free(pfile);
	return ret;
}

int GetPresetText(ST_PRESET_TEXT_TAG *pFrame)
{	
	int fp,ret,len;
	unsigned  char *pfile;
	struct stat  filestat;

    pfile = (unsigned char *)malloc(32);	
	
	memset(pfile,0x00,32);
	sprintf((char *)pfile,"/tc/downtext/text%d",pFrame->ucTextID);

    if(access((char *)pfile,F_OK))
    {
		printf("Err Color: file not found.%s\n",pfile);
		free(pfile);
		return -1;
	}	
//	printf("read text :%s\n",pfile);
	
	fp = open((char *)pfile,O_RDWR|O_CREAT,777);
	ret = stat((char *)pfile,&filestat);
	
	lseek(fp,0,SEEK_SET);		
	len = read(fp,pFrame,sizeof(ST_PRESET_TEXT_TAG));
	
	close(fp);
	
	free(pfile);
	return 0;
}

//------------Inser disptext---------------------------
int AddPresetInsertContext(unsigned char *pbuf,unsigned short size)
{	
	int fp,ret,len,i;
	unsigned  char *pfile;
	
    pfile = (unsigned char *)malloc(32);
	
    if(access("/tc/downInsert",F_OK))
    {
   	   mkdir("/tc/downInsert",0x0777);
       printf("create downInsert.\n");
    }

	memset(pfile,0x00,32);
	sprintf((char *)pfile,"/tc/downInsert/text%d",pbuf[0]);
	printf("write preset insert text:%s\n",pfile);

	fp = open((char *)pfile,O_RDWR|O_CREAT,777);
	lseek(fp,0,SEEK_SET);

	if((ret = write(fp,pbuf,size))<0)
	{
        perror("insert file");
	}
	i=truncate((char *)pfile,size);
	
	close(fp);

    free(pfile);
	return ret;
}

int GetPresetInsertContext(unsigned char *pbuf)
{	
	int fp,ret,len;
	unsigned  char *pfile;
	struct stat  filestat;

    pfile = (unsigned char *)malloc(32);	
	
	memset(pfile,0x00,32);
	sprintf((char *)pfile,"/tc/downInsert/text%d",pbuf[0]);

    if(access((char *)pfile,F_OK))
    {
		printf("Err Insert file: file not found.%s\n",pfile);
		free(pfile);
		return 0;
	}	

    printf("Get insert %s\n",pfile);
	
	fp = open((char *)pfile,O_RDWR|O_CREAT,777);
	ret = stat((char *)pfile,&filestat);
	
	lseek(fp,0,SEEK_SET);		
	len = read(fp,pbuf,filestat.st_size);
	
	close(fp);
	
	free(pfile);
	return filestat.st_size;
}
//-----------------------------------------------------


int AddPresetTextColor(unsigned char *sbuf)
{	
	int fp,ret;
	int seek;
	ST_PRESET_TEXT_COLOR_TAG *pFrame;
	unsigned  char *pfile;
	
	pFrame = (ST_PRESET_TEXT_COLOR_TAG *)malloc(sizeof(ST_PRESET_TEXT_COLOR_TAG));
		
    memset(pFrame,0x00,sizeof(ST_PRESET_TEXT_COLOR_TAG));
    pFrame->ucTextID = sbuf[0];
	pFrame->ucTextCount = sbuf[1];
	memcpy(&pFrame->unColor,&sbuf[2],pFrame->ucTextCount);
	seek = pFrame->ucTextCount + 2;
	memcpy(&pFrame->ucV_Bound,&sbuf[seek],6);
	
    if(access("/tc/downcolor",F_OK))
    {
   	   mkdir("/tc/downcolor",0x0777);
       printf("create downcolor.\n");
    }
	
    pfile = (unsigned char *)malloc(32);
	memset(pfile,0x00,32);
	sprintf((char *)pfile,"/tc/downcolor/color%d",pFrame->ucTextID);
	printf("write preset color:%s\n",pfile);

	fp = open((char *)pfile,O_RDWR|O_CREAT,777);
	if(fp == -1)
	{
        printf("open %s err.\n",pfile);
        return -1;
	}
	lseek(fp,0,SEEK_SET);
	if((ret = write(fp,(unsigned char *)pFrame,sizeof(ST_PRESET_TEXT_COLOR_TAG))) < 0)
	{
        printf("Err color write.ret=%d\n",ret);
	}
	
	close(fp);

    free(pfile);
	free(pFrame);
	return ret;
}

int GetPresetTextColor(ST_PRESET_TEXT_COLOR_TAG *pFrame)
{	
	int fp;	
	int len;
	char *pfile;
    unsigned char ucTextID;

    if(access("/tc/downcolor",F_OK))
    {
		printf("Err Color: file not found. /tc/downcolor\n");
		return -1;
	}	

    pfile = (char *)malloc(32);
	memset(pfile,0x00,32);
	sprintf((char *)pfile,"/tc/downcolor/color%d",pFrame->ucTextID);
	fp = open(pfile,O_RDWR|O_CREAT,777);
	
	lseek(fp,0,SEEK_SET);		
	ucTextID = pFrame->ucTextID;
	len = read(fp,pFrame,sizeof(ST_PRESET_TEXT_COLOR_TAG));
	close(fp);
	
	free(pfile);
    if((len != sizeof(ST_PRESET_TEXT_COLOR_TAG))||(pFrame->ucTextID != ucTextID))
	{
		return -1;
    }
    
	return 0;
}

#if 0
//显示组合方案
int AddZoneGroupDisScheme(unsigned char *sbuf,int size)
{	
	int fp,ret;
	int i;
	unsigned  char *pfile;
	
	ST_PRESET_ZONEGROUPDIS_AF35H_TAG *pFrame;
	

	pFrame = (ST_PRESET_ZONEGROUPDIS_AF35H_TAG *)sbuf;
	pfile = (unsigned char *)malloc(32);

	if(access("/tc/disgroup",F_OK))
	{
	   mkdir("/tc/disgroup",0x0777);
	   printf("create disgroup.\n");
	}

	memset(pfile,0x00,32);
	sprintf((char *)pfile,"/tc/disgroup/scheme%d",pFrame->PatternId);
	printf("write preset disgroup:%s\n",pfile);

	fp = open((char *)pfile,O_RDWR|O_CREAT,777);
	lseek(fp,0,SEEK_SET);

	if((ret = write(fp,sbuf,size))<0)
	{
		printf("Err disgroup write.\n");
	}
	i=truncate((char *)pfile,size);
	
	close(fp);

	free(pfile);
	return ret;
}

//读取区域组合显示方案
int GetZoneGroupDisScheme(unsigned char *dest,ST_GET_ZONEGROUPDIS_AF65H_TAG *pFrame)
{	
	int fp,ret,len;
	unsigned  char *pfile;
	unsigned  char *filebuf;
	struct stat  filestat;
	int i,zone_len;
	unsigned char *p;
	unsigned char ZoneCount;

    pfile = (unsigned char *)malloc(32);	
	
	memset(pfile,0x00,32);
	sprintf((char *)pfile,"/tc/disgroup/scheme%d",pFrame->PatternId);
    if(access((char *)pfile,F_OK))
    {
		printf("Err scheme: file not found.%s\n",pfile);
		free(pfile);
		return -1;
	}	
	printf("read scheme :%s\n",pfile);
	
	fp = open((char *)pfile,O_RDWR|O_CREAT,777);
	ret = stat((char *)pfile,&filestat);
    filebuf = malloc(filestat.st_size);
	
	zone_len = 0;
	lseek(fp,0,SEEK_SET);		
	len = read(fp,filebuf,filestat.st_size);

    if(!pFrame->ZoneNo) 
    {
        memcpy(dest,filebuf,len);
	}
	else
	{
		ZoneCount = filebuf[1];
		p = &filebuf[2];
		for(i=0;i<ZoneCount;i++)
		{
            zone_len = p[4]*2 + 5;
			if(i+1 == pFrame->ZoneNo)
			{
				dest[0] = pFrame->PatternId;
				dest[1] = 1;
                memcpy(&dest[2],p,zone_len);
				break;
			}
			p = p + zone_len;
		}
	}
	
	close(fp);
	free(filebuf);
	free(pfile);
	
	return (zone_len + 2);
}

int AddZoneGroupCutScheme(ST_SET_VIEW_AF34H_TAG *pFrame)
{	
	int fp,ret;
	int i,size;
	unsigned  char *pfile;
	
	pfile = (unsigned char *)malloc(32);

	if(access("/tc/disgroup",F_OK))
	{
	   mkdir("/tc/disgroup",0x0777);
	   printf("create disgroup.\n");
	}

	memset(pfile,0x00,32);
	sprintf((char *)pfile,"/tc/disgroup/zonecut%d",pFrame->PatternId);
	printf("write preset zonecut:%s\n",pfile);

	fp = open((char *)pfile,O_RDWR|O_CREAT,777);
	lseek(fp,0,SEEK_SET);

    size = pFrame->ZoneCount * sizeof(ST_VIEW_CUT_TAG) + 4;
	if((ret = write(fp,(unsigned char *)pFrame,size))<0)
	{
		printf("Err zonecut write.\n");
	}
	i=truncate((char *)pfile,size);
	
	close(fp);

	free(pfile);
	return ret;
}

int GetZoneGroupCutScheme(unsigned char *dest,ST_GET_VIEW_AF64H_TAG *psrc)
{	
	int fp,len;
	unsigned  char *pfile;
	int zone_len;
	ST_SET_VIEW_AF34H_TAG *pFrame;

    pfile = (unsigned char *)malloc(32);	
	
	memset(pfile,0x00,32);
	sprintf((char *)pfile,"/tc/disgroup/zonecut%d",psrc->PatternId);
    if(access((char *)pfile,F_OK))
    {
		printf("Err zonecut: file not found.%s\n",pfile);
		return -1;
	}	
	printf("read zonecut :%s\n",pfile);
	
	fp = open((char *)pfile,O_RDWR|O_CREAT,777);
    pFrame = (ST_SET_VIEW_AF34H_TAG *)malloc(sizeof(ST_SET_VIEW_AF34H_TAG));
	
	zone_len = 0;
	lseek(fp,0,SEEK_SET);		
	len = read(fp,(unsigned char *)pFrame,sizeof(ST_SET_VIEW_AF34H_TAG));

    if(!psrc->ZoneNo) 
    {
        memcpy(dest,(unsigned char *)pFrame,len);
	}
	else
	{
		memcpy(dest,(unsigned char *)pFrame,3);
		if(pFrame->ZoneCount < psrc->ZoneNo)
		{
			dest[3] = 0;
		}
        else
        {
			dest[3] = 1;
            memcpy(&dest[4],(unsigned char *)&pFrame->zoneCut[psrc->ZoneNo - 1],sizeof(ST_VIEW_CUT_TAG));
		}		
	}
	
	close(fp);
	free(pfile);
	
	return 0;
}
#endif
static unsigned char checkcrc(unsigned char *bufp,unsigned int len)
{
	unsigned char ret;
    unsigned int i;

	ret = 0;
	for(i=0;i<len;i++)
	{
        ret ^= bufp[i];
	}
    return ret;
}

#if 1
static int checkProtocol(ST_PORTRCVTXD_TAG *stPortp,ST_TXRX_TAG *pStRcv)
{
	unsigned int      i,len,crc;	
	unsigned int      info_len;	 
	unsigned char    *buffer_temp;
	unsigned char     bHead;
	ST_FRAME_MSG_TAG *msgFram_p;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	pStMsgFrame =(ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
	
	if(pStMsgFrame == NULL)
		return -1; //Err
		
    if(pStRcv->len < 8)
    {
		pStRcv->len = 0;
		return -1;
    }
	
	bHead = 0;
    memset(pStMsgFrame,0x00,sizeof(ST_FRAME_MSG_TAG));
	for(i=0;i<(pStRcv->len-1);i++)
    {   //查找数据头
        if(((pStRcv->buf[i+1]==0xBB)||(pStRcv->buf[i+1]==0xDD)||(pStRcv->buf[i+1]==0xEE))&&(pStRcv->buf[i]==0xAA))
        {
			if(i)
			{
	            memcpy(pStRcv->buf,&pStRcv->buf[i],pStRcv->len-i);				
			}
			pStRcv->len -= i;
			bHead =1;
			break;//debug
		}
	}
	if(!bHead)
	{
		pStRcv->len = 0;
        return -1;
	}
    
	msgFram_p = (ST_FRAME_MSG_TAG *)pStRcv->buf;
	len = msgFram_p->len[0] << 8;
	len += msgFram_p->len[1]; 
	if(len > pStRcv->len)//len本消息的长度 大于实际接收到的字节数，表示消息还没收完
	{
		//printf("Rcv Msg Not enough.\n");
		pStRcv->len = 0;
		return -1;
	}
	
	if(len > sizeof(ST_FRAME_MSG_TAG))
    {	
		pStRcv->len -= len; 
		printf("Rcv Data Len To More, curr msg len = %d,parcel len= %d\n",len,pStRcv->len);
        return -2;
    }

    memcpy(pStMsgFrame,msgFram_p,len);

	if(len < pStRcv->len)
	{
		memcpy(pStRcv->buf,&pStRcv->buf[len],pStRcv->len - len);
	}
	pStRcv->len -= len; 
	
	buffer_temp = (unsigned char *)pStMsgFrame;
	crc = checkcrc(buffer_temp,len-1);

	if(crc != buffer_temp[len - 1])
	{
        printf("Rcv Invaild:crc err\n");
		return -2;
	}

//    printf("Rcv %s Seq=%d,Cmd:%x %x",portname,pStMsgFrame->seq,pStMsgFrame->info[0],pStMsgFrame->info[1]);
	if(len<12)
	{
		//处理ACK及NAK		
		//printf("Rcv %s ACK Seq=0X0%x\n",portname,pStMsgFrame->seq);
		return 1;
	}
	

	if((buffer_temp[len-2]!=0xCC)||(buffer_temp[len-3]!=0xAA))
	{		
		return -2;
	}		

//--------------------过滤重叠码-------------------
    if(stPortp != &stgContextp->PortCms)
    {
		info_len = len - 10;	
		for(i=1;i<info_len;i++)
		{
			if((pStMsgFrame->info[i]==0xAA)&&(pStMsgFrame->info[i-1]==0xAA))
			{
			   memcpy(&pStMsgFrame->info[i-1],&pStMsgFrame->info[i],info_len-i+3);
			   info_len--;
			   len--;
			}
		}
    }
	pStMsgFrame->len[0] = len>>8;
	pStMsgFrame->len[1] = len;


	return 0;
}
#else 
static int checkProtocol(ST_PORTRCVTXD_TAG *stPortp,ST_TXRX_TAG *pStRcv)
{
	unsigned int      n,i,len,crc;	
	unsigned int      info_len,templen;	 
	ST_FRAME_MSG_TAG *pStMsgFrame;

    int ret=0;
    unsigned char temp[2];
	unsigned char *pt;

    ret = 0;
    pt = stPortp->stRcvAnalytice.buf;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)pStRcv->buf;
    if(!pStRcv->len)//查数据头
    {
        for(i=0;i<stPortp->stRcvAnalytice.len-1;i++)
        {
			temp[0] = pt[i];
			temp[1] = pt[i+1];
			if(  ((temp[1]==0xBB)||(temp[1]==0xDD)||(temp[1]==0xEE))
			   &&(temp[0]==0xAA)
			  )
			{
				 templen = stPortp->stRcvAnalytice.len;
				 stPortp->stRcvAnalytice.len -= i;
				 for(n=i;n<templen;n++)
				 {
                     pStRcv->buf[pStRcv->len] = pt[n];
				     pStRcv->len++;	 
					 
					 if(stPortp->stRcvAnalytice.len)
					 {
					 	stPortp->stRcvAnalytice.len--;
					 }
                     if(pStRcv->len < 8)
					 	continue;
					 
					 len = pStMsgFrame->len[0];					 
					 len <<= 8;
					 len |= pStMsgFrame->len[1];

                     if(pStRcv->len < len)
						 continue;

                     memcpy(stPortp->stRcvAnalytice.buf,&pt[n+1],stPortp->stRcvAnalytice.len); 
					 
                     crc = checkcrc(pStRcv->buf,len-1);
                     if(crc != pStRcv->buf[len-1])
                     {
                         return  -2; 
                     }
                     if((pStRcv->buf[1] == 0xBB)&&(len >= 10)) //INFO
                     {
                         if((pStRcv->buf[len-2] != 0xCC)||(pStRcv->buf[len-3] != 0xAA))
                         {		
                             return -2;
                         }
                         //--------------------过滤重叠码-------------------
                         if(stPortp != &stgContextp->PortCms)
                         {
                             info_len = len - 10;	
                             for(i=1;i<info_len;i++)
                             {
                                 if((pStMsgFrame->info[i]==0xAA)&&(pStMsgFrame->info[i-1]==0xAA))
                                 {
                                     memcpy(&pStMsgFrame->info[i-1],&pStMsgFrame->info[i],info_len-i+3);
                                     info_len--;
                                     len--;
                                 }
                             }
                             pStMsgFrame->len[0] = len>>8;
                             pStMsgFrame->len[1] = len;
                         }
                         //------------------------------------------------------
                         return 0;
                     }
                     else if((pStRcv->buf[1] == 0xDD)&&(len == 8))//ACK
                     {
                         return 1;
                     }
                     else if((pStRcv->buf[1] == 0xEE)&&(len == 9))//NAK
                     {
                         return 1;
                     }
				 }
				 break;
			}
		}
	}
	else if((pStRcv->len + stPortp->stRcvAnalytice.len) < sizeof(pStRcv->buf))
    {
        templen = stPortp->stRcvAnalytice.len;
        for(n=0;n<templen;n++)
        {
            pStRcv->buf[pStRcv->len] = pt[n];
            pStRcv->len++;	 
            
            if(stPortp->stRcvAnalytice.len)
            {
                stPortp->stRcvAnalytice.len--;
            }
            if(pStRcv->len < 8)
                continue;
            
            len = pStMsgFrame->len[0];					 
            len <<= 8;
            len |= pStMsgFrame->len[1];
            
            if(pStRcv->len < len)
                continue;
            
            memcpy(stPortp->stRcvAnalytice.buf,&pt[n+1],stPortp->stRcvAnalytice.len); 
            
            crc = checkcrc(pStRcv->buf,len-1);
            if(crc != pStRcv->buf[len-1])
            {
                return  -2; 
            }
            if((pStRcv->buf[1] == 0xBB)&&(len >= 10)) //INFO
            {
                if((pStRcv->buf[len-2] != 0xCC)||(pStRcv->buf[len-3] != 0xAA))
                {		
                    return -2;
                }
                //--------------------过滤重叠码-------------------
                if(stPortp != &stgContextp->PortCms)
                {
                    info_len = len - 10;	
                    for(i=1;i<info_len;i++)
                    {
                        if((pStMsgFrame->info[i]==0xAA)&&(pStMsgFrame->info[i-1]==0xAA))
                        {
                            memcpy(&pStMsgFrame->info[i-1],&pStMsgFrame->info[i],info_len-i+3);
                            info_len--;
                            len--;
                        }
                    }
                    pStMsgFrame->len[0] = len>>8;
                    pStMsgFrame->len[1] = len;
                }
                //------------------------------------------------------
                return 0;
            }
            else if((pStRcv->buf[1] == 0xDD)&&(len == 8))//ACK
            {
                return 1;
            }
            else if((pStRcv->buf[1] == 0xEE)&&(len == 9))//NAK
            {
                return 1;
            }
        }
     }
	else //溢出
	{
		pStRcv->len = 0;
	}
	
	stPortp->stRcvAnalytice.len = 0;
	pStRcv->len= 0;

    return -1;
}
#endif





//回应ACK
static int GetSendAckPacket(unsigned char *dest, unsigned char seq,unsigned char *address)
{
	dest[0] = 0xAA;
	dest[1] = INFO_ACK ;
	dest[2] = seq;
	dest[3] = address[0];
	dest[4] = address[1];
	dest[5] = 0;
	dest[6] = 8;
	dest[7] = checkcrc(dest,7);

    return 8;
}

//回应NAK, err =1,crc错误 2码框错误 4位址错误 8长度错误
static int GetSendNakPacket(unsigned char *dest, unsigned char seq,unsigned char err,unsigned char *address)
{
	dest[0] = 0xAA;
	dest[1] = INFO_NAK;
	dest[2] = seq;
	dest[3] = address[0];
	dest[4] = address[1];
	dest[5] = 0;
	dest[6] = 9;
	dest[7] = err;	
	dest[8] = checkcrc(dest,8);

    return 9;
}

//发送信息码
int GetSendInfoPacket(unsigned char *dest,unsigned char seq,unsigned char *address,unsigned char *infop,unsigned int len,ST_PORTRCVTXD_TAG *stPortp)
{
	unsigned char crc;
    unsigned int  i,n,lentemp;
	
    dest[0] = 0xAA;
	dest[1] = INFO_STX;
	dest[2] = seq;
    dest[3] = address[0];
	dest[4] = address[1];

    n=7;
	lentemp = len;
    if(stPortp != &stgContextp->PortCms)//XZF
    {
	    for(i=0;i<lentemp;i++)
	    {
	       dest[n++] = infop[i];
		   if(infop[i]==0xAA)
		   {
			   dest[n++]=infop[i];	
			   len++;
		   }
		}
    }
	else
	{
		memcpy(&dest[7],infop,len);
	}
	
	dest[5] = (len+10)>>8;
	dest[6] = len+10;
	dest[len + 7]=0xAA;
	dest[len + 8]=0xCC;

	crc = checkcrc(dest,len+9);
	dest[len + 9]=crc;
    len = len + 10;
	
    return len;
}


/************************************************
设置操作系统时间
参数:*dt数据格式为"2006-4-20 20:30:30"
调用方法:
char *pt="2006-4-20 20:30:30";
SetSystemTime(pt);
**************************************************/
int SetSystemTime(char *dt)
{
	struct rtc_time rtm;
	struct tm _tm;
	struct timeval tv;
	time_t timep;
	sscanf(dt, "%d-%d-%d %d:%d:%d", &rtm.tm_year,
	&rtm.tm_mon, &rtm.tm_mday,&rtm.tm_hour,
	&rtm.tm_min, &rtm.tm_sec);
	_tm.tm_sec = rtm.tm_sec;
	_tm.tm_min = rtm.tm_min;
	_tm.tm_hour = rtm.tm_hour;
	_tm.tm_mday = rtm.tm_mday;
	_tm.tm_mon = rtm.tm_mon - 1;
	_tm.tm_year = rtm.tm_year - 1900;

	timep = mktime(&_tm);
	tv.tv_sec = timep;
	tv.tv_usec = 0;
	if(settimeofday (&tv, (struct timezone *) 0) < 0)
	{
		printf("Set system datatime error!\n");
		return -1;
	}
	return 0;
} 

void SetDateTime(ST_DATETIME_TAG *pST_DATETIME)	
{
	unsigned char *pTimebuf;//={"2006-4-20 20:30:30"};

	unsigned int year;
	
    year =pST_DATETIME->Year + 1911;//民国年
    
	pTimebuf = (unsigned char *)malloc(64);
	memset(pTimebuf,0x00,64);
	sprintf((char *)pTimebuf,"%d-%d-%d %d:%d:%d",year,pST_DATETIME->Month,pST_DATETIME->Day,
		    pST_DATETIME->Hour,pST_DATETIME->Min,pST_DATETIME->Sec);

    printf("%s\n",pTimebuf);

	SetSystemTime((char *)pTimebuf);
	system("hwclock --systohc\n");
	free(pTimebuf);
}
void GetDateTime(ST_DATETIME_TAG *pST_DATETIME)
{
	time_t now;
	struct tm *timenow;
   
	time(&now);
	timenow = localtime(&now);
    pST_DATETIME->Day = timenow->tm_mday;
	pST_DATETIME->Hour= timenow->tm_hour;
	pST_DATETIME->Min = timenow->tm_min;
	pST_DATETIME->Sec = timenow->tm_sec;
	pST_DATETIME->Month=timenow->tm_mon + 1; //从
	pST_DATETIME->Week =timenow->tm_wday;
	pST_DATETIME->Year =timenow->tm_year - 11;//linux 时间年份从1900算起, 1911民国年
/*
	printf("Get:%d-%d-%d %d:%d:%d.\n",pST_DATETIME->Year,pST_DATETIME->Month,pST_DATETIME->Day,
		    pST_DATETIME->Hour,pST_DATETIME->Min,pST_DATETIME->Sec);*/
}

//计算模组所对应的字窗
unsigned int FromModuleToTextWin(unsigned int ModuleId)
{
	unsigned int row;   //行
    unsigned int column;//列

    if(ModuleId)
		ModuleId--;

	row = ModuleId / 45;
	column = (ModuleId / 3) % 5;

	return 5 * row + column;
}

void GetDisplayMap(unsigned short *dest,unsigned char errmodule_number,unsigned char *errmodule_idbuff)
{
    unsigned int i,n;

	for(i=0;i<errmodule_number;i++)
	{
		n = FromModuleToTextWin(errmodule_idbuff[i]);
		if(n<15)
		{
			dest[n] = 0xD691;
		}
	}
}

int AF10H_SET_CIRCULATION_DISPLAY_FUN(void *pbuf_context,void *pbuf_port)       //循环显示参数设置
{
	ST_CIRCULATION_DISPLAY_TAG *pST_CIRCULATION_DISPLAY;
	
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
    int ret=0;
	
	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	pST_CIRCULATION_DISPLAY = (ST_CIRCULATION_DISPLAY_TAG *)&pStMsgFrame->info[2];
	if( (pST_CIRCULATION_DISPLAY->ucDisplayTime<1)
	||(pST_CIRCULATION_DISPLAY->ucDisplayTime>60)
	)
	{
		stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 1;
		ret = 1;
	}
	else if(pST_CIRCULATION_DISPLAY->ucTextCount>32)
	{
		stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 2;
		ret = 1;
	}
	/*
	else
	{
		int i;
		for(i=0;i<pST_CIRCULATION_DISPLAY->ucTextCount;i++)
		{
		    if((pST_CIRCULATION_DISPLAY->ucTextID[i]<1)||(pST_CIRCULATION_DISPLAY->ucTextID[i]>48))
		    {
				stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 2+i;
		        ret= 1;						
				break;
			}						
		}
	}
	*/
	if(ret == 1)//Err
	{
		stPortp->stSendInfo.ST_RET_SETERR.ucErrCode = 0x04;
		stPortp->scmd = _0F81H_RET_SETERR;
		stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
	}
	else		
	{
		int len;
		memset((unsigned char *)&stContextp->stSysConfig.ST_CIRCULATION_DISPLAY,0x00,sizeof(ST_CIRCULATION_DISPLAY_TAG));
		len = pST_CIRCULATION_DISPLAY->ucTextCount + 2;
		memcpy((unsigned char *)&stContextp->stSysConfig.ST_CIRCULATION_DISPLAY,(unsigned char *)pST_CIRCULATION_DISPLAY,len);
		
		stContextp->stSysConfig.ucCurrShow_MsgId = 0;//清除固定显示 20140709
		stContextp->showmode = showmode_preset;
		sem_post(&stContextp->sem_UpdateConfig);	

		stPortp->scmd = _0F80H_RET_SETOK;	
		stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
	}			 
	
	return 0;
}
		   
int AF40H_GET_CIRCULATION_DISPLAY_FUN(void *pbuf_context,void *pbuf_port)       //循环显示参数查询   
{
	ST_CIRCULATION_DISPLAY_TAG *pST_CIRCULATION_DISPLAY;

	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	pST_CIRCULATION_DISPLAY =(ST_CIRCULATION_DISPLAY_TAG *)&stContextp->stSysConfig.ST_CIRCULATION_DISPLAY;
	memcpy((unsigned char *)&stPortp->stSendInfo.ST_CIRCULATION_DISPLAY,(unsigned char *)pST_CIRCULATION_DISPLAY,sizeof(ST_CIRCULATION_DISPLAY_TAG));

	stPortp->scmd = AFC0H_RET_CIRCULATION_DISPLAY;
	stPortp->infolen = stPortp->stSendInfo.ST_CIRCULATION_DISPLAY.ucTextCount + 2; //sizeof(ST_CIRCULATION_DISPLAY_TAG);
	
	return 0;
}
		   
int AF11H_SET_PRESET_TEXT_FUN(void *pbuf_context,void *pbuf_port) //预设全文管理之设定
{
	ST_PRESET_TEXT_TAG *pST_PRESET_TEXT;
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	int ret=0;
	int checkformat;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	pST_PRESET_TEXT = (ST_PRESET_TEXT_TAG *)&pStMsgFrame->info[2];
	if((pST_PRESET_TEXT->ucTextID<1)||(pST_PRESET_TEXT->ucTextID>MAX_PRESET_FRAME_COUNT))
	{
	   stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 1;
	   ret = 1;
	}
	else if(pST_PRESET_TEXT->ucShow > 1)
	{
	   stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 2;
	   ret = 1;
	}
	else if(pST_PRESET_TEXT->ucTextLength > 32)
	{
	   stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 3;
	   ret = 1;
	}
    
	checkformat = 0;//Check_AF11(&pST_PRESET_TEXT->ucWordCode[0][0],pST_PRESET_TEXT->ucTextLength);

	if((ret == 1)||(checkformat))//Err
	{
	   stPortp->stSendInfo.ST_RET_SETERR.ucErrCode = 0x04;
	   stPortp->scmd = _0F81H_RET_SETERR;	   
	   stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
	}
	else
	{
	   //DelImageMsg(pST_PRESET_TEXT->ucTextID);
       AddPresetText(pST_PRESET_TEXT);
	   if(pST_PRESET_TEXT->ucShow)
	   {
		   memset(stContextp->zonedisId,0x00,sizeof(stContextp->zonedisId));
		   memset(stContextp->zonetime,0x00,sizeof(stContextp->zonetime));
		   stContextp->stSysConfig.ViewMode = VIEW_NORMAL;
		   stContextp->stSysConfig.ucCurrShow_MsgId 	= pST_PRESET_TEXT->ucTextID;
		   stContextp->showmode = showmode_preset;
		   
		   stContextp->stSysConfig.ST_CIRCULATION_DISPLAY.ucTextCount = 0;
           memset(&stContextp->ST_MARQUEE,0x00,sizeof(ST_MARQUEE_TAG));
		   stContextp->disUpdateTimeUnit = DISPLAY_UPDATETIME_DEFAULT;
		   sem_post(&stContextp->Sem_RcvToUpdataCms);//刷新显示 
#if (PARK_CMS_ENB)
		   stContextp->showpark = 0;
#endif
		   sem_post(&stContextp->sem_UpdateConfig);    
	   }	   
	   
	   stPortp->scmd = _0F80H_RET_SETOK;
	   stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
	}		
	
	return 0;
}
		   
int AF41H_GET_PRESET_TEXT_FUN(void *pbuf_context,void *pbuf_port)               //预设全文管理之查询
{
    unsigned char ucTextID;
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	int ret=0;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
	
	ucTextID = pStMsgFrame->info[2];
	if((ucTextID<1)||(ucTextID > MAX_PRESET_FRAME_COUNT))
	{
	   stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 1;
	   ret = 1;
	}
	
	if(ret == 1)//Err
	{
	   stPortp->stSendInfo.ST_RET_SETERR.ucErrCode = 0x04;
	   
	   stPortp->scmd = _0F81H_RET_SETERR;	   
	   stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
	}
	else
	{
	   ST_PRESET_TEXT_TAG *pST_PRESET_TEXT;
	   
	   pST_PRESET_TEXT = (ST_PRESET_TEXT_TAG *)stPortp->stSendInfo.ucbuf;
	   pST_PRESET_TEXT ->ucTextID = ucTextID;
	   if(!GetPresetText(pST_PRESET_TEXT))
	   {
		   stPortp->infolen = pST_PRESET_TEXT->ucTextLength * 2 + 2;
		   memcpy(&stPortp->stSendInfo.ucbuf[1],&stPortp->stSendInfo.ucbuf[2],stPortp->infolen-1);
	   }
       else
       {
		   stPortp->stSendInfo.ucbuf[0] = 0;
		   stPortp->stSendInfo.ucbuf[1] = 0;
		   stPortp->infolen = 2;
	   }
	 
	   stPortp->scmd = AFC1H_RET_PRESET_TEXT;	   
	}

	return 0;
}

int AF12H_SET_PRESET_TEXT_COLOR_FUN(void *pbuf_context,void *pbuf_port) //预设全文彩色参数管理之设定
{
	ST_PRESET_TEXT_COLOR_TAG *pST_PRESET_TEXT_COLOR;
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	int ret=0;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
	pST_PRESET_TEXT_COLOR = (ST_PRESET_TEXT_COLOR_TAG *)&pStMsgFrame->info[2];

	if((pST_PRESET_TEXT_COLOR->ucTextID < 1)||(pST_PRESET_TEXT_COLOR->ucTextID > MAX_PRESET_FRAME_COUNT))
	{
		printf("rcv err:%2x;TextId=%2x.\n",AF12H_SET_PRESET_TEXT_COLOR,pST_PRESET_TEXT_COLOR->ucTextID);
	    stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 1;
	    ret = 1;
	}
	else if(pST_PRESET_TEXT_COLOR->ucTextCount > MAX_FRAME_TEXT_COUNT)
	{
		printf("rcv err:%x;ucTextCount=%x.\n",AF12H_SET_PRESET_TEXT_COLOR,pST_PRESET_TEXT_COLOR->ucTextCount);
	    stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 2;
	    ret = 1;
	}

	if(ret == 1)//Err
	{
	    stPortp->stSendInfo.ST_RET_SETERR.ucErrCode = 0x04;		
		stPortp->scmd = _0F81H_RET_SETERR;		
		stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
	}
	else
	{
        AddPresetTextColor(&pStMsgFrame->info[2]);

	    stPortp->scmd = _0F80H_RET_SETOK;
	    stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
		
		stContextp->disUpdateTimeUnit = DISPLAY_UPDATETIME_DEFAULT;
		sem_post(&stContextp->Sem_RcvToUpdataCms);//刷新显示 
	}	
	
	return 0;
}

int AF42H_GET_PRESET_TEXT_COLOR_FUN(void *pbuf_context,void *pbuf_port) //预设全文彩色参数管理之查询
{
    unsigned char ucTextID;

	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	int ret=0;

	ST_PRESET_TEXT_COLOR_TAG *pST_PRESET_TEXT_COLOR;	
	
	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	ucTextID = pStMsgFrame->info[2];
	if((ucTextID < 1)||(ucTextID > MAX_PRESET_FRAME_COUNT))
	{
	   stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 1;
	   ret = 1;
	}
	
	if(ret == 1)//Err
	{	
	   stPortp->stSendInfo.ST_RET_SETERR.ucErrCode = 0x04;
	   stPortp->scmd = _0F81H_RET_SETERR;
	   stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
	}
	else
	{  
   	   unsigned char len;
   	   unsigned char *bufp = &stPortp->stSendInfo.ucbuf[0];
   	   pST_PRESET_TEXT_COLOR = (ST_PRESET_TEXT_COLOR_TAG *)stPortp->stSendInfo.ucbuf;
	   pST_PRESET_TEXT_COLOR->ucTextID = ucTextID;
   	   if(GetPresetTextColor(pST_PRESET_TEXT_COLOR)==0)
   	   {
	   	 //  printf("ssss %d,%d\n",pST_PRESET_TEXT_COLOR->ucTextID,pST_PRESET_TEXT_COLOR->ucTextCount);
		
           len = pST_PRESET_TEXT_COLOR->ucTextCount + 2;
           memcpy(bufp,pST_PRESET_TEXT_COLOR,len);
           memcpy(&bufp[len],&pST_PRESET_TEXT_COLOR->ucV_Bound,6);
           
           stPortp->infolen = len + 6;
   	   }
	   else
	   {
           stPortp->stSendInfo.ucbuf[0]=0;
           stPortp->stSendInfo.ucbuf[1]=0;
		   stPortp->infolen = 2;
	   }
	   stPortp->scmd = AFC2H_RET_PRESET_TEXT_COLOR;
	}
	
	return 0;
}

int AF13H_SET_CURRENT_TEXT_FUN(void *pbuf_context,void *pbuf_port)              //当前全文显示管理之设定
{
    unsigned char ucTextID;
	
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	int ret=0;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	ret = 0;
	ucTextID = pStMsgFrame->info[2];
	if(/*(ucTextID < 1)||*/(ucTextID > MAX_PRESET_FRAME_COUNT))
	{
	   printf("Set Curr TextID = %d\n",ucTextID);	
	   stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 1;
	   ret = 1;
	}

	if(ret == 1)//Err
	{
	   stPortp->stSendInfo.ST_RET_SETERR.ucErrCode = 0x04;
	   stPortp->scmd = _0F81H_RET_SETERR;
	   stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
	}
	else
	{
	   stContextp->stSysConfig.ViewMode = VIEW_NORMAL;
	   stContextp->stSysConfig.ucCurrShow_MsgId = ucTextID;
	   stContextp->showmode = showmode_preset;
	   memset(&stContextp->ST_MARQUEE,0x00,sizeof(ST_MARQUEE_TAG));
	   memset(stContextp->zonedisId,0x00,sizeof(stContextp->zonedisId));
	   memset(stContextp->zonetime,0x00,sizeof(stContextp->zonetime));
	   stContextp->disUpdateTimeUnit = DISPLAY_UPDATETIME_DEFAULT;
	   sem_post(&stContextp->Sem_RcvToUpdataCms);//刷新显示 
	   sem_post(&stContextp->sem_UpdateConfig);    
#if (PARK_CMS_ENB)
	   stContextp->showpark = 0;
#endif
	   stPortp->scmd = _0F80H_RET_SETOK;
	   stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
	}			 
	
	return 0;
}
		   
int AF43H_GET_CURRENT_TEXT_FUN(void *pbuf_context,void *pbuf_port) //当前全文显示管理之查询
{
	ST_PRESET_TEXT_TAG *pST_CURRENT_TEXT;
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	unsigned char currTextId = stContextp->stSysConfig.ucCurrShow_MsgId;
    if((currTextId >= 1)&&(currTextId <= 127))
    {
		pST_CURRENT_TEXT =(ST_PRESET_TEXT_TAG *)stPortp->stSendInfo.ucbuf;
		pST_CURRENT_TEXT->ucTextID = currTextId;
		GetPresetText(pST_CURRENT_TEXT);
		stPortp->infolen = pST_CURRENT_TEXT->ucTextLength *2 + 2;
		memcpy(&stPortp->stSendInfo.ucbuf[1],&stPortp->stSendInfo.ucbuf[2],stPortp->infolen-1);
	}
	else
	{
        stPortp->stSendInfo.ucbuf[0] = 0;//pST_CURRENT_TEXT->ucTextID;
		stPortp->stSendInfo.ucbuf[1] = 0;//pST_CURRENT_TEXT->ucTextLength;
        stPortp->infolen = 2;
	}
	
	stPortp->scmd = AFC3H_RET_CURRENT_TEXT;

	return 0;
}
		   
int AF14H_CLR_CURRENT_TEXT_FUN(void *pbuf_context,void *pbuf_port) //当前显示消除处理
{
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
	stContextp->stSysConfig.ST_CIRCULATION_DISPLAY.ucTextCount = 0;
	stContextp->stSysConfig.ucCurrShow_MsgId = 0;
	
	stContextp->showmode = showmode_preset;
	
	stContextp->disUpdateTimeUnit = DISPLAY_UPDATETIME_DEFAULT;
	sem_post(&stContextp->Sem_RcvToUpdataCms);//刷新显示 
	sem_post(&stContextp->sem_UpdateConfig);	
#if (PARK_CMS_ENB)
    stContextp->showpark = 0;
#endif

	stPortp->scmd = _0F80H_RET_SETOK;
	stPortp->infolen = sizeof(ST_RET_SETOK_TAG);

	return 0;
}

int AF44H_GET_CURRENT_TEXT_COLOR_FUN(void *pbuf_context,void *pbuf_port)        //当前全文彩色参数管理之查询
{
	ST_PRESET_TEXT_COLOR_TAG *pST_PRESET_TEXT_COLOR;

	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	unsigned char *bufp;

	unsigned char len;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
	
	bufp = &stPortp->stSendInfo.ucbuf[2];
	
	pST_PRESET_TEXT_COLOR = (ST_PRESET_TEXT_COLOR_TAG *)stPortp->stSendInfo.ucbuf;
	pST_PRESET_TEXT_COLOR->ucTextID = stContextp->stSysConfig.ucCurrShow_MsgId;
	if(GetPresetTextColor(pST_PRESET_TEXT_COLOR)==0)
	{
	   len = pST_PRESET_TEXT_COLOR->ucTextCount	+ 2;
	   memcpy(bufp,pST_PRESET_TEXT_COLOR,len);
       memcpy(&bufp[len],&pST_PRESET_TEXT_COLOR->ucV_Bound,6);
	   stPortp->infolen = len + 6;
	}
	else
	{
	   stPortp->stSendInfo.ucbuf[0]=0;
	   stPortp->stSendInfo.ucbuf[1]=0;
	   stPortp->infolen = 2;
	}

	stPortp->scmd = AFC4H_RET_CURRENT_TEXT_COLOR;
	
	return 0;
}

int AF15H_SET_MARQUEE_FUN(void *pbuf_context,void *pbuf_port)                  //跑马灯设定
{
	ST_MARQUEE_TAG  *pST_MARQUEE;
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	int ret=0;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
	
	pST_MARQUEE = (ST_MARQUEE_TAG *)&pStMsgFrame->info[2];

	ret = 1;
    if(pST_MARQUEE->ucTextID > 32)
    {
		stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 1;
	}
    else if(pST_MARQUEE->ucStartX > 6)
    {
		stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 2;
	}
    else if(pST_MARQUEE->ucStartY > 2)
    {
		stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 3;
	}
    else if(pST_MARQUEE->ucWinWide > 7)
    {
		stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 4;
	}
    else if(pST_MARQUEE->ucWinHigh > 3)
    {
		stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 5;
	}
	else if(pST_MARQUEE->ucSpeed >100)
	{
	   stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 8;
	}
	else if(/*(pST_MARQUEE->ucLoops < 1)||*/(pST_MARQUEE->ucLoops > 96))
	{
	   stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 9;
	}
	else
	{
       ret = 0;
	}

    {
	   	char *pfile;
		pfile = malloc(64);
		sprintf((char *)pfile,"/tc/downtext/text%d",pST_MARQUEE->ucTextID);
		
		if(access((char *)pfile,F_OK))
		{
			ret = 1;
		}	
		free(pfile);
    }
   
	if(ret == 1)//Err
	{
	   stPortp->stSendInfo.ST_RET_SETERR.ucErrCode = 0x04;
	   stPortp->scmd = _0F81H_RET_SETERR;
	   stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
	}
	else
	{
	   memcpy((unsigned char *)&stContextp->ST_MARQUEE,(unsigned char *)pST_MARQUEE,sizeof(ST_MARQUEE_TAG));
	   sem_post(&stContextp->Sem_RcvToUpdataCms);//刷新显示 
	   sem_post(&stContextp->Sem_Marquee);//刷新显示 
	   
	   stPortp->scmd = _0F80H_RET_SETOK;
	   stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
	}
	
	return 0;
}
		   
int AF45H_GET_MARQUEE_FUN(void *pbuf_context,void *pbuf_port)                  //跑马灯查询 
{
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
	
//	ST_MARQUEE_TAG *pST_MARQUEE;
	//pST_MARQUEE = (ST_MARQUEE_TAG *)&stContextp->stSysConfig.ST_MARQUEE;

	//memcpy((unsigned char *)&stPortp->stSendInfo.ST_MARQUEE,(unsigned char *)pST_MARQUEE,sizeof(ST_MARQUEE_TAG));

	stPortp->scmd = AFC5H_RET_MARQUEE;
	stPortp->infolen = sizeof(ST_MARQUEE_TAG);

	return 0;
}
		   
int AF16H_SET_CHINESE_FONT_FUN(void *pbuf_context,void *pbuf_port) //中文字形处理设置
{
	ST_USER_CHINESE_FONT_TAG *pST_USER_CHINESE_FONT;
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	pST_USER_CHINESE_FONT = (ST_USER_CHINESE_FONT_TAG *)&pStMsgFrame->info[2];

	if(  (pST_USER_CHINESE_FONT->ucFrameColumn%8)||(pST_USER_CHINESE_FONT->ucFrameRow%8)
	  ||(pST_USER_CHINESE_FONT->ucFrameNumber==0)
	  ||(pST_USER_CHINESE_FONT->ucFrameNumber > pST_USER_CHINESE_FONT->ucFrameTotal))	
	{
	   stPortp->scmd = _0F81H_RET_SETERR;
	   stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
	}
	else
	{
	   printf("AddSelfFontLib,Bit5=%x%x,frameid=%d,frametotoa=%d\n",pST_USER_CHINESE_FONT->ucBig5Code[0],pST_USER_CHINESE_FONT->ucBig5Code[1],pST_USER_CHINESE_FONT->ucFrameNumber,pST_USER_CHINESE_FONT->ucFrameTotal);
	   AddSelfFontLib((unsigned char *)pST_USER_CHINESE_FONT,stContextp);

	   stPortp->scmd = _0F80H_RET_SETOK;
	   stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
	}
	
	return 0;
}
		   
int AF46H_GET_CHINESE_FONT_FUN(void *pbuf_context,void *pbuf_port)//中文字形处理查询  
{
	ST_USER_CHINESE_FONT_TAG *pST_USER_CHINESE_FONT;
	ST_GET_CHINESE_FONT_TAG *pST_GET_CHINESE_FONT;

	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	pST_USER_CHINESE_FONT = &stPortp->stSendInfo.ST_USER_CHINESE_FONT;
	pST_GET_CHINESE_FONT = (ST_GET_CHINESE_FONT_TAG *)&pStMsgFrame->info[2];

	if((pST_GET_CHINESE_FONT->ucFrameColumn%8)||(pST_GET_CHINESE_FONT->ucFrameRow%8))
	{
		stPortp->scmd = _0F81H_RET_SETERR;
		stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
	}
	else
	{
		memcpy((unsigned char *)pST_USER_CHINESE_FONT,(unsigned char *)pST_GET_CHINESE_FONT,sizeof(ST_GET_CHINESE_FONT_TAG));
		pST_USER_CHINESE_FONT->ucFrameNumber = pST_USER_CHINESE_FONT->ucFrameTotal;
		pST_USER_CHINESE_FONT->ucFrameTotal = 0;
		GetSelfFontLib(pST_USER_CHINESE_FONT);

		stPortp->scmd = AFC6H_RET_CHINESE_FONT;
		
		stPortp->infolen  = 6 + pST_USER_CHINESE_FONT->ucFrameColumn * pST_USER_CHINESE_FONT->ucFrameRow/8;
	}
	
	return 0;
}
		   
int AF17H_CLR_CHINESE_FONT_FUN(void *pbuf_context,void *pbuf_port)//中文字形处理清除
{
	ST_CLR_CHINESE_FONT_TAG *pST_CLE_CHINESE_FONT;

    int ret;

	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	pST_CLE_CHINESE_FONT = (ST_CLR_CHINESE_FONT_TAG *)&pStMsgFrame->info[2];

	//DelSelfFontLib((unsigned char *)&stContextp->ST_PTHREAD_USERFONT.stFontFrame,stContextp);
	ret = DelSelfFontLib((unsigned char *)pST_CLE_CHINESE_FONT,stContextp);

    if(ret < 0)
    {
		stPortp->scmd = _0F81H_RET_SETERR;
		stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
	}
	else
	{
		stPortp->scmd = _0F80H_RET_SETOK;
		stPortp->infolen = sizeof(ST_RET_SETOK_TAG);	
	}
	return 0;
}
		   
int AF47H_GET_DISPLAY_MODULE_STATUS_FUN(void *pbuf_context,void *pbuf_port)    //显示模组状态查询
{
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
	
	sem_trywait(&stContextp->Sem_CmsCheckerrRet);
	sem_post(&stContextp->Sem_CmsModuleCheck); 
	sem_wait(&stContextp->Sem_CmsCheckerrRet);
	memcpy((unsigned char *)&stPortp->stSendInfo.ST_RET_DISPLAY_MODULE_STATUS,(unsigned char *)&stContextp->ST_CMS_MODULE_ERR,sizeof(ST_RET_DISPLAY_MODULE_STATUS_TAG));

	stPortp->scmd = AFC7H_RET_DISPLAY_MODULE_STATUS;
	stPortp->infolen = 1 + stPortp->stSendInfo.ST_RET_DISPLAY_MODULE_STATUS.ucErrModuleNO;
	
	return 0;
}

int AF18H_SET_COLOR_SINGLE_TEXT_FUN(void *pbuf_context,void *pbuf_port)        //彩色字窗图形管理之设定
{
	ST_COLOR_SINGLE_TEXT_TAG *pST_SET_COLOR_SINGLE_TEXT;
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
	
	pST_SET_COLOR_SINGLE_TEXT = (ST_COLOR_SINGLE_TEXT_TAG *)&pStMsgFrame->info[2];

	AddSelfImageLib((unsigned char *)pST_SET_COLOR_SINGLE_TEXT, stContextp);
	
	stPortp->scmd = _0F80H_RET_SETOK;
	stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
	
	return 0;
}

int AF48H_GET_COLOR_SINGLE_TEXT_FUN(void *pbuf_context,void *pbuf_port)        //彩色字窗图形管理之查询   
{
	ST_COLOR_SINGLE_TEXT_TAG *pST_RET_COLOR_SINGLE_TEXT;
	ST_GET_COLOR_SINGLE_TEXT_TAG *pST_GET_COLOR_SINGLE_TEXT;
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	int ret;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	pST_RET_COLOR_SINGLE_TEXT = (ST_COLOR_SINGLE_TEXT_TAG *)&stPortp->stSendInfo.ST_COLOR_SINGLE_TEXT;
	pST_GET_COLOR_SINGLE_TEXT = (ST_GET_COLOR_SINGLE_TEXT_TAG *)&pStMsgFrame->info[2];

	memcpy((unsigned char *)pST_RET_COLOR_SINGLE_TEXT,(unsigned char *)pST_GET_COLOR_SINGLE_TEXT,sizeof(ST_GET_COLOR_SINGLE_TEXT_TAG));
	ret = GetSelfImageLib(pST_RET_COLOR_SINGLE_TEXT);

    if(ret <= 0)//无数据
    {
		memset(&pST_RET_COLOR_SINGLE_TEXT->ucFrameRow,0x00,sizeof(ST_COLOR_SINGLE_TEXT_TAG)-2);
    }
	
	stPortp->scmd = AFC8H_RET_COLOR_SINGLE_TEXT;
	stPortp->infolen = 6 + (pST_RET_COLOR_SINGLE_TEXT->ucFrameRow / 4 * pST_RET_COLOR_SINGLE_TEXT->ucFrameColumn);
	
	return 0;
}


int AF31H_SET_OFFLINE_MODE_FUN(void *pbuf_context,void *pbuf_port)              //脱机显示模式之设定
{
	ST_SET_OFFLINE_MODE_TAG  *pST_SET_OFFLINE_MODE;

	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	int ret=0;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	pST_SET_OFFLINE_MODE = (ST_SET_OFFLINE_MODE_TAG *)&pStMsgFrame->info[2];
	if((pST_SET_OFFLINE_MODE->disp < 1)||(pST_SET_OFFLINE_MODE->disp > 4))
	{
	   stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 1;
	   ret = 1;				   
	}

	if(ret == 1)//Err
	{
	   stPortp->stSendInfo.ST_RET_SETERR.ucErrCode = 0x04;
	   
	   stPortp->scmd = _0F81H_RET_SETERR;
	   stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
	}
	else
	{
	   memcpy((unsigned char *)&stContextp->stSysConfig.stOffLine,(unsigned char *)pST_SET_OFFLINE_MODE,sizeof(ST_SET_OFFLINE_MODE_TAG));
	   sem_post(&stContextp->sem_UpdateConfig);    
	   
	   stPortp->scmd = _0F80H_RET_SETOK;
	   stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
	}
	
	return 0;
}
		   
int AF61H_GET_OFFLINE_MODE_FUN(void *pbuf_context,void *pbuf_port)//脱机显示模式之查询    
{
	ST_RET_OFFLINE_MODE_TAG *pST_RET_OFFLINE_MODE;
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	pST_RET_OFFLINE_MODE =(ST_RET_OFFLINE_MODE_TAG *)&stContextp->stSysConfig.stOffLine;

    memcpy((unsigned char *)&stPortp->stSendInfo.ST_RET_OFFLINE_MODE,(unsigned char *)pST_RET_OFFLINE_MODE,sizeof(ST_RET_OFFLINE_MODE_TAG));

	stPortp->scmd = AFE1H_RET_OFFLINE_MODE;
	stPortp->infolen = sizeof(ST_RET_OFFLINE_MODE_TAG) -(pST_RET_OFFLINE_MODE->disp != OFFLINEMODE_DELAY);

	return 0;
}


int AE11H_SET_BREAKLINE_FUN(void *pbuf_context,void *pbuf_port)
{
	ST_SET_OFFLINE_MODE_TAG  *pST_SET_OFFLINE_MODE;

	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	int ret=0;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	pST_SET_OFFLINE_MODE = (ST_SET_OFFLINE_MODE_TAG *)&pStMsgFrame->info[2];
	if((pST_SET_OFFLINE_MODE->disp < 1)||(pST_SET_OFFLINE_MODE->disp > 4))
	{
	   stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 1;
	   ret = 1;				   
	}

	if(ret == 1)//Err
	{
	   stPortp->stSendInfo.ST_RET_SETERR.ucErrCode = 0x04;
	   
	   stPortp->scmd = _0F81H_RET_SETERR;
	   stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
	}
	else
	{
	   memcpy((unsigned char *)&stContextp->stSysConfig.stBreakLine,(unsigned char *)pST_SET_OFFLINE_MODE,sizeof(ST_SET_OFFLINE_MODE_TAG));
	   sem_post(&stContextp->sem_UpdateConfig);    
	   
	   stPortp->scmd = _0F80H_RET_SETOK;
	   stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
	}
	
	return 0;
}
int AE12H_GET_BREAKLINE_FUN(void *pbuf_context,void *pbuf_port)
{
	ST_RET_OFFLINE_MODE_TAG *pST_RET_OFFLINE_MODE;
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	pST_RET_OFFLINE_MODE =(ST_RET_OFFLINE_MODE_TAG *)&stContextp->stSysConfig.stBreakLine;

	memcpy((unsigned char *)&stPortp->stSendInfo.ST_RET_OFFLINE_MODE,(unsigned char *)pST_RET_OFFLINE_MODE,sizeof(ST_RET_OFFLINE_MODE_TAG));

	stPortp->scmd = AE15H_RET_BREAKLINE;
	stPortp->infolen = sizeof(ST_RET_OFFLINE_MODE_TAG) -(pST_RET_OFFLINE_MODE->disp != OFFLINEMODE_DELAY);

	return 0;
}

int AE60H_SET_PACKSIZE_UPLOAD_FUN(void *pbuf_context,void *pbuf_port)
{
	unsigned short temp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_CONTEXT_TAG *stContextp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame=  (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	temp = pStMsgFrame->info[2];
	temp <<= 8;
	temp |= pStMsgFrame->info[3];
	if((temp < LOAD_PACKETSIZE_MIN)||(temp > LOAD_PACKETSIZE_MAX))
	{		
		stPortp->stSendInfo.ST_RET_SETERR.ucErrCode = 0x04;
		
		stPortp->scmd = _0F81H_RET_SETERR;
		stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
	}
	else
	{
		stContextp->stSysConfig.packetsize_upload = temp;
		
		stPortp->scmd = _0F80H_RET_SETOK;
		stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
		sem_post(&stContextp->sem_UpdateConfig);	
	}
	return 0;
}

int AE61H_GET_PACKSIZE_UPLOAD_FUN(void *pbuf_context,void *pbuf_port)
{
	unsigned short uploadPacketSize;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_CONTEXT_TAG *stContextp;

//	unsigned char statue=0;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;

	uploadPacketSize = stContextp->stSysConfig.packetsize_upload;
	if((uploadPacketSize < LOAD_PACKETSIZE_MIN)||(uploadPacketSize > LOAD_PACKETSIZE_MAX))
	{
		uploadPacketSize = LOAD_PACKETSIZE_MIN;
	}

	stPortp->stSendInfo.ucbuf[0] = uploadPacketSize >> 8;
	stPortp->stSendInfo.ucbuf[1] = uploadPacketSize;

	stPortp->scmd	 = AE62H_RET_PACKSIZE_UPLOAD;
	stPortp->infolen = 2;
	
	return 0;
}


int AE18H_SET_CENTER_PORT_FUN(void *pbuf_context,void *pbuf_port)
{
	unsigned short temp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_CONTEXT_TAG *stContextp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame=  (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	temp = pStMsgFrame->info[2];
	stContextp->stSysConfig.remoteportsel = temp;
	
	stPortp->scmd = _0F80H_RET_SETOK;
	stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
	sem_post(&stContextp->sem_UpdateConfig);	
	return 0;
}

int AE19H_GET_CENTER_PORT_FUN(void *pbuf_context,void *pbuf_port)
{
//	unsigned short uploadPacketSize;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_CONTEXT_TAG *stContextp;

//	unsigned char statue=0;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;

	stPortp->stSendInfo.ucbuf[0] = stContextp->stSysConfig.remoteportsel;

	stPortp->scmd	 = AE21H_REG_CENTER_PORT;
	stPortp->infolen = 1;
	
	return 0;
}
	   
int AF3EH_SET_BRIGHTNESS_FUN(void *pbuf_context,void *pbuf_port) //设备亮度控制之设定
{
	ST_BRIGHTNESS_TAG *pST_BRIGHTNESS;

	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	int ret=0;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
	
	pST_BRIGHTNESS = (ST_BRIGHTNESS_TAG *)&pStMsgFrame->info[2];
	if(pST_BRIGHTNESS->ucStartHour >23)
	{
	   stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 2;
	   ret = 1;
	}
	else if(pST_BRIGHTNESS->ucStartMin > 60)
	{
	   stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 2;
	   ret = 1;
	}
	else if(pST_BRIGHTNESS->ucEndHour >23)
	{
	   stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 3;
	   ret = 1;
	}
	else if(pST_BRIGHTNESS->ucEndMin > 60)
	{
	   stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 4;
	   ret = 1;
	}

	if(ret == 1)
	{
	   stPortp->stSendInfo.ST_RET_SETERR.ucErrCode = 0x04;
	   
	   stPortp->scmd = _0F81H_RET_SETERR;
	   stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
	}
	else
	{
	   memcpy((unsigned char *)&stContextp->stSysConfig.ST_BRIGHTNESS,(unsigned char *)pST_BRIGHTNESS,sizeof(ST_BRIGHTNESS_TAG));	   
	   
	   sem_post(&stContextp->Sem_RcvToUpdataCms);//刷新显示 
	   sem_post(&stContextp->sem_UpdateConfig);    

	   stPortp->scmd = _0F80H_RET_SETOK;
	   stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
	}
	
	return 0;
}

		   
int AF6EH_GET_BRIGHTNESS_FUN(void *pbuf_context,void *pbuf_port)//设备亮度控制之查询回报
{
	ST_BRIGHTNESS_TAG *pST_BRIGHTNESS_SLAVE;
	
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
	
	pST_BRIGHTNESS_SLAVE = (ST_BRIGHTNESS_TAG *)&stContextp->stSysConfig.ST_BRIGHTNESS;

    memcpy((unsigned char *)&stPortp->stSendInfo.ST_BRIGHTNESS,(unsigned char *)pST_BRIGHTNESS_SLAVE,sizeof(ST_BRIGHTNESS_TAG));
    
	stPortp->scmd = AFEEH_RET_BRIGHTNESS_SLAVE;
	stPortp->infolen = sizeof(ST_BRIGHTNESS_TAG);
		
	return 0;
}

//--------------------------通用信息------------------------------
int _0F11H_SET_RESET_FUN(void *pbuf_context,void *pbuf_port)//重设定现场设备
{
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

#if(!TC_PANEL_ENB)
	switch_ctr();
#endif

	stPortp->scmd = _0F91H_RET_RESET;
	stPortp->infolen = 0;//sizeof(ST_RET_SETOK_TAG);
		
	return 0;
}

int _0F10H_SET_REBOOT_FUN(void *pbuf_context,void *pbuf_port)//重启现场设备
{
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
	
	stPortp->stSendInfo.ST_REBOOT.ucReset[0]=0x52;
	stPortp->stSendInfo.ST_REBOOT.ucReset[1]=0x52;
	
	stPortp->scmd = _0F90H_RET_REBOOT;
	stPortp->infolen = sizeof(ST_REBOOT_TAG);
	AddInfoToSendList(stPortp,_SEND_INFO);
	
//--------------------reset card---------------------------------
	pthread_mutex_lock(&stContextp->PortCms.stSendInfoLock);
	stContextp->PortCms.scmd = A008H_SET_CMS_REBOOT;
	stContextp->PortCms.infolen = 0;
//	stContextp->PortCms.stSendInfo.ucbuf[0] = 0;
	AddInfoToSendList(&stContextp->PortCms, _SEND_INFO);
	pthread_mutex_unlock(&stContextp->PortCms.stSendInfoLock);		
//---------------------------------------------------------------
    system("sync");
	sleep(2);
	reboot(RB_AUTOBOOT);
	
	return 1;
}
		   
int _0F40H_GET_DEVICE_NUM_FUN(void *pbuf_context,void *pbuf_port)//查询现场设备编号
{
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	ST_GET_DEVICE_NUM_TAG *pST_GET_DEVICE_NUM;
	ST_RET_DEVICE_NUM_TAG *pST_RET_DEVICE_NUM;
	
	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	pST_GET_DEVICE_NUM = (ST_GET_DEVICE_NUM_TAG *)&pStMsgFrame->info[2];;
	pST_RET_DEVICE_NUM = (ST_RET_DEVICE_NUM_TAG *)&stContextp->stSysConfig.ST_RET_DEVICE_NUM;

	switch(pST_GET_DEVICE_NUM->ucEquipmentNo)
	{
		case 0:
		pST_RET_DEVICE_NUM->ucEquipmentNo = 0;
		pST_RET_DEVICE_NUM->ucSubCount = 1;
		pST_RET_DEVICE_NUM->stSubDevice[0].equ = 0;
		pST_RET_DEVICE_NUM->stSubDevice[0].number[0]=stContextp->stSysConfig.address[0];
		pST_RET_DEVICE_NUM->stSubDevice[0].number[1]=stContextp->stSysConfig.address[1];

		memcpy((unsigned char *)&stPortp->stSendInfo.ST_RET_DEVICE_NUM,(unsigned char *)pST_RET_DEVICE_NUM,sizeof(ST_RET_DEVICE_NUM_TAG));
		
		stPortp->scmd = _0FC0H_RET_DEVICE_NUM;
		stPortp->infolen = pST_RET_DEVICE_NUM->ucSubCount * 3 + 2;
		break;

		case 1:
		case 2:
		case 3:
		{
			unsigned char n;
			n= pST_GET_DEVICE_NUM->ucEquipmentNo;
			pST_RET_DEVICE_NUM->ucEquipmentNo = n;
			pST_RET_DEVICE_NUM->ucSubCount = 1;
			pST_RET_DEVICE_NUM->stSubDevice[0].equ = pST_GET_DEVICE_NUM->ucEquipmentNo;

			memcpy((unsigned char *)&stPortp->stSendInfo.ST_RET_DEVICE_NUM,(unsigned char *)pST_RET_DEVICE_NUM,sizeof(ST_RET_DEVICE_NUM_TAG));
			
			stPortp->scmd = _0FC0H_RET_DEVICE_NUM;
			stPortp->infolen = pST_RET_DEVICE_NUM->ucSubCount * 3 + 2;
		}
		break;

		case 0xff:
		{
			unsigned char i;
			pST_RET_DEVICE_NUM->ucEquipmentNo = 0xff;
			pST_RET_DEVICE_NUM->ucSubCount = 3;

			for(i=0;i<3;i++)
			{
				pST_RET_DEVICE_NUM->stSubDevice[i].equ = i+1;
			}
			
			memcpy((unsigned char *)&stPortp->stSendInfo.ST_RET_DEVICE_NUM,(unsigned char *)pST_RET_DEVICE_NUM,sizeof(ST_RET_DEVICE_NUM_TAG));
			
			stPortp->scmd = _0FC0H_RET_DEVICE_NUM;
			stPortp->infolen = pST_RET_DEVICE_NUM->ucSubCount * 3 + 2;
		}					 
		break;

		default:			

		
		stPortp->scmd = _0F81H_RET_SETERR;
		stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
		break;
	}
	
	return 0;
}
		   
		   
int _0F41H_GET_DEVICE_STATUS_FUN(void *pbuf_context,void *pbuf_port) //查询现场设备状态
{
//	ST_RET_HARDSTATUE_TAG *pHardwarestaues;
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	//ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
/*
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	pHardwarestaues = (ST_RET_HARDSTATUE_TAG *)malloc(sizeof(ST_RET_HARDSTATUE_TAG));
	memset((unsigned char *)pHardwarestaues,0x00,sizeof(ST_RET_HARDSTATUE_TAG));
	pHardwarestaues->ST_HARDERR_BITMAP.commlinebad |= (stContextp->PortCms.lineStatue != CONNECT_ONLINE);
	pHardwarestaues->ST_HARDERR_BITMAP.communicationConnect |= (stContextp->PortCms.lineStatue != CONNECT_ONLINE);
*/
	GetHardWareStatus(stContextp);
	memcpy((unsigned char *)&stPortp->stSendInfo.ST_RET_HARDSTATUE,(unsigned char *)&stContextp->stHardState,sizeof(ST_RET_HARDSTATUE_TAG));

	stPortp->scmd = _0FC1H_RET_DEVICE_STATUS;
	stPortp->infolen = sizeof(ST_RET_HARDSTATUE_TAG);
	
	//free(pHardwarestaues);
	
	return 0;
}
		   
int _0F12H_SET_DATETIME_FUN(void *pbuf_context,void *pbuf_port)//设定现场设备日期时间
{
	ST_DATETIME_TAG *pST_DATETIME;

	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	int ret=0;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	pST_DATETIME = (ST_DATETIME_TAG *)&pStMsgFrame->info[2];

	if((pST_DATETIME->Month <1)||(pST_DATETIME->Month >12))
	{
		stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 2;
		ret = 1;
	}
	else if((pST_DATETIME->Day<1)||(pST_DATETIME->Month >31))
	{
		stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 3;
		ret = 1;
	}
	else if((pST_DATETIME->Week<1)||(pST_DATETIME->Week >7))
	{
		stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 4;
		ret = 1;
	}
	else if(pST_DATETIME->Hour>23)
	{
		stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 5;
		ret = 1;
	}
	else if(pST_DATETIME->Min>59)
	{
		stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 6;
		ret = 1;
	}
	else if(pST_DATETIME->Sec>59)
	{
		stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 7;
		ret = 1;
	}

	if(ret == 1)
	{
		printf("Set DataTime Err\n");
		stPortp->stSendInfo.ST_RET_SETERR.ucErrCode = 0x04;
		
		
		stPortp->scmd = _0F81H_RET_SETERR;
		stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
	}
	else
	{
		ST_DATETIME_TAG *stCurrDataTimep;

		stCurrDataTimep = (ST_DATETIME_TAG *)malloc(sizeof(ST_DATETIME_TAG));			
        GetDateTime(stCurrDataTimep);
		if(  (stCurrDataTimep->Year != pST_DATETIME->Year)
		   ||(stCurrDataTimep->Month!= pST_DATETIME->Month)
		   ||(stCurrDataTimep->Day  != pST_DATETIME->Day)
		   ||(stCurrDataTimep->Hour != pST_DATETIME->Hour)
		   ||(stCurrDataTimep->Min  != pST_DATETIME->Min) )
		{
			
			stPortp->scmd	= _0F92H_DATETIME_SYNCH;
			stPortp->stSendInfo.ST_DATETIME_SYNCH.SecDif = 0x80;
			stPortp->infolen = sizeof(ST_DATETIME_SYNCH_TAG);

			//sem_post(&stContextp->upReportDelay);

			stContextp->stHardState.timererr = 1;
		}
		else
		{
			
			stPortp->scmd = _0F80H_RET_SETOK;
			stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
		   
		    stContextp->stHardState.timererr = 0;
		}
		
		SetDateTime(pST_DATETIME);				  
		
		sem_post(&stContextp->check_datatime);  //更新断电时

		free(stCurrDataTimep);

        /*
		//-----------------向中心回应0F02-------------			
		printf("**********************report to center: local change timer\n");
		pthread_mutex_lock(&stContextp->PortCenter_Eth.stSendInfoLock);
		stContextp->PortCenter_Eth.scmd	  = _0F02H_REBOOT_REP;
		stContextp->PortCenter_Eth.infolen = 0;
		AddInfoToSendList(stPortp,_SEND_INFO);
		pthread_mutex_unlock(&stContextp->PortCenter_Eth.stSendInfoLock);
		//--------------------------------------------		
		*/
	}

	return 0;
}
		   
int _0F42H_GET_DATETIME_FUN(void *pbuf_context,void *pbuf_port) //查询现场设备日期时间
{
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	GetDateTime(&stPortp->stSendInfo.ST_DATETIME);

	stPortp->scmd = _0FC2H_RET_DATETIME;
	stPortp->infolen = sizeof(ST_DATETIME_TAG);

	return 0;
}
		   
int _0F13H_SET_SOFTVER_FUN(void *pbuf_context,void *pbuf_port)
{
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
	
    stContextp->stSysConfig.ST_RET_SOFTVER.CmdSet = pStMsgFrame->info[2];
	sem_post(&stContextp->sem_UpdateConfig);	

	
	stPortp->scmd = _0F80H_RET_SETOK;
	stPortp->infolen = sizeof(ST_RET_SETOK_TAG);

	return 0;
}

		   
int _0F43H_GET_SOFTVER_FUN(void *pbuf_context,void *pbuf_port)//查询软件版本
{
	struct stat fileinfo;
	//time_t now;
	struct tm *timenow;

	ST_RET_SOFTVER_TAG *pST_RET_SOFTVER;
	
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	//int ret;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
	
	pST_RET_SOFTVER = (ST_RET_SOFTVER_TAG *)malloc(sizeof(ST_RET_SOFTVER_TAG));

	stat("/tc/tc",&fileinfo); 
	timenow=gmtime(&fileinfo.st_mtim);
	printf("%s\n",asctime(gmtime(&fileinfo.st_mtim)));

	pST_RET_SOFTVER->Year = timenow->tm_year+1900-1911;
	pST_RET_SOFTVER->Month = timenow->tm_mon+1;
	pST_RET_SOFTVER->Day = timenow->tm_mday;

	pST_RET_SOFTVER->Ver = _VERSION_NUMBER;//stContextp->stSysConfig.ST_RET_SOFTVER.Ver;
	pST_RET_SOFTVER->CompanyID = 0x12;//stContextp->stSysConfig.ST_RET_SOFTVER.CompanyID;
	pST_RET_SOFTVER->CmdSet = stContextp->stSysConfig.ST_RET_SOFTVER.CmdSet;

	printf("-------Ver Year=%d,Mon=%d,Day=%d,Ver=%x.\n",timenow->tm_year,timenow->tm_mon,timenow->tm_mday,pST_RET_SOFTVER->Ver);

    memcpy((unsigned char *)&stPortp->stSendInfo.ST_RET_SOFTVER,(unsigned char *)pST_RET_SOFTVER,sizeof(ST_RET_SOFTVER_TAG));
	
	
	stPortp->scmd = _0FC3H_RET_SOFTVER;
	stPortp->infolen = sizeof(ST_RET_SOFTVER_TAG);

	free(pST_RET_SOFTVER);
	
	return 0;
}
		   
int _0F14H_SET_HARDSTATUS_REPORTCLY_FUN(void *pbuf_context,void *pbuf_port) //设定现场设备回报硬件状态之周期
{
	ST_SET_HARDSTATUS_REPORTCLY_TAG *pST_SET_HARDSTATUS_REPORTCLY;

	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
	
	pST_SET_HARDSTATUS_REPORTCLY = (ST_SET_HARDSTATUS_REPORTCLY_TAG *)&pStMsgFrame->info[2];
	stContextp->ST_SET_HARDSTATUS_REPORTCLY.Time = pST_SET_HARDSTATUS_REPORTCLY->Time;
	
	stPortp->scmd = _0F80H_RET_SETOK;
	stPortp->infolen = sizeof(ST_RET_SETOK_TAG);

	return 0;
}

		   
int _0F4FH_GET_HARDSTATUS_REPORTCLY_FUN(void *pbuf_context,void *pbuf_port)    //查询现场设备回报硬件状态之周期
{
	ST_SET_HARDSTATUS_REPORTCLY_TAG  *pST_RET_HARDSTATUS_REPORTCLY;
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	pST_RET_HARDSTATUS_REPORTCLY = &stContextp->ST_SET_HARDSTATUS_REPORTCLY;

    memcpy((unsigned char *)&stPortp->stSendInfo.ST_SET_HARDSTATUS_REPORTCLY,(unsigned char *)pST_RET_HARDSTATUS_REPORTCLY,sizeof(ST_SET_HARDSTATUS_REPORTCLY_TAG));

	
	stPortp->scmd = _0FC4H_RET_HARDSTATUS_REPORTCLY;
	stPortp->infolen = sizeof(ST_SET_HARDSTATUS_REPORTCLY_TAG);

	return 0;
}

int _0F15H_SET_DATA_PASSWORD_FUN(void *pbuf_context,void *pbuf_port)
{
	ST_PASSWORD_TAG *pST_PASSWORD;
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	pST_PASSWORD=(ST_PASSWORD_TAG *)&pStMsgFrame->info[2];
	
	memcpy(stContextp->stSysConfig.ST_PASSWORD.password,pST_PASSWORD,6);
	sem_post(&stContextp->sem_UpdateConfig);	
	
	stPortp->scmd = _0F80H_RET_SETOK;
	stPortp->infolen = sizeof(ST_RET_SETOK_TAG);

	return 0;
}

int _0F45H_GET_DATA_PASSWORD_FUN(void *pbuf_context,void *pbuf_port)
{	
	ST_PASSWORD_TAG *pST_PASSWORD;
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
	
    pST_PASSWORD =(ST_PASSWORD_TAG *)&stContextp->stSysConfig.ST_PASSWORD;
    memcpy((unsigned char *)&stPortp->stSendInfo.ST_PASSWORD,(unsigned char *)pST_PASSWORD,sizeof(ST_PASSWORD_TAG));

	stPortp->scmd = _0FC5H_RET_DATA_PASSWORD;
	stPortp->infolen = sizeof(ST_PASSWORD_TAG);

	return 0;
}

  
int _0F16H_SET_LOCK_UNLOCK_FUN(void *pbuf_context,void *pbuf_port)   //锁定或解除现场操作		  
{
	ST_SET_LOCK_UNLOCK_TAG *pST_SET_LOCK_UNLOCK;
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	pST_SET_LOCK_UNLOCK = (ST_SET_LOCK_UNLOCK_TAG *)&pStMsgFrame->info[2];
	memcpy((unsigned char *)&stContextp->stSysConfig.ST_SET_LOCK_UNLOCK,
	      (unsigned char *)pST_SET_LOCK_UNLOCK,sizeof(ST_SET_LOCK_UNLOCK_TAG));

	//printf("_SET_LOCK_UNLOCK:LockDB=%d\n",stContextp->stSysConfig.ST_SET_LOCK_UNLOCK.LockDB);

	sem_post(&stContextp->sem_UpdateConfig);	
	
	stPortp->scmd = _0F80H_RET_SETOK;
	stPortp->infolen = sizeof(ST_RET_SETOK_TAG);

	return 0;
}
		   
int _0F46H_GET_LOCK_UNLOCK_FUN(void *pbuf_context,void *pbuf_port)  //查询锁定或解除现场操作
{
	ST_RET_LOCK_UNLOCK_TAG *pST_RET_LOCK_UNLOCK;

	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
	
	pST_RET_LOCK_UNLOCK = (ST_RET_LOCK_UNLOCK_TAG *)&stContextp->stSysConfig.ST_SET_LOCK_UNLOCK;
    memcpy((unsigned char *)&stPortp->stSendInfo.ST_RET_LOCK_UNLOCK,(unsigned char *)pST_RET_LOCK_UNLOCK,sizeof(ST_RET_LOCK_UNLOCK_TAG));

	
	stPortp->scmd = _0FC6H_RET_LOCK_UNLOCK;
	stPortp->infolen = sizeof(ST_RET_LOCK_UNLOCK_TAG);

	return 0;
}

int _0F47H_GET_PROTOL_CODE_FUN(void *pbuf_context,void *pbuf_port)
{
	unsigned char *uubuf;
	unsigned int i;
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
	
    uubuf = malloc(128);
     
    uubuf[0]= pStMsgFrame->info[2];			   
    uubuf[1]= pStMsgFrame->info[3];
    uubuf[2]= 1;
    for(i=0;i<sizeof(ucgProtolCode_Cmd)/sizeof(unsigned short);i++)
    {
         if((uubuf[0] == (ucgProtolCode_Cmd[i]>>8)) &&(uubuf[1] == (ucgProtolCode_Cmd[i]&0xff)) )	
         {
			uubuf[2]= 0;
            break;
		 }
    }

    memcpy(stPortp->stSendInfo.ucbuf,uubuf,3);
	
    stPortp->scmd = _0FC7H_RET_PROTOL_CODE;
    stPortp->infolen = 3;
	
    free(uubuf);
	
    return 0;
}

int AE33H_SET_CMSERRCHECK_FUN(void *pbuf_context,void *pbuf_port)
{	
    ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

    if(pStMsgFrame->info[2] <= 2)
    {
        sem_post(&stContextp->Sem_CmsCheckErr);  
		
		stPortp->scmd = _0F80H_RET_SETOK;
		stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
	}
    else
    {        
		stPortp->scmd = _0F81H_RET_SETERR;
		stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
	}
    
	return 0;
}

int AE43H_GET_CHECKFONT_FUN(void *pbuf_context,void *pbuf_port)
{
	int len;
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FONTIMAGE_CRC_TAG *fontCrcp;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;

    fontCrcp = (ST_FONTIMAGE_CRC_TAG *)stPortp->stSendInfo.ucbuf;
	GetSelfFontLib_crc(fontCrcp);
	stPortp->scmd = AEC3H_RET_CHECKFONT;
	len = fontCrcp->len_ml[0];
	len <<= 8;
	len |= fontCrcp->len_ml[1];
	stPortp->infolen = len * sizeof(ST_USERFONT_CRC_TAG) + sizeof(fontCrcp->len_ml);

	return 0;
}

int AE44H_GET_CHECKIMAGE_FUN(void *pbuf_context,void *pbuf_port)
{
	int len;
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FONTIMAGE_CRC_TAG *imageCrcp;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;

    imageCrcp = (ST_FONTIMAGE_CRC_TAG *)stPortp->stSendInfo.ucbuf;
	memset(imageCrcp,0x00,sizeof(ST_FONTIMAGE_CRC_TAG));
	GetSelfImageLib_crc(imageCrcp);

	stPortp->scmd = AEC4H_RET_CHECKIMAGE;
	len = imageCrcp->len_ml[0];
	len <<= 8;
	len |= imageCrcp->len_ml[1];
	stPortp->infolen = len * sizeof(ST_USERFONT_CRC_TAG) + sizeof(imageCrcp->len_ml);

	return 0;
}

int AE06H_SET_ADDRESSCODE_FUN(void *pbuf_context,void *pbuf_port)//下载的地址码
{
//	int fp;

	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

    printf("Set my address:%x%x\n",pStMsgFrame->info[2],pStMsgFrame->info[3]);

	stContextp->stSysConfig.address[0]=pStMsgFrame->info[2];
	stContextp->stSysConfig.address[1]=pStMsgFrame->info[3];
	sem_post(&stContextp->sem_UpdateConfig);	

	
	stPortp->scmd = _0F80H_RET_SETOK;	
	stPortp->infolen = sizeof(ST_RET_SETOK_TAG);	
	
	return 0;
}

int AE46H_GET_ADDRESSCODE_FUN(void *pbuf_context,void *pbuf_port)//下载的地址码
{
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;

	stPortp->stSendInfo.ucbuf[0] = stContextp->stSysConfig.address[0];
	stPortp->stSendInfo.ucbuf[1] = stContextp->stSysConfig.address[1];

	stPortp->scmd = AEC6H_RET_ADDRESSCODE;	
	stPortp->infolen = 2;	
	
	return 0;
}

int AE10H_UPDATASOF_SET_FUN(void *pbuf_context,void *pbuf_port)
{	
	//static unsigned char crc_debug;
	//int fp;
	int packnum,packtotal,textlen;
	ST_UPDATASOF_SET_TAG *pStUpdataSof_Set;

	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	//int i;
	//int ret=0;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	pStUpdataSof_Set = (ST_UPDATASOF_SET_TAG *)&pStMsgFrame->info[2];

	packnum =( (pStUpdataSof_Set->packnum[0]<<8) | (pStUpdataSof_Set->packnum[1]) ) - 1;
	packtotal=(pStUpdataSof_Set->packtotal[0]<<8) | (pStUpdataSof_Set->packtotal[1]);
	textlen = (pStUpdataSof_Set->textlen[0]<<8) | (pStUpdataSof_Set->textlen[1]);

#if 0
/*
    fp = open("/tc/sunsky.t",O_RDWR|O_CREAT,777);

	if((fp < 0)||(packnum >= packtotal)||(!textlen))
	{
        printf("open ./sunsky.t err\n");
		
		stPortp->scmd = _0F81H_RET_SETERR;
		stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
	}
	else
	{
		unsigned int seek;
		
		if(packnum==0)
		{
			ret=truncate("/tc/sunsky.t",0);//设置文件长度
			crc_debug = 0;
		}

       // seek = packnum * 1024;   
       // lseek(fp,seek,SEEK_END);
			
		lseek(fp,0,SEEK_END);
			
		i=write(fp,pStUpdataSof_Set->text,textlen);
		close(fp);

        sem_post(&stContextp->upReportDelay);
		
		
		stPortp->scmd = _0F80H_RET_SETOK;
		stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
	}
	*/
#else

    if((packnum >= packtotal)||(!textlen))
	{
		
		stPortp->scmd = _0F81H_RET_SETERR;
		stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
	}
	else
	{  		
		unsigned int seek;
     
	    printf("SofDown:packnum=%d,packtotal=%d,packlen=%d\n",packnum,packtotal,textlen);
	    seek = packnum * 1024;	 
        memcpy(&ucGrcvUpdataSoft_p[seek],pStUpdataSof_Set->text,textlen);
        uiGRcvUpdataSoftLen = seek + textlen;
		
        sem_post(&stContextp->upReportDelay);
		
		stPortp->scmd = _0F80H_RET_SETOK;
		stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
	}

#endif
	return 0;
}


int AE40H_UPDATASOF_GETCRC_FUN(void *pbuf_context,void *pbuf_port)
{	
//	int filelen;
	int i;
//	unsigned char *ucbuf;
	unsigned char crc;
//	ST_UPDATASOF_RETCRC_TAG updataSoftCrc_p;

	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
    crc = 0;
#if 0	
	ucbuf = malloc(1024);
	fp = open("/tc/sunsky.t",O_RDONLY,777);
	lseek(fp,0,SEEK_SET);	
	filelen = 0;
	while(1)
	{
		len = read(fp,ucbuf,1024);
		filelen += len;
		for(i=0;i<len;i++)
		{
			crc ^= ucbuf[i];
		}
		if(len != 1024)
			break;
	}
	close(fp);
	printf("-------file ./sunsky.t crc = %d,filelen=%d\n",crc,filelen);
#else
    for(i=0;i<uiGRcvUpdataSoftLen;i++)
    {
		crc ^= ucGrcvUpdataSoft_p[i];
	}
#endif
 	sem_init(&stContextp->upReportDelay,0,0); 

    stPortp->stSendInfo.ST_UPDATASOF_CRC.crc = crc;
	stPortp->scmd = AEC0H_UPDATASOF_RETCRC;
	stPortp->infolen = sizeof(ST_UPDATASOF_RETCRC_TAG);
	//free(ucbuf);

    /*
    printf("rev update len=%d\n",uiGRcvUpdataSoftLen);
	{
		int fp;
		int ret;
		fp = open("/tc/upsoft",O_RDWR|O_CREAT,777);
		lseek(fp,0,SEEK_SET);
		ret = write(fp,ucGrcvUpdataSoft_p,uiGRcvUpdataSoftLen);
		i=truncate("/tc/upsoft",uiGRcvUpdataSoftLen);
		
		close(fp);
	}
	*/
	return 0;
}
		
int AEC1H_UPDATASOF_SETEND_FUN(void *pbuf_context,void *pbuf_port)
{
	ST_UPDATASOF_SETEND_TAG *pST_UPDATASOF_SETEND;
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
    int ret=0;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	pST_UPDATASOF_SETEND = (ST_UPDATASOF_SETEND_TAG *)&pStMsgFrame->info[2];
	
	if((!pST_UPDATASOF_SETEND->err)&&(uiGRcvUpdataSoftLen))
	{
		int fp;

		rename("/tc/tc","/tc/sunskytc.bat");	 		
	    
	    fp = open("/tc/tc",O_RDWR|O_CREAT,777);
		
		ret=truncate("/tc/tc",0);//设置文件长度
//        lseek(fp,0,SEEK_END);
        lseek(fp,0,SEEK_SET);
        write(fp,ucGrcvUpdataSoft_p,uiGRcvUpdataSoftLen);
		uiGRcvUpdataSoftLen=0;
		close(fp);
	}
	
	
	stPortp->scmd = _0F80H_RET_SETOK;
	stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
	printf("sync\n");
	system("sync");
	sleep(1);
    printf("reboot\n");	
	//ret = system("reboot\n");
	reboot(RB_AUTOBOOT);

	
	return 0;
}

int AE20H_CLEAN_USERFILE_FUN(void *pbuf_context,void *pbuf_port)
{
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
    int ret=0;
	
	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	if(!access("/tc/downimage",F_OK))
	{
		ret = system("rm -r /tc/downimage\n");		
	}
	else
	{
		printf("user image file already clean\n");
	}

	stPortp->scmd = _0F80H_RET_SETOK;
	stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
    return 0;
}

int AE05H_SET_NET_CONF_FUN(void *pbuf_context,void *pbuf_port)
{
	ST_CONTEXT_TAG *stContextp;
	ST_NET_CONF_TAG *stNetConfp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
	stNetConfp = (ST_NET_CONF_TAG *)&pStMsgFrame->info[2];

    if(  (stNetConfp->gateway[0] != stNetConfp->ipaddr[0])
	   ||(stNetConfp->gateway[1] != stNetConfp->ipaddr[1])
       ||(stNetConfp->gateway[2] != stNetConfp->ipaddr[2])
       )
    {
		stPortp->scmd = _0F81H_RET_SETERR;
		stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
		
		return 0;
	}
    memcpy(&stContextp->stSysConfig.ipConfig,stNetConfp,sizeof(ST_NET_CONF_TAG));   
	
	sem_post(&stContextp->sem_UpdateConfig);	
	
	stPortp->scmd = _0F80H_RET_SETOK;
	stPortp->infolen = sizeof(ST_RET_SETOK_TAG);

    return 0;
	
}

int AE45H_GET_NET_CONF_FUN(void *pbuf_context,void *pbuf_port)
{
	ST_NET_CONF_TAG *stNetConfp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame=  (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
	stNetConfp =  (ST_NET_CONF_TAG *)stPortp->stSendInfo.ucbuf;

    memcpy(stNetConfp,&stContextp->stSysConfig.ipConfig,sizeof(ST_NET_CONF_TAG));   
	
	stPortp->scmd = AEC5H_REG_NET_CONF;
	stPortp->infolen = sizeof(ST_NET_CONF_TAG);
	
	return 0;
	
}

int AE07H_SET_CURRBRIGHT_FUN(void *pbuf_context,void *pbuf_port)
{
    ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame=  (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	stContextp->stSysConfig.ST_BRIGHTNESS.unBright.ST_BRIGHT_BITMAP.r50 = !stContextp->stSysConfig.ST_BRIGHTNESS.unBright.ST_BRIGHT_BITMAP.r50;
	sem_post(&stContextp->sem_UpdateConfig);	
    if(stContextp->stSysConfig.ST_BRIGHTNESS.unBright.ST_BRIGHT_BITMAP.r50)
	{	stContextp->CmsMode = 3;//全亮
    }
    else
	{	stContextp->CmsMode = 0;//退出测试状态
    }
	sem_post(&stContextp->Sem_RcvToUpdataCms);//刷新显示 
	
	stPortp->scmd = _0F80H_RET_SETOK;
	stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
	
  	return 0;
}

int AE47H_GET_CURRBRIGHT_FUN(void *pbuf_context,void *pbuf_port)
{
	ST_PORTRCVTXD_TAG *stPortp;
	//ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
		
	stPortp->scmd = AEC7H_RET_CURRBRIGHT;
	
    stPortp->stSendInfo.ucbuf[0]=Brightness_Control(stContextp);
	stPortp->infolen = 1;
	
	return 0;
}

int AE08H_SET_BRIGHTTAB_FUN(void *pbuf_context,void *pbuf_port)
{
    int i;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame=  (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	for(i=0;i<sizeof(stContextp->stSysConfig.ST_BRIGHTNESS_DEF.adc)/sizeof(unsigned short);i++)
	{		
		stContextp->stSysConfig.ST_BRIGHTNESS_DEF.adc[i] = pStMsgFrame->info[2+i*2];
		stContextp->stSysConfig.ST_BRIGHTNESS_DEF.adc[i] <<= 8;
		stContextp->stSysConfig.ST_BRIGHTNESS_DEF.adc[i] |= pStMsgFrame->info[2+i*2+1];
	}

    memcpy(&stContextp->stSysConfig.ST_BRIGHTNESS_DEF.rate,&pStMsgFrame->info[2+i*2],sizeof(stContextp->stSysConfig.ST_BRIGHTNESS_DEF.rate));
	sem_post(&stContextp->sem_UpdateConfig);	
		
	sem_post(&stContextp->Sem_RcvToUpdataCms);//刷新显示 
	stPortp->scmd = _0F80H_RET_SETOK;
	stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
	
	return 0;
}

int AE48H_GET_BRIGHTTAB_FUN(void *pbuf_context,void *pbuf_port)
{
	int i;
	ST_PORTRCVTXD_TAG *stPortp;
	//ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;

    for(i=0;i<sizeof(stContextp->stSysConfig.ST_BRIGHTNESS_DEF.adc)/sizeof(unsigned short);i++)
    {
        stPortp->stSendInfo.ucbuf[i*2] = stContextp->stSysConfig.ST_BRIGHTNESS_DEF.adc[i]>>8;
		stPortp->stSendInfo.ucbuf[i*2+1] = stContextp->stSysConfig.ST_BRIGHTNESS_DEF.adc[i];
	}
	memcpy(&stPortp->stSendInfo.ucbuf[i*2],stContextp->stSysConfig.ST_BRIGHTNESS_DEF.rate,sizeof(stContextp->stSysConfig.ST_BRIGHTNESS_DEF.rate));
		
	stPortp->scmd = AEC8H_RET_BRIGHTTAB;
	stPortp->infolen = sizeof(ST_BRIGHTNESS_DEF_TAG);
	
	return 0;
}

int  AE23H_GET_CONTRMODE_FUN(void *pbuf_context,void *pbuf_port)
{
	ST_PORTRCVTXD_TAG *stPortp;
	ST_CONTEXT_TAG *stContextp;

    //unsigned char statue=0;
	t_AE53_RET_CTRMODE ctrmode;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;

    printf("Local:%d, Remote:%d\n",stContextp->PortLocate_p->lineStatue,stContextp->PortRemote_p->lineStatue);

	ctrmode.byte = 0;
	ctrmode.bit.LocalCtr = (stContextp->PortLocate_p->lineStatue == CONNECT_ONLINE)? 1 : 0;
	ctrmode.bit.RemoteCtr = (stContextp->PortRemote_p->lineStatue == CONNECT_ONLINE)? 1 : 0;
#if(!TC_PANEL_ENB)
	ctrmode.bit.LocalSwitch = (Get_CtrStatues()==0)? 1 : 0 ; // 0现场控制;
#else
    ctrmode.bit.LocalSwitch = stContextp->autotest;
#endif

	stPortp->stSendInfo.ucbuf[0] = ctrmode.byte;//statue;
    stPortp->stSendInfo.ucbuf[1] = Brightness_Control(stContextp);

	stPortp->scmd    = AE53H_REG_CONTRMODE;
	stPortp->infolen = 2;
	
	return 0;
}

int AE65H_GET_CMSTYPE_FUN(void *pbuf_context,void *pbuf_port)
{		
	ST_PORTRCVTXD_TAG *stPortp;
	ST_CONTEXT_TAG *stContextp;
    char *upload_p;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;

	upload_p = &stPortp->stSendInfo.ucbuf[0];
	
    #if(CMS_SPCE_CONFIG==CMS_RG8X1_F32X32)
    sprintf(upload_p,"CMS_RG_%dX%d_%dX%d",8,1,32,32);
	#else
    sprintf(upload_p,"CMS_RG_%dX%d_%dX%d",_CMS_CTRCARD_DISP_V/CMS_FONT_VIEW_V,
                                          _CMS_CTRCARD_DISP_W*_CMS_CTRCARD_NUM/CMS_FONT_VIEW_W,
                                          CMS_FONT_VIEW_W,
                                          CMS_FONT_VIEW_V);
	#endif
	
	stPortp->scmd = AE66H_REG_CMSTYPE;
	stPortp->infolen = 16;
	
	return 0;
}

int AE67H_GET_MISC_FUN(void *pbuf_context,void *pbuf_port)
{
    ST_PORTRCVTXD_TAG *stPortp;
	ST_CONTEXT_TAG *stContextp;

    ST_MISC_CONF_TAG *pstMisc;

    int fp;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;

	
    #if (CMS_FONT_VIEW_V == 48)
    	fp = open("/tc/font48X48" ,O_RDONLY,777);
    #elif (CMS_FONT_VIEW_V == 32)
    	fp = open("/tc/font32X32" ,O_RDONLY,777);
    #endif

 
	pstMisc = (ST_MISC_CONF_TAG *)&stPortp->stSendInfo.ucbuf[0];
	if(fp < 0)
	{
        pstMisc->fontlib_lenm = 0;
		pstMisc->fontlib_lenl = 0;
	}
	else
	{
        pstMisc->fontlib_lenm = 14106>>8;
		pstMisc->fontlib_lenl = 14106&0xff;
	}

    pstMisc->xinbei = stContextp->stSysConfig.tMisc.xinbei;
	pstMisc->Mfail_LedNum = stContextp->stSysConfig.tMisc.Mfail_LedNum;

  //  printf("xinbei=%d,Mfail_LedNum=%d\n",stContextp->stSysConfig.tMisc.xinbei,stContextp->stSysConfig.tMisc.Mfail_LedNum);
		
	stPortp->scmd = AE68H_RET_MISC;
	stPortp->infolen = sizeof(ST_MISC_CONF_TAG);

	return 0;
}

int AE69H_SET_MISC_FUN(void *pbuf_context,void *pbuf_port)
{
    ST_PORTRCVTXD_TAG *stPortp;
	ST_CONTEXT_TAG *stContextp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

    ST_MISC_CONF_TAG *pstMisc;	

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
	pstMisc = (ST_MISC_CONF_TAG *)&pStMsgFrame->info[2];

	stContextp->stSysConfig.tMisc.xinbei = pstMisc->xinbei;
	stContextp->stSysConfig.tMisc.Mfail_LedNum = pstMisc->Mfail_LedNum;

    //printf("xinbei=%d,Mfail_LedNum=%d\n",pstMisc->xinbei,pstMisc->Mfail_LedNum);

	sem_post(&stContextp->sem_UpdateConfig);	
		
	stPortp->scmd = _0F80H_RET_SETOK;
	stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
	
	return 0;
}



int AF52H_GET_BITMAP_JUNYE_FUN(void *pbuf_context,void *pbuf_port)
{
	int totalbyte;
	unsigned short uploadPacketSize,TotalPacket;
	unsigned char len;
    ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	uploadPacketSize = stContextp->stSysConfig.packetsize_upload;
	if((uploadPacketSize < LOAD_PACKETSIZE_MIN)||(uploadPacketSize > LOAD_PACKETSIZE_MAX))
	{
		uploadPacketSize = 256;
	}
	//(96 * 256 / 4) = 6114
	totalbyte = CMS_PIXEL_W * CMS_PIXEL_H / 4;
	TotalPacket = totalbyte / uploadPacketSize + ((totalbyte  % uploadPacketSize) > 0);

	len =  pStMsgFrame->len[0];
	len <<= 8;
	len |= pStMsgFrame->len[1];

	len -= 10;
	if(len == 2)//只接收52H,中心查询BitMap的FramNo.	
	{        
		pthread_mutex_lock(&stContextp->bitmap_lock);
		CreatCmsBitMap(stContextp);
		pthread_mutex_unlock(&stContextp->bitmap_lock);
		
		stPortp->scmd = AF52H_GET_BITMAP_JUNYE;
        stPortp->stSendInfo.ucbuf[0] = TotalPacket;      
		stPortp->infolen = 1;		
	}
	else if(len == 3)//中心要求读取BitMap数据
	{
		unsigned short frameId;
        ST_BITMAP_UPLOAD_TAG *ST_BITMAP_UPLOAD_p;

		ST_BITMAP_UPLOAD_p = (ST_BITMAP_UPLOAD_TAG *)stPortp->stSendInfo.ucbuf;
		frameId = pStMsgFrame->info[2];

		stPortp->scmd = AF52H_GET_BITMAP_JUNYE;

		if(frameId > TotalPacket)
        {
			stPortp->scmd = AF52H_GET_BITMAP_JUNYE;
			stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
			stPortp->stSendInfo.ST_RET_SETERR.ucErrCode = 0x04;
			stPortp->stSendInfo.ST_RET_SETERR.ucParameterNum = 0x01;
			return 0; 
	    }

		ST_BITMAP_UPLOAD_p->frame_total = TotalPacket;//17;
		ST_BITMAP_UPLOAD_p->frame_num  = frameId;
		ST_BITMAP_UPLOAD_p->widthh  = CMS_PIXEL_W >> 8;
		ST_BITMAP_UPLOAD_p->widthl  = CMS_PIXEL_W;
		ST_BITMAP_UPLOAD_p->heighth = CMS_PIXEL_H >> 8;
		ST_BITMAP_UPLOAD_p->heightl = CMS_PIXEL_H;

		memcpy(ST_BITMAP_UPLOAD_p->pattern_color,&stContextp->CmsMap_t[frameId * uploadPacketSize],uploadPacketSize);
		stPortp->infolen = uploadPacketSize + 6;// 518
	}
	return 0;
}


int A007H_RET_CMS_SHOWD_FUN(void *pbuf_context,void *pbuf_port)
{
	return -1;
}

int A002H_RET_CMS_TICK_FUN(void *pbuf_context,void *pbuf_port)
{
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame=  (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

    if(pStMsgFrame->info[2] < _CMS_CTRCARD_NUM)//读取的是光感值(由于TC硬件设计有误码,故从下层控制板读取)
    {
        if(pStMsgFrame->info[2] < (sizeof(stContextp->CmsCardAdc)/sizeof(unsigned short)))
        {
			stContextp->CmsCardAdc[pStMsgFrame->info[2]] = pStMsgFrame->info[3];		
			stContextp->CmsCardAdc[pStMsgFrame->info[2]] <<= 8;
			stContextp->CmsCardAdc[pStMsgFrame->info[2]] |= pStMsgFrame->info[4];
        }
		stContextp->Sem_CmsTickAck[pStMsgFrame->info[2]] = pStMsgFrame->info[2];
	}
    return -1;
}


int A005H_RET_CMS_ERRMAP_FUN(void *pbuf_context,void *pbuf_port)
{
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;
	ST_CMS_DATABODY_TAG *rcvbody;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame=  (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	rcvbody = (ST_CMS_DATABODY_TAG *)&pStMsgFrame->info[2];

    memcpy(&stContextp->CmsRcvGetDate,rcvbody,sizeof(ST_CMS_DATABODY_TAG));
	sem_post(&stContextp->Sem_CmsRetErrMap); 
	
    return -1;
}

int AE6A_WDT_TEST_FUN(void *pbuf_context,void *pbuf_port)
{
	ST_CONTEXT_TAG *stContextp;
	ST_PORTRCVTXD_TAG *stPortp;

	stContextp = (ST_CONTEXT_TAG *)pbuf_context;
	stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;

    stContextp->wdttest = 1;

	GetHardWareStatus(stContextp);
	memcpy((unsigned char *)&stPortp->stSendInfo.ST_RET_HARDSTATUE,(unsigned char *)&stContextp->stHardState,sizeof(ST_RET_HARDSTATUE_TAG));

	stPortp->scmd = _0FC1H_RET_DEVICE_STATUS;
	stPortp->infolen = sizeof(ST_RET_HARDSTATUE_TAG);
	
	return 0;
}

#if(TC_PANEL_ENB)
//------PANEL
int A009H_GET_PANEL_STATE_FUN(void *pbuf_context,void *pbuf_port)
{
    t_A009H_GET_PANEL_STATE *rcv;
	t_A00AH_RET_PANEL_STATE *retp;
	
	ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_PORTRCVTXD_TAG     *stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	ST_CONTEXT_TAG        *stContextp = (ST_CONTEXT_TAG *)pbuf_context;

	pStMsgFrame=  (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

    rcv = (t_A009H_GET_PANEL_STATE *)&pStMsgFrame->info[2];
	//stContextp->stHardState.powererr = rcv->pwrstate;
	stContextp->stHardState.cabinatedOpened = (rcv->casedoor > 0);
    if(rcv->optmode)//现场操作开关
    {
		stContextp->autotest = 1;
	}
	else
	{
		stContextp->autotest = 0;
	}

    retp = &stPortp->stSendInfo.t_PanelStateRet;
	retp->centererr = (stContextp->PortRemote_p->lineStatue == CONNECT_BREAK);
	retp->localopt = (stContextp->PortLocate_p->lineStatue == CONNECT_BREAK);
	retp->localrun = (stContextp->CmsMode!=0);
	retp->remotectr = (stContextp->PortRemote_p->lineStatue != CONNECT_BREAK);
	retp->deviceerr = (stContextp->CmsCardReady > 0);

	sem_post(&stContextp->Sem_RcvToUpdataCms);//刷新显示 
	
    stPortp->scmd = A00AH_RET_PANEL_STATE;    
	stPortp->infolen = sizeof(t_A00AH_RET_PANEL_STATE);

	return 0;
}

int A00BH_GET_PANEL_IN_FUN(void *pbuf_context,void *pbuf_port)
{
	t_A00BH_GET_PANEL_IN  *rcv;
	ST_FRAME_MSG_TAG      *pStMsgFrame;
	
    ST_PORTRCVTXD_TAG     *stPortp = (ST_PORTRCVTXD_TAG *)pbuf_port;
	ST_CONTEXT_TAG        *stContextp = (ST_CONTEXT_TAG *)pbuf_context;

	pStMsgFrame=  (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

    rcv = (t_A00BH_GET_PANEL_IN  *)&pStMsgFrame->info[2];
 
    if(rcv->signtest)
    {
		stContextp->CmsMode++;
		stContextp->autotest = 0;
		if(stContextp->CmsMode >=4)
		{
			stContextp->CmsMode = 0;
		}

		printf("CmsMode=%d\n",stContextp->CmsMode);
    }		
	else if(rcv->exittest)
	{
		stContextp->CmsMode = 0;
		stContextp->autotest = 0;
	}
	else if(rcv->halfbritest)
	{
		stContextp->stSysConfig.ST_BRIGHTNESS.unBright.ST_BRIGHT_BITMAP.r50 = !stContextp->stSysConfig.ST_BRIGHTNESS.unBright.ST_BRIGHT_BITMAP.r50;

	    if(stContextp->stSysConfig.ST_BRIGHTNESS.unBright.ST_BRIGHT_BITMAP.r50)
		   stContextp->CmsMode = 3;//全亮
	    else
		   stContextp->CmsMode = 0;//退出测试状态
	}	

	sem_post(&stContextp->Sem_RcvToUpdataCms);//刷新显示 

	stPortp->scmd = _0F80H_RET_SETOK;
	stPortp->infolen = sizeof(ST_RET_SETOK_TAG);

	return 0;
}
#endif

int AddImageMsg(ST_SET_MSG_AF32H_TAG *pImageFrame)
{	
	int fp,ret,len,i;
	char *pfile;
	
    pfile = (char *)malloc(32);
	
    if(access("/tc/downmsg",F_OK))
    {
   	   mkdir("/tc/downmsg",0x0777);
       printf("create downmsg.\n");
    }

	memset(pfile,0x00,32);
	sprintf((char *)pfile,"/tc/downmsg/msg%d.temp",pImageFrame->MsgId);
	printf("write Image msg :%s\n",pfile);

//	ret = stat((char *)pfile,&filestat);
//    printf("PackNo:%d\n",pImageFrame->PackNo);

	fp = open((char *)pfile,O_RDWR|O_CREAT,777);
	len = pImageFrame->MsgSizeH << 8 | pImageFrame->MsgSizeL;
    if(pImageFrame->PackNo == 1)
    {
		lseek(fp,0,SEEK_SET);
		i=truncate((char *)pfile,len);
    }
	else
	{
		lseek(fp,0,SEEK_END);
	}
	
	if((ret = write(fp,pImageFrame->MsgBuf,len))<0)
	{
        printf("Err pthread_userImage write.\n");
	}
	//i=truncate((char *)pfile,len);
	
	close(fp);

    if(pImageFrame->PackNo == pImageFrame->TotalPacket)
    {
		char *pfile_e = malloc(64);

		sprintf((char *)pfile_e,"/tc/downmsg/msg%d",pImageFrame->MsgId);
		pthread_mutex_lock(&stgContextp->imagemsg_lock);        
		i = rename(pfile,pfile_e);						
		pthread_mutex_unlock(&stgContextp->imagemsg_lock);
		perror(pfile_e);
        free(pfile_e);
	}


    free(pfile);
	return ret;
}

int DelImageMsg(unsigned char MsgId)
{	
	int ret;
	char *pfile;
	struct stat  filestat;
	
    pfile = (char *)malloc(32);
    
	memset(pfile,0x00,32);
	sprintf((char *)pfile,"/tc/downmsg/msg%d",MsgId);
	ret = stat((char *)pfile,&filestat);
	if(filestat.st_size)
	{
        remove(pfile);
		printf("Del msg file OK: %s.\n",pfile);
	}
    else
    {
		printf("Del msg file Err: %s.\n",pfile);
	}

    free(pfile);
	return ret;
}

int GetImageMsg(ST_GET_MSG_AF62H_TAG *pST_GET_MSG,ST_SET_MSG_AFE2H_TAG *dbuf,unsigned short maxlen)
{	
	int fp,ret,len;
	int seek;
	unsigned  char *pfile;
	struct stat  filestat;

    pfile = (unsigned char *)malloc(32);	
	
	memset(pfile,0x00,32);
	sprintf((char *)pfile,"/tc/downmsg/msg%d",pST_GET_MSG->MsgId);

    if(access((char *)pfile,F_OK))
    {
		printf("Err Image msg: file not found.%s\n",pfile);
		return -1;
	}	
	printf("read msg Image :%s\n",pfile);
	
	fp = open((char *)pfile,O_RDWR|O_CREAT,777);
	maxlen = ((maxlen < LOAD_PACKETSIZE_MIN)||(maxlen > LOAD_PACKETSIZE_MAX)) ? LOAD_PACKETSIZE_DEF : maxlen;

    printf("total:=%d,no=%d\n",pST_GET_MSG->TotalPacket,pST_GET_MSG->PackNo);//debug
	
	ret = stat((char *)pfile,&filestat);
    if((!pST_GET_MSG->PackNo)&&(!pST_GET_MSG->TotalPacket))
    {
        dbuf->MsgId = pST_GET_MSG->MsgId;
		//dbuf->TotalPacket = (filestat.st_size / sizeof(dbuf->MsgBuf)) + ((filestat.st_size % sizeof(dbuf->MsgBuf)) > 0);
		dbuf->TotalPacket = (filestat.st_size / maxlen) + ((filestat.st_size % maxlen) > 0);
		dbuf->PackNo = 1;
		
		lseek(fp,0,SEEK_SET);		
		//len = read(fp,dbuf->MsgBuf,sizeof(dbuf->MsgBuf));
		len = read(fp,dbuf->MsgBuf,maxlen);
	}
	else
	{
        dbuf->MsgId = pST_GET_MSG->MsgId;
		dbuf->TotalPacket = pST_GET_MSG->TotalPacket;
		dbuf->PackNo = pST_GET_MSG->PackNo;
		
        //seek = (dbuf->PackNo-1) * sizeof(dbuf->MsgBuf);
        seek = (dbuf->PackNo-1) * maxlen;
		if(seek >= filestat.st_size)
			len = 0;
		else
		{
			lseek(fp,seek,SEEK_SET);		
			//len = read(fp,dbuf->MsgBuf,sizeof(dbuf->MsgBuf));
			len = read(fp,dbuf->MsgBuf,maxlen);
		}
	}
	dbuf->MsgSizeH = len >> 8;
	dbuf->MsgSizeL = len;
    		
	close(fp);
	
	free(pfile);
	return 0;
}

/*
int AF51H_SET_RGIMAGE_FUN(void *pbuf_context,void *pbuf_port)
{
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;

	t_AF51H_SET_RGIMAGE *AF51H_SET_RGIMAGE_p;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	AF51H_SET_RGIMAGE_p = (t_AF51H_SET_RGIMAGE *)&pStMsgFrame->info[2];

	stPortp->scmd = _0F80H_RET_SETOK;
	stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
	return 0;
}
*/
int AF32H_SET_PRESET_MSG_FUN(void *pbuf_context,void *pbuf_port)//预设信息设定
{
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;

	ST_SET_MSG_AF32H_TAG *pST_SET_MSG_AF32H;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame=  (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	pST_SET_MSG_AF32H = (ST_SET_MSG_AF32H_TAG *)&pStMsgFrame->info[2];
	if((pST_SET_MSG_AF32H->MsgId < 128)||(!pST_SET_MSG_AF32H->MsgType))
	{
		stPortp->scmd = _0F81H_RET_SETERR;
		stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
		return 0;
	}

	if(!pST_SET_MSG_AF32H->ActionType)
	{
		//DelPresetText(pST_SET_MSG_AF32H->MsgId);
		AddImageMsg(pST_SET_MSG_AF32H);
	}	
	else
	{
		DelImageMsg(pST_SET_MSG_AF32H->MsgId);
	}

	stPortp->scmd = _0F80H_RET_SETOK;
	stPortp->infolen = sizeof(ST_RET_SETOK_TAG);

	return 0;
}

int AF62H_GET_PRESET_MSG_FUN(void *pbuf_context,void *pbuf_port)//预设信息查询
{
	int ret;
	
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame=  (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	ST_SET_MSG_AFE2H_TAG *st_msgdestp = (ST_SET_MSG_AFE2H_TAG *)&stPortp->stSendInfo.ucbuf[0];

	memset(st_msgdestp,0x00,sizeof(ST_SET_MSG_AFE2H_TAG));
	ret = GetImageMsg((ST_GET_MSG_AF62H_TAG *)&pStMsgFrame->info[2],st_msgdestp,stContextp->stSysConfig.packetsize_upload);   
	if(ret)
	{
		stPortp->stSendInfo.ST_RET_SETERR.cmd_ml[0]=AF62H_GET_PRESET_MSG >> 8;
		stPortp->stSendInfo.ST_RET_SETERR.cmd_ml[0]=AF62H_GET_PRESET_MSG & 0xff;

		stPortp->scmd = _0F81H_RET_SETERR;
		stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
	}
	else
	{
		stPortp->scmd = AFE2H_RET_PRESET_MSG;
		stPortp->infolen = st_msgdestp->MsgSizeH << 8 | st_msgdestp->MsgSizeL;
		stPortp->infolen += 6;
	}
	return 0;
}

int AF33H_SET_CURR_MSG_FUN(void *pbuf_context,void *pbuf_port)//选择预设信息
{
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;

	char *pfile;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame=  (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
	
	pfile = malloc(64);
	sprintf((char *)pfile,"/tc/downmsg/msg%d",pStMsgFrame->info[2]);	
	
	if(access((char *)pfile,F_OK))
	{
		stPortp->scmd = _0F81H_RET_SETERR;
		stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
	}
	else
	{
		stContextp->stSysConfig.ucCurrShow_MsgId = pStMsgFrame->info[2];
		stContextp->showmode = showmode_preset;

		
		stContextp->stSysConfig.ViewMode = VIEW_NORMAL;
		memset(stContextp->zonedisId,0x00,sizeof(stContextp->zonedisId));
		memset(stContextp->zonetime,0x00,sizeof(stContextp->zonetime));
		sem_post(&stContextp->Sem_RcvToUpdataCms);//刷新显示 
		stContextp->disUpdateTimeUnit = DISPLAY_UPDATETIME_DEFAULT;
		sem_post(&stContextp->sem_UpdateConfig);	

		stPortp->scmd = _0F80H_RET_SETOK;
		stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
	}
	
	free(pfile);
	return 0;
}

int AF63H_GET_CURR_MSG_FUN(void *pbuf_context,void *pbuf_port)//查询当前信息
{
	int ret,seek;
	int fp;
	unsigned  char *pfile;
	struct stat  filestat;
	int packetsize;
		
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;
	ST_SET_MSG_AFE3H_TAG *st_msgdestp;	  
	ST_GET_MSG_AF62H_TAG *st_msgsrcp;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame=  (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	st_msgdestp = (ST_SET_MSG_AFE3H_TAG *)&stPortp->stSendInfo.ucbuf[0];	
	st_msgsrcp = (ST_GET_MSG_AF62H_TAG *)&pStMsgFrame->info[2];

	if(!st_msgsrcp->PackNo)
	{
		CreatCmsBitMap(stContextp);
	}
	pfile = (unsigned char *)malloc(32);	
	memset(pfile,0x00,32);
	sprintf((char *)pfile,"/tc/cmsmap.bmp");

	fp = open((char *)pfile,O_RDONLY,777);
	ret = stat((char *)pfile,&filestat);
	packetsize = stContextp->stSysConfig.packetsize_upload;
	if((packetsize < LOAD_PACKETSIZE_MIN)||(packetsize > LOAD_PACKETSIZE_MAX))
	{
		packetsize = LOAD_PACKETSIZE_DEF;
	}
	if(!ret)
	{
		st_msgdestp->MsgId = (stContextp->stSysConfig.ViewMode == VIEW_NORMAL)? stContextp->stSysConfig.ucCurrShow_MsgId : 255;
		st_msgdestp->TotalPacket = filestat.st_size / packetsize;
		if(filestat.st_size % packetsize)
			st_msgdestp->TotalPacket++;

		st_msgdestp->PackNo = st_msgsrcp->PackNo;
		if(!st_msgdestp->PackNo)
			st_msgdestp->PackNo++;

		seek = (st_msgdestp->PackNo - 1) * packetsize;
		lseek(fp,seek,SEEK_SET);		
		packetsize = read(fp,st_msgdestp->MsgBuf,packetsize);
		st_msgdestp->MsgSizeH = packetsize >> 8;
		st_msgdestp->MsgSizeL = packetsize;
		stPortp->scmd = AFE3H_RET_CURR_MSG;
		//stPortp->infolen = sizeof(ST_SET_MSG_AFE3H_TAG);
		stPortp->infolen = packetsize + 5;
	}
	else
	{
		stPortp->scmd = AFE3H_RET_CURR_MSG;
		stPortp->infolen = 0;
	}
	
	close(fp);	
	free(pfile);

	return 0;
}


int AF1AH_SET_INSERT_FUN(void * pbuf_context, void * pbuf_port)
{	
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame=  (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

    unsigned char *psrc;
	unsigned short size;

	psrc = &pStMsgFrame->info[2];
	size = pStMsgFrame->len[0];
	size <<= 8;
	size |= pStMsgFrame->len[1];

	size -= 12;// 連同指令AF1A減掉
    AddPresetInsertContext(psrc,size);

    stContextp->showmode = showmode_insert;

	stPortp->scmd = _0F80H_RET_SETOK;
	stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
	sem_post(&stContextp->Sem_RcvToUpdataCms);//刷新显示 

	return 0;
}

int AF4AH_GET_INSERT_FUN(void * pbuf_context, void * pbuf_port)
{
	unsigned char *pdest;
	unsigned char *pbuf = malloc(4096);
    unsigned short size;

	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame=  (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
  
    pdest = &stPortp->stSendInfo.ucbuf[0];
	pdest[0] = pStMsgFrame->info[2];
	size = GetPresetInsertContext(pdest);

    if(size == 0)
    {
		pdest[0] = pStMsgFrame->info[2];
        pdest[1] = 0;
		pdest[2] = 0;
		pdest[3] = 0;
		size = 4;
	}

	stPortp->scmd = AFCAH_RET_INSERT;	
	stPortp->infolen = size;	
	
    free(pbuf);
	
	return 0;
}


#if (PARK_CMS_ENB)
int AF19H_SET_PARK_CONTEXT_FUN(void *pbuf_context,void *pbuf_port)
{
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;
    t_PARK_CONTEXT_TEXT *pPARK_CONTEXT_TEXT;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame=  (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	pPARK_CONTEXT_TEXT = (t_PARK_CONTEXT_TEXT *)&pStMsgFrame->info[2];

    memcpy(&stContextp->PARK_CONTEXT,pPARK_CONTEXT_TEXT,sizeof(t_PARK_CONTEXT_TEXT));

	stPortp->scmd = _0F80H_RET_SETOK;
	stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
    return 0;      
}

int AF43H_GET_CURR_PARK_CONTEXT_FUN(void *pbuf_context,void *pbuf_port)
{
	ST_PORTRCVTXD_TAG *stPortp;
//	ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;
    t_PARK_CONTEXT_TEXT *pPARK_CONTEXT_TEXT;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;

	pPARK_CONTEXT_TEXT = &stPortp->stSendInfo.PARK_CONTEXT_TEXT;
	memcpy(pPARK_CONTEXT_TEXT,&stContextp->PARK_CONTEXT,sizeof(t_PARK_CONTEXT_TEXT));

	stPortp->scmd = AFC9H_RET_PARK_CONTEXT;
	stPortp->infolen = pPARK_CONTEXT_TEXT->parkcount * sizeof(t_ParkInfo) + 5;
    return 0;
}

int AF44H_GET_CURR_PARK_TEXT_COLOR_FUN(void *pbuf_context,void *pbuf_port)
{
    int len;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_CONTEXT_TAG *stContextp;
	t_PARK_CONTEXT_rgColor *pContextRgColor;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;

	pContextRgColor = &stPortp->stSendInfo.PARK_CONTEXT_rgColor;
	memcpy(&pContextRgColor->mode,&stContextp->PARK_CONTEXT.mode,5);

	len = stContextp->PARK_CONTEXT.parkcount * sizeof(t_ParkInfo_rgColor);
    memcpy(pContextRgColor->color,stContextp->PARK_CONTEXT.color,len);
	len += 5;

	stPortp->scmd = AFE2H_RET_PARK_CONTEXT_RGCOLOR;
	stPortp->infolen = len;
	return 0;
}

int AF32H_SET_PARK_CONTEXT_RGCOLOR_FUN(void *pbuf_context,void *pbuf_port)
{
	int len;
	
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;
	t_PARK_CONTEXT_rgColor *pPARK_CONTEXT_TEXT_cL;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame=  (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	pPARK_CONTEXT_TEXT_cL = (t_PARK_CONTEXT_TEXT *)&pStMsgFrame->info[2];

	len = pPARK_CONTEXT_TEXT_cL->parkcount * sizeof(t_ParkInfo_rgColor);
	memcpy(stContextp->PARK_CONTEXT.color,pPARK_CONTEXT_TEXT_cL->color,len);
#if (PARK_CMS_ENB)
	stContextp->showpark = 1;
#endif
	sem_post(&stContextp->Sem_RcvToUpdataCms);//刷新显示 

	stPortp->scmd = _0F80H_RET_SETOK;
	stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
	return 0;	   
}



int AF23H_ADD_PARK_INFO_FUN(void *pbuf_context,void *pbuf_port)
{
    unsigned int len,temp;

	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;

    char *filep = malloc(64);

	unsigned char *rcv;

//	t_PARK_CONTEXT_rgColor *PARK_CONTEXT_rgColor;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
	
	if(access("/tc/park",F_OK))
	{
	   mkdir("/tc/park",0x0777);
	}
    rcv = &pStMsgFrame->info[2];
    memset(filep,0x00,64);
	temp = rcv[0];
	temp <<= 8;
    temp |=rcv[1];	
    sprintf(filep,"/tc/park/cn%x",temp);
    savefile(filep,(char *)&rcv[0],rcv[2]*2+3);

    len = rcv[2] * 2 + 3;
    rcv = rcv + len;
    memset(filep,0x00,64);
	temp = rcv[0];
	temp <<= 8;
    temp |=rcv[1];	
    sprintf(filep,"/tc/park/en%x",temp);
    savefile(filep,(char *)&rcv[0],rcv[2]*2+3);

    free(filep);
	
	stPortp->scmd = _0F80H_RET_SETOK;
	stPortp->infolen = sizeof(ST_RET_SETOK_TAG);

	return 0;
}

int AF53H_GET_PARK_INFO_FUN(void *pbuf_context,void *pbuf_port)
{
	int fp;
	unsigned int len,temp;

	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;

	char *filep = malloc(64);

	t_PARK_NAME_INQ *rcv;
    unsigned char *psend;

//	t_PARK_CONTEXT_rgColor *PARK_CONTEXT_rgColor;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	
	rcv = (t_PARK_NAME_INQ *)&pStMsgFrame->info[2];

	memset(filep,0x00,64);
	temp = rcv->CN_Id[0];
	temp <<= 8;
    temp |=rcv->CN_Id[1];	
	sprintf(filep,"/tc/park/cn%x",temp);
	fp = open(filep,O_RDONLY,777);
    if(fp <=0)
    {
		stPortp->scmd = _0F81H_RET_SETERR;
		stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
		return 0;
	}
	else
	{
        psend = stPortp->stSendInfo.ucbuf;
		lseek(fp,0,SEEK_SET);
        len = read(fp,psend,65);
		len = psend[2]*2 + 3;
		psend = psend + len;
	}
	close(fp);

    temp = rcv->EN_Id[0];
	temp <<= 8;
    temp |=rcv->EN_Id[1];		
	memset(filep,0x00,64);
	sprintf(filep,"/tc/park/en%x",temp);
	fp = open(filep,O_RDONLY,777);
	if(fp <=0)
	{
		stPortp->scmd = _0F81H_RET_SETERR;
		stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
		return 0;
	}
	else
	{
		lseek(fp,0,SEEK_SET);
		temp = read(fp,psend,65);
		len += psend[2]*2 + 3;
		
	}
	close(fp);

	stPortp->scmd = AFD3H_RET_PARK_INFO;
	stPortp->infolen = len;
	
	return 0;
}

int AF24H_DEL_PARK_INFO_FUN(void *pbuf_context,void *pbuf_port)
{
	int fp;
	unsigned int temp;

	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;

	char *filep = malloc(64);

	t_PARK_NAME_INQ *rcv;
//	unsigned char *psend;

//	t_PARK_CONTEXT_rgColor *PARK_CONTEXT_rgColor;

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	rcv = (t_PARK_NAME_INQ *)&pStMsgFrame->info[2];

	temp = rcv->CN_Id[0];
	temp <<= 8;
	temp |=rcv->CN_Id[1];	
	memset(filep,0x00,64);
	sprintf(filep,"/tc/park/cn%x",temp);
	fp = remove(filep);
	if(fp)
	{
		stPortp->scmd = _0F81H_RET_SETERR;
		stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
		return 0;
	}
	
	temp = rcv->EN_Id[0];
	temp <<= 8;
	temp |=rcv->EN_Id[1];		
	memset(filep,0x00,64);
	sprintf(filep,"/tc/park/en%x",temp);
	fp = remove(filep);
	if(fp)
	{
		stPortp->scmd = _0F81H_RET_SETERR;
		stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
		return 0;
	}
	
	stPortp->scmd = _0F80H_RET_SETOK;
	stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
	return 0;
}


int AF55H_GET_PARK_INFO_ERR_FUN(void *pbuf_context,void *pbuf_port)
{
	int i;
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG  *pStMsgFrame;
	ST_CONTEXT_TAG    *stContextp;
	t_PARK_CONTEXT    *PARK_CONTEXT;
	t_PARK_ERRORCODE_u   ErrorCode;
	unsigned  char *pfile;

	stContextp  =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp     =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame =  (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	PARK_CONTEXT = &stContextp->PARK_CONTEXT;
	ErrorCode.byte = 0;
    if(PARK_CONTEXT->mode >1)
    {
        ErrorCode.strc.mode = 1;		
	}
	

    pfile = (unsigned char *)malloc(32);	

	memset(pfile,0x00,32);
	sprintf((char *)pfile,"/tc/downtext/text%d",PARK_CONTEXT->textid_msg);
    if(access((char *)pfile,F_OK))
    {
		ErrorCode.strc.textid = 1;
	}
	memset(pfile,0x00,32);
	sprintf((char *)pfile,"/tc/downtext/text%d",PARK_CONTEXT->textid_park);
    if(access((char *)pfile,F_OK))
    {
		ErrorCode.strc.textid = 1;
	}

    for(i=0;i<PARK_CONTEXT->parkcount;i++)
    {
		memset(pfile,0x00,32);
		sprintf(pfile,"/tc/park/icon%d",PARK_CONTEXT->text[i].Icon);
		if(access(pfile,F_OK))//文件不存在
		{	
			ErrorCode.strc.parkGraphId = 1;
			break;
		}
    }
	
	for(i=0;i<PARK_CONTEXT->parkcount;i++)
	{
		memset(pfile,0x00,32);
		sprintf(pfile,"/tc/park/cn%d",PARK_CONTEXT->text[i].CN);
		if(access(pfile,F_OK))//文件不存在
		{	
			ErrorCode.strc.parkChId = 1;
			break;
		}
	}

	for(i=0;i<PARK_CONTEXT->parkcount;i++)
	{
		memset(pfile,0x00,32);
		sprintf(pfile,"/tc/park/en%d",PARK_CONTEXT->text[i].EN);
		if(access(pfile,F_OK))//文件不存在
		{	
			ErrorCode.strc.parkEnId = 1;
			break;
		}
	}
	for(i=0;i<PARK_CONTEXT->parkcount;i++)
	{
		memset(pfile,0x00,32);
		sprintf(pfile,"/tc/park/arrow%d",PARK_CONTEXT->text[i].arrow);
		if(access(pfile,F_OK))//文件不存在
		{	
			ErrorCode.strc.parkArrowId = 1;
			break;
		}
	}

    if(PARK_CONTEXT->parkcount == 0)
    {
		ErrorCode.strc.parkCount = 1;
	}
		
	free(pfile);

	stPortp->stSendInfo.ucbuf[0] = 1;
	stPortp->stSendInfo.ucbuf[1] = ErrorCode.byte;

	stPortp->scmd = AFE5H_REG_PARK_INFO_ERR;
	stPortp->infolen = 2;

    return 0;
}

int AF62H_SET_PARK_ICON_FUN(void *pbuf_context,void *pbuf_port)
{
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;

	t_PARK_GRAPH *PARK_GRAPH_p;
	char *filep = malloc(64);

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	PARK_GRAPH_p = (t_PARK_GRAPH *)&pStMsgFrame->info[2];

	memset(filep,0x00,64);
    sprintf(filep,"/tc/park/icon%d",PARK_GRAPH_p->Num);
	if(access("/tc/park",F_OK))
	{
	   mkdir("/tc/park",0x0777);
	}
	
	savefile(filep, (char *)PARK_GRAPH_p, sizeof(t_PARK_GRAPH));
   
    free(filep);

	stPortp->scmd = _0F80H_RET_SETOK;
	stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
	
	return 0;
}

int AF72H_GET_PARK_ICON_FUN(void *pbuf_context,void *pbuf_port)
{
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;
    int fp,len = 0;

	t_PARK_GRAPH *PARK_GRAPH_p;
	unsigned char *rcv;
	char *filep = malloc(64);

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	PARK_GRAPH_p = &stPortp->stSendInfo.PARK_GRAPH;
	rcv = &pStMsgFrame->info[2];

    memset(PARK_GRAPH_p,0x00,sizeof(t_PARK_GRAPH));
	memset(filep,0x00,64);
	sprintf(filep,"/tc/park/icon%d",rcv[1]);
    fp = open(filep,O_RDONLY,777);	
	if(fp >0)
	{
		lseek(fp,0,SEEK_SET);
		len = read(fp,(char *)PARK_GRAPH_p,sizeof(t_PARK_GRAPH));
	}	
    close(fp);
	free(filep);

	len = (PARK_GRAPH_p->Mode)? 130 : 34;
	stPortp->scmd = AFE6H_RET_PARK_ICON;
	stPortp->infolen = len;

	return 0;
}

int AF63H_SET_PARK_INDICATE_FUN(void *pbuf_context,void *pbuf_port)
{
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;

	t_PARK_GRAPH *PARK_GRAPH_p;
	char *filep = malloc(64);

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	PARK_GRAPH_p = (t_PARK_GRAPH *)&pStMsgFrame->info[2];

	memset(filep,0x00,64);
	sprintf(filep,"/tc/park/arrow%d",PARK_GRAPH_p->Num);
	if(access("/tc/park",F_OK))
	{
	   mkdir("/tc/park",0x0777);
	}
	savefile(filep, (char *)PARK_GRAPH_p, sizeof(t_PARK_GRAPH));
   
	free(filep);

	stPortp->scmd = _0F80H_RET_SETOK;
	stPortp->infolen = sizeof(ST_RET_SETOK_TAG);
	
	return 0;
}

int AF73H_GET_PARK_INDICATE_FUN(void *pbuf_context,void *pbuf_port)
{
	ST_PORTRCVTXD_TAG *stPortp;
	ST_FRAME_MSG_TAG *pStMsgFrame;
	ST_CONTEXT_TAG *stContextp;
	int fp,len = 0;

	t_PARK_GRAPH *PARK_GRAPH_p;
	unsigned char *rcv;
	char *filep = malloc(64);

	stContextp =  (ST_CONTEXT_TAG *)pbuf_context;
	stPortp    =  (ST_PORTRCVTXD_TAG *)pbuf_port;
	pStMsgFrame = (ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;

	PARK_GRAPH_p = &stPortp->stSendInfo.PARK_GRAPH;
	rcv = &pStMsgFrame->info[2];

	memset(PARK_GRAPH_p,0x00,sizeof(t_PARK_GRAPH));
	memset(filep,0x00,64);
	sprintf(filep,"/tc/park/arrow%d",rcv[1]);
	fp = open(filep,O_RDONLY,777); 
	if(fp >0)
	{
		lseek(fp,0,SEEK_SET);
		len = read(fp,(char *)PARK_GRAPH_p,sizeof(t_PARK_GRAPH));
	}	
	close(fp);
	free(filep);

	len = (PARK_GRAPH_p->Mode)? 130 : 34;
	stPortp->scmd = AFE6H_RET_PARK_ICON;
	stPortp->infolen = len;

	return 0;
}

#endif

static int AddInfoToSendList(ST_PORTRCVTXD_TAG *stPortp,unsigned char type)
{
	unsigned char i;
	int ret,len;
	unsigned char *bufp;	
	ST_CONTEXT_TAG *stContextp = stgContextp;
	
    bufp = malloc(sizeof(ST_INFO_CONTEXT_TAG));
	unsigned char seq;
	
	ret = -1;
	
	pthread_mutex_lock(&stPortp->stSendListLock);
    for(i = 0;i<__SEND_LIST_MAX;i++)
    {
        if(stPortp->stSendList[i].statue == __IDLE)
        {
			unsigned char *address;
			
            memset(&stPortp->stSendList[i],0x00,sizeof(ST_SEND_LIST_TAG));
			address = stContextp->stSysConfig.address;
			if(type == _SEND_INFO)
			{
				seq = ++stContextp->sendseq;
				if(stPortp->scmd)
				{
					len = stPortp->infolen + 2;
					bufp[0] = stPortp->scmd >> 8;
					bufp[1] = stPortp->scmd;
					memcpy(&bufp[2],stPortp->stSendInfo.ucbuf,stPortp->infolen);
				} 
				else
				{
					len = stPortp->infolen;
					memcpy(&bufp[0],stPortp->stSendInfo.ucbuf,stPortp->infolen);
				}
				stPortp->stSendList[i].type = _SEND_INFO;
				stPortp->stSendList[i].infoLen = GetSendInfoPacket((unsigned char *)&stPortp->stSendList[i].infoBody,seq,address, bufp,len,stPortp);							
				stPortp->stSendList[i].statue = __WAIT_SEND;
			}
            else if(type == _SEND_ACK)//ack
            {
				seq = stPortp->stSendInfo.ucbuf[0]; // seq

				stPortp->stSendList[i].type = _SEND_ACK;
				stPortp->stSendList[i].infoLen = GetSendAckPacket((unsigned char *)&stPortp->stSendList[i].infoBody,seq,address);							
				stPortp->stSendList[i].statue = __WAIT_SEND;	
			}
            else if(type == _SEND_NAK)//nak
            {
				seq = stPortp->stSendInfo.ucbuf[0]; // seq
				bufp[0]= stPortp->stSendInfo.ucbuf[1]; // err

				stPortp->stSendList[i].type = _SEND_NAK;
				stPortp->stSendList[i].infoLen = GetSendNakPacket((unsigned char *)&stPortp->stSendList[i].infoBody,seq,bufp[0],address);							
				stPortp->stSendList[i].statue = __WAIT_SEND;				            
			}
			ret = 0;

			break;
		}
	}
	pthread_mutex_unlock(&stPortp->stSendListLock);
	return ret;
}

static void DecInfoToSendList(ST_PORTRCVTXD_TAG *stPortp)
{
	pthread_mutex_lock(&stPortp->stSendListLock);
    memcpy(&stPortp->stSendList[0],&stPortp->stSendList[1],sizeof(ST_SEND_LIST_TAG)*(__SEND_LIST_MAX - 1));
	memset(&stPortp->stSendList[__SEND_LIST_MAX-1],0x00,sizeof(ST_SEND_LIST_TAG));	
	pthread_mutex_unlock(&stPortp->stSendListLock);
}

static void pthread_SendToPort(ST_PORTRCVTXD_TAG *stPortp)
{
	unsigned long timeout_count=0;
    unsigned char wait_ack_time;
	struct timespec ts; 		  
	ST_CONTEXT_TAG *stContextp = stgContextp;
	sigset_t setsig;
	
	sigemptyset(&setsig);
	sigaddset(&setsig, SIGALRM);
	sigprocmask(SIG_BLOCK,&setsig,NULL);//屏蔽信号

    while(1)
    {
		Wdg_Check(stContextp,1);
		usleep(1000); // 1ms
		sem_trywait(&stPortp->Sem_RcvToSendPth); 
        if(stPortp->stSend.len)
		    continue;

		if(stPortp->stSendList[0].statue == __IDLE)
			continue;

		//if(stPortp == &stContextp->PortCms)
			//stContextp->countTickComm = 0;
		
		if(stPortp->stSendList[0].reSendCount >= 2)//端口发送超时
		{
            DecInfoToSendList(stPortp);
			//stPortp->lineStatue = CONNECT_BREAK;//重发三次无回应就是断线						
		}
		else
		{
			memcpy(stPortp->stSend.buf,&stPortp->stSendList[0].infoBody,stPortp->stSendList[0].infoLen);
			stPortp->stSendList[0].reSendCount++;
			stPortp->stSend.len = stPortp->stSendList[0].infoLen;
			wait_ack_time = 2 + (stPortp->stSend.len / 256);
			wait_ack_time = (wait_ack_time > 5) ? 5 : wait_ack_time;
			if(stPortp->stSendList[0].type == _SEND_INFO)
			{
				stPortp->stSendList[0].statue = __WAIT_ACK;
				timeout_count = 0;
#if(TC_PANEL_ENB)
                if(stPortp == &stContextp->PortCenter_Com4)
                {
					DecInfoToSendList(stPortp);
				}
				else
					
#endif
				{
				 while(1)
				 {
					ts.tv_sec =time(NULL)+ wait_ack_time;//2;	
					if(!sem_timedwait(&stPortp->Sem_RcvToSendPth,&ts)) 
					{
						if(stPortp->stSendList[0].infoBody.seq == stPortp->seq_rcv)// OK	
						{							
							DecInfoToSendList(stPortp);
							break;
						}
						else//dummy
						{
							printf("dummy:%s,Txd %x,Rcv %x \n",stPortp->portname,stPortp->stSendList[0].infoBody.seq,stPortp->seq_rcv);                            
							timeout_count++;
							if(timeout_count >= 20)
							{
                                timeout_count = 0;
							//	saveRebootInfo(5, "Rcv Ack Date Err And Time Out");
								break;
							}
						}
					}
					else //超时
					{
						//printf("wait rcv err %s",stPortp->portname);
						//perror("sem_timedwait");
						break;
					}
				 }
				}				
			}	
			else
			{
				DecInfoToSendList(stPortp);
			}				
		}
	}
}

#if 1
static void pthread_RcvFromPort(ST_PORTRCVTXD_TAG *stPortp)
{
	ST_CONTEXT_TAG *stContextp;
	int flag;
    int ret,i;
    int isBrokentoConnect;//从断线转换到连线
    int sendPowerbrokenMsg;
	ST_TXRX_TAG *stRcv_p;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stRcv_p	= (ST_TXRX_TAG *)malloc(sizeof(ST_TXRX_TAG));

    if(stRcv_p==NULL)
    {
		printf("Not enough memorey\n");
        return;
	}

	stContextp = stgContextp;

	sem_init(&stPortp->Sem_RcvBuffNotEmpt,0,0);
	sem_init(&stPortp->Sem_RcvToSendPth,0,0);

	isBrokentoConnect = 0;
	stPortp->count_breakline = 0xfffffff0;
	stPortp->lineStatue = CONNECT_BREAK;//CONNECT_ONLINE;
    ret = 0;
    sendPowerbrokenMsg = 0;
	while(1)
	{		
		usleep(1000); // 1ms
		Wdg_Check(stContextp,1);

//---------------------端口状态处理------------------------------
		if(stPortp->count_breakline < 0xfffffff0)
			stPortp->count_breakline++;
		if(stPortp->count_offline < 0xfffffff0)			
			stPortp->count_offline++;		 

        /*
        if(!stPortp->portLock)
        {
			isBrokentoConnect =((stPortp->lineStatue != CONNECT_OFFLINE)&&(stPortp->lineStatue != CONNECT_ONLINE))? 1 : isBrokentoConnect;
			if((stPortp->count_offline >= 130000)&&(stPortp->lineStatue != CONNECT_BREAK)) // 3min	        
				stPortp->lineStatue = CONNECT_OFFLINE;
        }
		else
		{	
			stPortp->count_breakline = 0xfffffff0;
			stPortp->lineStatue = CONNECT_LOCK;
			stPortp->stRcvAnalytice.len = 0;//Clean rcv buf			
		}*/		

        if(stPortp->portLock)
        {
			stPortp->lineStatue = CONNECT_LOCK;
			//stPortp->stRcvAnalytice.len = 0;//Clean rcv buf	
			stPortp->count_breakline = 0;
			stPortp->count_offline = 0;
		}
        else 
        {            
			isBrokentoConnect =((stPortp->lineStatue != CONNECT_OFFLINE)&&(stPortp->lineStatue != CONNECT_ONLINE))? 1 : isBrokentoConnect;
            /*
			if(stPortp->count_breakline >= 130000)
            {
				stPortp->lineStatue = CONNECT_BREAK;
			}
            else 
			*/
			if(stPortp->count_offline >= 160000)
            {
				if((stPortp == &stContextp->PortCenter_Eth)&&(stContextp->eth0_isdown != 0))
				{
					stPortp->lineStatue = CONNECT_BREAK;
				}
				else
				{
					stPortp->lineStatue = CONNECT_OFFLINE;
				}
			}
		}
//-------------------------------------------------------------		
        if(sem_trywait(&stPortp->Sem_RcvBuffNotEmpt))//没有消息到来时再发送			
		{	
	        if(sendPowerbrokenMsg)
	        {
#if (TC_PANEL_ENB)
                if(stPortp != &stContextp->PortCenter_Com4)
#endif
				{
					printf("reconnect port: %s\n",stPortp->portname);
	                sendPowerbrokenMsg = 0;
				
				    pthread_mutex_lock(&stPortp->stSendInfoLock);
					stPortp->scmd	  = _0F00H_RET_POWERBREAK;
					memcpy(&stPortp->stSendInfo.ST_RET_POWERBREAK,&stContextp->ST_GET_POWERBROKKEN,sizeof(ST_RET_POWERBREAK_TAG));
					stPortp->infolen = sizeof(ST_RET_POWERBREAK_TAG);
					AddInfoToSendList(stPortp,_SEND_INFO);
					pthread_mutex_unlock(&stPortp->stSendInfoLock);
				}				
	        }
			continue;
        }
		pthread_mutex_lock(&stPortp->stMutexRcv);//debug

		if(stPortp->portLock)
		{
		   //stPortp->lineStatue = CONNECT_LOCK;
		   if(stPortp->stRcvAnalytice.len)
		   {
               printf("%s PortLock = %d\n",stPortp->portname,stPortp->portLock);
		   }
       	   stPortp->stRcvAnalytice.len = 0;//Clean rcv buf
       	   
		}


		pStMsgFrame =(ST_FRAME_MSG_TAG *)stPortp->stRcvAnalytice.buf;
		memcpy(stRcv_p,&stPortp->stRcvAnalytice,sizeof(ST_TXRX_TAG));	
		while(stRcv_p->len)
		{
			ret = checkProtocol(stPortp,stRcv_p);
			
            if(ret>= 0)
            {
                stPortp->rcvmsgtotal++;
			}
			
			pthread_mutex_lock(&stPortp->stSendInfoLock);
			if(!ret)
			{
			  stPortp->count_offline = 0;//接收指令下发,离线计时清除
			  stPortp->stSendInfo.ucbuf[0] = pStMsgFrame->seq;
			  
			  if( (stPortp == &stContextp->PortCms)//下层通信不需ACK交互
#if (TC_PANEL_ENB)
                ||(stPortp == &stContextp->PortCenter_Com4)
#endif
                )
              {

              }
              else
			  {
				  i=AddInfoToSendList(stPortp,_SEND_ACK);
			  }			  
			  flag = -1;
			  for(i=0;i<sizeof(stProtoCmdFun)/sizeof(ST_CMDFUN_TAG);i++)
			  {
				  if( (pStMsgFrame->info[0] != (stProtoCmdFun[i].Cmd >>8))
					||(pStMsgFrame->info[1] != (stProtoCmdFun[i].Cmd & 0xff)))
				  {
				  	continue;
				  }
				  
                  flag = 0;
								  
				  usleep(10*1000); // 10ms //两次发送消息间隔太短会收不到
				  stPortp->seq_rcv = pStMsgFrame->seq; //debug
				  sem_post(&stPortp->Sem_RcvToSendPth); 
				  
                  memcpy(stPortp->stSendInfo.ST_RET_SETOK.cmd_ml,&pStMsgFrame->info[0],2);
				  if(!stProtoCmdFun[i].fun(stContextp,stPortp))
				  {				
					  AddInfoToSendList(stPortp,_SEND_INFO);
				  }

                  if((stContextp->PortLocate_p == stPortp)||(stContextp->PortRemote_p == stPortp))
                  {				  	
				  	  if(stContextp->autotest)
				  	  {
						  stContextp->CmsMode = 0;
						  stContextp->autotest = 0;   
				  	  }
                  }

				  break;
			  }
              if(flag)//无效的指令
              {
				  stPortp->scmd = _0F81H_RET_SETERR;
				  stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
                  memcpy(stPortp->stSendInfo.ST_RET_SETOK.cmd_ml,&pStMsgFrame->info[0],2);
				  AddInfoToSendList(stPortp,_SEND_INFO);
			  }
			}
			else if(ret == 1)//收到有ACK 或NAK
			{
				stPortp->seq_rcv = pStMsgFrame->seq;				
				sem_post(&stPortp->Sem_RcvToSendPth);  				
			}	
			else if(ret == -2)//消息错，得回应NAK
			{
				stPortp->stSendInfo.ucbuf[0] = pStMsgFrame->seq;
				stPortp->stSendInfo.ucbuf[1] = 1;
				AddInfoToSendList(stPortp,_SEND_NAK);
			}
			pthread_mutex_unlock(&stPortp->stSendInfoLock);

			stPortp->count_breakline = 0;

			if(stRcv_p->len)
	        {
				printf("Next msg len=%d\n",stRcv_p->len);
				memcpy(&stPortp->stRcvAnalytice,stRcv_p,sizeof(ST_TXRX_TAG));	
	        }	

			if(!stPortp->portLock)
			{
				stPortp->lineStatue = CONNECT_ONLINE;//只要收到消息就是在线的
			}
			else
			{

			}
			
		}		
		
		pthread_mutex_unlock(&stPortp->stMutexRcv); 

		if((isBrokentoConnect)&&(ret >= 0))//断线后重新连接上了
		{
			isBrokentoConnect = 0;
			if(stPortp != &stContextp->PortCms)
			{			    
                sendPowerbrokenMsg = 1;				
			}
		}
	}

	free(stRcv_p);
}
#else
static void pthread_RcvFromPort(ST_PORTRCVTXD_TAG *stPortp)
{
	ST_CONTEXT_TAG *stContextp;
	int flag;
    int ret,i;
    int isBrokentoConnect;//从断线转换到连线
    int sendPowerbrokenMsg;
	ST_TXRX_TAG *stRcv_p;
	ST_FRAME_MSG_TAG *pStMsgFrame;

	stRcv_p	= (ST_TXRX_TAG *)malloc(sizeof(ST_TXRX_TAG));

    if(stRcv_p==NULL)
    {
		printf("Not enough memorey\n");
        return;
	}

	stContextp = stgContextp;

	sem_init(&stPortp->Sem_RcvBuffNotEmpt,0,0);
	sem_init(&stPortp->Sem_RcvToSendPth,0,0);

	isBrokentoConnect = 0;
	stPortp->count_breakline = 0xfffffff0;
	stPortp->lineStatue = CONNECT_BREAK;//CONNECT_ONLINE;
    ret = 0;
    sendPowerbrokenMsg = 0;
	while(1)
	{		
		usleep(1000); // 1ms
		Wdg_Check(stContextp,1);

//---------------------端口状态处理------------------------------
		if(stPortp->count_breakline < 0xfffffff0)
			stPortp->count_breakline++;
		if(stPortp->count_offline < 0xfffffff0)			
			stPortp->count_offline++;		 

        if(!stPortp->portLock)
        {
			isBrokentoConnect =((stPortp->lineStatue != CONNECT_OFFLINE)&&(stPortp->lineStatue != CONNECT_ONLINE))? 1 : isBrokentoConnect;
			if((stPortp->count_offline >= 180000)&&(stPortp->lineStatue != CONNECT_BREAK)) // 3min	        
				stPortp->lineStatue = CONNECT_OFFLINE;
        }
		else
		{	
			stPortp->count_breakline = 0xfffffff0;
			stPortp->lineStatue = CONNECT_LOCK;
			stPortp->stRcvAnalytice.len = 0;//Clean rcv buf			
		}		
//-------------------------------------------------------------		
        if(sem_trywait(&stPortp->Sem_RcvBuffNotEmpt))//没有消息到来时再发送			
		{	
	        if(sendPowerbrokenMsg)
	        {
				printf("reconnect port: %s\n",stPortp->portname);
                sendPowerbrokenMsg = 0;
			
			    pthread_mutex_lock(&stPortp->stSendInfoLock);
				stPortp->scmd	  = _0F00H_RET_POWERBREAK;
				memcpy(&stPortp->stSendInfo.ST_RET_POWERBREAK,&stContextp->ST_GET_POWERBROKKEN,sizeof(ST_RET_POWERBREAK_TAG));
				stPortp->infolen = sizeof(ST_RET_POWERBREAK_TAG);
				AddInfoToSendList(stPortp,_SEND_INFO);
				pthread_mutex_unlock(&stPortp->stSendInfoLock);
	        }
			continue;
        }

		if(!stPortp->stRcvAnalytice.len)
			continue;
		
		pthread_mutex_lock(&stPortp->stMutexRcv);//debug
		pStMsgFrame = stRcv_p->buf;
		ret = checkProtocol(stPortp,stRcv_p);
		//while(stRcv_p->len)
		if(stRcv_p->len >=8)
		{
	        if(ret>= 0)
	        {
	            stPortp->rcvmsgtotal++;
			}
			
			pthread_mutex_lock(&stPortp->stSendInfoLock);
			if(!ret)
			{
			  stPortp->count_offline = 0;//接收指令下发,离线计时清除
			  stPortp->stSendInfo.ucbuf[0] = pStMsgFrame->seq;
			  if(stPortp != &stContextp->PortCms)//下层通信不需ACK交互
				  i=AddInfoToSendList(stPortp,_SEND_ACK);
			  
			  flag = -1;
			  for(i=0;i<sizeof(stProtoCmdFun)/sizeof(ST_CMDFUN_TAG);i++)
			  {
				  if( (pStMsgFrame->info[0] != (stProtoCmdFun[i].Cmd >>8))
					||(pStMsgFrame->info[1] != (stProtoCmdFun[i].Cmd & 0xff)))
				  {
				  	continue;
				  }
				  
	              flag = 0;
								  
				  usleep(10*1000); // 10ms //两次发送消息间隔太短会收不到
				  stPortp->seq_rcv = pStMsgFrame->seq; //debug
				  sem_post(&stPortp->Sem_RcvToSendPth); 
				  
	              memcpy(stPortp->stSendInfo.ST_RET_SETOK.cmd_ml,&pStMsgFrame->info[0],2);
				  if(!stProtoCmdFun[i].fun(stContextp,stPortp))
				  {				
					  AddInfoToSendList(stPortp,_SEND_INFO);
				  }

				  break;
			  }
	          if(flag)//无效的指令
	          {
				  stPortp->scmd = _0F81H_RET_SETERR;
				  stPortp->infolen = sizeof(ST_RET_SETERR_TAG);
	              memcpy(stPortp->stSendInfo.ST_RET_SETOK.cmd_ml,&pStMsgFrame->info[0],2);
				  AddInfoToSendList(stPortp,_SEND_INFO);
			  }
			}
			else if(ret == 1)//收到有ACK 或NAK
			{
				stPortp->seq_rcv = pStMsgFrame->seq;				
				sem_post(&stPortp->Sem_RcvToSendPth);  				
			}	
			else if(ret == -2)//消息错，得回应NAK
			{
				stPortp->stSendInfo.ucbuf[0] = pStMsgFrame->seq;
				stPortp->stSendInfo.ucbuf[1] = 1;
				AddInfoToSendList(stPortp,_SEND_NAK);
			}
			pthread_mutex_unlock(&stPortp->stSendInfoLock);

			stPortp->count_breakline = 0;

			if(stRcv_p->len)
	        {
				printf("Next msg len=%d\n",stRcv_p->len);
				memcpy(&stPortp->stRcvAnalytice,stRcv_p,sizeof(ST_TXRX_TAG));	
	        }	

			if(!stPortp->portLock)
			{
				stPortp->lineStatue = CONNECT_ONLINE;//只要收到消息就是在线的
			}
			
		}		
		
		pthread_mutex_unlock(&stPortp->stMutexRcv); 

		if((isBrokentoConnect)&&(ret >= 0))//断线后重新连接上了
		{
			isBrokentoConnect = 0;
			if(stPortp != &stContextp->PortCms)
			{			    
                sendPowerbrokenMsg = 1;				
			}
		}
	}

	free(stRcv_p);
}

#endif

#if(!TC_PANEL_ENB)
static void led_ctl(unsigned char num,unsigned char on)
{
	int fp;

	fp = open("/dev/tcled_ctl",O_RDWR);
	if(fp < 0)
	{
	   return;
	}

    num <<= 4;
	on &= 0x0f;

	num |= on;
	
    ioctl(fp,0,num);//工作状态
	close(fp);
}

//0.1秒调用一次
static void led_sysrun(ST_CONTEXT_TAG *stContextp)
{
    static unsigned char counter = 0;
    unsigned char led=0;

	led = (counter > 3)? 1 : 0;
	if(counter++ >=10)
	{
        counter = 0; 
	}

	led_ctl(LED_SYS,led);
}
//现场操作
static void led_locatl(ST_CONTEXT_TAG *stContextp)
{
	static unsigned char delay=0;
    static unsigned char counter = 0;
    unsigned char led=0;
	
	if(stContextp->PortLocate_p->portLock)
	{
		led = 0; //锁时熄灭
	}
	else if(  (stContextp->PortRemote_p->lineStatue == CONNECT_BREAK)
			&&(stContextp->PortLocate_p->lineStatue == CONNECT_BREAK) )//断线时慢闪
	{
		led = (counter > 20)? 1 : 0;
		if(counter++ >=40)
		{
			counter = 0; 
		}
	}
	else
	{
		if(stContextp->PortLocate_p->rcvmsgtotal)
		{
			led = (counter >= 1)? 1 : 0;
			counter = !counter; 
			stContextp->PortLocate_p->rcvmsgtotal--;
			delay = 0;
		}
		else 
		{
			if(delay < 10)
				delay++;
			else
				led = 1;				
		}
	}

	led_ctl(LED_LOCATE,led);
}

//中心控制
static void led_remote(ST_CONTEXT_TAG *stContextp)
{
	static unsigned char delay=0;
    static unsigned char counter = 0;
    unsigned char led=0;

    if(stContextp->PortRemote_p->lineStatue == CONNECT_BREAK)
    {
		led = (counter > 20)? 1 : 0;//中断断链时闪烁
		if(counter++ >= 40)
		{
			counter = 0; 
		}
	}
    else
	{
		if(stContextp->PortRemote_p->rcvmsgtotal)
		{
			led = (counter >= 1)? 1 : 0;
			counter = !counter; 
			stContextp->PortRemote_p->rcvmsgtotal--;
			delay = 0;
		}
		else 
		{
			if(delay < 10)
				delay++;
			else
				led = 1;				
		}
	}
	
	led_ctl(LED_REMOTE,led);
}

//下层通信
static void led_cms(ST_CONTEXT_TAG *stContextp)
{
	static unsigned char delay=0;
    static unsigned char counter = 0;
    unsigned char led=0;
    
    if(stContextp->PortCms.lineStatue == CONNECT_BREAK)
    {
		led = (counter > 20)? 1 : 0;//中断断链时闪烁
		if(counter++ >= 40)
		{
			counter = 0; 
		}
	}
    else
	{
		if(stContextp->PortCms.rcvmsgtotal)
		{
			led = (counter >= 1)? 1 : 0;
			counter = !counter; 
			stContextp->PortCms.rcvmsgtotal--;
			delay = 0;
		}
		else 
		{
			if(delay < 10)
				delay++;
			else
				led = 1;				
		}
	}
	
	led_ctl(LED_CMS,led);
}

static void led_powererr(ST_CONTEXT_TAG *stContextp)
{
    static unsigned char counter = 0;
    unsigned char led=0;
	
	led = (counter > 20)? 1 : 0;//中断断链时闪烁
	if(counter++ >= 40)
	{
		counter = 0; 
	}
	
	led_ctl(0x00,led);
	led_ctl(0x01,led);
	led_ctl(0x02,led);
	led_ctl(0x03,led);
}

int Get_CtrStatues()//现场或远程控制开关
{
	int fp,ret;
	char buf[2];

	fp = open("/dev/tcled_ctl",O_RDWR);
	if(fp <0)
	{
		perror("open err");
	}

    ret = read(fp,buf,sizeof(buf));
    if(ret < 0)
    {
       perror("read err");
    }
	
	close(fp);

	return (int)buf[0];
}

int Get_CaseDoorStatues()
{
	int fp,ret;
	char buf[2];
	
	fp = open("/dev/tcled_ctl",O_RDWR);
	if(fp <0)
	{
		perror("open err");
	}

    ret = read(fp,buf,sizeof(buf));
    if(ret < 0)
    {
       perror("read err");
    }
	
	close(fp);

	return (int)buf[1];
}


static void buzzer_ctr()
{
	led_ctl(4,1);
    usleep(1000 * 300);
	led_ctl(4,0);
}

static void buzzer_ctr_keybeep()
{
	led_ctl(4,1);
    usleep(1000 * 50);
	led_ctl(4,0);
}

static void switch_ctr()
{
	led_ctl(5,0);
    sleep(2);
	led_ctl(5,1);
}


int Get_PowerStatues(ST_CONTEXT_TAG *stContextp)
{
	int fp;
	char adc[10];
	int adcval;
	
	fp = open("/sys/class/am335x_adc/chan5",O_RDWR);

	if(fp < 0)
	{
       perror("open /sys/class/am335x_adc/chan5");
	   close(fp);
	   return -1;
	}
	memset(adc,0x00,sizeof(adc));
    read(fp,(char *)adc,5);	
	adcval = atoi(adc);
	close(fp);
	
	stContextp->stHardState.powererr = ((adcval < POWER_ADCVAL_LOW)|| (adcval > POWER_ADCVAL_HIGHT)); 	

    //printf("adc=%d\n",adcval);
	
	return 0;
}


void pthread_ledstatue(ST_CONTEXT_TAG *stContextp)
{
	int counter=0;
	
    while(1)
	{				
		Wdg_Check(stContextp,1);

        counter++;
		if(!(counter%10)) // 1S读一次门状态
		{
			Get_PowerStatues(stContextp);
	        stContextp->stHardState.cabinatedOpened = !Get_CaseDoorStatues();
		}


        
		if(stContextp->stHardState.powererr)
		{
			led_powererr(stContextp);
		}
		else
		{
			led_sysrun(stContextp);
	        led_remote(stContextp);
	        led_cms(stContextp);
	        led_locatl(stContextp);
		}

		usleep(100*1000); //0.1S
    }
}
#endif


void GetIpAddrStr(ST_NET_CONF_TAG *netConfig,char *ipbuf)
{
	int ret;

	ret = netConfig->ipaddr[0] | netConfig->ipaddr[1]| netConfig->ipaddr[2] | netConfig->ipaddr[3];
	if(!ret)
	{
		netConfig->ipaddr[0] = 192;
		netConfig->ipaddr[1] = 168;
		netConfig->ipaddr[2] = 1;
		netConfig->ipaddr[3] = 6;

		netConfig->gateway[0] = netConfig->ipaddr[0];
		netConfig->gateway[1] = netConfig->ipaddr[1];
		netConfig->gateway[2] = netConfig->ipaddr[2];
	    netConfig->gateway[3] = 1;

		netConfig->netmask[0]=255;
		netConfig->netmask[1]=255;
		netConfig->netmask[2]=255;
		netConfig->netmask[3]=0;		
	}
	ret = netConfig->portaddr[0] | netConfig->portaddr[1];
	if(!ret)
	{
		netConfig->portaddr[0]=0x1f;
		netConfig->portaddr[1]=0x90;
	}
	
    if(ipbuf != NULL)
    {
		sprintf(ipbuf,"ifconfig eth0 %d.%d.%d.%d netmask 255.255.255.0",
		        netConfig->ipaddr[0] , netConfig->ipaddr[1], netConfig->ipaddr[2] , netConfig->ipaddr[3]);
    }	
}


void pthread_ConfigSysFile(ST_CONTEXT_TAG *stContextp)
{
   int fp,temp,ret,i;  
   int count_2min,len;// 10分钟计数
   ST_SYSCONFIG_TAG *SysConfig_p;

   char *ipstrbuf; 
   
   sigset_t setsig;
   sigemptyset(&setsig);
   sigaddset(&setsig, SIGALRM);
   sigprocmask(SIG_BLOCK,&setsig,NULL);//屏蔽信号

   temp = access("/tc/syspara",F_OK);
   SysConfig_p = &stContextp->stSysConfig;
   fp = open("/tc/syspara",O_RDWR|O_CREAT,777);

   if(temp)
   {
       memcpy((unsigned char *)&stContextp->stSysConfig,(unsigned char *)&sysconfig_init,sizeof(ST_SYSCONFIG_TAG));
	   lseek(fp,0,SEEK_SET);
	   write(fp,(unsigned char *)&stContextp->stSysConfig,sizeof(ST_SYSCONFIG_TAG));
   }
   else
   {
	   lseek(fp,0,SEEK_SET);
	   len = read(fp,(unsigned char *)&stContextp->stSysConfig,sizeof(ST_SYSCONFIG_TAG));
   }
 //  SysConfig_p->address[0]=0xff;
//   SysConfig_p->address[1]=0xff;
   close(fp);

   fp = open("/tc/powertime",O_RDWR|O_CREAT,777);//保存上次断电时间
   lseek(fp,0,SEEK_SET);
   len = read(fp,(unsigned char *)&stContextp->ST_GET_POWERBROKKEN,sizeof(ST_RET_POWERBREAK_TAG));
   printf("power broken:mon=%d,day=%d,hour=%d,min=%d\n",
   	      stContextp->ST_GET_POWERBROKKEN.Month,stContextp->ST_GET_POWERBROKKEN.Day,
   	      stContextp->ST_GET_POWERBROKKEN.Hour,stContextp->ST_GET_POWERBROKKEN.Min);
   close(fp);

   count_2min = 0;

   ipstrbuf = malloc(80);
   memset(ipstrbuf,0x00,80);
  // sprintf(ipstrbuf,"ifconfig eth0 ");
   GetIpAddrStr(&stContextp->stSysConfig.ipConfig,&ipstrbuf[strlen(ipstrbuf)]);
   printf("%s\n",ipstrbuf);

   for(i=0;i<10;i++)
   {
	   if(ret = system(ipstrbuf))
	   {
		   sleep(1);
	   }
	   else
	   {
		   break;
	   }
   }
   if(ret)
   {
       printf("FAIL:%s\n",ipstrbuf);
   }
   
   memset(ipstrbuf,0x00,80);
   
   for(i=0;i<2;i++)
   {
       if(ret = system("route del default\n"))
       {
           sleep(1);
       }
       else
       {
           break;
       }
   }
   if(ret)
   {
	   printf("FAIL: route del default\n");
   }

   sprintf(ipstrbuf,"route add default gw %d.%d.%d.%d",stContextp->stSysConfig.ipConfig.gateway[0]
   	 ,stContextp->stSysConfig.ipConfig.gateway[1],stContextp->stSysConfig.ipConfig.gateway[2]
   	 ,stContextp->stSysConfig.ipConfig.gateway[3]);
   
   printf("%s\n",ipstrbuf);
   for(i=0;i<3;i++)
   {
	   if(ret = system(ipstrbuf))
	   {
		   sleep(1);
	   }
	   else
	   {
		   break;
	   }
   }
   if(ret)
   {
       printf("FAIL:%s\n",ipstrbuf);
   }  
   
   temp = stContextp->stSysConfig.ipConfig.portaddr[0] << 8 | stContextp->stSysConfig.ipConfig.portaddr[1];
   printf("%s,port=%d\n",ipstrbuf,temp);
   free(ipstrbuf);

   stContextp->PortRemote_p = (stContextp->stSysConfig.remoteportsel)? &stContextp->PortCenter_Com0 : &stContextp->PortCenter_Eth;
   stContextp->PortLocate_p = (stContextp->stSysConfig.remoteportsel)? &stContextp->PortCenter_Eth	: &stContextp->PortCenter_Com0;

   sem_post(&stContextp->sysDataInitPth);  
   sem_init(&stContextp->sem_UpdateConfig,0,0);	

   while(1)
   {   	      	   
   	   Wdg_Check(stContextp,1);

	   if(sem_trywait(&stContextp->sem_UpdateConfig)==0)//文件有更新
	   {		   		   
		   fp = open("/tc/syspara",O_RDWR|O_CREAT,777);
		   lseek(fp,0,SEEK_SET);
		   if(write(fp,(unsigned char *)SysConfig_p,sizeof(ST_SYSCONFIG_TAG))< 0)
		   {
			   printf("Err:pthread_sysParaFile,write.\n");
		   }
	       printf("---Updata sys file finish:------------\n");	
		   close(fp);
	   }
       
       stContextp->PortRemote_p = (stContextp->stSysConfig.remoteportsel)? &stContextp->PortCenter_Com0 : &stContextp->PortCenter_Eth;
       stContextp->PortLocate_p = (stContextp->stSysConfig.remoteportsel)? &stContextp->PortCenter_Eth  : &stContextp->PortCenter_Com0;

	   stContextp->PortRemote_p->portLock = 0;
       if( (stContextp->PortRemote_p->lineStatue == CONNECT_BREAK)//断线或离线时本地解锁
         ||(stContextp->PortRemote_p->lineStatue == CONNECT_OFFLINE))
       {
	       stContextp->PortLocate_p->portLock = 0;			
       }
       else
       {
	       stContextp->PortLocate_p->portLock = stContextp->stSysConfig.ST_SET_LOCK_UNLOCK.LockDB & 0x01;
       }

       if((count_2min++ >= 120*1000)||(sem_trywait(&stContextp->check_datatime)==0))
       {
	   	   ST_DATETIME_TAG *currDateTime_p;
		   ST_RET_POWERBREAK_TAG *powerBreak_p;

		   currDateTime_p = (ST_DATETIME_TAG *)malloc(sizeof(ST_DATETIME_TAG));
		   powerBreak_p = (ST_RET_POWERBREAK_TAG *)malloc(sizeof(ST_RET_POWERBREAK_TAG));
		   
	   	   count_2min = 0;
		   GetDateTime(currDateTime_p);
		   powerBreak_p->Month = currDateTime_p->Month;
		   powerBreak_p->Day   = currDateTime_p->Day;
		   powerBreak_p->Hour  = currDateTime_p->Hour;		   
		   powerBreak_p->Min   = currDateTime_p->Min;
		   
		   fp = open("/tc/powertime",O_RDWR|O_CREAT,777);//保存上次断电时间
		   lseek(fp,0,SEEK_SET);
		   write(fp,(unsigned char *)powerBreak_p,sizeof(ST_RET_POWERBREAK_TAG));
		   close(fp);

		   free(currDateTime_p);
		   free(powerBreak_p);
       }
	   
	   usleep(1000);// 1ms
   }
}

//const unsigned short f_environmentbright[3]={1500,2000,2400};//{2048,2482,2978};
//const unsigned short f_signbrightness[4]= {100,75,45,30};
unsigned char Brightness_Control(ST_CONTEXT_TAG *stContextp)
{
	ST_BRIGHTNESS_TAG *scbtpt;
	unsigned char brightness,i,brightTime_on;
	unsigned int currTimes_min,startTime_min,endTime_min;

    if(stContextp->autotest)
		return 100; // 20 debug
	
	brightness = 100;
	scbtpt = &stContextp->stSysConfig.ST_BRIGHTNESS;	
	//printf("brigByte:%d\n",scbtpt->unBright.ucBright);
	if(scbtpt->unBright.ST_BRIGHT_BITMAP.r50 == 1)
	{
		brightness = 50;
	}
	else if(scbtpt->unBright.ST_BRIGHT_BITMAP.ForceDimCtr == 1)
	{		
		if(scbtpt->unBright.ST_BRIGHT_BITMAP.r65)
			brightness = stContextp->stSysConfig.ST_BRIGHTNESS_DEF.rate[2];
		else if(scbtpt->unBright.ST_BRIGHT_BITMAP.r80)
			brightness = stContextp->stSysConfig.ST_BRIGHTNESS_DEF.rate[1];
		else 
			brightness = stContextp->stSysConfig.ST_BRIGHTNESS_DEF.rate[0];
	}
	else if (scbtpt->unBright.ST_BRIGHT_BITMAP.LocalCtr == 1)
	{
		currTimes_min = stContextp->CmsCardAdc[0];
		for(i=0;i<3;i++)
		{
			if(currTimes_min <= stContextp->stSysConfig.ST_BRIGHTNESS_DEF.adc[i])//f_environmentbright[i])
			{
			   printf("brig:%d, %d\n",i,stContextp->stSysConfig.ST_BRIGHTNESS_DEF.adc[i]);	
			   break;
			}
		}

		printf("brig:%d\n",i);
		brightness = stContextp->stSysConfig.ST_BRIGHTNESS_DEF.rate[i];//f_signbrightness[i];
	}	
	else 
	{
		ST_DATETIME_TAG    *currDateTime_p;

		currDateTime_p = (ST_DATETIME_TAG *)malloc(sizeof(ST_DATETIME_TAG));

		GetDateTime(currDateTime_p);
		currTimes_min = currDateTime_p->Hour * 60 + currDateTime_p->Min;
		startTime_min = scbtpt->ucStartHour * 60 + scbtpt->ucStartMin;
		endTime_min   = scbtpt->ucEndHour * 60 + scbtpt->ucEndMin;
		
		if(startTime_min <= endTime_min)
		{
			 brightTime_on = ((currTimes_min >= startTime_min)&&(currTimes_min < endTime_min))? TRUE : FALSE;
		}
		else if(startTime_min > endTime_min)
		{
			 brightTime_on = ((currTimes_min >= startTime_min)||(currTimes_min < endTime_min)) ? TRUE : FALSE;
		}

		if(brightTime_on)// in the timing
		{
			if(scbtpt->unBright.ST_BRIGHT_BITMAP.r65)
				//brightness = 65;
				brightness = stContextp->stSysConfig.ST_BRIGHTNESS_DEF.rate[2];
			else if(scbtpt->unBright.ST_BRIGHT_BITMAP.r80)
				//brightness = 80;
				brightness = stContextp->stSysConfig.ST_BRIGHTNESS_DEF.rate[1];
			else 
				//brightness = 100;
				brightness = stContextp->stSysConfig.ST_BRIGHTNESS_DEF.rate[0];			
		}
		else  // the end of the timing
		{
			//brightness = 100;
			brightness = stContextp->stSysConfig.ST_BRIGHTNESS_DEF.rate[0];			
		}
		free(currDateTime_p);
	}
	return brightness;
}


void FontLibToFullColorCms(unsigned short *viewp,unsigned short vw,unsigned short vh,
	                       unsigned short dx,unsigned short dy,
	                       unsigned short fcolor,unsigned short bcolor,unsigned char show,
	                       unsigned char *fontbuf,unsigned char fw,unsigned char fh)
{
    unsigned short *dest;
	unsigned short cmsx=0;
    unsigned char y,i,xb,n;
    unsigned char temp;

    xb = fw / 8 + ((fw % 8) > 0);
	for(y=0;y < fh;y++)
	{
		if((dy+y) >= vh)
			break;
		dest = &viewp[(dy+y) * vw];
		cmsx = dx;
        for(i=0;i<xb;i++)
        {
            temp = fontbuf[y * xb + i];
			for(n=0;n<8;n++)
			{
				if(show)
	                dest[cmsx]=(temp & 0x80) ? bcolor : fcolor;
				else
					dest[cmsx]= bcolor;
				if(++cmsx >= vw)
					break;
				
				temp <<= 1;
			}
			if(cmsx >= vw)
				break;
		}
	}
}

void FontLibToFullColorCms_Insert(unsigned short *viewp,unsigned short vw,unsigned short vh,
	                       unsigned short dx,unsigned short dy,
	                       unsigned short fcolor,unsigned short bcolor,
	                       unsigned char *fontbuf,unsigned char fw,unsigned char fh)
{
    unsigned short *dest;
	unsigned short cmsx=0;
    unsigned char y,i,xb,n;
    unsigned char temp;

    xb = fw / 8 + ((fw % 8) > 0);
	for(y=0;y < fh;y++)
	{
		if((dy+y) >= vh)
			break;
		dest = &viewp[(dy+y) * vw];
		cmsx = dx;
        for(i=0;i<xb;i++)
        {
            temp = fontbuf[y * xb + i];
			for(n=0;n<8;n++)
			{
                if(temp & 0x80)
                {
					if(bcolor)
						dest[cmsx] = bcolor;
				}
				else
				{
					dest[cmsx]=fcolor;

				}
				if(++cmsx >= vw)
					break;
				
				temp <<= 1;
			}
			if(cmsx >= vw)
				break;
		}
	}
}


void FontLibToFullColorCms_Part(unsigned short *viewp,unsigned short vw,unsigned short vh,
	                       unsigned short dx,unsigned short dy,
	                       unsigned short fcolor,unsigned short bcolor,unsigned char show,
	                       unsigned char *fontbuf,unsigned char fw,unsigned char fh,unsigned char fsize_s)
{
    unsigned short *dest;
	unsigned short cmsx=0;
    unsigned char y,i,xb,n;
    unsigned char temp;

    xb = fw / 8 + ((fw % 8) > 0);
	for(y=0;y < fh;y++)
	{
		if((dy+y) >= vh)
			break;
		dest = &viewp[(dy+y) * vw];
		cmsx = dx;
        for(i=0;i<xb;i++)
        {
            temp = fontbuf[y * (fsize_s/8) + i];
			for(n=0;n<8;n++)
			{
				if(show)
	                dest[cmsx]=(temp & 0x80) ? bcolor : fcolor;
				else
					dest[cmsx]= bcolor;
				if(++cmsx >= vw)
					break;
				
				temp <<= 1;
			}
			if(cmsx >= vw)
				break;
		}
	}
}



#if (PARK_CMS_ENB)
void GetIconToBuf(unsigned short *dest,unsigned char *src,unsigned char type,ST_COLOR_TAG color)
{
    int x,y,j,ind,i;
    unsigned char temp;

	i = 0;
	ind = 0;
	for(y=0;y<type;y++)
	{
		for(j=0;j<type/8;j++)
		{
			temp = src[ind++];
            for(x=0;x<8;x++)
            {
                if((temp & 0x80)==0)
                {
                    dest[i]= GetColor(color.font);
				}
				else
				{
                    dest[i]= GetColor(color.Background);
				}
				i++;
				temp <<=1;
			}
		}
	}

}
#endif


void SelfImageToView(unsigned short *dest,unsigned short *code,unsigned short bcolor)
{
	int ret,k,n,i,j;
	unsigned char rg;
	
	ST_COLOR_SINGLE_TEXT_TAG *pST_RET_COLOR_SINGLE_TEXT;
	pST_RET_COLOR_SINGLE_TEXT = (ST_COLOR_SINGLE_TEXT_TAG *)malloc(sizeof(ST_COLOR_SINGLE_TEXT_TAG));
	memset(pST_RET_COLOR_SINGLE_TEXT,0x00,sizeof(ST_COLOR_SINGLE_TEXT_TAG ));
	pST_RET_COLOR_SINGLE_TEXT->ucPatternCode[0]=code[0]>>8;
	pST_RET_COLOR_SINGLE_TEXT->ucPatternCode[1]=code[0];
	ret = GetSelfImageLib(pST_RET_COLOR_SINGLE_TEXT);
	i=0;
	for(n=0;n<CMS_FONT_VIEW_V;n++)
	{
		for(k=0;k<CMS_FONT_VIEW_W/4;k++)
		{
			for(j=3;j>=0;j--)
			{
				rg = (pST_RET_COLOR_SINGLE_TEXT->ucColorMap[n * CMS_FONT_VIEW_W/4 + k] >> (j*2))& 0x03; // bin0 r,bit1 g
				dest[i] = GetColor(rg >> 1 | rg << 1);
		        if(!dest[i])
		        {
					dest[i] = bcolor;
		        }
				i++;
			}
		}
	}
	free(pST_RET_COLOR_SINGLE_TEXT);
}


//从一个视窗COPY到全局显示区
//dest,全局显示存储，src,源窗口, pdw,pdh,dest父窗口的宽高
//dvx,dvy,相对于显示器原点的位置，dvw,dvh,将要copy的宽高

//svx,svy,相对于源窗口原点的位置, svw,svh,源窗口的大小
//矩形区域取数据,适用于平移 
void ViewToView(unsigned short *dest,unsigned short *src,unsigned short pdw,unsigned short pdh,
                unsigned short dvx,  unsigned short dvy, unsigned short dvw,unsigned short dvh,
                unsigned short svx,  unsigned short svy, unsigned short svw,unsigned short svh)
{	
    unsigned short l,h;
    unsigned short px,py;//目标窗口的
	unsigned short sx,sy;//源窗口的

    if(dvw > pdw)
		dvw = pdw;
	if(dvh > pdh)
		dvh = pdh;

	for(h=0;h<dvh;h++)
	{
	   py = h + dvy;
	   sy = h + svy;
	   if((py >= pdh)||(sy >= svh))
	   {
		   break;
	   }
       for(l=0;l<dvw;l++)
       {
           px = l + dvx;
		   sx = l + svx;

		   if((px >= pdw)||(sx >= svw))
		   {
               continue;
		   }
		   
		   dest[pdw * py + px] = src[svw * sy + sx];		   
	   }
	   
	}
}

void ViewMemSet(unsigned short *dest,unsigned short pdw,unsigned short pdh,unsigned short color,
                unsigned short dvx,unsigned short dvy,unsigned short dvw,unsigned short dvh)
{	
	unsigned short	xd,yd;

    if((dvw > pdw)||(dvh > pdh))
		return;

	for(yd = dvy;  yd < dvh + dvy; yd++)
	{
		if(yd >= pdh)
			break;
		for(xd=dvx; xd < dvw + dvx; xd++)
		{			
			if(xd >= pdw)
				break;
			dest[pdw * yd + xd] = color;			
		}
	}
}

unsigned short GetColor(unsigned char rg)
{
	unsigned short r,g,color;
	
	r = (rg & 0x01);
	g = (rg & 0x02);
    if(r)
		r= 0x1f;
    if(g)
		g= 0x1f;

    color = (g<<10) | (r<<5);	
	
	return color;
}

#if (PARK_CMS_ENB)
unsigned int GetPartContext(t_rgColor *pcl,unsigned char *pBIT5CODE)
{
	int parkcount;
	int ret,len,fp,temp,i;
	char *pfile = malloc(64);
	ST_PRESET_TEXT_TAG *pFrame = (ST_PRESET_TEXT_TAG *)malloc(sizeof(ST_PRESET_TEXT_TAG));
	ST_PRESET_TEXT_COLOR_TAG *pFrameColor = (ST_PRESET_TEXT_COLOR_TAG *)malloc(sizeof(ST_PRESET_TEXT_COLOR_TAG));
    unsigned char *pbuf = (unsigned char *)pFrame;

    len = 0;
	pFrame->ucTextID = stgContextp->PARK_CONTEXT.textid_msg;
	pFrameColor->ucTextID = stgContextp->PARK_CONTEXT.textid_msg;
    ret = GetPresetText(pFrame);
	if(!ret)
	{
		memcpy(pBIT5CODE,pFrame->ucWordCode,pFrame->ucTextLength*2);

		GetPresetTextColor(pFrameColor);		
        memcpy(pcl,&pFrameColor->unColor.ucColor[0],pFrame->ucTextLength);		
		len = pFrame->ucTextLength;		
	}
	pFrame->ucTextID = stgContextp->PARK_CONTEXT.textid_park;
	pFrameColor->ucTextID = stgContextp->PARK_CONTEXT.textid_park;
    ret = GetPresetText(pFrame);
	if(!ret)
	{
		memcpy(&pBIT5CODE[len*2],pFrame->ucWordCode,pFrame->ucTextLength*2);

		GetPresetTextColor(pFrameColor);		
        memcpy(&pcl[len],&pFrameColor->unColor.stColor[0],pFrame->ucTextLength);		
		len += pFrame->ucTextLength;		
	}

    for(parkcount=0;parkcount<stgContextp->PARK_CONTEXT.parkcount;parkcount++)
    {
	//---------------------------------icon---------------------------------
		memset(pfile,0x00,64);
		sprintf(pfile,"/tc/park/icon%d",stgContextp->PARK_CONTEXT.text[parkcount].Icon);
		if(access(pfile,F_OK)==0)//文件存在
	    {	
	       pBIT5CODE[len*2]=0x01;
		   pBIT5CODE[len*2+1]= stgContextp->PARK_CONTEXT.text[parkcount].Icon;
		   pcl[len].byte = stgContextp->PARK_CONTEXT.color[parkcount].Icon.byte;
		   len++;
		}
		
	//---------------------------------CN-----------------------------------
		memset(pfile,0x00,64);
		sprintf(pfile,"/tc/park/cn%d",stgContextp->PARK_CONTEXT.text[parkcount].CN);
		fp = open(pfile,O_RDONLY,777);	
		if(fp >0)
		{
		   lseek(fp,0,SEEK_SET);
		   memset(pbuf,0x00,67);
		   ret = read(fp,pbuf,67);

		   if(pbuf[2])
		   {
		   	   if(pbuf[2]>32)
		   	   {
	              pbuf[2] = 32;
			   }
	           for(i=0;i<pbuf[2];i++)
	           {
	               pBIT5CODE[len*2] = pbuf[3+i*2];
				   pBIT5CODE[len*2+1] = pbuf[3+i*2+1];
				   pcl[len].byte = stgContextp->PARK_CONTEXT.color[parkcount].CN.byte;
				   len++;
			   }
		   }
		}	
		close(fp);
		
	//---------------------------------EN-----------------------------------
		memset(pfile,0x00,64);
		sprintf(pfile,"/tc/park/en%d",stgContextp->PARK_CONTEXT.text[parkcount].EN);
		fp = open(pfile,O_RDONLY,777);	
		if(fp >0)
		{
		   lseek(fp,0,SEEK_SET);
		   memset(pbuf,0x00,67);
		   ret = read(fp,pbuf,67);

		   if(pbuf[2])
		   {
			   if(pbuf[2]>32)
			   {
				  pbuf[2] = 32;
			   }
			   
	           for(i=0;i<pbuf[2];i++)
	           {
	               pBIT5CODE[len*2] = pbuf[3+i*2];
				   pBIT5CODE[len*2+1] = pbuf[3+i*2+1];
				   pcl[len].byte = stgContextp->PARK_CONTEXT.color[parkcount].EN.byte;
				   len++;
			   }
		   }
		}	
		close(fp);
		
	//---------------------------------arrow---------------------------------
		memset(pfile,0x00,64);
		sprintf(pfile,"/tc/park/arrow%d",stgContextp->PARK_CONTEXT.text[parkcount].Icon);
		if(access(pfile,F_OK)==0)//文件存在
		{
		   pBIT5CODE[len*2]=0x02;
		   pBIT5CODE[len*2+1]=stgContextp->PARK_CONTEXT.text[parkcount].arrow;

		   pcl[len].byte = stgContextp->PARK_CONTEXT.color[parkcount].arrow.byte;

		   len++;
		}
	//-------------------------------distance--------------------------------    
		memset(pbuf,0x00,32);
	    sprintf(pbuf,"%dM",stgContextp->PARK_CONTEXT.text[parkcount].distance);
		
		temp = strlen(pbuf);
		if(temp%2)
		{
	        pbuf[temp]=' ';		
		}
		temp = strlen(pbuf);
		for(i=0;i<temp/2;i++)
		{
		   pBIT5CODE[len*2] = pbuf[i*2];
		   pBIT5CODE[len*2+1] = pbuf[i*2+1];
		   pcl[len].byte = stgContextp->PARK_CONTEXT.color[parkcount].distance.byte;
		   len++;
		}

	//-------------------------------stall--------------------------------	 
		pBIT5CODE[len*2+0]=0xA8;
		pBIT5CODE[len*2+1]=0xAE;
		pBIT5CODE[len*2+2]=0xA6;
		pBIT5CODE[len*2+3]=0xEC;//車位 	
		pcl[len++].byte = stgContextp->PARK_CONTEXT.color[parkcount].stall.byte;
		pcl[len++].byte = stgContextp->PARK_CONTEXT.color[parkcount].stall.byte;

		memset(pbuf,0x00,12);

		
		sprintf(pbuf,"%d",stgContextp->PARK_CONTEXT.text[parkcount].stall);
	    temp = strlen(pbuf);
		if(temp%2)
		{
	        pbuf[temp]=' ';		
		}
		temp = strlen(pbuf); 
		for(i=0;i<temp/2;i++)
		{
			pBIT5CODE[len*2] = pbuf[i*2];
			pBIT5CODE[len*2+1] = pbuf[i*2+1];
			pcl[len].byte = stgContextp->PARK_CONTEXT.color[parkcount].stall.byte;
			len++;
		}
    }
//----------------------------------------------------------------------------

    free(pFrame);
	free(pFrameColor);
	free(pfile);	


	return len;
}

int SetPartConTextToBuf(unsigned short *dest,unsigned short vw,unsigned short vh)
{
	int temp;
	unsigned char glitter;

    int getCount;	
	int fp,i;
	unsigned short code;
	unsigned short cmsx,cmsy;
	unsigned int addr,len;
	unsigned char *buf;
	unsigned short fcolor,bcolor;
	unsigned char *pBIT5CODE;
	t_rgColor *pcl;
	unsigned char *asciic,k;
	int fp_park;
	
	unsigned short *pbuf = (unsigned short *)malloc(CMS_FONT_VIEW_W * CMS_FONT_VIEW_V * sizeof(pbuf)); 
	char *filep = malloc(64);
	unsigned char *text_buf = malloc(128);
	t_PARK_GRAPH *PARK_GRAPH_p =(t_PARK_GRAPH *)malloc(sizeof(t_PARK_GRAPH));
	
    const unsigned char glitter_sleep[]={0,2,1,4};

#if (CMS_FONT_VIEW_V == 48)
	fp = open("/tc/font48X48" ,O_RDONLY,777);
#elif (CMS_FONT_VIEW_V == 32)
    fp = open("/tc/font32X32" ,O_RDONLY,777);
#endif
	if(fp < 0)
	{
        printf("Err: no found the fontlib file.\n");
		return -1;
	}

	buf = malloc(290);
	pcl = (t_rgColor *)malloc(256);
    pBIT5CODE = (unsigned char *)malloc(512);
	getCount = GetPartContext(pcl,pBIT5CODE);

	cmsx = 0;
	cmsy = 0;	
    for(i = 0;i<getCount;i++)
    {			
		code = pBIT5CODE[i*2];
		code <<= 8;
		code |= pBIT5CODE[i*2+1];
		
		fcolor = GetColor(pcl[i].color.font);
		bcolor = GetColor(pcl[i].color.Background);
		glitter = pcl[i].color.flash;
		
        if(glitter)
        {
            if(stgContextp->disUpdateTimeUnit > DISPLAY_UPDATETIME_FLASH)
            {
				stgContextp->disUpdateTimeUnit = DISPLAY_UPDATETIME_FLASH;				
            }
			temp = (stgContextp->countTickComm / (DISPLAY_UPDATETIME_FLASH * glitter_sleep[glitter])) % 2;
			if(temp)//显背景
			{
				ViewMemSet(dest,vw,vh,bcolor,cmsx,cmsy,CMS_FONT_VIEW_W,CMS_FONT_VIEW_V);
				
				cmsx += CMS_FONT_VIEW_W;
		        if(cmsx >= vw)//换行
				{
					cmsx = 0;
					cmsy += CMS_FONT_VIEW_V;
				}
				if(cmsy >= vh)
				{
					break;
				}             
				continue;
			}
		}
		
		if((code >= 0xfa40)&&(code <= 0xfefe))//自己造字
		{
			ST_USER_CHINESE_FONT_TAG *fontp;

			fontp = (ST_USER_CHINESE_FONT_TAG *)malloc(sizeof(ST_USER_CHINESE_FONT_TAG));
			memset(fontp,0x00,sizeof(ST_USER_CHINESE_FONT_TAG));
			fontp->ucBig5Code[0] = pBIT5CODE[i*2];
			fontp->ucBig5Code[1] = pBIT5CODE[i*2+1];
			GetSelfFontLib(fontp);
			FontLibToFullColorCms(dest,vw,vh,cmsx,cmsy,fcolor,bcolor,1,fontp->ucBitMap,CMS_FONT_VIEW_W,CMS_FONT_VIEW_V);
            free(fontp);
			cmsx += CMS_FONT_VIEW_W;
		}
		else if( ((code >= 0x2100)&&(code <= 0x2500))//自己造图
               &&( (((code&0xff)>='0')&&((code&0xff)<='9')) 
                 ||(((code&0xff)>='A')&&((code&0xff)<='Z'))
			     ||(((code&0xff)>='a')&&((code&0xff)<='z')) )                
		       )
		{
            memset(pbuf,0x00,(CMS_FONT_VIEW_W * CMS_FONT_VIEW_V * sizeof(pbuf)));
			SelfImageToView(pbuf,&code,bcolor);
			ViewToView(dest,pbuf,vw,vh,cmsx,cmsy,CMS_FONT_VIEW_W,CMS_FONT_VIEW_V,0,0,CMS_FONT_VIEW_W,CMS_FONT_VIEW_V);
            free(pbuf);

			cmsx += CMS_FONT_VIEW_W;
		}
		
		else if((code > 0x0300)&&(code < 0x8000))
		{
			asciic = &pBIT5CODE[i*2]; 		 
            for(k=0;k<2;k++)
            {
	            addr = 0x0800 + asciic[k]*(CMS_FONT_VIEW_W/2 * CMS_FONT_VIEW_V / 8 + 1);
				lseek(fp,addr,SEEK_SET);		
				len = read(fp,buf,CMS_FONT_VIEW_V * (CMS_FONT_VIEW_W/2) / 8 + 1);//48 * 24/8 + 1(crc)	
				FontLibToFullColorCms(dest,vw,vh,cmsx,cmsy,fcolor,bcolor,1,buf,(CMS_FONT_VIEW_W/2),CMS_FONT_VIEW_V);

				cmsx += (CMS_FONT_VIEW_W/2);
				if(cmsx >= vw)//换行
				{   
                    break;
				}
            }	                  
		}
		else if(code >= 0xA140)
		{
			unsigned char r,h;//行列地址 89 X 157的表格
			
			asciic = &pBIT5CODE[i*2]; 		 

			if(asciic[1] <= 0x7E)
			{
                r = asciic[1] - 0x40;
			}
			else
			{
                r = asciic[1] - 0xA1 + 0x3F; // 0x7E - 0x40 + 1 = 0x3f;
			}
            h = asciic[0] - 0xA1;
			
			addr = 0x0800+0x80*(CMS_FONT_VIEW_W/2 * CMS_FONT_VIEW_V / 8 + 1); // 0x0800 + 145 * 0x80 = 5080
            addr += ((h*157)+r) * (CMS_FONT_VIEW_W * CMS_FONT_VIEW_V / 8 + 1); // 0x0800 + 145 * 0x80 = 5080

			lseek(fp,addr,SEEK_SET);		
			len = read(fp,buf,(CMS_FONT_VIEW_W * CMS_FONT_VIEW_V / 8 + 1));//48 * 48 / 8 + 1(crc)		

			FontLibToFullColorCms(dest,vw,vh,cmsx,cmsy,fcolor,bcolor,1,buf,CMS_FONT_VIEW_W,CMS_FONT_VIEW_V);
				                  
			cmsx += CMS_FONT_VIEW_W;
		}
        else if((code >= 0x0100)&&(code < 0x0200))
        {
			memset(PARK_GRAPH_p,0x00,sizeof(t_PARK_GRAPH));
			memset(filep,0x00,64);
			sprintf(filep,"/tc/park/icon%d",pBIT5CODE[i*2+1]);
			fp_park = open(filep,O_RDONLY,777);  
			if(fp >0)
			{
			   lseek(fp_park,0,SEEK_SET);
			   len = read(fp_park,(char *)PARK_GRAPH_p,sizeof(t_PARK_GRAPH));
			}	
			close(fp_park);

			temp = (PARK_GRAPH_p->Mode > 0)? 32 : 16;
			GetIconToBuf(pbuf, PARK_GRAPH_p->Bitmap,temp,pcl[i].color);

			ViewToView(dest,pbuf,vw,vh,cmsx,cmsy,temp,temp,0,0,temp,temp);
			
			cmsx += CMS_FONT_VIEW_W;
		}
        else if((code >= 0x0200)&&(code < 0x0300))
        {		
			memset(PARK_GRAPH_p,0x00,sizeof(t_PARK_GRAPH));
			memset(filep,0x00,64);
			sprintf(filep,"/tc/park/arrow%d",pBIT5CODE[i*2+1]);
			fp_park = open(filep,O_RDONLY,777);  
			if(fp >0)
			{
			   lseek(fp_park,0,SEEK_SET);
			   len = read(fp_park,(char *)PARK_GRAPH_p,sizeof(t_PARK_GRAPH));
			}	
			close(fp_park);

			temp = (PARK_GRAPH_p->Mode > 0)? 32 : 16;
			GetIconToBuf(pbuf, PARK_GRAPH_p->Bitmap,temp,pcl[i].color);

			ViewToView(dest,pbuf,vw,vh,cmsx,cmsy,temp,temp,0,0,temp,temp);
			cmsx += CMS_FONT_VIEW_W;
		}

		if(cmsx >= vw)//换行
		{
			cmsx = 0;
			cmsy += CMS_FONT_VIEW_V;
		}
		if(cmsy >= vh)
		{
			break;
		}
		
	}

	free(pBIT5CODE);
	free(pcl);
	
	free(pbuf);
	free(filep);
	free(text_buf);
	free(PARK_GRAPH_p);
	free(buf);
	
	close(fp);

	return 0;
}


#endif




/*
Format of the file:
1.starting: "JYV210"+0X30+0X30+0x30
2.0x800 ~ 0x800+0x80*145: ASCII character, quantity: 0x80
 Each character: 144 bytes + CRC8(1byte);    
 Range(ASCII):   0x0000-0x007F   

3.0x800+0x80*145 - the end: Chinese characters, quantity: 13973
 Each character : 289 bytes + CRC8(1byte);

Range(Big 5): 
High Byte: $A1~ $F9
Low Byte: $40~ $7E,  $A1-$FE
*/
/*
fun:将预设的文本信息转换成显示画面
return:-1,无效的执行；ret >=0,下一个字符ID
input: dest,视窗缓存,vw,vh,视窗大小,TextId,信息编号,stid,字符起址
*/
int GetPresetTextToBuf(unsigned short *dest,unsigned short vw,unsigned short vh,int TextId,unsigned char *stid)
{
	int vspace,hspace;
	int fp,ret,i;
	unsigned char sttextid;
	unsigned short code;
	unsigned short cmsx,cmsy;
	unsigned int addr,len;
	unsigned char *buf;
	unsigned short fcolor,bcolor;
	ST_PRESET_TEXT_TAG *pFrame;
	ST_PRESET_TEXT_COLOR_TAG *pFrameColor;
    const unsigned char glitter_sleep[]={0,2,1,4};

#if (CMS_FONT_VIEW_V == 48)
	fp = open("/tc/font48X48" ,O_RDONLY,777);
#elif (CMS_FONT_VIEW_V == 32)
    fp = open("/tc/font32X32" ,O_RDONLY,777);
#endif
	if(fp < 0)
	{
        printf("Err: no found the fontlib file.\n");
		return -1;
	}

	pFrame = (ST_PRESET_TEXT_TAG *)malloc(sizeof(ST_PRESET_TEXT_TAG));
	pFrameColor = (ST_PRESET_TEXT_COLOR_TAG *)malloc(sizeof(ST_PRESET_TEXT_COLOR_TAG));

	pFrame->ucTextID = TextId;
	pFrameColor->ucTextID = TextId;
	ret = GetPresetText(pFrame);
	if(ret)//无此TEXID
	{
		close(fp);
		printf("Err: Show Texid.\n");
		return -1;
	}
	GetPresetTextColor(pFrameColor);
	
	buf = malloc(290);
	cmsx = pFrameColor->ucH_Bound;
	cmsy = pFrameColor->ucV_Bound;
	
	sttextid = *stid;
    for(i = sttextid;i<pFrame->ucTextLength;i++)
    {
		int temp;
		unsigned char glitter;
		
		stid[0]++;
		code = pFrame->ucWordCode[i][0]<<8 | pFrame->ucWordCode[i][1];
		fcolor = GetColor(pFrameColor->unColor.stColor[i].font);//GetColor(pFrameColor->fontColor[i]);
		bcolor = GetColor(pFrameColor->unColor.stColor[i].Background);//GetColor(pFrameColor->bgColor[i]);

		vspace = pFrameColor->vSpacem;
		vspace <<= 8;
		vspace |= pFrameColor->vSpacel;

		hspace = pFrameColor->hSpacem;
		hspace <<= 8;
		hspace |= pFrameColor->hSpacel;
			
		glitter = pFrameColor->unColor.stColor[i].flash;
        if(glitter)
        {
            if(stgContextp->disUpdateTimeUnit > DISPLAY_UPDATETIME_FLASH)
            {
				stgContextp->disUpdateTimeUnit = DISPLAY_UPDATETIME_FLASH;				
            }
			temp = (stgContextp->countTickComm / (DISPLAY_UPDATETIME_FLASH * glitter_sleep[glitter])) % 2;
			if(temp)//显背景
			{
				ViewMemSet(dest,vw,vh,bcolor,cmsx,cmsy,CMS_FONT_VIEW_W,CMS_FONT_VIEW_V);
				
				cmsx += CMS_FONT_VIEW_W + hspace;
		        if(cmsx >= vw)//换行
				{
					cmsx = pFrameColor->ucH_Bound;
					cmsy += CMS_FONT_VIEW_V + vspace;
				}
				if(cmsy >= vh)
				{
					break;
				}             
				continue;
			}
		}
		
		if((code >= 0xfa40)&&(code <= 0xfefe))//自己造字
		{
			ST_USER_CHINESE_FONT_TAG *fontp;

			fontp = (ST_USER_CHINESE_FONT_TAG *)malloc(sizeof(ST_USER_CHINESE_FONT_TAG));
			memset(fontp,0x00,sizeof(ST_USER_CHINESE_FONT_TAG));
			fontp->ucBig5Code[0] = pFrame->ucWordCode[i][0];
			fontp->ucBig5Code[1] = pFrame->ucWordCode[i][1];
			GetSelfFontLib(fontp);
			if((!fontp->ucFrameRow)||(!fontp->ucFrameColumn))
			{
                memset(fontp->ucBitMap,0xff,sizeof(fontp->ucBitMap));
			}
			FontLibToFullColorCms(dest,vw,vh,cmsx,cmsy,fcolor,bcolor,1,fontp->ucBitMap,CMS_FONT_VIEW_W,CMS_FONT_VIEW_V);
            free(fontp);
			cmsx += CMS_FONT_VIEW_W + hspace;
		}
		//else if((code >= 0x2130)&&(code <= 0x257a))//自己造图
		else if( ((code >= 0x2100)&&(code <= 0x25FF))
               &&( (((code&0xff)>='0')&&((code&0xff)<='9')) 
                 ||(((code&0xff)>='A')&&((code&0xff)<='Z'))
			     ||(((code&0xff)>='a')&&((code&0xff)<='z')) )                
		       )
		{
			unsigned short *pbuf;
			
			pbuf = (unsigned short *)malloc(CMS_FONT_VIEW_W * CMS_FONT_VIEW_V * sizeof(pbuf)); 
            memset(pbuf,0x00,(CMS_FONT_VIEW_W * CMS_FONT_VIEW_V * sizeof(pbuf)));
			SelfImageToView(pbuf,&code,bcolor);
			ViewToView(dest,pbuf,vw,vh,cmsx,cmsy,CMS_FONT_VIEW_W,CMS_FONT_VIEW_V,0,0,CMS_FONT_VIEW_W,CMS_FONT_VIEW_V);
            free(pbuf);

			cmsx += CMS_FONT_VIEW_W + hspace;
		}
		else if(code < 0x8000)
		{
			unsigned char *asciic,k;

			asciic = pFrame->ucWordCode[i]; 		 
            for(k=0;k<2;k++)
            {
	            addr = 0x0800 + asciic[k]*(CMS_FONT_VIEW_W/2 * CMS_FONT_VIEW_V / 8 + 1);
	            //addr = 0x0800 + asciic[k]*145;
				lseek(fp,addr,SEEK_SET);		
				len = read(fp,buf,CMS_FONT_VIEW_V * (CMS_FONT_VIEW_W/2) / 8 + 1);//48 * 24/8 + 1(crc)	
				FontLibToFullColorCms(dest,vw,vh,cmsx,cmsy,fcolor,bcolor,1,buf,(CMS_FONT_VIEW_W/2),CMS_FONT_VIEW_V);

				cmsx += (CMS_FONT_VIEW_W/2) + hspace;
				
				if(cmsx >= vw)//换行
				{   
                    break;
				}
            }	                  
		}
		else if(code >= 0xA140)
		{
			unsigned char r,h;//行列地址 89 X 157的表格
			unsigned char *asciic;
			
			asciic = pFrame->ucWordCode[i]; 		 

			if(asciic[1] <= 0x7E)
			{
                r = asciic[1] - 0x40;
			}
			else
			{
                r = asciic[1] - 0xA1 + 0x3F; // 0x7E - 0x40 + 1 = 0x3f;
			}
            h = asciic[0] - 0xA1;
			
            //addr = 0x5080 + ((h*157)+r) * (CMS_FONT_VIEW_W * CMS_FONT_VIEW_V / 8 + 1); // 0x0800 + 145 * 0x80
			addr = 0x0800+0x80*(CMS_FONT_VIEW_W/2 * CMS_FONT_VIEW_V / 8 + 1); // 0x0800 + 145 * 0x80 = 5080
            addr += ((h*157)+r) * (CMS_FONT_VIEW_W * CMS_FONT_VIEW_V / 8 + 1); // 0x0800 + 145 * 0x80 = 5080

            //printf("Font Addr:%d,h=%d,r=%d\n",addr,h,r);

			lseek(fp,addr,SEEK_SET);		
			len = read(fp,buf,(CMS_FONT_VIEW_W * CMS_FONT_VIEW_V / 8 + 1));//48 * 48 / 8 + 1(crc)		

			FontLibToFullColorCms(dest,vw,vh,cmsx,cmsy,fcolor,bcolor,1,buf,CMS_FONT_VIEW_W,CMS_FONT_VIEW_V);
				                  
			cmsx += CMS_FONT_VIEW_W + hspace;
		}
		if(cmsx >= vw)//换行
		{
			cmsx = pFrameColor->ucH_Bound;
			cmsy += CMS_FONT_VIEW_V + vspace;
		}
		//printf("cmsx=%d,cmsy=%d,vw=%d,vh=%d\n",cmsx,cmsy,vw,vh);
		if(cmsy >= vh)
		{
			break;
		}
		
	}
	free(buf);
	free(pFrame);
	free(pFrameColor);		
	close(fp);

    //printf("cmsx=%d,cmsy=%d,vw=%d,vh=%d\n",cmsx,cmsy,vw,vh);

	ret = (stid[0]	>= pFrame->ucTextLength)? 1 : 0;
	
	return ret;
}


#if (CMS_SPCE_CONFIG == CMS_RG6X1_F32X32)
void ImageToCmsCase(unsigned int dest[_CMS_CTRCARD_SCAN_RATE][CMS_COLOR_BITNUM][_CMS_CTRCARD_SCAN_W], unsigned short srcp[_CMS_CTRCARD_DISP_V][_CMS_CTRCARD_DISP_W])
{
	unsigned short sbit,i,temp;
	unsigned char dv,dh,n,k;
    unsigned short src_x,src_y;
   
    unsigned int *dp;
    unsigned int r,g,b;
    unsigned int rgb;

    sbit = 5 - CMS_COLOR_BITNUM;
	for(dv=0;dv<8;dv++)// 8行扫
	{
		for(k=0;k<CMS_COLOR_BITNUM;k++)
		{
			dp = dest[dv][k];
			for(i=0;i<6;i++)//对每个模组分别取数 6 * 16 = 96个CLK点
			{
		        for(dh=0;dh<16;dh++)//水平方向16个点
		        {
					dp[dh+i*16] = 0;
					for(n=0;n <8;n++)// 8个接口
					{
						if(i%2)
						{
							src_y = n/2*48 + dv + i/2*16;
							src_x = 15-dh;
						}
						else
						{
							src_y = n/2*48 + dv + 8 + i/2*16;
                            src_x = dh;
						}
						if(n%2)
						{						
							src_x += 16;
						}

                        temp = srcp[src_y][src_x];
						
						r = (temp >> (10 + sbit + k)) & 0x01;//取每个color的最高位
						g = (temp >> (5+ sbit + k)) & 0x01;
						b = (temp >> (0+ sbit + k)) & 0x01;

	                    rgb = (b<<2) | (g<<1) | r;
						dp[dh+i*16] |= (rgb << (3*n));
					}
				}
			}
		}
	}
}
#elif (CMS_SPCE_CONFIG == CMS_RG1X1_F48X48_SHG)
void Get8X32To16X16(int x,int y,int *dx,int *dy)//单个模组的坐标转换反转
{
    if(x > 15)
    {
        *dy = y;
		*dx = (x-16);
	}
	else
	{
		*dx = x;
        *dy = y + 8;
	}
}
void Get16X16To8X32(int x,int y,int *dx,int *dy)//单个模组的坐标转换
{
	if(y<8)
	{
		*dx = 31-x;//x+16;
        *dy = y;
	}
	else
	{
        *dy = y%8;
		*dx = x;
	}
}

void Get32X72To48X48(int x,int y,int *dx,int *dy)
{
	int mline,my;
    int xt,yt;
    int y_16X144;

	mline = y/8;//第几列模组
	my = y%8;   //模组内第几行
	Get8X32To16X16(x,my,&xt,&yt);
	y_16X144 = mline * 16 + yt;   // 16X144的Y

	*dy = y_16X144 % 48;
	*dx = (y_16X144 / 48)*16 + xt;	

	//printf("%d,%d  ",*dx,*dy);
}


void Get16X144To32X72(int x,int y,int *dx,int *dy)
{
    int y_module,ym;
    int y_8x32;

	y_module = y/16;                         //第几列模组
	ym = y%16;                               //16X16时模组内Y值
	Get16X16To8X32(x, ym, dx, &y_8x32);
	*dy = y_module * 8 + y_8x32;
}
void Get48X48To16X144(int x,int y,int *dx,int *dy)
{
    *dx = x%16;
	*dy = x/16*48+y;
}


void GetPointFrom48X48ToFPGA_DATA(int x,int y,int *dx,int *dy,int *sb)
{
    int x_16X144,y_16X144;
    int scany;
	int module;

	Get48X48To16X144(x,y,&x_16X144,&y_16X144);
	Get16X144To32X72(x_16X144,y_16X144,dx,dy);

	scany = *dy%8;  // 8扫中的行号
	module = *dy/8; // 垂直方向模组号
	*sb = module *3;
	*dy = scany;
}

void ImageToCmsCase(unsigned int dest[_CMS_CTRCARD_SCAN_RATE][CMS_COLOR_BITNUM][_CMS_CTRCARD_SCAN_W][3], unsigned short srcp[_CMS_CTRCARD_DISP_V][_CMS_CTRCARD_DISP_W])
{
	unsigned short sbit,temp;
	unsigned char k;
 
    unsigned int *dp;
    unsigned int r,g,b;
    unsigned int rgb;
	int destx,desty,sb;
	int x,y;
    int index,s;

    sbit = 5 - CMS_COLOR_BITNUM;
    for(k=0;k<CMS_COLOR_BITNUM;k++)
    {
        for(y=0;y<_CMS_CTRCARD_DISP_V;y++)
        {
            for(x = 0;x<_CMS_CTRCARD_DISP_W;x++)
            {
				temp = srcp[y][x];
				
				r = (temp >> (10 + sbit + k)) & 0x01;//取每个color的最高位
				g = (temp >> (5+ sbit + k)) & 0x01;
				b = (temp >> (0+ sbit + k)) & 0x01;
				
				rgb = (b<<2) | (g<<1) | r;

                GetPointFrom48X48ToFPGA_DATA(x,y,&destx,&desty,&sb);
				dp = dest[desty][k][destx];
				index = sb/32;
				s = sb%32;
				dp[index] &= ~(7UL<<s);
				dp[index] |= (rgb<<s);
			
				if((32-s)<3)//下一字
				{
                    s = 3-(32-s);//在低位补上
					dp[index+1] &= ~(7UL>>(3-s));
					dp[index+1] |= rgb>>(3-s);
				}				
			}
		}			
	}
}

/*
void ImageToCmsCase(unsigned int dest[_CMS_CTRCARD_SCAN_RATE][CMS_COLOR_BITNUM][_CMS_CTRCARD_SCAN_W], unsigned short srcp[_CMS_CTRCARD_DISP_V][_CMS_CTRCARD_DISP_W])
{
	unsigned short sbit,i,temp;
	unsigned char dv,dh,n,k;
   
    unsigned int *dp;
    unsigned int r,g,b;
    unsigned int rgb;

    sbit = 5 - CMS_COLOR_BITNUM;
	for(dv=0;dv<8;dv++)// 8行扫
	{
		for(k=0;k<CMS_COLOR_BITNUM;k++)
		{
			dp = dest[dv][k];
			for(i=0;i<8;i++)//对每个模组分别取数 8 * 16 = 128个CLK点
			{
		        for(dh=0;dh<16;dh++)//水平方向16个点
		        {
					dp[dh+i*16] = 0;
					for(n=0;n <6;n++)// 6个接口
					{
						if(i<4)//第一行
						{
							if(i%2)
							{	temp = srcp[n*32+dv][15-dh+(i/2)*16];
							}
							else
							{	temp = srcp[n*32+8+dv][i*8+dh];
							}
						}
						else//第二行
						{
							if(i%2)
							{	temp = srcp[n*32+16+dv][15-dh+(i/2)*16];
							}
							else
							{	temp = srcp[n*32+16+8+dv][i*8+dh];
							}
						}

						r = (temp >> (10 + sbit + k)) & 0x01;//取每个color的最高位
						g = (temp >> (5+ sbit + k)) & 0x01;
						b = (temp >> (0+ sbit + k)) & 0x01;

	                    rgb = (b<<2) | (g<<1) | r;
						dp[dh+i*16] |= (rgb << (3*n));
					}
				}
			}
		}
	}
}
*/
#else
/*
dest:垂直方向每次扫描共输出 3(RGB) * 9(MODULE) = 27bit,占两个word的长度，16扫,宽度是48
srcp:数据源是一个144*48的图片数据
*/
void ImageToCmsCase(unsigned int dest[_CMS_CTRCARD_SCAN_RATE][CMS_COLOR_BITNUM][_CMS_CTRCARD_SCAN_W], unsigned short srcp[_CMS_CTRCARD_DISP_V][_CMS_CTRCARD_DISP_W])
{
	unsigned short sbit,i,temp;
	unsigned char dv,dh,n,k;
   
    unsigned int *dp;
    unsigned int r,g,b;
    unsigned int rgb;

    sbit = 5 - CMS_COLOR_BITNUM;
	for(dv=0;dv<8;dv++)// 8行扫
	{
		for(k=0;k<CMS_COLOR_BITNUM;k++)
		{
			dp = dest[dv][k];
			//for(i=0;i<6;i++)//每行个点数i * 16
			for(i=0;i<_CMS_CTRCARD_SCAN_W/16;i++)
			{
		        for(dh=0;dh<16;dh++)
		        {
					dp[dh+i*16] = 0;
					//for(n=0;n < 9;n++)
					for(n=0;n < _CMS_CTRCARD_DISP_V/16;n++)
					{
						if(i%2)
							temp = srcp[n*16+dv][15-dh+(i/2)*16];
						else
							temp = srcp[n*16+8+dv][i*8+dh];
						
						r = (temp >> (10 + sbit + k)) & 0x01;//取每个color的最高位
						g = (temp >> (5+ sbit + k)) & 0x01;
						b = (temp >> (0+ sbit + k)) & 0x01;

	                    rgb = (b<<2) | (g<<1) | r;
						dp[dh+i*16] |= (rgb << (3*n));
					}
				}
			}
		}
	}
}
#endif

#if(CMS_SPCE_CONFIG  == CMS_RG8X1_F32X32)
void CmsCaseToCtr(unsigned short *dest,unsigned short *src,unsigned short ys)
{
    unsigned short x,y;

	for(y=0;y<_CMS_CTRCARD_DISP_V;y++)
	{
        for(x=0;x<_CMS_CTRCARD_DISP_W;x++)
        {
            dest[y * _CMS_CTRCARD_DISP_W + x] = src[(y+ys) * CMS_PIXEL_W + x];
		}
	}
}
#else
//从整箱到一个控制器的数据 (1/5)
void CmsCaseToCtr(unsigned short *dest,unsigned short *src,unsigned short xs)
{
    unsigned short x,y;

	for(y=0;y<CMS_PIXEL_H;y++)
	{
        for(x=0;x<_CMS_CTRCARD_DISP_W;x++)
        {
            dest[y * _CMS_CTRCARD_DISP_W + x] = src[y * CMS_PIXEL_W + xs + x];
		}
	}
}
#endif

#if(PARK_CMS_ENB)
int Show_ParkContext(ST_CONTEXT_TAG *stContextp)
{
	int ModeConnect;
    unsigned char Cms_bOn;

	Cms_bOn = TRUE;

    ModeConnect = ( stContextp->PortCenter_Eth.lineStatue  &  stContextp->PortCenter_Com0.lineStatue & stContextp->PortCenter_Com1.lineStatue & stContextp->PortCenter_Com2.lineStatue);
	switch(ModeConnect)
	{
		case CONNECT_ONLINE:
		{
		}
		break;
		
		case CONNECT_OFFLINE:
		{
            if(stContextp->stSysConfig.stOffLine.disp == OFFLINEMODE_CYCLE)
			{
			}
			else if(  (stContextp->stSysConfig.stOffLine.disp == OFFLINEMODE_HOLD)
				    ||((stContextp->stSysConfig.stOffLine.disp == OFFLINEMODE_DELAY)&&((stContextp->offlineTime/60) < stContextp->stSysConfig.stOffLine.delay) )
                   )
			{	
			}
			else
			{
				Cms_bOn = FALSE;
			}	
		}
		break;

		case CONNECT_BREAK:
        if(stContextp->stSysConfig.stBreakLine.disp == OFFLINEMODE_CYCLE)
		{
			
		}
		else if(  (stContextp->stSysConfig.stBreakLine.disp == OFFLINEMODE_HOLD)
			    ||((stContextp->stSysConfig.stBreakLine.disp == OFFLINEMODE_DELAY)&&((stContextp->offlineTime/60) < stContextp->stSysConfig.stOffLine.delay) )
               )
		{	
		}
		else
		{
			Cms_bOn = FALSE;
		}
		break;

	    default:
		Cms_bOn = FALSE;
		break;
			
	}

	memset(stContextp->CmsBuf,0x00,sizeof(stContextp->CmsBuf));
	if(Cms_bOn)
	{	
		SetPartConTextToBuf(stContextp->CmsBuf,CMS_PIXEL_W,CMS_PIXEL_H);
		return 1;
	}
	return 0;
}

#endif

/*
fun:将预设的MBP信息转换成显示画面
return:-1,无效的执行
input: dest,视窗缓存,vw,vh,视窗大小,MsgId,信息编号,fitsize,是否缩放
*/
int GetPresetMsgToBuf(unsigned short *dest,unsigned short vw,unsigned short vh,
	                  unsigned short  MsgId,unsigned char fitsize)
{
	int w,h,i,n;
	int x,y,s;
    int fp;
	unsigned char byte,rg;
	
	char *pfile = malloc(64);

	sprintf((char *)pfile,"/tc/downmsg/msg%d",MsgId);	

	
	pthread_mutex_lock(&stgContextp->imagemsg_lock);
	fp = open((char *)pfile,O_RDONLY,777);

    perror(pfile);

	if(fp <=0 )
	{
		pthread_mutex_unlock(&stgContextp->imagemsg_lock);
		return 0;
	}

    char *pbuf = malloc(CMS_PIXEL_H * CMS_PIXEL_W / 4 + 64);

	lseek(fp,0,SEEK_SET);		
	i = read(fp,pbuf,CMS_PIXEL_H * CMS_PIXEL_W / 4 + 4);
    close(fp);
	pthread_mutex_unlock(&stgContextp->imagemsg_lock);


	i=0;
    w = pbuf[i++];
	w <<= 8;
	w |=pbuf[i++];

    h = pbuf[i++];
	h <<= 8;
	h |=pbuf[i++];

    if(h > CMS_PIXEL_H)
    {
		h = CMS_PIXEL_H;
    }
	if(w > CMS_PIXEL_W)
	{
		w = CMS_PIXEL_W;
	}
	
	s = 0;
	byte = pbuf[i++];			  
	for(y=0;y<h;y++)
	{
		for(x=0;x<w;x++)
		{
            rg = byte & 0x03;
            dest[y*vw + x] = GetColor(rg >> 1 | rg << 1); 

		    byte >>= 2; 			
			s++;
			if(s >=4)
			{
              s = 0;
			  byte = pbuf[i++];			  
			}
			
		}
	}
	free(pfile);
    free(pbuf);

	return 0;
}

void PutInsertStringToDisplay(ST_CONTEXT_TAG *stContextp,ST_INSERT_Mes *pStr)
{
	int fp,ret,i,kk;
	unsigned short code;
	unsigned short cmsx,cmsy;
	unsigned int addr,len;
	unsigned char *buf;
	unsigned short fcolor,bcolor;
	const unsigned char glitter_sleep[]={0,2,1,4};
    const unsigned char fontsize[2]={16,32};

    char *pfiles = malloc(64);

	memset(pfiles,0x00,64);
	sprintf(pfiles,"/tc/font%dX%d",fontsize[pStr->wt],fontsize[pStr->wt]);

	fp = open(pfiles,O_RDONLY,777);
	if(fp < 0)
	{
        perror(pfiles);
		return -1;
	}

	
	buf = malloc(290);
	cmsx = pStr->StX[0];
	cmsx <<= 8;
	cmsx|= pStr->StX[1];

	cmsy = pStr->StY[0];
	cmsy <<= 8;
	cmsy|= pStr->StY[1];
	
	for(i = 0;i<pStr->MesSize;i++)
	{
		int temp;
		unsigned char glitter;
		
		code = pStr->mWord[i][0]<<8 | pStr->mWord[i][1];
		fcolor = GetColor(pStr->mCl[i].stcl.font);
		bcolor = GetColor(pStr->mCl[i].stcl.Background);			
		/*
		if(!pStr->wt)
		{			
			if((code >= 0xfa40)&&(code <= 0xfefe))//自己造字
			{
			}
			else if( ((code >= 0x2100)&&(code <= 0x25FF))
			   &&( (((code&0xff)>='0')&&((code&0xff)<='9')) 
				 ||(((code&0xff)>='A')&&((code&0xff)<='Z'))
				 ||(((code&0xff)>='a')&&((code&0xff)<='z')) )				 
			   )
			{
			}
			else
			{
				fcolor = GetColor(pStr->mCl[i].stcl.Background);
				bcolor = GetColor(pStr->mCl[i].stcl.font);			
			}
		}
		*/
		glitter= pStr->mCl[i].stcl.flash;

		printf("Code=%X\n",code);		
		if(glitter)
		{
			if(stContextp->disUpdateTimeUnit > DISPLAY_UPDATETIME_FLASH)
			{
				stContextp->disUpdateTimeUnit = DISPLAY_UPDATETIME_FLASH;				
			}
			temp = (stContextp->countTickComm / (DISPLAY_UPDATETIME_FLASH * glitter_sleep[glitter])) % 2;
			if(temp)//显背景
			{
				ViewMemSet(stContextp->CmsBuf,CMS_PIXEL_W,CMS_PIXEL_H,bcolor,cmsx,cmsy,fontsize[pStr->wt],fontsize[pStr->wt]);				
				cmsx += fontsize[pStr->wt];
				if(cmsx >= CMS_PIXEL_W)//换行
				{
					break;
				}
				continue;
			}
		}
		
		if((code >= 0xfa40)&&(code <= 0xfefe))//自己造字
		{
			ST_USER_CHINESE_FONT_TAG *fontp;

			fontp = (ST_USER_CHINESE_FONT_TAG *)malloc(sizeof(ST_USER_CHINESE_FONT_TAG));
			memset(fontp,0x00,sizeof(ST_USER_CHINESE_FONT_TAG));
			fontp->ucBig5Code[0] = code >> 8;
			fontp->ucBig5Code[1] = code & 0xff;
			GetSelfFontLib(fontp);
			if((!fontp->ucFrameRow)||(!fontp->ucFrameColumn))
			{
				memset(fontp->ucBitMap,0xff,sizeof(fontp->ucBitMap));
			}
			FontLibToFullColorCms_Part(stContextp->CmsBuf,CMS_PIXEL_W,CMS_PIXEL_H,cmsx,cmsy,fcolor,bcolor,1,fontp->ucBitMap,fontsize[pStr->wt],fontsize[pStr->wt],CMS_FONT_VIEW_V);
			free(fontp);
			cmsx += fontsize[pStr->wt];
		}
		//else if((code >= 0x2130)&&(code <= 0x257a))//自己造图
		else if( ((code >= 0x2100)&&(code <= 0x25FF))
			   &&( (((code&0xff)>='0')&&((code&0xff)<='9')) 
				 ||(((code&0xff)>='A')&&((code&0xff)<='Z'))
				 ||(((code&0xff)>='a')&&((code&0xff)<='z')) )				 
			   )
		{
			unsigned short *pbuf;
			
			pbuf = (unsigned short *)malloc(CMS_FONT_VIEW_W * CMS_FONT_VIEW_V * sizeof(pbuf)); 
			memset(pbuf,0x00,(CMS_FONT_VIEW_W * CMS_FONT_VIEW_V * sizeof(pbuf)));
			SelfImageToView(pbuf,&code,bcolor);
			ViewToView(stContextp->CmsBuf,pbuf,CMS_PIXEL_W,CMS_PIXEL_H,cmsx,cmsy,fontsize[pStr->wt],fontsize[pStr->wt],0,0,CMS_FONT_VIEW_W,CMS_FONT_VIEW_W);
			free(pbuf);

			cmsx += fontsize[pStr->wt];
		}
		else if(code < 0x8000)
		{
			unsigned char *asciic,k;

			asciic = pStr->mWord[i]; 		 
			for(k=0;k<2;k++)
			{
				addr = 0x0800 + asciic[k]*(fontsize[pStr->wt]/2 * fontsize[pStr->wt] / 8 + (pStr->wt != 0));// 16型的字庫無校驗碼
				lseek(fp,addr,SEEK_SET);		
				len = read(fp,buf,fontsize[pStr->wt] * (fontsize[pStr->wt]/2) / 8 + (pStr->wt != 0));//48 * 24/8 + 1(crc)	

				if(!pStr->wt)// 16X16
                {
                  for(kk = 0;kk < len;kk++)
                  {
					  buf[kk]=~buf[kk];
				  }
				}
				
				FontLibToFullColorCms_Insert(stContextp->CmsBuf,CMS_PIXEL_W,CMS_PIXEL_H,cmsx,cmsy,fcolor,bcolor,buf,(fontsize[pStr->wt]/2),fontsize[pStr->wt]);

				cmsx += (fontsize[pStr->wt]/2);
				
				if(cmsx >= CMS_PIXEL_W)//换行
				{	
					break;
				}
			}					  
		}
		else if(code >= 0xA140)
		{
			unsigned char r,h;//行列地址 89 X 157的表格
			unsigned char *asciic;
			
			asciic = pStr->mWord[i]; 		 

			if(asciic[1] <= 0x7E)
			{
				r = asciic[1] - 0x40;
			}
			else
			{
				r = asciic[1] - 0xA1 + 0x3F; // 0x7E - 0x40 + 1 = 0x3f;
			}
			h = asciic[0] - 0xA1;
			
			addr = 0x0800+0x80*(fontsize[pStr->wt]/2 * fontsize[pStr->wt] / 8 + (pStr->wt != 0)); // 0x0800 + 145 * 0x80 = 5080
			addr += ((h*157)+r) * (fontsize[pStr->wt] * fontsize[pStr->wt] / 8 + (pStr->wt != 0)); // 0x0800 + 145 * 0x80 = 5080

			lseek(fp,addr,SEEK_SET);		
			len = read(fp,buf,(fontsize[pStr->wt] * fontsize[pStr->wt] / 8 + (pStr->wt != 0)));//48 * 48 / 8 + 1(crc)		
			if(!pStr->wt)// 16X16
			{
			  for(kk = 0;kk < len;kk++)
			  {
				  buf[kk]=~buf[kk];
			  }
			}

			FontLibToFullColorCms_Insert(stContextp->CmsBuf,CMS_PIXEL_W,CMS_PIXEL_H,cmsx,cmsy,fcolor,bcolor,buf,fontsize[pStr->wt],fontsize[pStr->wt]);
								  
			cmsx += fontsize[pStr->wt];
		}
		if(cmsx >= CMS_PIXEL_W)//换行
		{
			break;
		}
		if(cmsy >= CMS_PIXEL_H)
		{
			break;
		}
		
	}
	free(buf);
	close(fp);
}


void PutInsertMesToDisplay(ST_CONTEXT_TAG *stContextp,unsigned char textid)
{
	int i,n,j;
	int len,index;
	unsigned char *pbuf;
    ST_AF1AH_SET_INSERT *pInsert = (ST_AF1AH_SET_INSERT *)malloc(sizeof(ST_AF1AH_SET_INSERT));

    pbuf = malloc(sizeof(ST_AF1AH_SET_INSERT));
	pbuf[0]=textid;
    len = GetPresetInsertContext(pbuf);    
    if(len <=0)
    {
		return;
    }

	index = 0;
	pInsert->TextId = pbuf[index++];
	pInsert->clt = pbuf[index++];
	pInsert->AreaCount = pbuf[index++];
	for(i=0;i<pInsert->AreaCount;i++)
	{
		pInsert->area[i].StX[0]=pbuf[index++];
		pInsert->area[i].StX[1]=pbuf[index++];

		pInsert->area[i].StY[0]=pbuf[index++];
		pInsert->area[i].StY[1]=pbuf[index++];

		pInsert->area[i].width[0]=pbuf[index++];
		pInsert->area[i].width[1]=pbuf[index++];
		
		pInsert->area[i].hight[0]=pbuf[index++];
		pInsert->area[i].hight[1]=pbuf[index++];

		pInsert->area[i].cla.byte=pbuf[index++];
	}
	
	pInsert->MesCount = pbuf[index++];
    for(i=0;i<pInsert->MesCount;i++)
    {
        pInsert->Mes[i].StX[0]=pbuf[index++];
        pInsert->Mes[i].StX[1]=pbuf[index++];
        pInsert->Mes[i].StY[0]=pbuf[index++];
        pInsert->Mes[i].StY[1]=pbuf[index++];

	    pInsert->Mes[i].wt = pbuf[index++];
		pInsert->Mes[i].MesSize = pbuf[index++];
		for(n=0;n<pInsert->Mes[i].MesSize;n++)
		{
            pInsert->Mes[i].mWord[n][0]=pbuf[index++];
            pInsert->Mes[i].mWord[n][1]=pbuf[index++];
		}
		for(n=0;n<pInsert->Mes[i].MesSize;n++)
		{
            pInsert->Mes[i].mCl[n].byte=pbuf[index++];
		}		
	}

    int x,y,w,h;
	unsigned short cl;

	printf("AreaCount=%d\n",pInsert->AreaCount);
    for(i=0;i<pInsert->AreaCount;i++)
    {
        x = pInsert->area[i].StX[0];
		x <<= 8;
		x |=pInsert->area[i].StX[1];

		y = pInsert->area[i].StY[0];
		y <<= 8;
		y |=pInsert->area[i].StY[1];

		w = pInsert->area[i].width[0];
		w <<= 8;
		w |=pInsert->area[i].width[1];

		h = pInsert->area[i].hight[0];
		h <<= 8;
		h |=pInsert->area[i].hight[1];

        cl = GetColor(pInsert->area[i].cla.stcl.Background);

		printf("i=%d,x=%d,y=%d\n",i,x,y);
		for(n=y;n<y+h;n++)
		{
            if(n >=CMS_PIXEL_H)
				break;
			for(j=x;j<x+w;j++)
			{
	           if(j>=CMS_PIXEL_W)
				   break;

	           index = n * CMS_PIXEL_W+j;
			   stContextp->CmsBuf[index]=cl;
			}
		}
	}

    for(i=0;i<pInsert->MesCount;i++)
    {
		PutInsertStringToDisplay(stContextp,&pInsert->Mes[i]);
    }

    free(pbuf);
	free(pInsert);
}


int Show_CharText(ST_CONTEXT_TAG *stContextp)
{
	unsigned char CommState,EthState,state;
	unsigned char temp;
	static unsigned int count=0;
    static unsigned int currTextId=0;
	unsigned int ShowTextId;
    unsigned char Cms_bOn;

	Cms_bOn = TRUE;

	//CommState = (stContextp->PortCenter_Com0.lineStatue & stContextp->PortCenter_Com1.lineStatue & stContextp->PortCenter_Com2.lineStatue);
    //EthState = stContextp->PortCenter_Eth.lineStatue;

    if(stContextp->PortLocate_p->lineStatue == CONNECT_ONLINE)
    {//使用现场端口
		state = stContextp->PortLocate_p->lineStatue;
	}
	else
	{//使用远端端口
		state = stContextp->PortRemote_p->lineStatue;
	}

	ShowTextId = 0;
	switch(state)
	{
		case CONNECT_ONLINE:
		{
			if(stContextp->stSysConfig.ucCurrShow_MsgId)//保持当前
			{
				ShowTextId = stContextp->stSysConfig.ucCurrShow_MsgId;
			}
			else if( (stContextp->stSysConfig.ST_CIRCULATION_DISPLAY.ucTextCount)
			  &&(stContextp->stSysConfig.ST_CIRCULATION_DISPLAY.ucDisplayTime) )//循环显示参数正确
			{
				if(stContextp->disUpdateTimeUnit > DISPLAY_UPDATETIME_FLASH)
				{
					stContextp->disUpdateTimeUnit = DISPLAY_UPDATETIME_FLASH;				
				}
				
				count++;
				temp = (count * stContextp->disUpdateTimeUnit)/1000;
				if(temp >= (stContextp->stSysConfig.ST_CIRCULATION_DISPLAY.ucDisplayTime))
				{
					count=0;
					currTextId++;
				}
				if(currTextId >= stContextp->stSysConfig.ST_CIRCULATION_DISPLAY.ucTextCount)
				{  
					currTextId = 0;
				}
				ShowTextId = stContextp->stSysConfig.ST_CIRCULATION_DISPLAY.ucTextID[currTextId]; 
			}
			else//当前显示已清除
			{
				Cms_bOn = FALSE;
			}
		}
		break;

		case CONNECT_OFFLINE:
		{
			//printf("textid=%d,cmsbon=%d\n",ShowTextId,Cms_bOn);
			if(stContextp->stSysConfig.stOffLine.disp == OFFLINEMODE_OFF)
			{
				Cms_bOn = FALSE;
			}
			else if(stContextp->stSysConfig.stOffLine.disp == OFFLINEMODE_CYCLE)
			{
				if( (stContextp->stSysConfig.ST_CIRCULATION_DISPLAY.ucTextCount)
				  &&(stContextp->stSysConfig.ST_CIRCULATION_DISPLAY.ucDisplayTime) )//循环显示参数正确
				{		
					if(stContextp->disUpdateTimeUnit > DISPLAY_UPDATETIME_FLASH)
					{
						stContextp->disUpdateTimeUnit = DISPLAY_UPDATETIME_FLASH;				
					}
					
					count++;
					temp = (count * stContextp->disUpdateTimeUnit)/1000;
					if(temp >= (stContextp->stSysConfig.ST_CIRCULATION_DISPLAY.ucDisplayTime))
					{
						count=0;
						currTextId++;
					}
					if(currTextId >= stContextp->stSysConfig.ST_CIRCULATION_DISPLAY.ucTextCount)
					{
						currTextId = 0;
					}
					ShowTextId = stContextp->stSysConfig.ST_CIRCULATION_DISPLAY.ucTextID[currTextId];							
				}
			}
			else if(  (stContextp->stSysConfig.stOffLine.disp == OFFLINEMODE_HOLD)
				    ||((stContextp->stSysConfig.stOffLine.disp == OFFLINEMODE_DELAY)&&((stContextp->offlineTime/60) < stContextp->stSysConfig.stOffLine.delay) )
                   )
			{	
				//从循环显示模式进入的离线状态
				if(stContextp->stSysConfig.ucCurrShow_MsgId)//保持当前
				{
					ShowTextId = stContextp->stSysConfig.ucCurrShow_MsgId;
				}
				else if( (stContextp->stSysConfig.ST_CIRCULATION_DISPLAY.ucTextCount)
				  &&(stContextp->stSysConfig.ST_CIRCULATION_DISPLAY.ucDisplayTime) )//循环显示参数正确						
				{
					ShowTextId = stContextp->stSysConfig.ST_CIRCULATION_DISPLAY.ucTextID[currTextId];							
				}
				
				else//当前显示已清除
				{
					Cms_bOn = FALSE;
				}
			}
			else
			{
				Cms_bOn = FALSE;//默认是关闭的
			}			
		}
		break;
		
		case CONNECT_BREAK:
		if(stContextp->stSysConfig.stBreakLine.disp == OFFLINEMODE_OFF)
		{
			Cms_bOn = FALSE;
		}
		else if(stContextp->stSysConfig.stBreakLine.disp == OFFLINEMODE_CYCLE)
		{
			if( (stContextp->stSysConfig.ST_CIRCULATION_DISPLAY.ucTextCount)
			  &&(stContextp->stSysConfig.ST_CIRCULATION_DISPLAY.ucDisplayTime) )//循环显示参数正确
			{		
				if(stContextp->disUpdateTimeUnit > DISPLAY_UPDATETIME_FLASH)
				{
					stContextp->disUpdateTimeUnit = DISPLAY_UPDATETIME_FLASH;				
				}
				
				count++;
				temp = (count * stContextp->disUpdateTimeUnit)/1000;
				if(temp >= (stContextp->stSysConfig.ST_CIRCULATION_DISPLAY.ucDisplayTime))
				{
					count=0;
					currTextId++;
				}
				if(currTextId >= stContextp->stSysConfig.ST_CIRCULATION_DISPLAY.ucTextCount)
				{
					currTextId = 0;
				}
				ShowTextId = stContextp->stSysConfig.ST_CIRCULATION_DISPLAY.ucTextID[currTextId];							
			}
		}
		else if(  (stContextp->stSysConfig.stBreakLine.disp == OFFLINEMODE_HOLD)
			    ||((stContextp->stSysConfig.stBreakLine.disp == OFFLINEMODE_DELAY)&&((stContextp->offlineTime/60) < stContextp->stSysConfig.stOffLine.delay) )
               )
		{	
			//从循环显示模式进入的离线状态
		    if(stContextp->stSysConfig.ucCurrShow_MsgId)//保持当前
			{
				ShowTextId = stContextp->stSysConfig.ucCurrShow_MsgId;
			}	
			else if( (stContextp->stSysConfig.ST_CIRCULATION_DISPLAY.ucTextCount)
			  &&(stContextp->stSysConfig.ST_CIRCULATION_DISPLAY.ucDisplayTime) )//循环显示参数正确						
			{
				ShowTextId = stContextp->stSysConfig.ST_CIRCULATION_DISPLAY.ucTextID[currTextId];							
			}
			
			else//当前显示已清除
			{
				Cms_bOn = FALSE;
			}
		}
		else
		{
			Cms_bOn = FALSE;//默认是关闭的
		}			
		break;
	}

	memset(stContextp->CmsBuf,0x00,sizeof(stContextp->CmsBuf));
	if(Cms_bOn && ShowTextId)
	{
		if(ShowTextId <= 127)
		{
			unsigned char nouser=0;
			
			GetPresetTextToBuf(stContextp->CmsBuf,CMS_PIXEL_W,CMS_PIXEL_H,ShowTextId,&nouser);  
		}
		else
		{
			GetPresetMsgToBuf(stContextp->CmsBuf,CMS_PIXEL_W,CMS_PIXEL_H,ShowTextId,0);
		}
		
		PutInsertMesToDisplay(stContextp,ShowTextId);
		return 1;
	}
	return 0;
}

int CommTestSendToCms(ST_CONTEXT_TAG *stContextp)
{
	int CmsCardReady;
	int ret,count;
	unsigned char i;

	CmsCardReady = 0; 
    for(i=0;i<_CMS_CTRCARD_NUM;i++)
    {
		stContextp->Sem_CmsTickAck[i]=-1;
		pthread_mutex_lock(&stContextp->PortCms.stSendInfoLock);
		stContextp->PortCms.scmd = A001H_GET_CMS_TICK;
		stContextp->PortCms.infolen = 2;
		stContextp->PortCms.stSendInfo.ucbuf[0] = i;
		stContextp->PortCms.stSendInfo.ucbuf[1] = _CMS_CTRCARD_NUM;

		ret = AddInfoToSendList(&stContextp->PortCms, _SEND_INFO);
		pthread_mutex_unlock(&stContextp->PortCms.stSendInfoLock);		

        count = 0;
		while(count++ < 100)
		{	
			usleep(1000*100);
			if((ret = stContextp->Sem_CmsTickAck[i])==i)
			{
				break;
			}
	    }
		if(ret != i)//串联直接退出
		{
			CmsCardReady = i+1; 
			break;
        }
	}

	if((stContextp->CmsCardReady != CmsCardReady)&&(!CmsCardReady))//重新连接上时做一次全检
	{
		CmsErrCheck(stContextp,0);
	}
	stContextp->CmsCardReady = CmsCardReady;
	
	return (0);	
}

#define RGB_LEVEL  0x1f
static const unsigned short TestColor[]={RGB_LEVEL<<5,RGB_LEVEL<<10,RGB_LEVEL<<5|RGB_LEVEL<<10};
int CmsTestMode(ST_CONTEXT_TAG *stContextp)
{
	int mode;
	int i,n;
	unsigned char  temp;
	unsigned short x,y;
	unsigned short *p;
	static unsigned char count=0;

    if(!stContextp->CmsMode)
		return 0;

    mode = stContextp->CmsMode-1;
    switch(mode)
    {
        case 0:
		case 1:
		case 2:
		for(i=0;i<sizeof(stContextp->CmsBuf)/sizeof(unsigned short);i++)//336 * 144
		{
			stContextp->CmsBuf[i] = TestColor[mode];//全屏同一色
        }
		break;

		case 3:	
		case 4:	
        case 5:
		{
			if(++count>3)
				count = 0;
			
#if (CMS_SPCE_CONFIG  == CMS_RG8X1_F32X32) 			
			for(y=0;y<CMS_PIXEL_H/4;y++) // 36 * 4 = 144行
			{
				for(i=0;i<4;i++)
				{
					for(n=0;n<(CMS_PIXEL_W / 4);n++) //84 -> CMS_PIXEL_W ; 84 * 4 = 336 列
					{
						p = &stContextp->CmsBuf[(y*4+i)*CMS_PIXEL_W+n*4]; // 336 -> CMS_PIXEL_W
						for(x=0;x<4;x++)
						{
							temp = count + i;	
							if(temp > 3)
								temp = temp - 4;
							if(x==temp)
								p[x]=0x7fff & TestColor[mode - 3];
							else
								p[x]=0x0000;
						}
					}
				}
			}           
#else
			for(y=0;y<_CMS_CTRCARD_DISP_V/4;y++) // 36 * 4 = 144行
			{
				for(i=0;i<4;i++)
				{
					for(n=0;n<(CMS_PIXEL_W / 4);n++) //84 -> CMS_PIXEL_W ; 84 * 4 = 336 列
					{
						p = &stContextp->CmsBuf[(y*4+i)*CMS_PIXEL_W+n*4]; // 336 -> CMS_PIXEL_W
						for(x=0;x<4;x++)
						{
							temp = count + i;	
							if(temp > 3)
								temp = temp - 4;
							if(x==temp)
								p[x]=0x7fff & TestColor[mode - 3];
							else
								p[x]=0x0000;
						}
					}
				}
			}
#endif			
		}
		break;	

		case 6:	
		if(count)
		{
            count = 0;
		    memset(stContextp->CmsBuf,0x00,sizeof(stContextp->CmsBuf));
		}
		else
		{
            count = 1;
		    memset(stContextp->CmsBuf,0xff,sizeof(stContextp->CmsBuf));
		}
		break;	
		
		case 7:
		case 8:
		case 9:	
		{
			memset(stContextp->CmsBuf,0x00,sizeof(stContextp->CmsBuf));
			if(++count>15)
				count = 0;
			for(y=count;y<CMS_PIXEL_H;y+=16)
			{
               for(i=0;i<CMS_PIXEL_W;i++) // CMS_PIXEL_W -> 336
               {
                   stContextp->CmsBuf[y*CMS_PIXEL_W + i] = TestColor[mode - 7];
			   }
			}
		}
		break;	
		
		default:
			return 0;
			break;
	}
	return 1;
}

#if(CMS_SPCE_CONFIG == CMS_RG1X1_F48X48_SHG)
void PreTreat_BreakLine(unsigned int *buf)
{    
	unsigned int temp[3],i;
	unsigned int n;
  
    temp[0] = buf[0] ^ buf[3];
    temp[1] = buf[1] ^ buf[4];	
    temp[2] = buf[2] ^ buf[5];
    for(i=0;i<_CMS_CTRCARD_SCAN_W * _CMS_CTRCARD_SCAN_RATE;i++)
    {
		for(n=0;n<3;n++)
		{
            buf[i*3+n]=temp[n];
		}
	}
}
#else
void PreTreat_BreakLine(unsigned int *buf)
{
	unsigned int temp,i;
   
    temp = buf[0] ^ buf[1];
    for(i=0;i<_CMS_CTRCARD_SCAN_W * _CMS_CTRCARD_SCAN_RATE;i++)
    {
        buf[i]=temp;
	}
}
#endif

#if(CMS_SPCE_CONFIG == CMS_RG1X1_F48X48_SHG)
void GetErrModule(ST_CONTEXT_TAG *stContextp)
{
	int i,n,m,j;
	unsigned int *temp;
    unsigned int rgb,sb,bi,tt;
    unsigned int x,y;

    memset(stContextp->CmsModule,0x00,sizeof(stContextp->CmsModule));
    for(i=0;i<_CMS_CTRCARD_SCAN_RATE;i++)
    {			
		for(n=0;n<_CMS_CTRCARD_SCAN_W;n++)
		{			    	
           temp = stContextp->CmsLedS[0][i][n];

		   //printf("temp=%x ",temp[0]);
		   for(m=0;m<9;m++)
		   {
			   bi = (m*3)/32;
               sb = (m*3)%32;
			   rgb = 7ul & (temp[bi]>>sb);
			   if((sb+3)>31)
			   {
                   sb = 32-sb;//当前字占用的位数
				   tt = (7ul>>sb)&temp[bi+1];
				   tt <<= sb;
				   rgb |= tt;
			   }
               y = m%3;
			   x = m/3;
			   sb = y*3 + x;
               if((rgb&0x03) !=0x03UL)
               {
			      stContextp->CmsModule[0][sb]++;
			   }
		   }
		}
    }

	printf("modules:\n");
    memset(&stContextp->ST_CMS_MODULE_ERR,0x00,sizeof(ST_RET_DISPLAY_MODULE_STATUS_TAG));
    for(m=0;m<9;m++)
	{
		if(stContextp->CmsModule[0][m])
		{
			printf("%d:%d ",m+1,stContextp->CmsModule[0][m]);
		}
		if(stContextp->CmsModule[0][m] > 62) // 512 * 8% = 42
		{					
			stContextp->ST_CMS_MODULE_ERR.ucDisplayMOduleId[stContextp->ST_CMS_MODULE_ERR.ucErrModuleNO++] = m+1;
		}
	}				  		
	printf("\n");
}
void CmsCaseToImage(unsigned short *pdest,unsigned int psrc[_CMS_CTRCARD_SCAN_RATE][_CMS_CTRCARD_SCAN_W][3])
{
	unsigned short x,y;
    unsigned int *temp;
    unsigned short r,g,b;
	unsigned int rgb,tt;

    int i,m;

	int bi,sb;	
    int x_4848,y_4848,xf,yf;
 
	for(i=0;i<_CMS_CTRCARD_SCAN_RATE;i++)
	{
		for(xf=0;xf<_CMS_CTRCARD_SCAN_W;xf++)
		{     
			temp = psrc[i][xf];
			for(m=0;m<9;m++)// 9是一列口的模组个数
			{
	            bi = (3*m)/32;
				sb = (3*m)%32;
				rgb = (temp[bi]>>sb)&0x07UL;
	            if((32-sb)<3)
	            {
	                 sb = 32-sb;//当前字占用的位数
					 tt = (7ul>>sb)&temp[bi+1];
					 tt <<= sb;
					 rgb |= tt;
				}
				yf = m*8 + i;
				Get32X72To48X48(xf,yf,&x_4848,&y_4848);
				
				r = (rgb & 0x01)? 0x1f : 0;
				g = (rgb & 0x02)? 0x1f : 0;
				b = (rgb & 0x04)? 0x1f : 0;

	            r <<= (5 - CMS_COLOR_BITNUM);
	            g <<= (5 - CMS_COLOR_BITNUM);
	            b <<= (5 - CMS_COLOR_BITNUM);

	            pdest[y_4848*_CMS_CTRCARD_DISP_W + x_4848] = r << 10 | g << 5 | b;						
			}
	    }
	}
}

#elif (CMS_SPCE_CONFIG  == CMS_RG6X1_F32X32)
void GetErrModule(ST_CONTEXT_TAG *stContextp)
{
	int k,i,n,m,j;
	unsigned int temp;
    unsigned char id,s;

    memset(stContextp->CmsModule,0x00,sizeof(stContextp->CmsModule));
    for(i=0;i<_CMS_CTRCARD_SCAN_RATE;i++)
    {			
		for(n=0;n<_CMS_CTRCARD_SCAN_W;n++)
		{
           temp = stContextp->CmsLedS[0][i][n];
		   for(m=0;m<8;m++)
		   {
		   	   id = n / 32 * 2;
               s = m/2 *6 + m%2 + id;
			   if((temp & 0x06ul) != 0x06)
			   {				   	  
			      stContextp->CmsModule[s]++;
			   }
			   temp >>= 3;
		   }
		}
    }

    temp = stContextp->stSysConfig.tMisc.Mfail_LedNum;
    temp *=256;
	temp /=100;

    memset(&stContextp->ST_CMS_MODULE_ERR,0x00,sizeof(ST_RET_DISPLAY_MODULE_STATUS_TAG));
	printf("modules:\n");
	for(i=0;i<24;i++)
	{
		if(stContextp->CmsModule[i])
		{
			printf("%d:%d ",i+1,stContextp->CmsModule[i]);
		}
        if(stContextp->CmsModule[i]>temp)// 512 * 8% = 42
        {
			stContextp->ST_CMS_MODULE_ERR.ucDisplayMOduleId[stContextp->ST_CMS_MODULE_ERR.ucErrModuleNO++] = i+1;
		}
	}
	printf("\n");
}

//灯位状态图转换成图片
void CmsCaseToImage(unsigned short *pdest,unsigned int *psrc)
{
	unsigned char k;
	unsigned short i,j;
	unsigned short x,y,n;
    unsigned int temp;
    unsigned short r,g,b;
    
	
	for(j=0;j<_CMS_CTRCARD_SCAN_RATE;j++)
	{
		for(i=0;i<_CMS_CTRCARD_SCAN_W;i++)
		{
		    temp = psrc[j*_CMS_CTRCARD_SCAN_W+i];
			for(k=0;k<8;k++)
			{				
				b = (temp & 0x01)? 0x1f : 0;
                temp >>= 1;
				g = (temp & 0x01)? 0x1f : 0;
                temp >>= 1;				 
				r = (temp & 0x01)? 0x1f : 0;
                temp >>= 1;

                y = i/32 * 16;
				y *= (k/2);				
				y +=j;
				n = i%32;
				if(n >= 16)
				{
                    y += 8;
				}

			    if(n < 16)
			    {
                  x = n;
				}
				else
				{
                  x = 32 - n; 
				}
                x += ((k%2)*16);

                pdest[y*_CMS_CTRCARD_DISP_W + x] = r << 10 | g << 5 | b;				
			}
	    }
	}
}
#else
//灯位状态图转换成图片
void CmsCaseToImage(unsigned short *pdest,unsigned int *psrc)
{
	unsigned char k;
	unsigned short i,j;
	unsigned short x,y;
    unsigned int temp;
    unsigned short r,g,b;
	
	for(j=0;j<_CMS_CTRCARD_SCAN_RATE;j++)
	{
		for(i=0;i<_CMS_CTRCARD_SCAN_W;i++)
		{
		    temp = psrc[j*_CMS_CTRCARD_SCAN_W+i];
			for(k=0;k<_CMS_CTRCARD_DISP_V/16;k++)
			{				
				b = (temp & 0x01)? 0x1f : 0;
                temp >>= 1;
				g = (temp & 0x01)? 0x1f : 0;
                temp >>= 1;				
				r = (temp & 0x01)? 0x1f : 0;
                temp >>= 1;

				y =((i/16) % 2)? k*16+j : k*16+8+j;
				x =((i/16) % 2)? 15 - i%16 + (i/32)*16 : i%16 + (i/32)*16;

                if((k<3)&&(!r || !g))
                {
                 //  printf("point j=%d,i=%d,k=%d,x=%d,y=%d\n",j,i,k,x,y);
				}
				
                pdest[y*_CMS_CTRCARD_DISP_W + x] = r << 10 | g << 5 | b;				
			}
	    }
	}
}

#if (CMS_SPCE_CONFIG  == CMS_RG8X1_F32X32) 
void GetErrModule(ST_CONTEXT_TAG *stContextp)
{
	int k,i,n,m,j;
	unsigned int temp;

    memset(stContextp->CmsModule,0x00,sizeof(stContextp->CmsModule));
	for(k=0;k<_CMS_CTRCARD_NUM;k++)
	{
        for(i=0;i<_CMS_CTRCARD_SCAN_RATE;i++)
        {			
			for(n=0;n<_CMS_CTRCARD_SCAN_W;n++)
			{
               temp = stContextp->CmsLedS[k][i][n];
			   for(m=0;m<_CMS_CTRCARD_DISP_V/16;m++)
			   {
				   if((temp & 0x06ul) != 0x06)
				   {				   	  
				      stContextp->CmsModule[k][m][n/32]++;
				   }
				   temp >>= 3;
			   }
			}
        }
	}

    temp = stContextp->stSysConfig.tMisc.Mfail_LedNum;
    temp *=256;
	temp /=100;

	printf("modules:\n");
    memset(&stContextp->ST_CMS_MODULE_ERR,0x00,sizeof(ST_RET_DISPLAY_MODULE_STATUS_TAG));
	for(k=0;k<_CMS_CTRCARD_NUM;k++)
	{
		for(m=0;m<8;m++)//垂直方向个模组
		{
			for(j=0;j<2;j++)//水平方向个模组
			{
				i = m * 2+j+ k * 16;
				if(stContextp->CmsModule[k][m][j])
				{
					printf("%d:%d ",i+1,stContextp->CmsModule[k][m][j]);
				}
				if(stContextp->CmsModule[k][m][j] > temp) // 512 * 8% = 42
				{					
					stContextp->ST_CMS_MODULE_ERR.ucDisplayMOduleId[stContextp->ST_CMS_MODULE_ERR.ucErrModuleNO++] = i+1;
				}
			}				  
		}
	}
	printf("\n");
}

#else
void GetErrModule(ST_CONTEXT_TAG *stContextp)
{
	int k,i,n,m,j;
	unsigned int temp;

    memset(stContextp->CmsModule,0x00,sizeof(stContextp->CmsModule));
	for(k=0;k<_CMS_CTRCARD_NUM;k++)
	{
        for(i=0;i<_CMS_CTRCARD_SCAN_RATE;i++)
        {			
			for(n=0;n<_CMS_CTRCARD_SCAN_W;n++)
			{
               temp = stContextp->CmsLedS[k][i][n];
			   for(m=0;m<_CMS_CTRCARD_DISP_V/16;m++)
			   {
				   if((temp & 0x06ul) != 0x06)
				   {				   	  
				      stContextp->CmsModule[k][m][n/32]++;
				   }
				   temp >>= 3;
			   }
			}
        }
	}

	printf("modules:\n");
	/*
	if(stContextp->CmsMode == 0xff)	
	{
		stContextp->ST_CMS_MODULE_ERR.ucErrModuleNO = 0;
	}
	else
	*/	
	{
	    memset(&stContextp->ST_CMS_MODULE_ERR,0x00,sizeof(ST_RET_DISPLAY_MODULE_STATUS_TAG));
	}

    if((stContextp->stSysConfig.tMisc.Mfail_LedNum <2)||(stContextp->stSysConfig.tMisc.Mfail_LedNum > 50))
    {
		stContextp->stSysConfig.tMisc.Mfail_LedNum = 2;
	}

    temp = stContextp->stSysConfig.tMisc.Mfail_LedNum;
    temp *=256;
	temp /=100;
	
	for(k=0;k<_CMS_CTRCARD_NUM;k++)
	{
#if TEST_9CELL_ENB		
        for(m=0;m<3;m++)
#else
		for(m=0;m<(_CMS_CTRCARD_DISP_V/16);m++)//垂直方向个模组
#endif			
		{
			for(j=0;j<(_CMS_CTRCARD_DISP_W/16);j++)//水平方向个模组
			{
				//i = m * (_CMS_CTRCARD_NUM * 3)+ k*3 + j;
				i = m * (_CMS_CTRCARD_NUM * (_CMS_CTRCARD_DISP_W/16))+ k * (_CMS_CTRCARD_DISP_W/16) + j;
				if(stContextp->CmsModule[k][m][j])
				{
					printf("%d:%d ",i+1,stContextp->CmsModule[k][m][j]);
				}
/*
			    if(stContextp->CmsMode == 0xff)
			    {
					if(stContextp->CmsModule[k][m][j] >= 1) // 512 * 8% = 42
					{					
						stContextp->ST_CMS_MODULE_ERR.ucDisplayMOduleId[i] = i+1;
					}
				}
				else
*/					
				{
					if(stContextp->CmsModule[k][m][j] > temp) // 512 * 8% = 42
					{					
						stContextp->ST_CMS_MODULE_ERR.ucDisplayMOduleId[stContextp->ST_CMS_MODULE_ERR.ucErrModuleNO++] = i+1;
					}
				}
			}				  
		}
	}
	printf("\n");
}
#endif

#endif


void SetErrModule_ClearDis(ST_CONTEXT_TAG *stContextp)
{
    int i,module;   
    int x,y,v_num;
    int j,n;
    int modulenum;

#if TEST_9CELL_ENB
	v_num = 3;
#elif(CMS_SPCE_CONFIG  == CMS_RG8X1_F32X32)
    v_num = 2;
#else
	v_num = _CMS_CTRCARD_NUM * _CMS_CTRCARD_DISP_W / 16;//一行的模组个数
#endif
	
    modulenum = (stContextp->CmsMode == 0xff)? sizeof(stContextp->ST_CMS_MODULE_ERR.ucDisplayMOduleId) : stContextp->ST_CMS_MODULE_ERR.ucErrModuleNO;
	for(i=0;i<modulenum;i++)
	{
		if(stContextp->ST_CMS_MODULE_ERR.ucDisplayMOduleId[i])
		{
	        module = stContextp->ST_CMS_MODULE_ERR.ucDisplayMOduleId[i] - 1;
			y = (module / v_num) * 16;
			x = (module % v_num) * 16;
			for(j=y;j<y+16;j++)
			{
				n = j * CMS_PIXEL_W+x;
				memset(&stContextp->CmsBuf[n],0x00,16 * sizeof(unsigned short));
			}
		}
	}
}

#if (CMS_SPCE_CONFIG == CMS_RG1X1_F48X48_SHG)
void CreatCmsLedstatues(ST_CONTEXT_TAG *stContextp)
{
	unsigned int n;
	unsigned int i,k,line;
	unsigned int  *errs,*erro,*errb,err;	
	
    unsigned int *temp;

	temp = (unsigned int *)malloc(sizeof(unsigned int) * _CMS_CTRCARD_SCAN_W*3);
	for(k=0;k<_CMS_CTRCARD_NUM;k++)//短路或开路时灯都是不亮的(去掉了消隐电阻)
	{
		for(line = 0;line < _CMS_CTRCARD_SCAN_RATE;line++)
		{
			erro = (unsigned int *)&stContextp->CmsErrBuf[0][k][line * _CMS_CTRCARD_SCAN_W * 12];
			errs = (unsigned int *)&stContextp->CmsErrBuf[1][k][line * _CMS_CTRCARD_SCAN_W * 12];
			errb = (unsigned int *)&stContextp->CmsErrBuf[2][k][line * _CMS_CTRCARD_SCAN_W * 12];
			for(i=0;i<_CMS_CTRCARD_SCAN_W;i++)
			{
				for(n=0;n<3;n++)
				{
					err = erro[i*3+n] & errs[i*3+n] & errb[i*3+n];
					stContextp->CmsLedS[k][line][i][n] = err;
				}
			}
		}
		memcpy(temp,stContextp->CmsLedS[k][0],sizeof(unsigned int) * _CMS_CTRCARD_SCAN_W*3);
		memcpy(stContextp->CmsLedS[k][0],stContextp->CmsLedS[k][1],sizeof(unsigned int) * _CMS_CTRCARD_SCAN_W * (_CMS_CTRCARD_SCAN_RATE-1)*3);
		memcpy(stContextp->CmsLedS[k][_CMS_CTRCARD_SCAN_RATE-1],temp,sizeof(unsigned int) * _CMS_CTRCARD_SCAN_W* 3);
	}	
}


#else
void CreatCmsLedstatues(ST_CONTEXT_TAG *stContextp)
{
	unsigned int i,k,line;
	unsigned int  *errs,*erro,*errb,err;	
    unsigned int *temp;

	temp = (unsigned int *)malloc(sizeof(unsigned int) * _CMS_CTRCARD_SCAN_W);
	for(k=0;k<_CMS_CTRCARD_NUM;k++)//短路或开路时灯都是不亮的(去掉了消隐电阻)
	{
		for(line = 0;line < _CMS_CTRCARD_SCAN_RATE;line++)
		{
			erro = (unsigned int *)&stContextp->CmsErrBuf[0][k][line * _CMS_CTRCARD_SCAN_W * 4];
			errs = (unsigned int *)&stContextp->CmsErrBuf[1][k][line * _CMS_CTRCARD_SCAN_W * 4];
			errb = (unsigned int *)&stContextp->CmsErrBuf[2][k][line * _CMS_CTRCARD_SCAN_W * 4];
			for(i=0;i<_CMS_CTRCARD_SCAN_W;i++)
			{
				err = erro[i] & errs[i] & errb[i];
				stContextp->CmsLedS[k][line][i] = err;
			}
		}
		memcpy(temp,stContextp->CmsLedS[k][0],sizeof(unsigned int) * _CMS_CTRCARD_SCAN_W);
		memcpy(stContextp->CmsLedS[k][0],stContextp->CmsLedS[k][1],sizeof(unsigned int) * _CMS_CTRCARD_SCAN_W * (_CMS_CTRCARD_SCAN_RATE-1));
		memcpy(stContextp->CmsLedS[k][_CMS_CTRCARD_SCAN_RATE-1],temp,sizeof(unsigned int) * _CMS_CTRCARD_SCAN_W);
	}	
    free(temp);
}
#endif

void savefile(char *filep,char *filebuf,int len)
{
	int fp;
	fp = open(filep,O_RDWR|O_CREAT,777);
	lseek(fp,0,SEEK_SET);
	write(fp,filebuf,len);
	close(fp);
}

#if(CMS_SPCE_CONFIG  == CMS_RG8X1_F32X32)
void CreatCmsBitMap(ST_CONTEXT_TAG *stContextp)
{
	unsigned short temp;
	unsigned int i,k,line,n;
    unsigned short *pCaseLedStatues;
	unsigned char *destp;
    unsigned char r,g,rg;

    pCaseLedStatues = (unsigned short *)malloc(_CMS_CTRCARD_DISP_W * _CMS_CTRCARD_DISP_V * sizeof(unsigned short));
    memset(pCaseLedStatues,0x00,(_CMS_CTRCARD_DISP_W * _CMS_CTRCARD_DISP_V * sizeof(unsigned short)));
	memset(stContextp->CmsMap_t,0x00,sizeof(stContextp->CmsMap_t));
	for(k=0;k<_CMS_CTRCARD_NUM;k++)//短路或开路时灯都是不亮的(去掉了消隐电阻)
	{
		CmsCaseToImage(pCaseLedStatues,&stContextp->CmsLedS[k][0][0]);
		for(line=0;line < _CMS_CTRCARD_DISP_V;line++)
		{
			for(i=0;i<_CMS_CTRCARD_DISP_W;i++)
			{
				temp = stContextp->CmsBufBak[line * _CMS_CTRCARD_DISP_W + i + k*_CMS_CTRCARD_DISP_W*_CMS_CTRCARD_DISP_V] & pCaseLedStatues[line * _CMS_CTRCARD_DISP_W + i];
				r = ((temp >> 10)&0x1f)  > 0;
				g = ((temp >> 5)&0x1f) > 0;
				rg = r | (g<<1);

				destp = &stContextp->CmsMap_t[(line * _CMS_CTRCARD_DISP_W + i + k*_CMS_CTRCARD_DISP_W*_CMS_CTRCARD_DISP_V)/4];
				destp[0] |= rg << ((i%4)*2);
			}
		}
	}
    free(pCaseLedStatues);
}

#else
void CreatCmsBitMap(ST_CONTEXT_TAG *stContextp)
{
	unsigned short temp;
	unsigned int i,k,line;
    unsigned short *pCaseLedStatues;
	unsigned char *destp;
    unsigned char r,g,rg;

    pCaseLedStatues = (unsigned short *)malloc(_CMS_CTRCARD_DISP_W * _CMS_CTRCARD_DISP_V * sizeof(unsigned short));
    memset(pCaseLedStatues,0x00,(_CMS_CTRCARD_DISP_W * _CMS_CTRCARD_DISP_V * sizeof(unsigned short)));
	memset(stContextp->CmsMap_t,0x00,sizeof(stContextp->CmsMap_t));
	for(k=0;k<_CMS_CTRCARD_NUM;k++)//短路或开路时灯都是不亮的(去掉了消隐电阻)
	{
		CmsCaseToImage(pCaseLedStatues,&stContextp->CmsLedS[k][0][0]);
	//	savefile("/tc/ledcase",(char *)pCaseLedStatues,(_CMS_CTRCARD_DISP_W * _CMS_CTRCARD_DISP_V * sizeof(unsigned short)));
		for(line=0;line < CMS_PIXEL_H;line++)
		{
			for(i=0;i<_CMS_CTRCARD_DISP_W;i++)
			{
				temp = stContextp->CmsBufBak[line * CMS_PIXEL_W + i + k*_CMS_CTRCARD_DISP_W] & pCaseLedStatues[line * _CMS_CTRCARD_DISP_W + i];
				r = ((temp >> 10)&0x1f)  > 0;
				g = ((temp >> 5)&0x1f) > 0;
				rg = r | (g<<1);

				destp = &stContextp->CmsMap_t[CMS_PIXEL_W / 4 * line + k * _CMS_CTRCARD_DISP_W / 4 + i / 4];
				destp[0] |= rg << ((i%4)*2);
			}
		}
	}
    free(pCaseLedStatues);
}

#endif
int CmsErrCheck(ST_CONTEXT_TAG *stContextp,int allcheck)
{
//	struct timeval tv;
	int p;
	unsigned char CardNum,PackNum,ret,mode; // 0 - 7;
	unsigned char CardNum_Err;
    unsigned char ms;
	unsigned char TotalPacket;
	unsigned int  len,s;
	ST_CMS_DATAHEAD_TAG *GetMaph;
	ST_CMS_DATAHEAD_TAG *Sendheadp;
	ST_CMS_DATAHEAD_TAG *rcvheadp;
	ST_CMS_SETCHECKERR_TAG *setErrCheck_p;
		
	pthread_mutex_lock(&stContextp->bitmap_lock);
	
	rcvheadp = &stContextp->CmsRcvGetDate.head;

	Sendheadp = (ST_CMS_DATAHEAD_TAG *)malloc(sizeof(ST_CMS_DATAHEAD_TAG));
	if(stContextp->CmsCardReady)
	{
        allcheck = 0;//三种模式全检
	}
    ms = (!allcheck)? 0 : 2;
	CardNum_Err= 0;
    for(mode = ms;mode < 3;mode++)// 0,1.2.短路，开路，线路测试
    {    
		if(mode == 1)//关短路测试 debug
		{
			memset(stContextp->CmsErrBuf[mode],0xff,_CMS_CTRCARD_NUM * CMS_LEDNUM_ONECARD);
            continue;
		}

		memset(stContextp->CmsErrBuf[mode],0x00,_CMS_CTRCARD_NUM * CMS_LEDNUM_ONECARD);
		
		pthread_mutex_lock(&stContextp->PortCms.stSendInfoLock);
		stContextp->PortCms.scmd = A003H_SET_CMS_CHECK;
		stContextp->PortCms.infolen = sizeof(ST_CMS_SETCHECKERR_TAG);		
		setErrCheck_p = (ST_CMS_SETCHECKERR_TAG *)stContextp->PortCms.stSendInfo.ucbuf;
		setErrCheck_p->id = 0;
		setErrCheck_p->mode = (mode < 2)? mode + 1 : 6;
		setErrCheck_p->scanrate = _CMS_CTRCARD_SCAN_RATE;
		setErrCheck_p->lbith = _CMS_CTRCARD_SCAN_W >> 8;
		setErrCheck_p->lbitl = _CMS_CTRCARD_SCAN_W;
		AddInfoToSendList(&stContextp->PortCms, _SEND_INFO);
		pthread_mutex_unlock(&stContextp->PortCms.stSendInfoLock);
		usleep(100*1000);

        if(mode == 2)//断线测试只读5个long
			TotalPacket = 1;
		else
			TotalPacket = CMS_LEDNUM_ONECARD / CMS_DATABODY_LEN_MAX + ( (CMS_LEDNUM_ONECARD % CMS_DATABODY_LEN_MAX)>0);

		GetMaph = (ST_CMS_DATAHEAD_TAG *)&stContextp->PortCms.stSendInfo.ucbuf[0];
		for(CardNum=0;CardNum<_CMS_CTRCARD_NUM;CardNum++)
		{
			sem_init(&stContextp->Sem_CmsRetErrMap,0,0);
			stContextp->CmsErrLen[mode][CardNum] = 0;
		
			ret = 0;
			if(CardNum_Err == (CardNum+1))
			{
				ret = -1;
                break;
			}
			for(PackNum=0;PackNum < TotalPacket;PackNum++)
			{
				pthread_mutex_lock(&stContextp->PortCms.stSendInfoLock);
				stContextp->PortCms.scmd = A004H_GET_CMS_ERRMAP;
				
				GetMaph->TotalSub = _CMS_CTRCARD_NUM;
				GetMaph->SubId = CardNum;
				GetMaph->TotalPacket = TotalPacket;
				GetMaph->PackNo = PackNum+1;

                if(mode == 2)
                {
                    len = 160;
				}
				else
				{
					if((PackNum < (TotalPacket-1))||( (CMS_LEDNUM_ONECARD % CMS_DATABODY_LEN_MAX)==0))
					{
					   len = CMS_DATABODY_LEN_MAX;				   
					}
					else if(CMS_LEDNUM_ONECARD % CMS_DATABODY_LEN_MAX)
					{
					   len = CMS_LEDNUM_ONECARD % CMS_DATABODY_LEN_MAX;
					}  
				}
				
				GetMaph->sizeh = len >> 8;
				GetMaph->sizel = len & 0xff;
					
				stContextp->PortCms.infolen = sizeof(ST_CMS_DATAHEAD_TAG);
				AddInfoToSendList(&stContextp->PortCms, _SEND_INFO);

				memcpy(Sendheadp,GetMaph,sizeof(ST_CMS_DATAHEAD_TAG));
				pthread_mutex_unlock(&stContextp->PortCms.stSendInfoLock);

				s = 0;
				p = PackNum * CMS_DATABODY_LEN_MAX;
				while(s++ < 2500) // 2.5sec
				{				
					usleep(1000);
					if(ret = sem_trywait(&stContextp->Sem_CmsRetErrMap))
					{
						continue;
					}
					ret = memcmp(rcvheadp,Sendheadp,sizeof(ST_CMS_DATAHEAD_TAG));
                    if(!ret)
					{
						memcpy(&stContextp->CmsErrBuf[mode][CardNum][p],stContextp->CmsRcvGetDate.buf,len);
						stContextp->CmsErrLen[mode][CardNum] = p + len;
						break;
					}	
				}
				
				if(ret)//通信异常
				{
					printf("ERR:CC%d return Timeout!!!\n",CardNum);
					stContextp->CmsCardReady = CardNum + 1;
					CardNum_Err = stContextp->CmsCardReady;
					break;
				}
			}
			if(ret)//通信异常
			{
				break;
			}
			if(mode == 2)//断线检测
			{
				PreTreat_BreakLine((unsigned int *)stContextp->CmsErrBuf[2][CardNum]);
			}
		}
    }

	printf("Err check %s\n",(ret)? "failed" : "OK");
#if(DEBUG_NOTCONNECT_CC)	
    memset(stContextp->CmsErrBuf,0xff,sizeof(stContextp->CmsErrBuf));
#endif
	//if(!ret)
	{			
		CreatCmsLedstatues(stContextp);

		GetErrModule(stContextp);
/*		
		savefile("/tc/errshort", stContextp->CmsErrBuf[0], CMS_LEDNUM_ONECARD * _CMS_CTRCARD_NUM);
		savefile("/tc/erropen",  stContextp->CmsErrBuf[1], CMS_LEDNUM_ONECARD * _CMS_CTRCARD_NUM);
		savefile("/tc/errbreakline",stContextp->CmsErrBuf[2],CMS_LEDNUM_ONECARD * _CMS_CTRCARD_NUM);
		savefile("/tc/err",stContextp->CmsLedS, sizeof(stContextp->CmsLedS));
*/
	}

	pthread_mutex_unlock(&stContextp->bitmap_lock);

	
	free(Sendheadp);
	return ret;
}

int GetSelfFontLib_crc(ST_FONTIMAGE_CRC_TAG *fontCrcp)
{
	unsigned char *fontCodebuf;
	int fp,i,n;
	int len;

	ST_USER_CHINESE_FONT_TAG *rcp=(ST_USER_CHINESE_FONT_TAG *)malloc(sizeof(ST_USER_CHINESE_FONT_TAG));

	fp = open("./fontlib",O_RDWR|O_CREAT,777);

	fontCodebuf = malloc(300);
	memset(fontCodebuf,0x00,300);
	lseek(fp,0,SEEK_SET);
	len = read(fp,fontCodebuf,256);//前面256byte存放的是字码big5
	close(fp);
	
	n=0;
	for(i=0;i<len/2;i++)
	{
       if(fontCodebuf[i*2])
       { 
		  fontCrcp->crc[n].code0 = fontCodebuf[i*2+0];
		  fontCrcp->crc[n].code1 = fontCodebuf[i*2+1];
		  
		  memset((unsigned char *)rcp,0x00,sizeof(ST_USER_CHINESE_FONT_TAG));
		  memcpy(rcp->ucBig5Code,&fontCodebuf[i*2],2);
		  if(!GetSelfFontLib(rcp))
		  {
			  fontCrcp->crc[n].crc = CRC8(rcp->ucBitMap,CMS_FONT_VIEW_V * CMS_FONT_VIEW_W / 8);
			  n++;
		  }
	   }
	}
	
	fontCrcp->len_ml[0] = n >> 8;
	fontCrcp->len_ml[1] = n;
	
	free(fontCodebuf);
	free(rcp);
	
	return 0;
}

//计算用户图片的校验合
int GetSelfImageLib_crc(ST_FONTIMAGE_CRC_TAG *pDest)
{
	int ret;
    int fp,id;
	int rlen;//文件当前读取位置
	int len;
	struct stat  filestat;
	ST_COLOR_SINGLE_TEXT_TAG *pColorSingleText; 

    struct dirent *ptr;
	DIR *dir;
	unsigned char *file_name;

    pColorSingleText = (ST_COLOR_SINGLE_TEXT_TAG *)malloc(sizeof(ST_COLOR_SINGLE_TEXT_TAG)); 

    ret = 0;
	if(access("/tc/downimage",F_OK))
	{
        printf("not found user image file\n");
		ret = -1;
	}
	else
	{
		dir = opendir("/tc/downimage");
		id = 0;
		file_name = malloc(128);
		while(1)
		{
	        ptr = readdir(dir);
			if(ptr == NULL)
			{
				printf("scan image file over\n");
				break;
			}
		    if(ptr->d_name[0]=='.')
				continue;
			
			if(strlen(ptr->d_name)!=4)
				continue;

			sprintf((char *)file_name,"/tc/downimage/%s",ptr->d_name);
			file_name[strlen("/tc/downimage/")+4]=0;
			if(stat((char *)file_name,&filestat))
				continue;

			fp = open((char *)file_name,O_RDWR);
			if(fp<0)
				continue;

			memset(pColorSingleText,0x00,sizeof(pColorSingleText));
			{
				unsigned char code[4],i;
				for(i=0;i<4;i++)
				{
                    if(ptr->d_name[i]<=0x39)
						code[i] = ptr->d_name[i] - 0x30;
                    else if(ptr->d_name[i]<=0x45)
						code[i] = ptr->d_name[i] - 0x41+0x0A;
                    else if(ptr->d_name[i]<=0x65)
						code[i] = ptr->d_name[i] - 0x61+0x0A;					
				}
				
	            pColorSingleText->ucPatternCode[0] = code[0]<<4 | code[1];
	            pColorSingleText->ucPatternCode[1] = code[2]<<4 | code[3];
			}
			rlen = GetSelfImageLib(pColorSingleText);
			if(rlen<=0)
			{
	            printf("Image file not context :%x%x\n",ptr->d_name[0],ptr->d_name[1]);
			}
			else
			{
				pDest->crc[id].code0 = pColorSingleText->ucPatternCode[0];
				pDest->crc[id].code1 = pColorSingleText->ucPatternCode[1];		
				len = sizeof(pColorSingleText->ucColorMap);
				pDest->crc[id].crc = CRC8((unsigned char *)pColorSingleText->ucColorMap,len);
				id++;
				pDest->len_ml[0] = id >> 8;
				pDest->len_ml[1] = id;
			}
			close(fp);
			
		}

        free(file_name);
		
		printf("user image count=%d\n",id);
	}

    free(pColorSingleText);
	
	return ret;
}


/*字跑马灯
typedef struct
{
   unsigned char ucTextID;
   unsigned char ucStartY;
   unsigned char ucStartX;
   unsigned char ucWinWide;
   unsigned char ucWinHigh;
		    char StepX;
		    char StepY;
   unsigned char ucSpeed;//移动速度，以0.1S为单位, 0-100
   unsigned char ucLoops;//回路次数  1-96
}ST_MARQUEE_TAG; //跑马灯设定
input: TickerContextp,跑马灯运行参数缓冲，在该功能函数外面是表态变量
       pMarqueeConfig_t,系统设置的跑马灯参数
       tickTime,跑马灯运行时钟,以ms为单位.即每tickTime个时间单位调用一次本函数
       dest,跑马灯缓存数据
*/
void cms_ticker(unsigned short *dest,void *TickerContextp,ST_MARQUEE_TAG *pMarqueeConfig_t,unsigned int tickTime)
{

}

//显示参数定时更新
void pthread_Cms(ST_CONTEXT_TAG *stContextp)
{
	unsigned char k; // 0 - 7;
	int show,ret,sem,checkmode;
    int counter,counttemp;
	int Sem_RcvToUpdataCms;

	ST_DATETIME_TAG DATETIME;
	unsigned char   hour_last;
	
	sigset_t setsig;
	sigemptyset(&setsig);
	sigaddset(&setsig, SIGALRM);
	sigprocmask(SIG_BLOCK,&setsig,NULL);//屏蔽信号

    sysreset(stContextp);
	counter = 0;
	counttemp = 0;
	sem_init(&stContextp->Sem_RcvToUpdataCms,0,0);
	sem_init(&stContextp->Sem_CmsCheckErr,0,0);
	sem_init(&stContextp->Sem_CmsModuleCheck,0,0);
	sem_init(&stContextp->Sem_CmsCheckerrRet,0,0);
	sem_init(&stContextp->Sem_Marquee,0,0);

	//stContextp->countTickComm = 20000;//开始时显示
	
	//memset(stContextp->CmsErrBuf,0x00,sizeof(stContextp->CmsErrBuf));
	ret = CmsErrCheck(stContextp,0);

	stContextp->disUpdateTimeUnit = DISPLAY_UPDATETIME_DEFAULT;
    while(1)
    {		
		Wdg_Check(stContextp,1);
		show = 0;
		
		Sem_RcvToUpdataCms = sem_trywait(&stContextp->Sem_RcvToUpdataCms); 
#if 1		
		if(stContextp->CmsMode == 0xff)
		{
			if(counter++ >1000)
			{
				sem_post(&stContextp->Sem_CmsCheckErr);  
				if(stContextp->CmsCardReady)//断链清屏
				{
					memset(stContextp->CmsBuf,0x00,sizeof(stContextp->CmsBuf));
				}
				else//关闭故障的模组显示
				{
					memset(stContextp->CmsBuf,0xff,sizeof(stContextp->CmsBuf));
					SetErrModule_ClearDis(stContextp);
				}
				counter = 0;				
				show = 1;
			}
		}
        else
#endif			
        if((stContextp->CmsMode)||(stContextp->autotest))
        {
			if((counter++ >1000)||(!Sem_RcvToUpdataCms))
			{
				if(stContextp->autotest)
				{
					if(++stContextp->CmsMode >3)
					{
						stContextp->CmsMode = 1;
					}
				}
	            show = CmsTestMode(stContextp);
				counter = 0;				

			}
		}
        else
        if((!Sem_RcvToUpdataCms)||((stContextp->countTickComm % stgContextp->disUpdateTimeUnit)==0)) //接到中心来的新消息或定时时间到就刷新CMS
        {				
#if (PARK_CMS_ENB)
			if(stContextp->showpark)			
			{
				Show_ParkContext(stContextp);
			}
			else
#endif
            { 
				Show_CharText(stContextp);		
            }

#if DEBUG_OFF_FAIL_MODULE			
			if(stContextp->CmsCardReady)//断链清屏
			{
				memset(stContextp->CmsBuf,0x00,sizeof(stContextp->CmsBuf));
			}
			else//关闭故障的模组显示
			{
				SetErrModule_ClearDis(stContextp);
			}
#endif			
			show = 1;
		}
		if(!(stContextp->countTickComm % 20000))//CMS控制器断线测试
		{
			//printf("Center Port:%d %d %d %d\n", stContextp->PortCenter_Eth.lineStatue  , stContextp->PortCenter_Com0.lineStatue , stContextp->PortCenter_Com1.lineStatue , stContextp->PortCenter_Com2.lineStatue);
            //printf("break mode:%d\n",stContextp->stSysConfig.stBreakLine.disp);

			CommTestSendToCms(stContextp);
		}

#if 0
		if((stContextp->countTickComm%5)==0)
		{
			show = 1;
		}
#endif	
//------------------------------------------------------------------------------
		{		
			checkmode = -1;			
            GetDateTime(&DATETIME);
			if((!DATETIME.Min)&&(!DATETIME.Sec))
			{
                if(DATETIME.Hour != hour_last)
                {
					checkmode = 0; 
				}
				hour_last = DATETIME.Hour;					
			}
				
			ret = sem_trywait(&stContextp->Sem_CmsModuleCheck);//或模组故障检测 	   
			sem = sem_trywait(&stContextp->Sem_CmsCheckErr);//接收中心的错误贞测起动,全部模式
			if( // (!(stContextp->countTickComm % 3600000))//check time out// 1000ms * 60 *60 = 1小时
			   //||
			   (!sem)
			   ||(!ret)
			  )
			{
			   checkmode = 0; //all
			}
			/*
			else if(!ret)
			{
			   checkmode = 1;// broken
			}
			*/
			if(checkmode >=0)
			{	
				ret = CmsErrCheck(stContextp,checkmode);
				sem_post(&stContextp->Sem_CmsCheckerrRet); 
				sem_post(&stContextp->Sem_RcvToUpdataCms);//刷新显示 
			}
		}
//-----------------------------------------------------------------------------
        if(show)
        {
			unsigned short *pCmsCase;

			pthread_mutex_lock(&stContextp->bitmap_lock);
			memcpy(stContextp->CmsBufBak,stContextp->CmsBuf,CMS_PIXEL_W * CMS_PIXEL_H * sizeof(short));
            pthread_mutex_unlock(&stContextp->bitmap_lock);
			
			pCmsCase = (unsigned short *)malloc(_CMS_CTRCARD_DISP_V * _CMS_CTRCARD_DISP_W * sizeof(int));

#if 0 //字幕機測試
           //if((stContextp->countTickComm%10)==0)
		   {
		        unsigned short xlong;

	            memset(stContextp->CmsBuf_Tick,0x00,sizeof(stContextp->CmsBuf_Tick));
				
				xlong = CMS_PIXEL_W-stContextp->curr_x;
	            ViewToView(stContextp->CmsBuf_Tick,stContextp->CmsBuf,CMS_PIXEL_W, CMS_PIXEL_H, CMS_PIXEL_W - xlong, 0, xlong, CMS_PIXEL_H,0, 0,CMS_PIXEL_W,CMS_PIXEL_H);
	            stContextp->curr_x++;
				if(stContextp->curr_x >= CMS_PIXEL_W)
				{
					stContextp->curr_x = 0;
				}
		  }
#endif
			pthread_mutex_lock(&stContextp->CmsDataLock);
			for(k=0;k < _CMS_CTRCARD_NUM;k++)
			{
				#if(CMS_SPCE_CONFIG  == CMS_RG8X1_F32X32)
			        CmsCaseToCtr(pCmsCase,stContextp->CmsBuf,k * _CMS_CTRCARD_DISP_V);			    
				#else 
			        CmsCaseToCtr(pCmsCase,stContextp->CmsBuf,k * _CMS_CTRCARD_DISP_W);
			        //CmsCaseToCtr(pCmsCase,stContextp->CmsBuf_Tick,k * _CMS_CTRCARD_DISP_W);
				#endif

#if(CMS_SPCE_CONFIG == CMS_RG1X1_F48X48_SHG)
                ImageToCmsCase((unsigned int(*)[CMS_COLOR_BITNUM][_CMS_CTRCARD_SCAN_W][3])stContextp->CmsData[k],(unsigned short(*)[_CMS_CTRCARD_DISP_W])pCmsCase);
#else
				ImageToCmsCase((unsigned int(*)[CMS_COLOR_BITNUM][_CMS_CTRCARD_SCAN_W])stContextp->CmsData[k],(unsigned short(*)[_CMS_CTRCARD_DISP_W])pCmsCase);
#endif
				stContextp->CmsData_Len[k] = CMS_PART_DATALEN;//CmsCaseDataComp(stContextp->CmsData[k],9216);
			}
			stContextp->CmsUpdata = 1;
			pthread_mutex_unlock(&stContextp->CmsDataLock);
		    free(pCmsCase);
		}

		
        if(counttemp++ >= 1000)
        {
			counttemp = 0;
			if(stContextp->PortRemote_p->lineStatue != CONNECT_ONLINE)
			{
				if((stContextp->offlineTime + 1 )!=0)
					stContextp->offlineTime++; 
			}
			else
			{
				stContextp->offlineTime = 0;
			}
        }        
   
		usleep(1 * 1000);// 1ms
		stContextp->countTickComm++;
    }
}

int GetHardWareStatus(ST_CONTEXT_TAG *stContextp)
{
    int ret;
	ST_HARDWARESTATUS_BITMAP_TAG *hws = &stContextp->stHardState;
	ST_HARDWARESTATUS_BITMAP_TAG *hwsold= &stContextp->stHardStateLast;

	hws->communicationConnect = (stContextp->PortRemote_p->lineStatue == CONNECT_BREAK)? 1 : 0; //(stContextp->online_sign)? 1:0;

    if(stContextp->stSysConfig.tMisc.xinbei)
    {
		hws->CmsReady  = !(stContextp->CmsCardReady > 0);
    }
	else
	{
		hws->CmsReady  = (stContextp->CmsCardReady > 0);
	}	
	
	hws->commlinebad = 0;//hws->CmsReady;
	hws->cabinatedOpened = hws->cabinatedOpened;

	//光感处理				 
	hws->brightnessSensorErr = ((stContextp->CmsCardAdc[0]	> 4000) /*|| (stContextp->CmsCardAdc[0]<50)*/) ? 1 : 0;

	//显示模组错误
	hws->displayUnitErr = stContextp->ST_CMS_MODULE_ERR.ucErrModuleNO > 0; 
	hws->DisplayUnitFailure = hws->displayUnitErr;				  

	hws->watch_dog_t_err = stContextp->wdttest;

	ret =(memcmp(hws,hwsold,sizeof(ST_HARDWARESTATUS_BITMAP_TAG)));    
	memcpy(hwsold,hws,sizeof(ST_HARDWARESTATUS_BITMAP_TAG));
	
	return ret;
}

void pthread_HardwareStatusAutoReport(ST_CONTEXT_TAG *stContextp)
{
    int i;
	unsigned long timeslong,counter;
	char brightSrep,brightErep,brightTime_on;
    ST_PORTRCVTXD_TAG           *PortCenter_p[4];

	sigset_t setsig;
	sigemptyset(&setsig);
	sigaddset(&setsig, SIGALRM);
	sigprocmask(SIG_BLOCK,&setsig,NULL);//屏蔽信号

	counter = 0;
	brightSrep=FALSE;
	brightErep=FALSE;
	brightTime_on = FALSE;
	stContextp->ST_SET_HARDSTATUS_REPORTCLY.Time = 5;//重启后默认5分钟上报一次

    PortCenter_p[0] = &stContextp->PortCenter_Eth;
    PortCenter_p[1] = &stContextp->PortCenter_Com0;
    PortCenter_p[2] = &stContextp->PortCenter_Com1;
    PortCenter_p[3] = &stContextp->PortCenter_Com2;

    while(1)
	{
		sleep(1);
		Wdg_Check(stContextp,1);

        if(sem_trywait(&stContextp->upReportDelay)==0)//软件下载时主动上报停        
        {
			sleep(5);
			continue;
        }

        if(!stContextp->bsysRestEnd)
		{//	continue;
        }
		
		i = stContextp->ST_SET_HARDSTATUS_REPORTCLY.Time;// 1 - 5有效
		timeslong =(i < 6)? uigTimeReport[i] : 300; //墨认5min
			
//--------------------------------------------------------------------------------------------------

		counter++;

		if( //(stContextp->stCenterRcvTxd.stPortRcvTxd.lineStatue == CONNECT_BREAK)&&(counter2 >= 5) )//断线时2秒发送一次
			  ((counter >= timeslong)&&(timeslong))
			||(GetHardWareStatus(stContextp)))
		{
            for(i=0;i<4;i++)
            {
				pthread_mutex_lock(&PortCenter_p[i]->stSendInfoLock);
				PortCenter_p[i]->scmd = _0F04H_RET_HARDSTATUE_REPORT;
				memcpy(&PortCenter_p[i]->stSendInfo.ST_RET_HARDSTATUE,&stContextp->stHardState,sizeof(ST_RET_HARDSTATUE_TAG));
				PortCenter_p[i]->infolen = sizeof(ST_RET_HARDSTATUE_TAG);
				AddInfoToSendList(PortCenter_p[i], _SEND_INFO);
				pthread_mutex_unlock(&PortCenter_p[i]->stSendInfoLock);
			}
			
			counter = 0;
		}

		//if(lighter >=50)
		{
			ST_DATETIME_TAG    *currDateTime_p;
			ST_BRIGHTNESS_TAG  *pST_BRIGHTNESS;
			currDateTime_p = (ST_DATETIME_TAG *)malloc(sizeof(ST_DATETIME_TAG));
			pST_BRIGHTNESS = &stContextp->stSysConfig.ST_BRIGHTNESS;

			if( (pST_BRIGHTNESS->unBright.ST_BRIGHT_BITMAP.r50)
			  ||(pST_BRIGHTNESS->unBright.ST_BRIGHT_BITMAP.LocalCtr)
              ||(pST_BRIGHTNESS->unBright.ST_BRIGHT_BITMAP.ForceDimCtr && (!pST_BRIGHTNESS->unBright.ST_BRIGHT_BITMAP.RpForceDimCtr)))
			{

			}
			else
			{
				unsigned int currTimes_min,startTime_min,endTime_min;

				GetDateTime(currDateTime_p);
				currTimes_min = currDateTime_p->Hour * 60 + currDateTime_p->Min;
				startTime_min = pST_BRIGHTNESS->ucStartHour * 60 + pST_BRIGHTNESS->ucStartMin;
				endTime_min   = pST_BRIGHTNESS->ucEndHour * 60 + pST_BRIGHTNESS->ucEndMin;
				
                if(startTime_min <= endTime_min)
                {
                     brightTime_on = ((currTimes_min >= startTime_min)&&(currTimes_min < endTime_min))? TRUE : FALSE;
				}
				else if(startTime_min > endTime_min)
				{
                     brightTime_on = ((currTimes_min >= startTime_min)||(currTimes_min < endTime_min)) ? TRUE : FALSE;
				}

                if(brightTime_on)
                {
					if(!brightSrep)
					{
						brightSrep = TRUE;
						brightErep = FALSE;
						
						for(i=0;i<4;i++)
						{
							pthread_mutex_lock(&PortCenter_p[i]->stSendInfoLock);
							PortCenter_p[i]->scmd	  = AF0EH_RET_BRIGHTNESS_MASTER;
							PortCenter_p[i]->stSendInfo.ST_RET_BRIGHTNESS_MASTER.ucBeginEnd = 1;
							PortCenter_p[i]->infolen = sizeof(ST_RET_BRIGHTNESS_MASTER_TAG);
							AddInfoToSendList(PortCenter_p[i], _SEND_INFO);
							pthread_mutex_unlock(&PortCenter_p[i]->stSendInfoLock);
						}
					}
				}
				else if(!brightErep)
				{
					brightSrep = FALSE;
					brightErep = TRUE;
					for(i=0;i<4;i++)
					{
						pthread_mutex_lock(&PortCenter_p[i]->stSendInfoLock);
						PortCenter_p[i]->scmd	  = AF0EH_RET_BRIGHTNESS_MASTER;
						PortCenter_p[i]->stSendInfo.ST_RET_BRIGHTNESS_MASTER.ucBeginEnd = 0;
						PortCenter_p[i]->infolen = sizeof(ST_RET_BRIGHTNESS_MASTER_TAG);
						AddInfoToSendList(PortCenter_p[i], _SEND_INFO);
						pthread_mutex_unlock(&PortCenter_p[i]->stSendInfoLock);
					}
				}
			}        
			
			free(currDateTime_p);
		}
	}
}

//系统复位
void sysreset(ST_CONTEXT_TAG *stContextp)
{
//-----------------测试CMS板间连接状态-----------------
    CommTestSendToCms(stContextp);

	printf("PortLocate:%s\n",stContextp->PortLocate_p->portname);
	printf("PortRemote:%s\n",stContextp->PortRemote_p->portname);

//-----------------发送硬件状态------------------------
	GetHardWareStatus(stContextp);
	pthread_mutex_lock(&stContextp->PortLocate_p->stSendInfoLock);
	stContextp->PortLocate_p->scmd = _0F04H_RET_HARDSTATUE_REPORT;
	memcpy(&stContextp->PortLocate_p->stSendInfo.ST_RET_HARDSTATUE,&stContextp->stHardState,sizeof(ST_RET_HARDSTATUE_TAG));
	stContextp->PortLocate_p->infolen = sizeof(ST_RET_HARDSTATUE_TAG);
	AddInfoToSendList(stContextp->PortLocate_p, _SEND_INFO);
	pthread_mutex_unlock(&stContextp->PortLocate_p->stSendInfoLock);

	pthread_mutex_lock(&stContextp->PortRemote_p->stSendInfoLock);
	stContextp->PortRemote_p->scmd = _0F04H_RET_HARDSTATUE_REPORT;
	memcpy(&stContextp->PortRemote_p->stSendInfo.ST_RET_HARDSTATUE,&stContextp->stHardState,sizeof(ST_RET_HARDSTATUE_TAG));
	stContextp->PortRemote_p->infolen = sizeof(ST_RET_HARDSTATUE_TAG);
	AddInfoToSendList(stContextp->PortRemote_p, _SEND_INFO);
	pthread_mutex_unlock(&stContextp->PortRemote_p->stSendInfoLock);

//-----------------要求中心对时-------------------------------
    sleep(1);
	pthread_mutex_lock(&stContextp->PortLocate_p->stSendInfoLock);
	stContextp->PortLocate_p->scmd    =_0F92H_DATETIME_SYNCH;
	stContextp->PortLocate_p->stSendInfo.ST_DATETIME_SYNCH.SecDif = 0;
	stContextp->PortLocate_p->infolen = sizeof(ST_DATETIME_SYNCH_TAG);
	AddInfoToSendList(stContextp->PortLocate_p, _SEND_INFO);
	pthread_mutex_unlock(&stContextp->PortLocate_p->stSendInfoLock);

	pthread_mutex_lock(&stContextp->PortRemote_p->stSendInfoLock);
	stContextp->PortRemote_p->scmd    =_0F92H_DATETIME_SYNCH;
	stContextp->PortRemote_p->stSendInfo.ST_DATETIME_SYNCH.SecDif = 0;
	stContextp->PortRemote_p->infolen = sizeof(ST_DATETIME_SYNCH_TAG);
	AddInfoToSendList(stContextp->PortRemote_p, _SEND_INFO);
	pthread_mutex_unlock(&stContextp->PortRemote_p->stSendInfoLock);
	
//-----------------回复复位结束-------------------------------	
	sleep(1);
	pthread_mutex_lock(&stContextp->PortLocate_p->stSendInfoLock);
	stContextp->PortLocate_p->scmd    = _0F90H_RET_REBOOT;
	stContextp->PortLocate_p->stSendInfo.ST_REBOOT.ucReset[0]=0x52;
	stContextp->PortLocate_p->stSendInfo.ST_REBOOT.ucReset[1]=0x52;
	stContextp->PortLocate_p->infolen = sizeof(ST_REBOOT_TAG);
	AddInfoToSendList(stContextp->PortLocate_p, _SEND_INFO);
	pthread_mutex_unlock(&stContextp->PortLocate_p->stSendInfoLock);
	
	pthread_mutex_lock(&stContextp->PortRemote_p->stSendInfoLock);
	stContextp->PortRemote_p->scmd    = _0F90H_RET_REBOOT;
	stContextp->PortRemote_p->stSendInfo.ST_REBOOT.ucReset[0]=0x52;
	stContextp->PortRemote_p->stSendInfo.ST_REBOOT.ucReset[1]=0x52;
	stContextp->PortRemote_p->infolen = sizeof(ST_REBOOT_TAG);
	AddInfoToSendList(stContextp->PortRemote_p, _SEND_INFO);
	pthread_mutex_unlock(&stContextp->PortRemote_p->stSendInfoLock);
	
	sleep(1);
	stContextp->bsysRestEnd = TRUE;
}

#if(!TC_PANEL_ENB)
int pthread_readkey(ST_CONTEXT_TAG *stContextp)
{   
    int keys_fd;   
    struct input_event t;   
    char filep[64],temp;

	sigset_t setsig;
	sigemptyset(&setsig);
	sigaddset(&setsig, SIGALRM);
	sigprocmask(SIG_BLOCK,&setsig,NULL);//屏蔽信号

    memset(filep,0x00,sizeof(filep));

    temp = 0; // myd
    sprintf(filep,"/dev/input/event%d",temp);
    keys_fd = open (filep, O_RDONLY);   
    if (keys_fd <= 0)   
	{   
		printf ("open %s device error!\n",filep);   
		return 0;   
	}   

    stContextp->autotest = !Get_CtrStatues(); // 0现场控制
  
    while (1)   
	{   
		usleep(100*1000);// 0.1s
		Wdg_Check(stContextp,0);

		if (read (keys_fd, &t, sizeof (t)) != sizeof (t))   //阻塞
			continue;
		if (t.type != EV_KEY)   
			continue;	

		if(!t.value)
			continue;

		if(stContextp->autotest)
		{
			stContextp->CmsMode = 0;
			stContextp->autotest = 0;	
		}
		stContextp->stSysConfig.ST_BRIGHTNESS.unBright.ST_BRIGHT_BITMAP.r50 = 0;
//		printf ("key %d %s\n", t.code,(t.value) ? "Pressed" : "Released");   
		switch(t.code)
		{
			case KEY_F1: 
			stContextp->CmsMode++;
			if(stContextp->CmsMode > 9)
			{
				stContextp->CmsMode = 0;
			}
			break;
			
			case KEY_F2:
			stContextp->CmsMode = 0;
			break;	
			
			case KEY_F4: 
			sem_post(&stContextp->Sem_CmsCheckErr);  
			stContextp->CmsMode = 0xff;//全亮
			memset(&stContextp->ST_CMS_MODULE_ERR,0x00,sizeof(ST_RET_DISPLAY_MODULE_STATUS_TAG));
			break;	
			
			case KEY_F3: 
			stContextp->stSysConfig.ST_BRIGHTNESS.unBright.ST_BRIGHT_BITMAP.r50 = !stContextp->stSysConfig.ST_BRIGHTNESS.unBright.ST_BRIGHT_BITMAP.r50;
			if(stContextp->stSysConfig.ST_BRIGHTNESS.unBright.ST_BRIGHT_BITMAP.r50)
				stContextp->CmsMode = 4;//全亮
			break;

			default:
			break;
		}
		sem_post(&stContextp->Sem_RcvToUpdataCms);//刷新显示 
		printf("Cms Show Mode:%d\n",stContextp->CmsMode);
		buzzer_ctr_keybeep();
	}   
    close (keys_fd);   
    return 0;   
}  
#endif

int Wdg_Create(ST_WDG_TAG *stWdg,unsigned long threadId,char *pname)
{
	strcpy(stWdg->theadName,pname);
	stWdg->threadid = threadId;
	stWdg->counter = 0;
	stWdg->en = 0;//默认 OFF
	return 0;
}

int Wdg_Check(ST_CONTEXT_TAG *stContextp,unsigned char bOn)
{
	unsigned char i;

    for(i=0;i<stContextp->WdtNum;i++)
    {
        if(stContextp->ThreadWdg[i].threadid == pthread_self())
        {
			if((stContextp->ThreadWdg[i].counter + 1) >0)
			{
				stContextp->ThreadWdg[i].counter++;
			}
			stContextp->ThreadWdg[i].en = bOn;
		}
	}

	return 0;
}



int main(void)
{		
	int fd_wdg;
	int i,wdtCounter;
	int flag=0;
	ST_CONTEXT_TAG *stContextp;

    ucGrcvUpdataSoft_p = malloc(128*1024);
	if(ucGrcvUpdataSoft_p == NULL)
	{
        printf("Not enough memory!\n");
		return 0;
	}
	uiGRcvUpdataSoftLen=0;

	stContextp = (ST_CONTEXT_TAG *)malloc(sizeof(ST_CONTEXT_TAG));
	stgContextp = stContextp;

	memset((unsigned char *)stContextp,0x00,sizeof(ST_CONTEXT_TAG));

    system("ifconfig eth1 192.168.2.7");
	
    if(access("/tc/",F_OK))
        mkdir("/tc/",0x0777);

    if(stContextp==NULL)
    {
		printf("Err: not memory,main exit.\n");
        exit(0);
	}
    sleep(2);
	
 	sem_init(&stContextp->upReportDelay,0,0); 
	sem_init(&stContextp->check_datatime,0,0); 

	stContextp->PortRemote_p = &stContextp->PortCenter_Eth;
	stContextp->PortLocate_p = &stContextp->PortCenter_Com0;

    strcpy(stContextp->PortCenter_Eth.portname, "Cte");
	sem_init(&stContextp->sysDataInitPth,0,0);   
    strcpy(stContextp->Thread_Name[stContextp->WdtNum],"pthread_ConfigSysFile");
	pthread_create(&stContextp->Thread_Id[stContextp->WdtNum++], NULL, (void *)&pthread_ConfigSysFile,stContextp);
	sem_wait(&stContextp->sysDataInitPth);//等待参数读取完
    sleep(1);
	
    strcpy(stContextp->Thread_Name[stContextp->WdtNum],"pthread_SendToPortCenterEth");
	pthread_create(&stContextp->Thread_Id[stContextp->WdtNum++],NULL,(void *)&pthread_SendToPort,(void *)&stContextp->PortCenter_Eth);
	
    strcpy(stContextp->Thread_Name[stContextp->WdtNum],"pthread_RcvFromPortCenterEth");
	pthread_create(&stContextp->Thread_Id[stContextp->WdtNum++],NULL,(void *)&pthread_RcvFromPort,(void *)&stContextp->PortCenter_Eth);

    strcpy(stContextp->Thread_Name[stContextp->WdtNum],"pthread_tcpAccept");
	pthread_create(&stContextp->Thread_Id[stContextp->WdtNum++], NULL, (void *)&pthread_tcpAccept,(void *)&stContextp->PortCenter_Eth);

    strcpy(stContextp->Thread_Name[stContextp->WdtNum],"pthread_SendToCom0");
	pthread_create(&stContextp->Thread_Id[stContextp->WdtNum++],NULL,(void *)&pthread_SendToPort,(void *)&stContextp->PortCenter_Com0);
	
    strcpy(stContextp->Thread_Name[stContextp->WdtNum],"pthread_RcvFromCom0");
	pthread_create(&stContextp->Thread_Id[stContextp->WdtNum++],NULL,(void *)&pthread_RcvFromPort,(void *)&stContextp->PortCenter_Com0);

    strcpy(stContextp->PortCenter_Com0.devPth,"/dev/ttyO2");
    strcpy(stContextp->PortCenter_Com0.portname, "Ctu");
    strcpy(stContextp->Thread_Name[stContextp->WdtNum],"pthread_uart0");
	stContextp->PortCenter_Com0.uart_baudrate = B9600;
	pthread_create(&stContextp->Thread_Id[stContextp->WdtNum++], NULL, (void *)&pthread_uart,(void *)&stContextp->PortCenter_Com0);

#if(TC_PANEL_ENB)
    strcpy(stContextp->Thread_Name[stContextp->WdtNum],"pthread_SendToCom4");
	pthread_create(&stContextp->Thread_Id[stContextp->WdtNum++],NULL,(void *)&pthread_SendToPort,(void *)&stContextp->PortCenter_Com4);
	
    strcpy(stContextp->Thread_Name[stContextp->WdtNum],"pthread_RcvFromCom4");
	pthread_create(&stContextp->Thread_Id[stContextp->WdtNum++],NULL,(void *)&pthread_RcvFromPort,(void *)&stContextp->PortCenter_Com4);

	strcpy(stContextp->PortCenter_Com4.devPth,"/dev/ttyO4");
	strcpy(stContextp->PortCenter_Com4.portname, "PAN");
	strcpy(stContextp->Thread_Name[stContextp->WdtNum],"pthread_uart4");
	stContextp->PortCenter_Com4.uart_baudrate = B9600;
	pthread_create(&stContextp->Thread_Id[stContextp->WdtNum++], NULL, (void *)&pthread_uart,(void *)&stContextp->PortCenter_Com4);
#endif

    strcpy(stContextp->Thread_Name[stContextp->WdtNum],"pthread_SendToCom1");
	pthread_create(&stContextp->Thread_Id[stContextp->WdtNum++],NULL,(void *)&pthread_SendToPort,(void *)&stContextp->PortCenter_Com1);
	
    strcpy(stContextp->Thread_Name[stContextp->WdtNum],"pthread_RcvFromCom1");
	pthread_create(&stContextp->Thread_Id[stContextp->WdtNum++],NULL,(void *)&pthread_RcvFromPort,(void *)&stContextp->PortCenter_Com1);

    strcpy(stContextp->PortCenter_Com1.devPth,"/dev/ttyO3");
    strcpy(stContextp->PortCenter_Com1.portname, "Lcu");
    strcpy(stContextp->Thread_Name[stContextp->WdtNum],"pthread_uart1");
	stContextp->PortCenter_Com1.uart_baudrate = B9600;
	pthread_create(&stContextp->Thread_Id[stContextp->WdtNum++], NULL, (void *)&pthread_uart,(void *)&stContextp->PortCenter_Com1);

    strcpy(stContextp->PortCenter_Com2.devPth,"/dev/ttyO5");
    strcpy(stContextp->PortCenter_Com2.portname, "Etu");
    strcpy(stContextp->Thread_Name[stContextp->WdtNum],"pthread_uart2");
	stContextp->PortCenter_Com2.uart_baudrate = B115200;
	pthread_create(&stContextp->Thread_Id[stContextp->WdtNum++], NULL, (void *)&pthread_uart,(void *)&stContextp->PortCenter_Com2);

    strcpy(stContextp->Thread_Name[stContextp->WdtNum],"pthread_SendToCom2");
	pthread_create(&stContextp->Thread_Id[stContextp->WdtNum++],NULL,(void *)&pthread_SendToPort,(void *)&stContextp->PortCenter_Com2);
	
    strcpy(stContextp->Thread_Name[stContextp->WdtNum],"pthread_RcvFromCom2");
	pthread_create(&stContextp->Thread_Id[stContextp->WdtNum++],NULL,(void *)&pthread_RcvFromPort,(void *)&stContextp->PortCenter_Com2);


    strcpy(stContextp->PortCms.portname, "Cms");
    strcpy(stContextp->Thread_Name[stContextp->WdtNum],"pthread_SendToPortCms");
	pthread_create(&stContextp->Thread_Id[stContextp->WdtNum++],NULL,(void *)&pthread_SendToPort,(void *)&stContextp->PortCms);

    strcpy(stContextp->Thread_Name[stContextp->WdtNum],"pthread_RcvFromPortCms");
	pthread_create(&stContextp->Thread_Id[stContextp->WdtNum++],NULL,(void *)&pthread_RcvFromPort,(void *)&stContextp->PortCms);

    strcpy(stContextp->Thread_Name[stContextp->WdtNum],"pthread_udpRcv");
	pthread_create(&stContextp->Thread_Id[stContextp->WdtNum++], NULL, (void *)&pthread_udpRcv, (void *)&stContextp->PortCms);

#if(!TC_PANEL_ENB)
    strcpy(stContextp->Thread_Name[stContextp->WdtNum],"pthread_ledstatue");
	pthread_create(&stContextp->Thread_Id[stContextp->WdtNum++], NULL, (void *)&pthread_ledstatue,stContextp);
#endif	
    strcpy(stContextp->Thread_Name[stContextp->WdtNum],"pthread_HardwareStatusAutoReport");
	pthread_create(&stContextp->Thread_Id[stContextp->WdtNum++], NULL, (void *)&pthread_HardwareStatusAutoReport,stContextp);

#if(!TC_PANEL_ENB)
    strcpy(stContextp->Thread_Name[stContextp->WdtNum],"pthread_readkey");
	pthread_create(&stContextp->Thread_Id[stContextp->WdtNum++], NULL, (void *)&pthread_readkey,stContextp);
#endif

	strcpy(stContextp->Thread_Name[stContextp->WdtNum],"pthread_Cms");
	pthread_create(&stContextp->Thread_Id[stContextp->WdtNum++], NULL, (void *)&pthread_Cms,stContextp);

    sleep(1);//待子线程初始化完成
	for(i=0;i<stContextp->WdtNum;i++)
	{
        Wdg_Create(&stContextp->ThreadWdg[i],stContextp->Thread_Id[i],stContextp->Thread_Name[i]);
	}

//---------------------------------------------
#if DEBUG_WDT_ENB   
	fd_wdg = open("/dev/watchdog",O_RDONLY);
	if(fd_wdg < 0) 
	{
		perror("/dev/watchdog");	
		return -1;
	}
	ioctl(fd_wdg,WDIOC_KEEPALIVE);
#endif	
//---------------------------------------------

#if(!TC_PANEL_ENB)
	buzzer_ctr();
    sleep(1);
    buzzer_ctr();
	switch_ctr();
#endif

	flag = FALSE;
	wdtCounter = 0; 		

#if 0
    int brel=1;
    int reloptcount = 0;
#endif

	
	while(1)
	{		
		wdtCounter++; 		
		if(!(wdtCounter % 2000))
		{
			flag = FALSE;
			for(i=0;i<stContextp->WdtNum;i++)
			{
				if((stContextp->ThreadWdg[i].counter == 0)&&(stContextp->ThreadWdg[i].en))
				{
					printf("Wdt TimeOut: %s id=%d\n",stContextp->ThreadWdg[i].theadName,i);
                    flag = TRUE;
				}					
				stContextp->ThreadWdg[i].counter = 0;
			}
		}
        if((!flag) && (!(wdtCounter % 100))) // 1s
        {
#if DEBUG_WDT_ENB   	
            if(!stContextp->wdttest)
            {
				ioctl(fd_wdg,WDIOC_KEEPALIVE); //debug
            }
#endif			
        }

        if((wdtCounter%100)==0)
        {
			stContextp->eth0_isdown = net_detect("eth0");
		}
		
		usleep(1000*10);// 10ms
	}
	close(fd_wdg);

	free(ucGrcvUpdataSoft_p);
	free(stContextp);
	return 0;
}


