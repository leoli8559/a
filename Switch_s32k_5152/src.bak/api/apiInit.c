#include <apiInit.h>
#include <deviceInfoModule.h>
#include <string.h>
#include <signal.h>
#include "smiasscess.h"

DeviceConfig g_allDevicesConfig[MAX_SOHO_DEVICES];

#define DEFAULT_ETHERTYPE_VALUE  0x9101


extern MSD_STATUS deviceAtuModuleInitial(void);
extern MSD_STATUS deviceVlanModuleInitial(void);
extern MSD_STATUS deviceFilterModuleInitial(void);

extern MSD_STATUS deviceMacModuleInitialMacInfo(IN MSD_U8 devNum);
extern MSD_STATUS deviceVlanModuleInitialVlanInfo(IN MSD_U8 devNum);
extern MSD_STATUS devicePortSegmentationModuleInitialSegmentationInfo(IN MSD_U8 devNum);
extern MSD_STATUS deviceFilterModuleInitialFilterInfo(IN MSD_U8 devNum);
extern MSD_STATUS checkVlanEntry(IN MSD_U8 devNum, IN MSD_U16 vid, OUT MSD_BOOL* isAdd);
extern MSD_STATUS setFidValue(IN MSD_U8 devNum, IN MSD_U16 fid);
extern void releaseAllFidValues(IN MSD_U8 devNum);

static MSD_STATUS initDeviceModule(void)
{
	deviceAtuModuleInitial();
	deviceVlanModuleInitial();
	deviceFilterModuleInitial();
	return MSD_OK;
}

#ifdef  USE_SEMAPHORE
static MSD_SEM osSemCreate(MSD_SEM_BEGIN_STATE state)
{
	MSD_SEM pSem;
	int count = (int)state;

	pSem = semaphoreCreate(count);

	return pSem;
}

static MSD_STATUS osSemDelete(MSD_SEM smid)
{
	int ret = 0;

	ret = semaphoreClose(smid);

	if (ret < 0)
		return MSD_FAIL;
	else
		return MSD_OK;
}
static MSD_STATUS osSemWait(MSD_SEM smid, MSD_U32 timeOut)
{
	int ret = 0;

	ret = semaphoreWait(smid, timeOut);

	if (ret < 0)
		return MSD_FAIL;
	else
		return MSD_OK;
}
static MSD_STATUS osSemSignal(MSD_SEM smid)
{
	int ret = 0;

	ret = semaphoreRelease(smid);

	if (ret < 0)
		return MSD_FAIL;
	else
		return MSD_OK;
}

#endif

/*******************************************************************************************
 * @brief init_all_device_config
 * 初始化设备配置对象，在调用任何API接口之前，应该先调用该接口初始化所有设备的配置对象
 * 该接口会初始化最多32个设备的配置对象，都为其设置了设备配置对象的默认参数值，只有调用了
 * 该接口，才可以为其指定设备编号的指定参数设置，设备编号（0~31），Device Number参考Global offset 0x1C
 * Device Number用于多芯片系统，用于标志不同的设备
 * @return
 ****************************************************************************************/
static MSD_STATUS initDeviceConfig(void)
{
	for(MSD_U8 i = 0; i < MAX_SOHO_DEVICES; ++i){
		msdMemSet(&g_allDevicesConfig[i], 0, sizeof(DeviceConfig));
		g_allDevicesConfig[i].devNum = i;//默认从0开始，如果只有1台设备（通常也只有1台），硬件重置时需要将设备编号设置为0
		g_allDevicesConfig[i].eventLoopHandle = NULL;
		g_allDevicesConfig[i].isOpen = MSD_FALSE;
		g_allDevicesConfig[i].xMutex = xSemaphoreCreateMutex();
		if(g_allDevicesConfig[i].xMutex == NULL){
			return MSD_FAIL;
		}
		g_allDevicesConfig[i].isCallAPI = MSD_FALSE;
	}
	return initDeviceModule();
}

/**
 * SMI协议读取函数.
 *
 * \param devNum[in]:设备num，暂未使用
 * \param phyAddr[in]:读取的物理地址0~31
 * \param miiReg[in]：SMI寄存器地址
 * \param value[out]:存放寄存器的读取结果
 * \return 成功返回MSD_OK,否则返回MSD_FAIL.
 */
