
//file name: communication_procotol.h
#ifndef __COMMUNICATION_PROCOTOL_H
#define __COMMUNICATION_PROCOTOL_H

#ifndef __COMMUNICATION_PROTOCOL_C
#define _EXTERN_CP extern
#else
#define _EXTERN_CP
#endif

#define CMS_RG3X8_F32X32       0
#define CMS_RG3X8_F48X48       1
#define CMS_RG6X1_F32X32       2	
#define CMS_RG3X5_F48X48       3
#define CMS_RG8X1_F32X32       4
#define CMS_RG3X6_F48X48       5
#define CMS_RG1X1_F48X48_SHG   6 //9���ư�ֱ�����P1-P9

#define CMS_SPCE_CONFIG        CMS_RG3X6_F48X48  

#define CMS_TYPE_VERSION_RG3X5_4848  0x34
#define CMS_TYPE_VERSION_RG3X6_4848  0x2f

#if(CMS_SPCE_CONFIG == CMS_RG3X6_F48X48)
#define _VERSION_NUMBER  CMS_TYPE_VERSION_RG3X6_4848  //����汾
#elif(CMS_SPCE_CONFIG == CMS_RG3X5_F48X48)
#define _VERSION_NUMBER  CMS_TYPE_VERSION_RG3X5_4848  //����汾
#endif

#define DEBUG_WDT_ENB          1
#define DEBUG_OFF_FAIL_MODULE  1 //debug
#define DEBUG_NOTCONNECT_CC    0
#define PARK_CMS_ENB           0
#define TC_PANEL_ENB           0 //���

#define POWER_ADCVAL_LOW    2010  //5.1V - 0.4V  2160=5.4V ���
#define POWER_ADCVAL_HIGHT  2270  //5.7V - 0.4V

#define TEST_9CELL_ENB        0  //9�������

#define _INFO_BODY_MAXLEN    65536 //32768// 8704

#if(TEST_9CELL_ENB)
#define _CMS_CTRCARD_NUM      1
//#define _VERSION_NUMBER       0x1A//����汾

#define _CMS_CTRCARD_SCAN_W    96  //һ����CLK�ĸ���
#define _CMS_CTRCARD_SCAN_RATE 8  //1/16ɨ

#define _CMS_CTRCARD_DISP_W    48 //��ʾ���
#define _CMS_CTRCARD_DISP_V    48 //��ֱ����9��ģ�� * 16

#define CMS_FONT_VIEW_W  48  //�ִ��Ŀ�
#define CMS_FONT_VIEW_V  48  //�ִ��ĸ�

#define CMS_PIXEL_W    (_CMS_CTRCARD_DISP_W * _CMS_CTRCARD_NUM) //336
#define CMS_PIXEL_H     _CMS_CTRCARD_DISP_V
#define CMS_LEDNUM_ONECARD    (4 * _CMS_CTRCARD_SCAN_W * _CMS_CTRCARD_SCAN_RATE) //3072  // (ÿ��4BYTE) * (�г�48) *(16ɨ)

#elif (CMS_SPCE_CONFIG == CMS_RG6X1_F32X32)

#define _CMS_CTRCARD_NUM      1

//#define _VERSION_NUMBER       0x11//����汾

#define _CMS_CTRCARD_SCAN_W    96  //һ����CLK�ĸ���
#define _CMS_CTRCARD_SCAN_RATE 8  //1/16ɨ

#define _CMS_CTRCARD_DISP_W    32   //��ʾ���
#define _CMS_CTRCARD_DISP_V    192

#define CMS_FONT_VIEW_W  32  //�ִ��Ŀ�
#define CMS_FONT_VIEW_V  32  //�ִ��ĸ�

#define CMS_PIXEL_W    (_CMS_CTRCARD_DISP_W * _CMS_CTRCARD_NUM) //336
#define CMS_PIXEL_H     _CMS_CTRCARD_DISP_V
#define CMS_LEDNUM_ONECARD    (4 * _CMS_CTRCARD_SCAN_W * _CMS_CTRCARD_SCAN_RATE) //3072  // (ÿ��4BYTE) * (�г�48) *(16ɨ)

#elif (CMS_SPCE_CONFIG  == CMS_RG3X8_F32X32) 

#define _CMS_CTRCARD_NUM      4 //8

//#define _VERSION_NUMBER       0x1A//����汾

#define _CMS_CTRCARD_SCAN_W    128//96  //һ����CLK�ĸ���
#define _CMS_CTRCARD_SCAN_RATE 8  //1/16ɨ

#define _CMS_CTRCARD_DISP_W    64//48   //��ʾ���
#define _CMS_CTRCARD_DISP_V    96//144  //��ֱ����9��ģ�� * 16

#define CMS_FONT_VIEW_W  32  //�ִ��Ŀ�
#define CMS_FONT_VIEW_V  32  //�ִ��ĸ�

#define CMS_PIXEL_W    (_CMS_CTRCARD_DISP_W * _CMS_CTRCARD_NUM) //336
#define CMS_PIXEL_H     _CMS_CTRCARD_DISP_V
#define CMS_LEDNUM_ONECARD    (4 * _CMS_CTRCARD_SCAN_W * _CMS_CTRCARD_SCAN_RATE) //3072  // (ÿ��4BYTE) * (�г�48) *(16ɨ)

#elif (CMS_SPCE_CONFIG  == CMS_RG3X8_F48X48) 
#define _CMS_CTRCARD_NUM    8

//#define _VERSION_NUMBER       0x1A//����汾

#define _CMS_CTRCARD_SCAN_W    96  //һ����CLK�ĸ���
#define _CMS_CTRCARD_SCAN_RATE 8  //1/16ɨ

#define _CMS_CTRCARD_DISP_W    48   //��ʾ���
#define _CMS_CTRCARD_DISP_V    144  //��ֱ����9��ģ�� * 16

#define CMS_FONT_VIEW_W  48  //�ִ��Ŀ�
#define CMS_FONT_VIEW_V  48  //�ִ��ĸ�

#define CMS_PIXEL_W    (_CMS_CTRCARD_DISP_W * _CMS_CTRCARD_NUM) //336
#define CMS_PIXEL_H     _CMS_CTRCARD_DISP_V

#define CMS_LEDNUM_ONECARD    (4 * _CMS_CTRCARD_SCAN_W * _CMS_CTRCARD_SCAN_RATE) //3072  // (ÿ��4BYTE) * (�г�48) *(16ɨ)

#elif (CMS_SPCE_CONFIG  == CMS_RG3X5_F48X48) 
#define _CMS_CTRCARD_NUM      5 

//#define _VERSION_NUMBER       0x1C//����汾

#define _CMS_CTRCARD_SCAN_W    96  //һ����CLK�ĸ���
#define _CMS_CTRCARD_SCAN_RATE 8  //1/16ɨ

#define _CMS_CTRCARD_DISP_W    48   //��ʾ���
#define _CMS_CTRCARD_DISP_V    144  //��ֱ����9��ģ�� * 16

#define CMS_FONT_VIEW_W  48  //�ִ��Ŀ�
#define CMS_FONT_VIEW_V  48  //�ִ��ĸ�

#define CMS_PIXEL_W    (_CMS_CTRCARD_DISP_W * _CMS_CTRCARD_NUM) //336
#define CMS_PIXEL_H     _CMS_CTRCARD_DISP_V

#define CMS_LEDNUM_ONECARD    (4 * _CMS_CTRCARD_SCAN_W * _CMS_CTRCARD_SCAN_RATE) //3072  // (ÿ��4BYTE) * (�г�48) *(16ɨ)

#elif (CMS_SPCE_CONFIG  == CMS_RG3X6_F48X48) 
#define _CMS_CTRCARD_NUM      6

#define _CMS_CTRCARD_SCAN_W    96  //һ����CLK�ĸ���
#define _CMS_CTRCARD_SCAN_RATE 8  //1/16ɨ

#define _CMS_CTRCARD_DISP_W    48   //��ʾ���
#define _CMS_CTRCARD_DISP_V    144  //��ֱ����9��ģ�� * 16

#define CMS_FONT_VIEW_W  48  //�ִ��Ŀ�
#define CMS_FONT_VIEW_V  48  //�ִ��ĸ�

#define CMS_PIXEL_W    (_CMS_CTRCARD_DISP_W * _CMS_CTRCARD_NUM) //336
#define CMS_PIXEL_H     _CMS_CTRCARD_DISP_V
#define CMS_LEDNUM_ONECARD    (4 * _CMS_CTRCARD_SCAN_W * _CMS_CTRCARD_SCAN_RATE) //3072  // (ÿ��4BYTE) * (�г�48) *(16ɨ)

#elif (CMS_SPCE_CONFIG  == CMS_RG8X1_F32X32) 
#define _CMS_CTRCARD_NUM      2

//#define _VERSION_NUMBER       0x1C //����汾

#define _CMS_CTRCARD_SCAN_W    64  //һ����CLK�ĸ���
#define _CMS_CTRCARD_SCAN_RATE 8   //1/16ɨ

#define _CMS_CTRCARD_DISP_W    32  //��ʾ���
#define _CMS_CTRCARD_DISP_V    128 //��ֱ����8��ģ�� * 16

#define CMS_FONT_VIEW_W  32  //�ִ��Ŀ�
#define CMS_FONT_VIEW_V  32  //�ִ��ĸ�

#define CMS_PIXEL_W    _CMS_CTRCARD_DISP_W
#define CMS_PIXEL_H    (_CMS_CTRCARD_DISP_V * _CMS_CTRCARD_NUM)
#define CMS_LEDNUM_ONECARD    (4 * _CMS_CTRCARD_SCAN_W * _CMS_CTRCARD_SCAN_RATE) //3072  // (ÿ��4BYTE) * (�г�48) *(16ɨ)

#elif(CMS_SPCE_CONFIG == CMS_RG1X1_F48X48_SHG)
#define _CMS_CTRCARD_NUM      1
//#define _VERSION_NUMBER       0x1B//����汾

