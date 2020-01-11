#ifndef __PROTOCOL_CMD_CFG_H__
#define __PROTOCOL_CMD_CFG_H__

#define PROTOCOL_EF02     		1u 
#define	PROTOCOL_EF02_PLUS		2u

//-----ef03
#define PC_ADDR             0x01
#define PD_ADDR				0x02
#define BMS_ADDR			0x03
#define INV_ADDR			0x04
#define MPPT_ADDR			0x05

#define AUTO_TEST_ADDR		0x09
#define 	HOST_ADDR         AUTO_TEST_ADDR



#define COMM_CMD_SET       0x1


//----shell的子命令ID
#define CMD_ID_SHELL_IN             0x21
#define CMD_ID_SHELL_OUT            0x22

//----参数配置的子命令ID
#define CMD_ID_GET_PARAM_CNT        0x31
#define CMD_ID_GET_PARAM_ITEM       0x32
#define CMD_ID_WR_PARAM_ITEM        0x33
#define CMD_ID_RD_PARAM_ITEM        0x34
#define CMD_ID_RS_PARAM_ITEM        0x35
#define CMD_ID_RS_ALL_PARAM_ITEM    0x36


#define REC_CMDSET_REC     0xff
    #define REC_CMDID_REC_CFG       0x01

//-------user cmd
#define TEST_CMD_SET	   0x30
	#define TEST_CMD_ID				0x01


	//------数据记录 0x80-0xFE
	#define TEST_CTRL_DAT_CMD_ID	0x80

#endif //__PROTOCOL_CMD_CFG_H__
