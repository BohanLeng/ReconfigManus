//
// Created by bohanleng on 04/11/2025.
//

#ifndef RECONFIGMANUS_MES_SERVER_DEF_H
#define RECONFIGMANUS_MES_SERVER_DEF_H
#include <cstdint>



//////////////////////////////////////////////////////////////////
// order run status
enum ORDER_RUN_STATUS
{
	ORDER_WAIT,
	ORDER_DOING,
	ORDER_DONE,
	ORDER_ERROR,
	ORDER_DELETE
};

// exec step status
enum STEP_EXEC_STATUS
{
	EXEC_WAITING,
	EXEC_DOING,
	EXEC_DONE,
	EXEC_FAILED
};


//////////////////////////////////////////////////////////////////
#define ME_MSG_START					0x000
// order insert
#define MSG_ORDER_INSERT_REQ		0x020
typedef struct {
	uint8_t		user_id;
	uint32_t	order_type;
	uint32_t	part_left;
	uint32_t	part_right;
	uint32_t	part_bottom;
	uint32_t	logo_type;
} OrderReq;

#define MSG_ORDER_INSERT_RSP		0x021
typedef struct
{
	uint32_t	order_id;
	uint32_t	rsp_code;
	char		rsp_msg[256];
} OrderRsp;

// order delete
#define MSG_ORDER_ACTION_REQ		0x022
typedef struct {
	uint32_t	order_id;
} OrderActionReq;

#define MSG_ORDER_ACTION_RSP		0x023
typedef struct {
	uint8_t		user_id;
	uint32_t	order_id;
	uint32_t	rsp_code;
	char		rsp_msg[256];
} OrderActionRsp;

#define MSG_ORDER_STATISTICS_PUSH	0x024
// order statistics info
typedef struct {
	uint32_t		order_type;
	uint32_t		order_num;
	uint32_t		order_done;
	uint32_t		order_undone;
} OrderStatisticsInfo;

#define MSG_ORDER_RUNTIME_INFO_RECEIVE		0x025
// order running info
typedef struct {
	uint32_t		order_type;
	uint32_t		order_id;
	uint32_t		tray_id;
	uint32_t		bear_car;
	uint32_t		step_id;

	uint32_t		module_type;
	uint32_t		action_id;
	uint32_t		action_param;

	uint32_t		module_id;
	uint32_t		station;
	uint32_t		error_id;

	STEP_EXEC_STATUS	step_status;
	ORDER_RUN_STATUS	order_status;
} OrderRunningInfo;

#define MSG_ORDER_RUNTIME_INFO_PUSH			0x026

// order exec step info
struct ST_ModuleExecStep {};

typedef struct
{
	ST_ModuleExecStep	exec_step;
	STEP_EXEC_STATUS	step_status;
	uint64_t			theory_time_in_ms;
	uint64_t			real_time_in_ms;
} ModuleExecStep;

#define MSG_ORDER_RESET_REQ			0x027
// failed order reset req
typedef struct
{
	uint32_t		car_id;
}ST_CarResetReq;

#define MSG_ORDER_RESET_RSP			0x028
// failed order reset rsp
typedef struct
{
	uint32_t		car_id;
	uint32_t		rsp_code;
	char			rsp_msg[256];
} ST_OrderResetRsp;


#define MSG_PRODUCTION_LINE_INFO_QUERY			0x029
#define MSG_PRODUCTION_LINE_INFO				0x02A
typedef struct
{
	uint32_t			num_of_running_device;
	double				current_power;
	double				current_gas_usage;
	uint32_t			num_of_staff;
	double				plant_temperature;
	double				plant_humidity;
} ST_MESProductionLineInfo;


typedef struct
{
	int base_id;
	int station_id;
	int action_id;
	int action_param;
} ST_MESModuleAction;

typedef struct
{
	int		id;
	int		action_id;
	char	action_id_comment[128];
	int		action_param;
	char	action_param_comment[128];
	int		module_type;
} ST_MESProcessData;

#define MSG_ORDER_STATUS_INFO_QUERY			0x032
typedef struct
{
	int		order_sn;
} ST_OrderStatus;

// Used for now
#define MSG_STATION_ACTION_QUERY				0x1046
#define MSG_STATION_ACTION_DONE_QUERY			0x1047

typedef struct
{
	uint32_t        workstation_id;
	uint32_t        tray_id;
} ST_StationActionQuery;

#define MSG_STATION_ACTION_RSP					0x1048

typedef struct
{
	ST_StationActionQuery qry;
	uint32_t    action_type; // 0 for release and read next_station_id, 1 for execute and ignore next_station_id
	uint32_t    next_station_id;
} ST_StationActionRsp;


#define ME_MSG_END					0x2FF

#endif