#define _CMS_CTRCARD_SCAN_W    32  //һ����CLK�ĸ���
#define _CMS_CTRCARD_SCAN_RATE 8  //1/16ɨ

#define _CMS_CTRCARD_DISP_W    48 //��ʾ���
#define _CMS_CTRCARD_DISP_V    48 //��ֱ����9��ģ�� * 16

#define CMS_FONT_VIEW_W  48  //�ִ��Ŀ�
#define CMS_FONT_VIEW_V  48  //�ִ��ĸ�

#define CMS_PIXEL_W    (_CMS_CTRCARD_DISP_W * _CMS_CTRCARD_NUM) //336
#define CMS_PIXEL_H     _CMS_CTRCARD_DISP_V

#define CMS_LEDNUM_ONECARD    (12 * _CMS_CTRCARD_SCAN_W * _CMS_CTRCARD_SCAN_RATE) //3072  // (ÿ��4BYTE) * (�г�48) *(16ɨ)
#endif

#define CMS_DATABODY_LEN_MAX  968  //���²�ͨ�Ų������ص���
#define __SEND_LIST_MAX       15
#define CMS_COLOR_BITNUM      1
#define CMS_PART_DATALEN      (CMS_LEDNUM_ONECARD * CMS_COLOR_BITNUM)

#define DISPLAY_UPDATETIME_DEFAULT 10000  // 10SEC
#define DISPLAY_UPDATETIME_FLASH     500  // 0.5SEC
#define DISPLAY_UPDATETIME_MARGUEE   50   // 50MS

#define LOAD_PACKETSIZE_MAX 32768 //2048
#define LOAD_PACKETSIZE_MIN 512
#define LOAD_PACKETSIZE_DEF 1024

typedef enum
{
   INFO_STX = 0XBB,
   INFO_ACK = 0XDD,
   INFO_NAK = 0XEE,
}ENUM_INFO_TYPE;

typedef enum
{
   _SEND_INFO = 0,
   _SEND_ACK  = 1,
   _SEND_NAK  = 2
}ST_SENDINFO_TYPE_TAG;


typedef struct
{
    unsigned char packnum[2];
    unsigned char packtotal[2];
	unsigned char textlen[2];
	unsigned char text[1024];
}ST_UPDATASOF_SET_TAG;//�ļ��������ݰ�

typedef struct
{
    unsigned char crc;
}ST_UPDATASOF_RETCRC_TAG;

typedef struct
{
    unsigned char err;
}ST_UPDATASOF_SETEND_TAG;


typedef struct
{
    unsigned char fontlib_lenl;
	unsigned char fontlib_lenm;
    unsigned char xinbei;//�±���ͨ��
    unsigned char Mfail_LedNum;//ģ�黵������
    unsigned char nouse2;
    unsigned char nouse3;
    unsigned char nouse4;	
}ST_MISC_CONF_TAG;


typedef struct
{
    unsigned char LocalCtr : 1;
	unsigned char RemoteCtr : 1;
	unsigned char LocalSwitch : 1;
	unsigned char nouse : 5;
}t_CtrModeDef;

typedef union
{
	t_CtrModeDef bit;
	unsigned char byte;   
}t_AE53_RET_CTRMODE;

/* 
  IP����/��ѯ��Ϣ�ṹ 
  IPADDR NETMASK GATEWAY PORT   MACADDR
  4BYTE  4BYTE   4BYTE   2BYTE  20BYTE
*/
typedef struct
{
    unsigned char ipaddr[4];
	unsigned char netmask[4];
	unsigned char gateway[4];
	unsigned char portaddr[2];
}ST_NET_CONF_TAG;

typedef struct
{   
   unsigned short Cmd; 	 //ָ����
   int (*fun)(void *pbuf_context,void *pbuf_port);//(void *pbuf_context,void *pbuf_port);	
}ST_CMDFUN_TAG;