static MSD_STATUS smiRead(MSD_U8 devNum, MSD_U8 phyAddr,
		MSD_U8 miiReg, MSD_U16* value)
{
	//使用Clause 45进行访问，如果想使用Clause 22进行访问，则通过全局配置变量进行配置访问方式
	int ret = readC45Register(phyAddr, 3, miiReg | 0x8000);
	*value = (MSD_U16)ret;
	return ret < 0 ? MSD_FAIL : MSD_OK;
}

/**
 * 使用SMI协议写入寄存器.
 *
 * \param dev_num[in]:暂未使用
 * \param phyAddr[in]:要写入的物理地址
 * \param miiReg[in]:要写入的寄存器
 * \param value[in]:要写入寄存器的值
 * \return
 */
static MSD_STATUS smiWrite(MSD_U8 devNum, MSD_U8 phyAddr,
		MSD_U8 miiReg, MSD_U16 value)
{
	/*For Amethyst, access register through C45 is supported, and devAddr been fixed as 3, and register has one 0x8000 offset*/
	//ret = writeRegister(phyAddr, miiReg, value); //warpper.h
	int ret = writeC45Register(phyAddr, 3, miiReg | 0x8000, value); //
	return ret < 0 ? MSD_FAIL : MSD_OK;
}


/**
 * Register function to BSP.
 *
 * \param devNum: 设备编号，如果只有1台设备，通常从0开始
 * \param baseAddr:  Physical Device Address
 * \param busInterface: 总线接口(访问)类型
 * \param tempDeviceId: 设备id，88Q5152为0xb520
 * \return
 */
static MSD_STATUS startDevice(MSD_U8 devNum, MSD_U8 baseAddr, MSD_INTERFACE busInterface, MSD_U16 tempDeviceId)
{
	MSD_STATUS status = MSD_FAIL;
	MSD_SYS_CONFIG   cfg;

	memset((char*)&cfg, 0, sizeof(MSD_SYS_CONFIG));

	//cfg.BSPFunctions.rmu_tx_rx = rmuSendAndReceivePacket; //
	cfg.BSPFunctions.rmu_tx_rx = NULL;
	cfg.BSPFunctions.readMii = smiRead;
	cfg.BSPFunctions.writeMii = smiWrite;
	cfg.InterfaceChannel = busInterface;
#ifdef USE_SEMAPHORE
	cfg.BSPFunctions.semCreate = osSemCreate; //创建信号量
	cfg.BSPFunctions.semDelete = osSemDelete;
	cfg.BSPFunctions.semTake = osSemWait;
	cfg.BSPFunctions.semGive = osSemSignal;
#else
	cfg.BSPFunctions.semCreate = NULL;
	cfg.BSPFunctions.semDelete = NULL;
	cfg.BSPFunctions.semTake = NULL;
	cfg.BSPFunctions.semGive = NULL;
#endif

	cfg.devNum = devNum;
	cfg.baseAddr = baseAddr;
	cfg.rmuMode = MSD_RMU_ETHERT_TYPE_DSA_MODE;
	cfg.eTypeValue = DEFAULT_ETHERTYPE_VALUE;
	cfg.tempDeviceId = tempDeviceId;
	status = msdLoadDriver(&cfg);
	return status;
}


static void eventThreadProc(void* param)
{
	DeviceConfig *deviceConfig = (DeviceConfig*)param;
	for(;;){
		//使用pdMS_TO_TICKS（）以毫秒为单位指定时间，而不是直接以tick为单位指定时间，可以确保在tick频率改变时应用程序中指定的时间不会改变。
		vTaskDelay(pdMS_TO_TICKS(1000));
		if (xSemaphoreTake(deviceConfig->xMutex, portMAX_DELAY) == pdTRUE) {
			MSD_VTU_INT_STATUS vlanInt;
			msdMemSet(&vlanInt, 0, sizeof(MSD_VTU_INT_STATUS));
			MSD_STATUS ret = msdVlanViolationGet(deviceConfig->devNum, &vlanInt);
			if (ret == MSD_OK && vlanInt.vtuIntCause.missVio) { //仅仅在启用了802.1Q的端口，才会发生VTU Miss，这时候创建一个FID和帧的VID相同的VTU项，交换机就会自动创建FID = VID的动态数据库条目。
				MSD_BOOL isAddAtu;
				ret = checkVlanEntry(deviceConfig->devNum, vlanInt.vid, &isAddAtu);
				if (ret != MSD_OK){
					xSemaphoreGive(deviceConfig->xMutex);
					continue;
				}
				setFidValue(deviceConfig->devNum, vlanInt.vid);
			}
			xSemaphoreGive(deviceConfig->xMutex);
		}

	}
	vTaskDelete(NULL);//删除自身
}