typedef enum
{
    AF10H_SET_CIRCULATION_DISPLAY = 0XAF10,//ѭ����ʾ��������
    AF40H_GET_CIRCULATION_DISPLAY = 0XAF40,//ѭ����ʾ������ѯ	
    AFC0H_RET_CIRCULATION_DISPLAY = 0XAFC0,//ѭ����ʾ�����ر�

    AF11H_SET_PRESET_TEXT = 0XAF11,//Ԥ��ȫ�Ĺ���֮�趨
    AF41H_GET_PRESET_TEXT = 0XAF41,//Ԥ��ȫ�Ĺ���֮��ѯ
    AFC1H_RET_PRESET_TEXT = 0XAFC1,//Ԥ��ȫ�Ĺ���֮�ر�
	
    AF12H_SET_PRESET_TEXT_COLOR = 0XAF12,//Ԥ��ȫ�Ĳ�ɫ��������֮�趨
    AF42H_GET_PRESET_TEXT_COLOR = 0XAF42,//Ԥ��ȫ�Ĳ�ɫ��������֮��ѯ
    AFC2H_RET_PRESET_TEXT_COLOR = 0XAFC2,//Ԥ��ȫ�Ĳ�ɫ��������֮�ر�

    AF13H_SET_CURRENT_TEXT = 0XAF13,//��ǰȫ����ʾ����֮�趨
    AF43H_GET_CURRENT_TEXT = 0XAF43,//��ǰȫ����ʾ����֮��ѯ
    AFC3H_RET_CURRENT_TEXT = 0XAFC3,//��ǰȫ����ʾ����֮�ر�
    AF14H_CLR_CURRENT_TEXT = 0XAF14,//��ǰ��ʾ��������

    AF44H_GET_CURRENT_TEXT_COLOR = 0XAF44,//��ǰȫ�Ĳ�ɫ��������֮��ѯ
    AFC4H_RET_CURRENT_TEXT_COLOR = 0XAFC4,//��ǰȫ�Ĳ�ɫ��������֮�ر�

    AF15H_SET_MARQUEE = 0XAF15,//������趨
    AF45H_GET_MARQUEE = 0XAF45,//����Ʋ�ѯ	
    AFC5H_RET_MARQUEE = 0XAFC5,//����ƻر�

    AF16H_SET_CHINESE_FONT = 0XAF16,//�������δ�������
    AF46H_GET_CHINESE_FONT = 0XAF46,//�������δ����ѯ	
    AFC6H_RET_CHINESE_FONT = 0XAFC6,//�������δ���ر�
    AF17H_CLR_CHINESE_FONT = 0XAF17,//�������δ������

    AF47H_GET_DISPLAY_MODULE_STATUS = 0XAF47,//��ʾģ��״̬��ѯ
    AFC7H_RET_DISPLAY_MODULE_STATUS = 0XAFC7,//��ʾģ��״̬�ر�

    AF18H_SET_COLOR_SINGLE_TEXT = 0XAF18,//��ɫ�ִ�ͼ�ι���֮�趨
    AF48H_GET_COLOR_SINGLE_TEXT = 0XAF48,//��ɫ�ִ�ͼ�ι���֮��ѯ	
    AFC8H_RET_COLOR_SINGLE_TEXT = 0XAFC8,//��ɫ�ִ�ͼ�ι���֮�ر�

    AF31H_SET_OFFLINE_MODE = 0XAF31,//�ѻ���ʾģʽ֮�趨
    AF61H_GET_OFFLINE_MODE = 0XAF61,//�ѻ���ʾģʽ֮��ѯ	
    AFE1H_RET_OFFLINE_MODE = 0XAFE1,//�ѻ���ʾģʽ֮�ر�

    AF3EH_SET_BRIGHTNESS = 0XAF3E,//�豸���ȿ���֮�趨
    AF6EH_GET_BRIGHTNESS = 0XAF6E,//�豸���ȿ���֮��ѯ�ر�
    AFEEH_RET_BRIGHTNESS_SLAVE = 0XAFEE,//�豸���ȿ���֮��ѯ�ر�
    AF0EH_RET_BRIGHTNESS_MASTER = 0XAF0E,//�豸���ȿ���֮�����ر�
	
	AF52H_GET_BITMAP_JUNYE = 0XAF52,//���Ķ�ȡbitmap
//----------------------------------NEW----------------------------------

	AF32H_SET_PRESET_MSG= 0XAF32,//Ԥ����Ϣ�趨
    AF62H_GET_PRESET_MSG = 0XAF62,//Ԥ����Ϣ��ѯ
    AFE2H_RET_PRESET_MSG = 0XAFE2,//Ԥ����Ϣ�ر�
 
    AF33H_SET_CURR_MSG   = 0XAF33,//ѡ��Ԥ����Ϣ
    AF63H_GET_CURR_MSG   = 0XAF63,//��ѯ��ǰ��Ϣ
    AFE3H_RET_CURR_MSG   = 0XAFE3,//�ر���ǰ��Ϣ

    AF1AH_SET_INSERT     = 0XAF1A,//�O������^�����օ���
    AF4AH_GET_INSERT     = 0XAF4A,//��ԃ����^�����օ���
	AFCAH_RET_INSERT	 = 0XAFCA,//�؈����^�����օ���

    /*
    AF34H_SET_PRESET_ZONECUT = 0XAF34,//Ԥ������ָ��趨
    AF64H_GET_PRESET_ZONECUT = 0XAF64,//Ԥ������ָ��ѯ
    AFE4H_RET_PRESET_ZONECUT = 0XAFE4,//Ԥ������ָ�ر�

    AF35H_SET_PRESET_ZONEGROUP = 0XAF35,//Ԥ����������趨
    AF65H_GET_PRESET_ZONEGROUP = 0XAF65,//Ԥ��������ϲ�ѯ
    AFE5H_RET_PRESET_ZONEGROUP = 0XAFE5,//Ԥ��������ϻر�

    AF36H_SET_CURR_ZONEGROUP = 0XAF36,//��ǰ��������趨
    AF66H_GET_CURR_ZONEGROUP = 0XAF66,//��ǰ������ϲ�ѯ
    AFE6H_RET_CURR_ZONEGROUP = 0XAFE6,//��ǰ������ϻر�

    AF19H_SET_STREET_DIS = 0XAF19,//��·��ʾ�����趨
    AF49H_GET_STREET_DIS = 0XAF49,//��·��ʾ������ѯ
    AFC9H_GET_STREET_DIS = 0XAFC9,//��·��ʾ�����ر�
   */ 
//-----------------------------------------------------------------------    

//----------------------��չ----------------------------
    _1008H_TEST_WDT      = 0X1008,

    AE11H_SET_BREAKLINE =  0XAE11,
	AE12H_GET_BREAKLINE =  0XAE12,    
    AE15H_RET_BREAKLINE =  0XAE15,

    AE10H_UPDATASOF_SET   = 0XAE10,//�������
    AE40H_UPDATASOF_GETCRC = 0XAE40,
    AEC0H_UPDATASOF_RETCRC = 0XAEC0,
    AEC1H_UPDATASOF_SETEND = 0XAEC1,
    AE20H_CLEAN_USERFILE   = 0XAE20,//����û�ͼƬ

    AE43H_GET_CHECKFONT = 0XAE43, 
    AEC3H_RET_CHECKFONT = 0XAEC3, 
    AE44H_GET_CHECKIMAGE= 0XAE44, 
    AEC4H_RET_CHECKIMAGE = 0XAEC4, 	
	
    AE05H_SET_NET_CONF     =0XAE05,//IP��MAC����
    AE45H_GET_NET_CONF     =0XAE45,//IP��MAC��ѯ
    AEC5H_REG_NET_CONF     =0XAEC5,//IP��MAC�ظ�
	
	AE06H_SET_ADDRESSCODE =0XAE06,  //
	AE46H_GET_ADDRESSCODE =0XAE46,  //
	AEC6H_RET_ADDRESSCODE =0XAEc6,  //

    AE07H_SET_CURRBRIGHT =0XAE07,//����һ������
    AE47H_GET_CURRBRIGHT =0XAE47,//��ѯ��ǰ����ֵ
    AEC7H_RET_CURRBRIGHT =0XAEC7,//�ر���ǰ����ֵ    
	
    AE08H_SET_BRIGHTTAB  =0XAE08,
    AE48H_GET_BRIGHTTAB  =0XAE48,
    AEC8H_RET_BRIGHTTAB  =0XAEc8,

    AE23H_GET_CONTRMODE  =0XAE23, //���Ʒ�ʽ Զ�̼��ֳ�
    AE53H_REG_CONTRMODE  =0XAE53,

	AE33H_SET_CMSERRCHECK=0XAE33,//PC���𷢵Ĳ��Ժ����һ������        

    AE18H_SET_CENTER_PORT=0XAE18,//Զ��ѡ���
	AE19H_GET_CENTER_PORT=0XAE19,//Զ��ѡ���
    AE21H_REG_CENTER_PORT=0XAE21,//
	
	AE60H_SET_PACKSIZE_UPLOAD=0XAE60,//�����ϴ����Ĵ�С
	AE61H_GET_PACKSIZE_UPLOAD=0XAE61,//��ȡ�ϴ����Ĵ�С
	AE62H_RET_PACKSIZE_UPLOAD=0XAE62,//�ش��ϴ����Ĵ�С
	
    AE65H_GET_CMSTYPE = 0XAE65,//��ȡCMS����
    AE66H_REG_CMSTYPE = 0XAE66,//�ش�CMS����

    AE67H_GET_MISC    = 0XAE67,//��ȡ�����������
    AE68H_RET_MISC    = 0XAE68,
	AE69H_SET_MISC	  = 0XAE69,

	AE6A_WDT_TEST     = 0xAE6A,

//--------------TC <-> CMS--------------------------------
    A001H_GET_CMS_TICK   = 0XA001, //TC -> CMS ��ʱ������Ϣ
    A002H_RET_CMS_TICK   = 0XA002, //TC <- CMS
    A003H_SET_CMS_CHECK  = 0XA003,
    A004H_GET_CMS_ERRMAP = 0XA004,
    A005H_RET_CMS_ERRMAP = 0XA005,
    A006H_SET_CMS_SHOWD  = 0XA006,
    A007H_RET_CMS_SHOWD  = 0XA007,    
    A008H_SET_CMS_REBOOT = 0XA008, //TC -> CMS ��λ��Ϣ
//-------------------------------------------------------
#if(TC_PANEL_ENB)
    A009H_GET_PANEL_STATE = 0XA009,//���ԵƺŰ������
    A00AH_RET_PANEL_STATE = 0XA00A,
	A00BH_GET_PANEL_IN    = 0XA00B,//���ԵƺŰ��KEY��Ϣ
#endif

	//-----------------------ͨ��-----------------------------
	_0F80H_RET_SETOK 	=0x0F80,//�ر��趨��Ϣ��ȷ
	_0F81H_RET_SETERR	=0X0F81,//�ر��趨���ѯָ����Ч
	_0F10H_SET_REBOOT	=0X0F10,//�����ֳ��豸
	_0F90H_RET_REBOOT	=0X0F90,//�ر��ֳ��豸������
	_0F40H_GET_DEVICE_NUM=0X0F40,//��ѯ�ֳ��豸���
	_0FC0H_RET_DEVICE_NUM=0X0FC0,//�ر��ֳ��豸���
	_0F00H_RET_POWERBREAK=0X0F00,//�ر�ǰ�ε�Դ�ж�
	_0F11H_SET_RESET 	=0X0F11,//���趨�ֳ��豸
	_0F91H_RET_RESET 	=0X0F91,//�ر�ͨѶ���趨
	_0F41H_GET_DEVICE_STATUS=0X0F41,//��ѯ�ֳ��豸״̬
	_0FC1H_RET_DEVICE_STATUS=0X0FC1,//�ر���ѯ�ֳ��豸״̬
	_0F12H_SET_DATETIME	   =0X0F12,//�趨�ֳ��豸����ʱ��
	_0F92H_DATETIME_SYNCH =0X0F92,//Ҫ�����Ķ�ʱ
	_0F42H_GET_DATETIME	 =0X0F42,//��ѯ�ֳ��豸����ʱ��
	_0FC2H_RET_DATETIME	 =0X0FC2,//�ر��ֳ��豸����ʱ��
	_0F13H_SET_SOFTVER	 =0X0F13,//�趨�ֳ��豸��Ϣ�ȼ�
	_0F43H_GET_SOFTVER	   =0X0F43,//��ѯ����汾
	_0FC3H_RET_SOFTVER	   =0X0FC3,//�ر�����汾
	_0F14H_SET_HARDSTATUS_REPORTCLY	 =0X0F14,//�趨�ֳ��豸�ر�Ӳ��״̬֮����
	_0F4FH_GET_HARDSTATUS_REPORTCLY	 =0X0F44,//��ѯ�ֳ��豸�ر�Ӳ��״̬֮����
	_0FC4H_RET_HARDSTATUS_REPORTCLY	 =0X0FC4,//��ѯ���ֳ��豸�ر�Ӳ��״̬֮����
	_0F04H_RET_HARDSTATUE_REPORT   =0X0F04,//�ֳ��豸�ر�Ӳ��״̬

	_0F15H_SET_DATA_PASSWORD = 0X0F15, //�趨���Ͽ����������
	_0F45H_GET_DATA_PASSWORD = 0X0F45, //��ѯ���Ͽ����������
	_0FC5H_RET_DATA_PASSWORD = 0X0FC5, //�ظ����Ͽ����������

	_0F16H_SET_LOCK_UNLOCK  =0X0F16,//���������ֳ�����
	_0F46H_GET_LOCK_UNLOCK  =0X0F46,//��ѯ���������ֳ�����
	_0FC6H_RET_LOCK_UNLOCK  =0X0FC6,//�ر���ѯ���������ֳ�����

	_0F47H_GET_PROTOL_CODE  =0X0F47,//��ѯͨ��Э�� 
	_0FC7H_RET_PROTOL_CODE  =0X0FC7,//�ظ���ѯͨ��Э�� 

	_0F02H_REBOOT_REP	   =0X0F02,//��������
	
	AF51H_SET_RGIMAGE = 0XAF51,
	AF54H_GET_RGIMAGE = 0XAF54,
	AF56H_RET_RGIMAGE = 0XAF56,

#if (PARK_CMS_ENB)
    AF19H_SET_PARK_CONTEXT=0XAF19,
    AF43H_GET_CURR_PARK_CONTEXT=0XAF43,
    AFC9H_RET_PARK_CONTEXT = 0XAFC9,
    AF32H_SET_PARK_CONTEXT_RGCOLOR = 0XAF32,
    AFE2H_RET_PARK_CONTEXT_RGCOLOR = 0XAFE2,
    AF23H_ADD_PARK_INFO = 0XAF23,
    AF53H_GET_PARK_INFO = 0XAF53,
    AFD3H_RET_PARK_INFO = 0XAFD3,
    AF24H_DEL_PARK_INFO = 0XAF24,
    AF55H_GET_PARK_INFO_ERR = 0XAF55,
    AFE5H_REG_PARK_INFO_ERR = 0XAFE5,
    AF62H_SET_PARK_ICON = 0XAF62,
    AF72H_GET_PARK_ICON = 0XAF72,
    AFE6H_RET_PARK_ICON = 0XAFE6,
    AF63H_SET_PARK_INDICATE =0XAF63,
    AF73H_GET_PARK_INDICATE =0XAF73,
    AFE7H_RET_PARK_INDICATE =0XAFE7,    

 #endif

}ENUM_CMD;

#define MAX_FRAME_TEXT_COUNT          32    //��֡ȫ����������
#define MAX_CIRCULATION_FRAME_COUNT   32    //ѭ����ʾ���֡��
#define MAX_PRESET_FRAME_COUNT        128   //Ԥ��֡�������

//--------------------------------CMSר��֮���ݽṹ---------------------------

typedef struct
{
    unsigned char ucDisplayTime;//��ʾʱ�� ��Ϊ��λ 1-60
    unsigned char ucTextCount;  //ȫ���� 0-32,0ʱ���,ֹͣѭ����ʾ
    unsigned char ucTextID[MAX_CIRCULATION_FRAME_COUNT]; //ȫ�ı�� 1-48
}ST_CIRCULATION_DISPLAY_TAG;//ѭ����ʾ��������

typedef struct
{
    unsigned char ucTextID;//ȫ�ı�� 1-127
    unsigned char ucShow;  //��ʱ��ʾ���� 0:���趨��������ʾ 1:�趨��������ʾ
    unsigned char ucTextLength;//ȫ���ִ����� 1-32
    unsigned char ucWordCode[MAX_FRAME_TEXT_COUNT][2];//�ִ�����
}ST_PRESET_TEXT_TAG;//Ԥ��ȫ�Ĺ���֮�趨

///////////////////////////////////////
typedef struct 
{
   unsigned char font : 2;//����ɫ b11:yellow, b01:green; b10:red; b00:black
   unsigned char Background : 2;//����b11:yellow, b01:green; b10:red; b00:black
   unsigned char flash : 2; //��˸�ٶ�b11:2s, b01:1s; b10:0.5s; b00:stop
   unsigned char retain : 2;//����
}ST_COLOR_TAG;

typedef union 
{
	ST_COLOR_TAG stcl;
	unsigned char byte;
}t_cl;

typedef union 
{
   ST_COLOR_TAG  stColor[MAX_FRAME_TEXT_COUNT];
   unsigned char ucColor[MAX_FRAME_TEXT_COUNT];
}UN_COLOR_TAG;

typedef struct
{
   unsigned char ucTextID;   //ȫ�ı�� 1-48
   unsigned char ucTextCount;//ȫ���� 0-32
   UN_COLOR_TAG  unColor;
   unsigned char ucV_Bound;  //�Ϸ��߽�������֮��ֱ����
   unsigned char ucH_Bound;  //�󷽱߽�������֮ˮƽ����

   unsigned char vSpacem;  //��ֱ�־����
   unsigned char vSpacel;  //��ֱ�־����
   unsigned char hSpacem;  //ˮƽ�־����   
   unsigned char hSpacel;  //ˮƽ�־����   
}
ST_PRESET_TEXT_COLOR_TAG;//Ԥ��ȫ�Ĳ�ɫ��������֮�趨

typedef struct
{
   unsigned char ucTextID;
   unsigned char ucStartY;
   unsigned char ucStartX;
   unsigned char ucWinWide;
   unsigned char ucWinHigh;
		    char StepX;
		    char StepY;
   unsigned char ucSpeed;//�ƶ��ٶȣ���0.1SΪ��λ, 0-100
   unsigned char ucLoops;//��·����  1-96
}ST_MARQUEE_TAG; //������趨

typedef struct
{
   unsigned char ucBig5Code[2];
   unsigned char ucFrameRow;
   unsigned char ucFrameColumn;
   unsigned char ucFrameTotal;
   unsigned char ucFrameNumber;
   unsigned char ucBitMap[288];//���48X48/8
}ST_USER_CHINESE_FONT_TAG;//�������δ�������

typedef struct
{
   unsigned char ucBig5Code[2];
   unsigned char ucFrameRow;
   unsigned char ucFrameColumn;
   unsigned char ucFrameTotal;   //��Э���������û�е�
   unsigned char ucFrameNumber;
}ST_GET_CHINESE_FONT_TAG;//�������δ����ѯ

typedef struct
{
   unsigned char ucBig5Code[2];
}ST_CLR_CHINESE_FONT_TAG;//�������δ������


typedef struct
{
   unsigned char ucErrModuleNO;//����֮��ʾģ������
   unsigned char ucDisplayMOduleId[256];//����֮��ʾģ����   
}ST_RET_DISPLAY_MODULE_STATUS_TAG;//��ʾģ��״̬�ر�


typedef struct
{
   unsigned char ucPatternCode[2];  //ͼ�α��
   unsigned char ucFrameRow;
   unsigned char ucFrameColumn;
   unsigned char ucFrameTotal;
   unsigned char ucFrameNumber;
   unsigned char ucColorMap[576];//���48X48/4
}ST_COLOR_SINGLE_TEXT_TAG;//��ɫ�ִ�ͼ�ι���֮�趨

typedef struct
{
   unsigned char ucPatternCode[2];  //ͼ�α��
   unsigned char ucFrameRow;
   unsigned char ucFrameColumn;
   unsigned char ucFrameTotal;
   unsigned char ucFrameNumber;
}ST_GET_COLOR_SINGLE_TEXT_TAG;//��ɫ�ִ�ͼ�ι���֮��ѯ	

typedef struct
{
   unsigned char disp;// 1:Ϩ��,2:ѭ��ģʽ,��ѭ������δ�趨����ΪϨ��,3��ǰҳģʽ,4���ߺ���������ʱ��   
   unsigned char delay;
}ST_SET_OFFLINE_MODE_TAG;//�ѻ���ʾģʽ֮�趨


#if (TC_PANEL_ENB)
//PANEL��Ϣͨ�Žṹ------------------------------------------------
typedef struct
{
	//unsigned char cmd0;
	//unsigned char cmd1;  	
	unsigned char optmode;
	unsigned char pwrstate;
	unsigned char casedoor;
}t_A009H_GET_PANEL_STATE;

typedef struct
{
	//unsigned char cmd0;
	//unsigned char cmd1;    

	unsigned char remotectr;
	unsigned char localrun;
	unsigned char localopt;
	unsigned char centererr;
	unsigned char deviceerr;	
}t_A00AH_RET_PANEL_STATE;

typedef struct
{
	//unsigned char cmd0;
	//unsigned char cmd1;    

	unsigned char signtest;
	unsigned char halfbritest;
	unsigned char exittest;
}t_A00BH_GET_PANEL_IN;

#endif


//------------����-----------------------
typedef struct
{
   unsigned char MsgId;
   unsigned char MsgType;
   unsigned char ActionType;
   unsigned char TotalPacket;
   unsigned char PackNo;
   unsigned char MsgSizeH;
   unsigned char MsgSizeL;
   unsigned char MsgBuf[_INFO_BODY_MAXLEN];//[LOAD_PACKETSIZE_MAX];
}ST_SET_MSG_AF32H_TAG;

typedef struct
{
   unsigned char MsgId;
   unsigned char MsgType;
   unsigned char TotalPacket;
   unsigned char PackNo;
   unsigned char MsgSizeH;
   unsigned char MsgSizeL;
   unsigned char MsgBuf[_INFO_BODY_MAXLEN];//[LOAD_PACKETSIZE_MAX];
}ST_SET_MSG_AFE2H_TAG;

typedef struct
{
   unsigned char MsgId;
   unsigned char TotalPacket;
   unsigned char PackNo;
   unsigned char MsgSizeH;
   unsigned char MsgSizeL;
   unsigned char MsgBuf[_INFO_BODY_MAXLEN];//[LOAD_PACKETSIZE_MAX];
}ST_SET_MSG_AFE3H_TAG;


typedef struct
{
	unsigned char MsgId;
	unsigned char TotalPacket;
	unsigned char PackNo;
}ST_GET_MSG_AF62H_TAG;

typedef struct
{
	unsigned char StX[2];
	unsigned char StY[2];
	unsigned char width[2];
	unsigned char hight[2];
	t_cl          cla; //rg,rgb is not supper
}ST_INSERT_AREA;

typedef struct
{
	unsigned char StX[2];
	unsigned char StY[2];
	unsigned char wt; //rg,rgb is not supper
	unsigned char MesSize;
    unsigned char mWord[32][2];
	t_cl     mCl[32];
}ST_INSERT_Mes;

typedef struct
{
    unsigned char  TextId;
	unsigned char  clt; // 0 rg,1 rgb
	unsigned char  AreaCount;//���ֵ18(3X6)ʱ
	ST_INSERT_AREA area[18];
	unsigned char  MesCount;
	ST_INSERT_Mes  Mes[18];
}ST_AF1AH_SET_INSERT;

#if 0
typedef struct
{
	unsigned char No;
    unsigned char LocX;
	unsigned char LocY;
	unsigned char SizeX;
	unsigned char SizeY;
}ST_VIEW_CUT_TAG;//���ִ�����

typedef struct
{
    unsigned char PatternId;
	unsigned char hc;
	unsigned char vc;
	unsigned char ZoneCount;
	ST_VIEW_CUT_TAG zoneCut[21];
}ST_SET_VIEW_AF34H_TAG;

typedef struct
{
    unsigned char PatternId;
	unsigned char ZoneNo;
}ST_GET_VIEW_AF64H_TAG;

typedef struct
{
    unsigned char DisplayTime;
	unsigned char MsgId;
}ST_SHOW_MSG_TAG;

typedef struct
{
    unsigned char ZoneNo;
	unsigned char Show;
	unsigned char FitSize;
	unsigned char FontType;
	unsigned char DisCount;
	ST_SHOW_MSG_TAG DisMsg[64];
}ST_ZONEDIS_TAG;

typedef struct
{
	unsigned char ZoneNo;
    unsigned char DisId;
	unsigned char Times;
}ST_ZONE_CONTEXT_TAG;
	
typedef struct
{
	unsigned char PatternId;
    unsigned char ZoneCount;
	ST_ZONEDIS_TAG ZoneDis[21];
}ST_PRESET_ZONEGROUPDIS_AF35H_TAG;

typedef struct
{
	unsigned char PatternId;
    unsigned char ZoneNo;
}ST_GET_ZONEGROUPDIS_AF65H_TAG;

typedef struct
{
	unsigned char PatternId;
    unsigned char ZoneNo;
}ST_SET_ZONEGROUPDIS_AF36H_TAG;


typedef struct
{
    unsigned char StreetNo;
	unsigned char StreetStatue[50];
	unsigned char DisCycle;
}ST_STREET_DISPLAY_SET_AF19H_TAG;
#endif

//-----------------------------------
typedef enum
{
   OFFLINEMODE_OFF = 1, //��
   OFFLINEMODE_CYCLE,   //ѭ��
   OFFLINEMODE_HOLD,    //���ֵ�ǰ
   OFFLINEMODE_DELAY
}EN_OFFLINEMODE_TAG;