MSD_STATUS initOpenDevice(MSD_U8 devNum)
{
	if (devNum > (MSD_U8)(MAX_SOHO_DEVICES - 1)){
		return MSD_BAD_PARAM;
	}
	if (g_allDevicesConfig[devNum].isOpen)
		return MSD_OK;
	MSD_STATUS status = initDeviceConfig();
	if(status != MSD_OK) return status;
	int tryCount = 0;//尝试连接设备的次数，多次尝试失败就返回
	while(1){
		//		   setUSBPort(0);
		//		   ret = openDefaultUSBDriver();
		//		   if(ret != 0) return MSD_FAIL;
		status = startDevice((MSD_U8)devNum, 0, MSD_INTERFACE_SMI, 0);//
		if(status == MSD_OK)
			break;
		vTaskDelay(pdMS_TO_TICKS(1000));
		if(++tryCount == 3){//3 time fail,return
			MSD_DBG_ERROR(("initOpenDevice failed,the status is %d\n",status));
			return MSD_FAIL;
		}
	}
	do {
		//设备初始化后的各个Module的初始化
		//ATU module
		status = deviceMacModuleInitialMacInfo(devNum);
		if (status != MSD_OK) {
			MSD_DBG_ERROR(("device_mac_module_initial_mac_info failed,the status is %d\n",status));
			break;
		}
		//VLAN Module
		status = deviceVlanModuleInitialVlanInfo(devNum);
		if (status != MSD_OK) {
			MSD_DBG_ERROR(("device_vlan_module_initial_vlan_info failed,the status is %d\n",status));
			break;
		}
		// port segmentation
		status = devicePortSegmentationModuleInitialSegmentationInfo(devNum);
		if (status != MSD_OK) {
			MSD_DBG_ERROR(("device_filter_module_initial_segmentation_info failed,the status is %d\n",status));
			break;
		}
		//filter Module
		status = deviceFilterModuleInitialFilterInfo(devNum);
		if (status != MSD_OK) {
			MSD_DBG_ERROR(("device_filter_module_initial_segmentation_info failed,the status is %d\n",status));
			break;
		}
	} while (0);
	if(status != MSD_OK)//相关模块的初始化出现错误
		status = msdUnLoadDriver(devNum);
	else {//打开成功
		g_allDevicesConfig[devNum].isOpen = MSD_TRUE;
		BaseType_t ret = xTaskCreate(eventThreadProc, "pollTask",configMINIMAL_STACK_SIZE, &g_allDevicesConfig[devNum],1,&g_allDevicesConfig[devNum].eventLoopHandle);
		if(ret  == pdPASS){
			vTaskStartScheduler();//启动调度器去执行任务
		}
	}
	return status;
}

MSD_STATUS initStartCallAPI(MSD_U8 devNum)
{
	CHECK_DEV_NUM_IS_CORRECT;
	if(g_allDevicesConfig[devNum].isCallAPI){//已经调用过该接口
		return MSD_OK;
	}
	if (xSemaphoreTake(g_allDevicesConfig[devNum].xMutex, portMAX_DELAY) == pdTRUE) {
		g_allDevicesConfig[devNum].isCallAPI = MSD_TRUE;
		vTaskSuspend(g_allDevicesConfig[devNum].eventLoopHandle);//挂起loop线程
		xSemaphoreGive(g_allDevicesConfig[devNum].xMutex);
		return MSD_OK;
	}
	return MSD_FAIL;
}

MSD_STATUS initStopCallAPI(MSD_U8 devNum)
{
	CHECK_DEV_NUM_IS_CORRECT;
	if(!g_allDevicesConfig[devNum].isCallAPI){
		return MSD_OK;
	}
	if (xSemaphoreTake(g_allDevicesConfig[devNum].xMutex, portMAX_DELAY) == pdTRUE) {
		g_allDevicesConfig[devNum].isCallAPI = MSD_FALSE;
		vTaskResume(g_allDevicesConfig[devNum].eventLoopHandle);////唤醒loop线程
		xSemaphoreGive(g_allDevicesConfig[devNum].xMutex);
		return MSD_OK;
	}
	return MSD_FAIL;
}

void initCloseDevice(MSD_U8 devNum)
{
	if (!g_allDevicesConfig[devNum].isOpen) return;
	releaseAllFidValues(devNum);
	vSemaphoreDelete(g_allDevicesConfig[devNum].xMutex);
	g_allDevicesConfig[devNum].isOpen = MSD_FALSE;
	msdUnLoadDriver(devNum);
}