typedef struct
{
   unsigned char disp;// 1:Ϩ��,2:ѭ��ģʽ,��ѭ������δ�趨����ΪϨ��,3��ǰҳģʽ   
   unsigned char delay;
}ST_RET_OFFLINE_MODE_TAG;//�ѻ���ʾģʽ֮�ر�

typedef struct ST_BRIGHT_BITMAP_TAG
{
    unsigned char r65  : 1;  //%65
    unsigned char r80  : 1;  //%80
	unsigned char r100 : 1;  //%100
	unsigned char r50  : 1;  //%50
	unsigned char RpForceDimCtr:1;//���ǿ�ȼ������
	unsigned char ForceDimCtr:1;//ǿ�ȼ������
	unsigned char NA2  : 1;
	unsigned char LocalCtr : 1;//�ֳ�����		
}ST_BRIGHT_BITMAP_TAG;

typedef union 
{
    ST_BRIGHT_BITMAP_TAG ST_BRIGHT_BITMAP;
	unsigned char ucBright;
}UN_BRIGHT_BITMAP_TAG;

typedef struct
{
	UN_BRIGHT_BITMAP_TAG unBright;
	unsigned char ucStartHour;
	unsigned char ucStartMin;
	unsigned char ucEndHour;
	unsigned char ucEndMin;
}ST_BRIGHTNESS_TAG;//�豸���ȿ���֮�趨

typedef struct
{
    unsigned char ucBeginEnd; //0x01,start,0x00,stop
}ST_RET_BRIGHTNESS_MASTER_TAG;//�豸���ȿ��ƿ�ʼִ�л����ִ��ʱ�����ر�
//----------------------------------------------------------------

//----------------------ͨ����Ϣ֮���ݽṹ------------------
typedef struct
{
    //unsigned short cmd;
    unsigned char cmd_ml[2];
}ST_RET_SETOK_TAG;//�ر��趨��Ϣ��Ч

typedef struct
{
	//unsigned short Cmd;        //ָ����
	unsigned char cmd_ml[2];

    unsigned char  ucErrCode;  //������
    unsigned char  ucParameterNum;//��һ���������֮λַ
}ST_RET_SETERR_TAG;//�ر��趨���ѯ����Ϣ��Ч


typedef struct
{
    unsigned char ucReset[2];// 0x52 0x52
}ST_REBOOT_TAG;//�����ֳ��豸


typedef struct
{
	unsigned char ucEquipmentNo;//��·��� 0��ѯ���豸�ı�� 1-8��·��� FF������·���
}ST_GET_DEVICE_NUM_TAG;//��ѯ�ֳ��豸���

typedef struct
{
    unsigned char equ;//Ϊ0 ����ĸ�豸�ı��
	unsigned char number[2];
}ST_DEVICE_NUM_TAG;
typedef struct
{	
	unsigned char ucEquipmentNo;//��·��� 0��ѯ���豸�ı�� 1-8��·��� FF������·���
    unsigned char ucSubCount;   //��·����
    ST_DEVICE_NUM_TAG stSubDevice[64];
}ST_RET_DEVICE_NUM_TAG;//�ر��ֳ��豸���

typedef struct
{
	unsigned char Month;
	unsigned char Day;
	unsigned char Hour;
	unsigned char Min;
}ST_RET_POWERBREAK_TAG;//�ر�ǰ�ε�Դ�ж�

typedef struct
{
    unsigned char ucHardwareStatus;
}ST_RET_DEVICE_STATUS_TAG;//�ر���ѯ�ֳ��豸״̬

typedef struct
{
    unsigned char Year;
	unsigned char Month;
	unsigned char Day;
	unsigned char Week;
	unsigned char Hour;
	unsigned char Min;
	unsigned char Sec;
}ST_DATETIME_TAG;//�趨�ֳ��豸����ʱ��

typedef struct
{
    unsigned char SecDif;//ʱ����� 0��128��0ʱҪ�����Ķ�ʱ
}ST_DATETIME_SYNCH_TAG;//Ҫ�����Ķ�ʱ

typedef struct
{
	unsigned char Year;
	unsigned char Month;
	unsigned char Day;
    unsigned char CompanyID;
	unsigned char Ver;
    unsigned char CmdSet;
}ST_RET_SOFTVER_TAG;//�ر�����汾

typedef struct
{
    unsigned char Time;//0ֹͣ����,1:1sec ,2:2sec,3:5Sec;4:1min;5:5min
}ST_SET_HARDSTATUS_REPORTCLY_TAG;//�趨�ֳ��豸�ر�Ӳ��״̬֮����

typedef struct
{	//---------high byte-------
	unsigned char communicationConnect:1;
	unsigned char cabinatedOpened:1;
	unsigned char DisplayUnitFailure:1;	
	unsigned char brightnessSensorErr:1;
	unsigned char signalpowererr:1;
	unsigned char AU5_LightingErr:1;
	unsigned char CmsReady:1; //����ʱoff������ʱon
    unsigned char commlinebad:1;
	//----------low byte--------
    unsigned char cpuerr:1;
	unsigned char memrr:1;
	unsigned char timererr:1;
	unsigned char watch_dog_t_err:1;
	unsigned char powererr:1;
	unsigned char iouniterr:1;
	unsigned char displayUnitErr:1;
	unsigned char signalheaderr:1;		
}ST_HARDWARESTATUS_BITMAP_TAG;
typedef union
{
	ST_HARDWARESTATUS_BITMAP_TAG ST_HARDERR_BITMAP;
	//unsigned short uiHardwareStatus;
	unsigned char hdrs[2];
}ST_RET_HARDSTATUE_TAG;//�ֳ��豸�ر�Ӳ��״̬
//--------------------------------------------------

typedef struct
{
    unsigned short adc[3];
	unsigned char  rate[4];
}ST_BRIGHTNESS_DEF_TAG;

typedef struct
{
    unsigned char LockDB;//0ȫ��������,�����ֳ����� 1ȫ������,�������ֳ����� 2����DB�������ֳ�����
}ST_SET_LOCK_UNLOCK_TAG;//���������ֳ�����

typedef struct
{
    unsigned char LockDB;//0ȫ��������,�����ֳ����� 1ȫ������,�������ֳ����� 2����DB�������ֳ�����
}ST_RET_LOCK_UNLOCK_TAG;//�ر���ѯ���������ֳ�����
//----------------------------------------------------------

typedef struct
{
    unsigned char dles;
	unsigned char stx;
	unsigned char seq;
	unsigned char address[2];
	unsigned char len[2];
	unsigned char info[_INFO_BODY_MAXLEN];
}
ST_FRAME_MSG_TAG;//ͨѶ��Ϣ���

typedef struct
{
    unsigned char    count;//���ʹ���
    unsigned char    seq_lates;//���һ�η��͵����к�
    ST_FRAME_MSG_TAG ST_FRAME_MSG;
}ST_SENDRECORD_TAG;//���ͼ�¼

typedef struct
{
    unsigned char  password[6];
}ST_PASSWORD_TAG;

typedef enum
{
	VIEW_NORMAL,   //һ��ͼ����ʾ,��3.0Э��˫ɫͬ
	VIEW_ZONEGROUP,//���������ʾ (��չ)
}ST_VIEW_MODE_T;//��ʾ����

typedef struct
{
	ST_CIRCULATION_DISPLAY_TAG		     ST_CIRCULATION_DISPLAY;      //ѭ����ʾ��������
	unsigned char                        ucCurrShow_MsgId;            // 1 - 127:text,128-254:binmap 
//	ST_SET_ZONEGROUPDIS_AF36H_TAG        CurrZoneGroupId_t;
	ST_VIEW_MODE_T                       ViewMode;

	ST_SET_OFFLINE_MODE_TAG 			 stOffLine;		              //�ѻ���ʾģʽ֮�趨
	ST_SET_OFFLINE_MODE_TAG              stBreakLine;
	ST_BRIGHTNESS_TAG				     ST_BRIGHTNESS; 		      //�豸���ȿ���֮�趨

	ST_RET_DEVICE_NUM_TAG                ST_RET_DEVICE_NUM;           //�豸���  
	ST_RET_SOFTVER_TAG                   ST_RET_SOFTVER;              //����汾
    ST_PASSWORD_TAG                      ST_PASSWORD;                 //���Ͽ�����    
	ST_SET_LOCK_UNLOCK_TAG		         ST_SET_LOCK_UNLOCK;		  //���������ֳ�����
		
	unsigned char                        address[2];                  //�����ַ	
	
	ST_NET_CONF_TAG                      ipConfig;
	ST_BRIGHTNESS_DEF_TAG                ST_BRIGHTNESS_DEF;
	unsigned short                       packetsize_upload;           //�ϴ����ݰ��Ĵ�С 64-8192 
	unsigned char                        remoteportsel; //0 eth; 1 com0; 2 com1;

	ST_MISC_CONF_TAG                     tMisc;
}ST_SYSCONFIG_TAG;//ϵͳ����

typedef enum
{
	LED_SYS=0,
	LED_CMS=1,
	LED_LOCATE=2,
	LED_REMOTE=3,
}BOARD_LED_DEF;


typedef struct
{
    unsigned char code0;
	unsigned char code1;
	unsigned char crc;
}ST_USERFONT_CRC_TAG;

typedef struct
{
    unsigned char len_ml[2];
	ST_USERFONT_CRC_TAG  crc[130];
}ST_FONTIMAGE_CRC_TAG;


#if (PARK_CMS_ENB)

typedef struct
{
    unsigned char mode : 1;
	unsigned char textid : 1;
	unsigned char parkCount : 1;
	unsigned char parkGraphId : 1;
	unsigned char parkChId : 1;
	unsigned char parkEnId : 1;
	unsigned char parkArrowId : 1;
}t_PARK_ERRORCODE;

typedef union
{
	t_PARK_ERRORCODE strc;
	unsigned char byte;
}t_PARK_ERRORCODE_u;

typedef union
{
	ST_COLOR_TAG  color;
	unsigned char byte; 
}t_rgColor;

typedef struct
{
    unsigned char Icon;
	unsigned char CN;
	unsigned char EN;
	unsigned char arrow;
	unsigned char distance;
	unsigned char stall;
}t_ParkInfo;

typedef struct
{
	unsigned char mode;
	unsigned char textid_msg;
	unsigned char textid_park;
	unsigned char show;
	unsigned char parkcount;
	t_ParkInfo    parkInfo[32];
}t_PARK_CONTEXT_TEXT;

typedef struct
{
    t_rgColor Icon;
	t_rgColor CN;
	t_rgColor EN;
	t_rgColor arrow;
	t_rgColor distance;
	t_rgColor stall;
}t_ParkInfo_rgColor;

typedef struct
{
	unsigned char mode;
	unsigned char textid_msg;
	unsigned char textid_park;
	unsigned char show;
	unsigned char parkcount;	
	t_ParkInfo_rgColor   color[32];
}t_PARK_CONTEXT_rgColor;


typedef struct
{
	unsigned char mode;
	unsigned char textid_msg;
	unsigned char textid_park;
	unsigned char show;
	unsigned char parkcount;	
	t_ParkInfo         text[32];
	t_ParkInfo_rgColor color[32];
}t_PARK_CONTEXT;

typedef struct
{
    unsigned char CN_Id[2];
    unsigned char EN_Id[2];
}t_PARK_NAME_INQ;


typedef struct
{
	unsigned char Mode;
	unsigned char Num;
    unsigned char Bitmap[128];
}t_PARK_GRAPH;
#endif

typedef struct
{
  // unsigned char cmd0;
  // unsigned char cmd1;
   unsigned char g_code_id;
   unsigned char frame_no;
   unsigned char frame_id;
   unsigned char g_width[2];
   unsigned char g_height[2];
   unsigned char g_patter_cl[512];
}t_AF51H_SET_RGIMAGE;


//--------------��ʾ����--------------------------
typedef struct
{
	unsigned char id;
	unsigned char mode;
	unsigned char scanrate; 
	unsigned char lbith;
	unsigned char lbitl;
}ST_CMS_SETCHECKERR_TAG;

typedef struct
{
    unsigned char id;
	unsigned char statue;
}ST_CMS_RETCHECKERR_TAG;

typedef struct
{
	unsigned char TotalSub;
    unsigned char SubId;
	unsigned char ColorBits;//ÿ��RGB��λ��
	unsigned char brightness;//����ֵ
	unsigned char TotalPacket;
	unsigned char PackNo;
	unsigned char sizeh;
	unsigned char sizel;
	unsigned char scanrate; 
	unsigned char lbith;
	unsigned char lbitl;
}ST_CMS_DATAHEAD_TAG;

typedef struct
{
	ST_CMS_DATAHEAD_TAG head;
	unsigned char buf[CMS_DATABODY_LEN_MAX]; // CMS_DATABODY_LEN_MAX + 11 + 10(��Ϣͷ + crc) 
}ST_CMS_DATABODY_TAG;


//----------------------------------------------------
typedef union
{
	ST_CIRCULATION_DISPLAY_TAG           ST_CIRCULATION_DISPLAY;  //ѭ����ʾ��������
	ST_PRESET_TEXT_TAG                   ST_PRESET_TEXT;          //Ԥ��ȫ�Ĺ���֮�趨
	ST_PRESET_TEXT_COLOR_TAG             ST_PRESET_TEXT_COLOR;    //Ԥ��ȫ�Ĳ�ɫ��������֮�趨
	ST_MARQUEE_TAG                       ST_MARQUEE;              //������趨	
	ST_USER_CHINESE_FONT_TAG             ST_USER_CHINESE_FONT;         //�������δ�������
	ST_CLR_CHINESE_FONT_TAG              ST_CLR_CHINESE_FONT;         //�������δ������
    ST_COLOR_SINGLE_TEXT_TAG             ST_COLOR_SINGLE_TEXT;    //��ɫ�ִ�ͼ�ι���֮�趨
    ST_GET_COLOR_SINGLE_TEXT_TAG         ST_GET_COLOR_SINGLE_TEXT;    //��ɫ�ִ�ͼ�ι���֮��ѯ	
	ST_SET_OFFLINE_MODE_TAG              stOffLine;         //�ѻ���ʾģʽ֮�趨
	ST_BRIGHTNESS_TAG                    ST_BRIGHTNESS;           //�豸���ȿ���֮�趨

	ST_REBOOT_TAG                        ST_REBOOT;               //�����ֳ��豸
	ST_GET_DEVICE_NUM_TAG                ST_GET_DEVICE_NUM;           //��ѯ�ֳ��豸���
	ST_DATETIME_TAG                      ST_DATETIME;             //�趨�ֳ��豸����ʱ��
	ST_SET_HARDSTATUS_REPORTCLY_TAG      ST_SET_HARDSTATUS_REPORTCLY; //�趨�ֳ��豸�ر�Ӳ��״̬֮����
	ST_SET_LOCK_UNLOCK_TAG               ST_SET_LOCK_UNLOCK;          //���������ֳ�����
//---------------------up-----------------------------------------------------------------------------
	ST_RET_DISPLAY_MODULE_STATUS_TAG     ST_RET_DISPLAY_MODULE_STATUS;//��ʾģ��״̬�ر�
	ST_RET_OFFLINE_MODE_TAG              ST_RET_OFFLINE_MODE;         //�ѻ���ʾģʽ֮�ر�
	ST_RET_BRIGHTNESS_MASTER_TAG         ST_RET_BRIGHTNESS_MASTER;    //�豸���ȿ��ƿ�ʼִ�л����ִ��ʱ�����ر�

	ST_RET_SETOK_TAG                     ST_RET_SETOK;                //�ر��趨��Ϣ��Ч
	ST_RET_SETERR_TAG                    ST_RET_SETERR;               //�ر��趨���ѯ����Ϣ��Ч

	ST_RET_DEVICE_NUM_TAG                ST_RET_DEVICE_NUM;           //�ر��ֳ��豸���
	ST_RET_POWERBREAK_TAG                ST_RET_POWERBREAK;           //�ر�ǰ�ε�Դ�ж�
	ST_DATETIME_SYNCH_TAG                ST_DATETIME_SYNCH;           //Ҫ�����Ķ�ʱ
	ST_RET_SOFTVER_TAG                   ST_RET_SOFTVER;              //�ر�����汾
	ST_SET_HARDSTATUS_REPORTCLY_TAG      ST_RET_HARDSTATUS_REPORTCLY; //��ѯ���ֳ��豸�ر�Ӳ��״̬֮����
	ST_RET_HARDSTATUE_TAG                ST_RET_HARDSTATUE;           //�ֳ��豸�ر�Ӳ��״̬
	ST_RET_LOCK_UNLOCK_TAG               ST_RET_LOCK_UNLOCK;          //�ر���ѯ���������ֳ�����

    ST_PASSWORD_TAG                      ST_PASSWORD;                 //���Ͽ�����

	ST_FONTIMAGE_CRC_TAG                 ST_GET_FONT_CRC;
	ST_FONTIMAGE_CRC_TAG                 ST_GET_IMAGE_CRC;

#if (PARK_CMS_ENB)
    t_PARK_CONTEXT_TEXT                  PARK_CONTEXT_TEXT;
    t_PARK_CONTEXT_rgColor               PARK_CONTEXT_rgColor;	
	t_PARK_NAME_INQ                      PARK_NAME_INQ;
	t_PARK_GRAPH                         PARK_GRAPH;
#endif


//PANEL
#if(TC_PANEL_ENB)
    t_A009H_GET_PANEL_STATE    t_PanelStateGet;
    t_A00AH_RET_PANEL_STATE    t_PanelStateRet;
	t_A00BH_GET_PANEL_IN       t_PanelInput;	
#endif

/*
	ST_SET_MSG_AFE2H_TAG                 ST_SET_MSG_AFE2H;
	ST_SET_MSG_AF32H_TAG                 ST_SET_MSG_AF32H;
	ST_SET_VIEW_AF34H_TAG                ST_SET_VIEW_AF34H;
	ST_PRESET_ZONEGROUPDIS_AF35H_TAG     ST_PRESET_ZONEGROUPDIS_AF35H;
    ST_SET_ZONEGROUPDIS_AF36H_TAG        ST_SET_AreaGroupShow_AF36H;
    ST_STREET_DISPLAY_SET_AF19H_TAG      ST_STREET_DISPLAY_SET_AF19H;
	*/
	unsigned char                        ucbuf[_INFO_BODY_MAXLEN];
	ST_UPDATASOF_RETCRC_TAG              ST_UPDATASOF_CRC;
}
ST_INFO_CONTEXT_TAG;//ͨѶЭ���������ݽṹ

typedef enum
{
	__IDLE=0,
	__WAIT_ACK,  //�Ͳ�Ӧ��
    __WAIT_SEND, //�ȴ�����
}ST_SEND_STATUE_TAG;


typedef struct
{
	ST_SEND_STATUE_TAG  statue;
	unsigned char       type; // 0 info,1 ack,2 nak
	unsigned char       reSendCount;//�ѷ�����
	unsigned short      infoLen;
	ST_FRAME_MSG_TAG    infoBody;//
}ST_SEND_LIST_TAG;

typedef enum
{
    CONNECT_ONLINE =  0,
	CONNECT_OFFLINE = 1,
	CONNECT_BREAK   = 2,
	CONNECT_LOCK    = 3,
}ENUM_CONNECT_MODE_TAG;

typedef struct
{   
	 char theadName[50];
	 unsigned long threadid;
     unsigned long counter;
	 unsigned int  en;
}ST_WDG_TAG;

typedef struct
{
	unsigned int  len;//����������ֵ�����
    unsigned char buf[_INFO_BODY_MAXLEN];//32768 0x3000
}ST_TXRX_TAG;

typedef struct
{
	unsigned char  frame_total;//������
    unsigned char  frame_num;//����
	unsigned char  widthh;
	unsigned char  widthl;
	unsigned char  heighth;
	unsigned char  heightl;
	unsigned char  pattern_color[LOAD_PACKETSIZE_MAX];	
}ST_BITMAP_UPLOAD_TAG;//�ϱ���

typedef struct
{	
	int               socketfd;
	pthread_mutex_t   stSendListLock;
	ST_SEND_LIST_TAG  stSendList[__SEND_LIST_MAX];
	
    ST_TXRX_TAG     stRcvAnalytice;  //����������
	ST_TXRX_TAG     stSend;          //�����ͻ���
		
	pthread_mutex_t stMutexRcv;      //���ջ�����
    sem_t           Sem_RcvBuffNotEmpt;//�˿������ݵ���
	sem_t           Sem_RcvToSendPth;  //���յ���Ϣʱ����,�Ա㷢���߳�֪����ACK

	unsigned char   seq_rcv;
	int             fd;
	char            portname[32];
    char            devPth[50];

    ST_INFO_CONTEXT_TAG   stSendInfo;
	pthread_mutex_t       stSendInfoLock;

	unsigned short  scmd;
	unsigned int   infolen;//�������ֽڵ�ָ��
	
	unsigned char  lineStatue;//��״̬ 0���� 1���� 2����
	unsigned char  portLock;
	
	unsigned long count_breakline;
	unsigned long count_offline;
	unsigned int   rcvmsgtotal;//����LED״̬��ʾ

	unsigned int   uart_baudrate;
}ST_PORTRCVTXD_TAG; 

typedef struct
{
    unsigned int x,y;
	unsigned int codeid;	
}t_point;

typedef enum
{
	showmode_preset = 0,
    showmode_insert = 1,
}t_showmode;


typedef struct
{
	unsigned short              CmsBuf[CMS_PIXEL_W * CMS_PIXEL_H]; // 144 X 336 16λɫ
	unsigned char               CmsData[_CMS_CTRCARD_NUM][CMS_PART_DATALEN];//ѹ��֮���ٴ��� 8064 
	unsigned short              CmsData_Len[_CMS_CTRCARD_NUM];
	int                         CmsCardReady;//0 OK, >0������-1
	unsigned short              CmsCardAdc[2];
	int                         Sem_CmsTickAck[_CMS_CTRCARD_NUM];
    unsigned char               CmsUpdata;    //��ʾ����
    unsigned long               countTickComm;//����ư����������ʱ�����
	pthread_mutex_t             CmsDataLock;

	ST_HARDWARESTATUS_BITMAP_TAG stHardState;//Ӳ��״̬
	ST_HARDWARESTATUS_BITMAP_TAG stHardStateLast;//Ӳ��״̬,�������Ƚ��в�ͬ�����ϱ�

    unsigned char               sendseq;
	sem_t                       check_datatime;//����Уʱ
	sem_t                       upReportDelay; //�����ϱ��ȴ�����
	sem_t                       sysDataInitPth;//ϵͳ������ʼ���߳�
	sem_t                       Sem_RcvToUpdataCms;	

	ST_PORTRCVTXD_TAG           PortCenter_Eth;     
	ST_PORTRCVTXD_TAG           PortCenter_Com0;
	ST_PORTRCVTXD_TAG           PortCenter_Com1;
	ST_PORTRCVTXD_TAG           PortCenter_Com2; // UART 5
#if(TC_PANEL_ENB)
	ST_PORTRCVTXD_TAG           PortCenter_Com4; // PANEL
#endif	
	ST_PORTRCVTXD_TAG           PortCms;  //�ײ�

    ST_PORTRCVTXD_TAG           *PortRemote_p;
    ST_PORTRCVTXD_TAG           *PortLocate_p;

	ST_MARQUEE_TAG			    ST_MARQUEE; //������趨	
	sem_t                       Sem_Marquee;
//	marguee_run_t               marquee_r;

	ST_SYSCONFIG_TAG            stSysConfig;
    sem_t                       sem_UpdateConfig;
		
	ST_RET_POWERBREAK_TAG       ST_GET_POWERBROKKEN;

    unsigned  char              autotest;//�Զ�����ģʽ
	unsigned  char              CmsMode;
    unsigned  char              bsysRestEnd;

    ST_SET_HARDSTATUS_REPORTCLY_TAG ST_SET_HARDSTATUS_REPORTCLY; //�趨�ֳ��豸�ر�Ӳ��״̬֮����

    unsigned char               WdtNum;
	char                        Thread_Name[50][50];
	pthread_t                   Thread_Id[50];	 
    ST_WDG_TAG                  ThreadWdg[50];

    sem_t                       Sem_CmsRetErrMap; //from cms ret
    sem_t                       Sem_CmsCheckErr;  //���ķ����Ĵ���������
    sem_t                       Sem_CmsModuleCheck;//���Ĳ�ѯģ�����ʱ����
    sem_t                       Sem_CmsCheckerrRet;//���������ʱ����
	
    //unsigned char               CheckErrType;// 0 short 1 open 2 broken 
    ST_CMS_DATABODY_TAG         CmsRcvGetDate;
    unsigned char               CmsErrBuf[3][_CMS_CTRCARD_NUM][CMS_LEDNUM_ONECARD]; // 32 * 16 * 48
    unsigned short              CmsErrLen[3][_CMS_CTRCARD_NUM];
#if (CMS_SPCE_CONFIG == CMS_RG1X1_F48X48_SHG)
    unsigned short				CmsModule[_CMS_CTRCARD_NUM][9]; //����ģ�黵�Ƹ���	 	
#elif (CMS_SPCE_CONFIG == CMS_RG6X1_F32X32)
    unsigned short				CmsModule[24]; //����ģ�黵�Ƹ���    
#elif(CMS_SPCE_CONFIG == CMS_RG8X1_F32X32)  
    unsigned short				CmsModule[_CMS_CTRCARD_NUM][_CMS_CTRCARD_DISP_V/16][_CMS_CTRCARD_DISP_W/16];//����ģ�黵�Ƹ���
#else
	unsigned short              CmsModule[_CMS_CTRCARD_NUM][_CMS_CTRCARD_DISP_V/16][_CMS_CTRCARD_DISP_W/16];//����ģ�黵�Ƹ���
#endif

#if (CMS_SPCE_CONFIG == CMS_RG1X1_F48X48_SHG)
    unsigned int				CmsLedS[_CMS_CTRCARD_NUM][_CMS_CTRCARD_SCAN_RATE][_CMS_CTRCARD_SCAN_W][3];//����״̬��3��״̬����
#else
    unsigned int                CmsLedS[_CMS_CTRCARD_NUM][_CMS_CTRCARD_SCAN_RATE][_CMS_CTRCARD_SCAN_W];//����״̬��3��״̬����
#endif
	unsigned char               CmsMap_t[CMS_PIXEL_W / 4 * CMS_PIXEL_H]; // 144 X 336 RG��һ�ֽ�ռ4��
	unsigned short              CmsBufBak[CMS_PIXEL_W * CMS_PIXEL_H]; // 144 X 336 16λɫ
	
	ST_RET_DISPLAY_MODULE_STATUS_TAG ST_CMS_MODULE_ERR;
	unsigned int                offlineTime;// sec
	unsigned int                zonetime[21];//�Ӵ���ʾ��ͣ��ʱ��
	unsigned char               zonedisId[21];
	unsigned int                disUpdateTimeUnit;//��λ��ms
	unsigned int                disUpdateTimeUnitBck;//��λ��ms

#if (PARK_CMS_ENB)
    t_PARK_CONTEXT              PARK_CONTEXT;
    unsigned char               showpark;
#endif

#if 0
    unsigned short              CmsBuf_Tick[CMS_PIXEL_W * CMS_PIXEL_H];
    unsigned short              curr_x;
#endif

    int                         eth0_isdown;
    int                         wdttest;	

	pthread_mutex_t             bitmap_lock;
	pthread_mutex_t             imagemsg_lock;

	t_showmode                  showmode;
}
ST_CONTEXT_TAG;

extern ST_CONTEXT_TAG *stgContextp;

#define PROTO_USER_IMAGE_SMALL //�û�ͼƬ�����ִ�Ϊһ�������صģ���������һ�������ص�

int AF10H_SET_CIRCULATION_DISPLAY_FUN(void *pbuf_context,void *pbuf_port); //ѭ����ʾ��������    
int AF40H_GET_CIRCULATION_DISPLAY_FUN(void *pbuf_context,void *pbuf_port);//ѭ����ʾ������ѯ	

int AF11H_SET_PRESET_TEXT_FUN(void *pbuf_context,void *pbuf_port);//Ԥ��ȫ�Ĺ���֮�趨
int AF41H_GET_PRESET_TEXT_FUN(void *pbuf_context,void *pbuf_port);//Ԥ��ȫ�Ĺ���֮��ѯ

int AF12H_SET_PRESET_TEXT_COLOR_FUN(void *pbuf_context,void *pbuf_port);//Ԥ��ȫ�Ĳ�ɫ��������֮�趨
int AF42H_GET_PRESET_TEXT_COLOR_FUN(void *pbuf_context,void *pbuf_port);//Ԥ��ȫ�Ĳ�ɫ��������֮��ѯ

int AF13H_SET_CURRENT_TEXT_FUN(void *pbuf_context,void *pbuf_port);//��ǰȫ����ʾ����֮�趨
int AF43H_GET_CURRENT_TEXT_FUN(void *pbuf_context,void *pbuf_port);//��ǰȫ����ʾ����֮��ѯ
int AF14H_CLR_CURRENT_TEXT_FUN(void *pbuf_context,void *pbuf_port);//��ǰ��ʾ��������

int AF44H_GET_CURRENT_TEXT_COLOR_FUN(void *pbuf_context,void *pbuf_port);//��ǰȫ�Ĳ�ɫ��������֮��ѯ

int AF15H_SET_MARQUEE_FUN(void *pbuf_context,void *pbuf_port);//������趨
int AF45H_GET_MARQUEE_FUN(void *pbuf_context,void *pbuf_port);//����Ʋ�ѯ 

int AF16H_SET_CHINESE_FONT_FUN(void *pbuf_context,void *pbuf_port);//�������δ�������
int AF46H_GET_CHINESE_FONT_FUN(void *pbuf_context,void *pbuf_port);//�������δ����ѯ	
int AF17H_CLR_CHINESE_FONT_FUN(void *pbuf_context,void *pbuf_port);//�������δ������

int AF47H_GET_DISPLAY_MODULE_STATUS_FUN(void *pbuf_context,void *pbuf_port);//��ʾģ��״̬��ѯ

int AF18H_SET_COLOR_SINGLE_TEXT_FUN(void *pbuf_context,void *pbuf_port);//��ɫ�ִ�ͼ�ι���֮�趨
int AF48H_GET_COLOR_SINGLE_TEXT_FUN(void *pbuf_context,void *pbuf_port);//��ɫ�ִ�ͼ�ι���֮��ѯ	

int AF31H_SET_OFFLINE_MODE_FUN(void *pbuf_context,void *pbuf_port);//�ѻ���ʾģʽ֮�趨
int AF61H_GET_OFFLINE_MODE_FUN(void *pbuf_context,void *pbuf_port);//�ѻ���ʾģʽ֮��ѯ	

int AF3EH_SET_BRIGHTNESS_FUN(void *pbuf_context,void *pbuf_port);//�豸���ȿ���֮�趨
int AF6EH_GET_BRIGHTNESS_FUN(void *pbuf_context,void *pbuf_port);//�豸���ȿ���֮��ѯ�ر�

int AE60H_SET_PACKSIZE_UPLOAD_FUN(void *pbuf_context,void *pbuf_port);
int AE61H_GET_PACKSIZE_UPLOAD_FUN(void *pbuf_context,void *pbuf_port);

int AF52H_GET_BITMAP_JUNYE_FUN(void *pbuf_context,void *pbuf_port);

int AE18H_SET_CENTER_PORT_FUN(void *pbuf_context,void *pbuf_port);
int AE19H_GET_CENTER_PORT_FUN(void *pbuf_context,void *pbuf_port);
//----------------------------------------------------------------------------------------------------------

int _0F10H_SET_REBOOT_FUN(void *pbuf_context,void *pbuf_port);//�����ֳ��豸
int _0F40H_GET_DEVICE_NUM_FUN(void *pbuf_context,void *pbuf_port);//��ѯ�ֳ��豸���
int _0F11H_SET_RESET_FUN(void *pbuf_context,void *pbuf_port);//���趨�ֳ��豸
int _0F41H_GET_DEVICE_STATUS_FUN(void *pbuf_context,void *pbuf_port);//��ѯ�ֳ��豸״̬
int _0F12H_SET_DATETIME_FUN(void *pbuf_context,void *pbuf_port);//�趨�ֳ��豸����ʱ��
int _0F42H_GET_DATETIME_FUN(void *pbuf_context,void *pbuf_port);//��ѯ�ֳ��豸����ʱ��
int _0F13H_SET_SOFTVER_FUN(void *pbuf_context,void *pbuf_port);//�趨�ֳ��豸��Ϣ�ȼ�
int _0F43H_GET_SOFTVER_FUN(void *pbuf_context,void *pbuf_port);//��ѯ����汾
int _0F14H_SET_HARDSTATUS_REPORTCLY_FUN(void *pbuf_context,void *pbuf_port);//�趨�ֳ��豸�ر�Ӳ��״̬֮����
int _0F4FH_GET_HARDSTATUS_REPORTCLY_FUN(void *pbuf_context,void *pbuf_port);//��ѯ�ֳ��豸�ر�Ӳ��״̬֮����

int _0F15H_SET_DATA_PASSWORD_FUN(void *pbuf_context,void *pbuf_port);//�趨���Ͽ����������
int _0F45H_GET_DATA_PASSWORD_FUN(void *pbuf_context,void *pbuf_port); //��ѯ���Ͽ����������

int _0F16H_SET_LOCK_UNLOCK_FUN(void *pbuf_context,void *pbuf_port);//���������ֳ�����
int _0F46H_GET_LOCK_UNLOCK_FUN(void *pbuf_context,void *pbuf_port);//��ѯ���������ֳ�����

int _0F47H_GET_PROTOL_CODE_FUN(void *pbuf_context,void *pbuf_port);//��ѯͨ��Э�� 

int AE10H_UPDATASOF_SET_FUN(void *pbuf_context,void *pbuf_port); //�������
int AE40H_UPDATASOF_GETCRC_FUN(void *pbuf_context,void *pbuf_port);
int AEC1H_UPDATASOF_SETEND_FUN(void *pbuf_context,void *pbuf_port);
int AE20H_CLEAN_USERFILE_FUN(void *pbuf_context,void *pbuf_port);//����û�ͼƬ
int AE05H_SET_NET_CONF_FUN(void *pbuf_context,void *pbuf_port);
int AE45H_GET_NET_CONF_FUN(void *pbuf_context,void *pbuf_port);
int AE43H_GET_CHECKFONT_FUN(void *pbuf_context,void *pbuf_port);
int AE44H_GET_CHECKIMAGE_FUN(void *pbuf_context,void *pbuf_port);

int AE06H_SET_ADDRESSCODE_FUN(void *pbuf_context,void *pbuf_port);//S
int AE46H_GET_ADDRESSCODE_FUN(void *pbuf_context,void *pbuf_port);//

int AE07H_SET_CURRBRIGHT_FUN(void *pbuf_context,void *pbuf_port);
int AE47H_GET_CURRBRIGHT_FUN(void *pbuf_context,void *pbuf_port);
int AE08H_SET_BRIGHTTAB_FUN(void *pbuf_context,void *pbuf_port);
int AE48H_GET_BRIGHTTAB_FUN(void *pbuf_context,void *pbuf_port);
int AE23H_GET_CONTRMODE_FUN(void *pbuf_context,void *pbuf_port);

int AE11H_SET_BREAKLINE_FUN(void *pbuf_context,void *pbuf_port);
int AE12H_GET_BREAKLINE_FUN(void *pbuf_context,void *pbuf_port);

int AE65H_GET_CMSTYPE_FUN(void *pbuf_context,void *pbuf_port);
int A007H_RET_CMS_SHOWD_FUN(void *pbuf_context,void *pbuf_port);
int A002H_RET_CMS_TICK_FUN(void *pbuf_context,void *pbuf_port);

int A005H_RET_CMS_ERRMAP_FUN(void *pbuf_context,void *pbuf_port);

int AE33H_SET_CMSERRCHECK_FUN(void *pbuf_context,void *pbuf_port);

int AE67H_GET_MISC_FUN(void *pbuf_context,void *pbuf_port);
int AE69H_SET_MISC_FUN(void *pbuf_context,void *pbuf_port);
int AE6A_WDT_TEST_FUN(void *pbuf_context,void *pbuf_port);

#if (PARK_CMS_ENB)
int AF19H_SET_PARK_CONTEXT_FUN(void *pbuf_context,void *pbuf_port);
int AF43H_GET_CURR_PARK_CONTEXT_FUN(void *pbuf_context,void *pbuf_port);
int AF32H_SET_PARK_CONTEXT_RGCOLOR_FUN(void *pbuf_context,void *pbuf_port);
int AF23H_ADD_PARK_INFO_FUN(void *pbuf_context,void *pbuf_port);
int AF53H_GET_PARK_INFO_FUN(void *pbuf_context,void *pbuf_port);
int AF24H_DEL_PARK_INFO_FUN(void *pbuf_context,void *pbuf_port);
int AF55H_GET_PARK_INFO_ERR_FUN(void *pbuf_context,void *pbuf_port);
int AF62H_SET_PARK_ICON_FUN(void *pbuf_context,void *pbuf_port);
int AF72H_GET_PARK_ICON_FUN(void *pbuf_context,void *pbuf_port);
int AF63H_SET_PARK_INDICATE_FUN(void *pbuf_context,void *pbuf_port);
int AF73H_GET_PARK_INDICATE_FUN(void *pbuf_context,void *pbuf_port);
int AF44H_GET_CURR_PARK_TEXT_COLOR_FUN(void *pbuf_context,void *pbuf_port);
#endif

//int AF51H_SET_RGIMAGE_FUN(void *pbuf_context,void *pbuf_port);
int AF32H_SET_PRESET_MSG_FUN(void *pbuf_context,void *pbuf_port);//Ԥ����Ϣ�趨
int AF62H_GET_PRESET_MSG_FUN(void *pbuf_context,void *pbuf_port);//Ԥ����Ϣ��ѯ

int AF33H_SET_CURR_MSG_FUN(void *pbuf_context,void *pbuf_port);//ѡ��Ԥ����Ϣ
int AF63H_GET_CURR_MSG_FUN(void *pbuf_context,void *pbuf_port);//��ѯ��ǰ��Ϣ

int GetSendInfoPacket(unsigned char *dest,unsigned char seq,unsigned char *address,unsigned char *infop,unsigned int len,ST_PORTRCVTXD_TAG *stPortp);

int Wdg_Create(ST_WDG_TAG *stWdg,unsigned long threadId,char *pname);

int Wdg_Check(ST_CONTEXT_TAG *stContextp,unsigned char bOn);

unsigned char Brightness_Control(ST_CONTEXT_TAG *stContextp);

//void test_uploadbitmap(ST_SET_MSG_AFE3H_TAG *pack);

int CmsErrCheck(ST_CONTEXT_TAG *stContextp,int allcheck);

//panel
#if(TC_PANEL_ENB)
int A009H_GET_PANEL_STATE_FUN(void *pbuf_context,void *pbuf_port);
int A00BH_GET_PANEL_IN_FUN(void *pbuf_context,void *pbuf_port);
#endif

int AF4AH_GET_INSERT_FUN(void * pbuf_context, void * pbuf_port);
int AF1AH_SET_INSERT_FUN(void *pbuf_context,void *pbuf_port);


#endif
