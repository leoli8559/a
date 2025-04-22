#include <apiInit.h>
#include <devicePortSegmentationModule.h>
#include <stdlib.h>
#include "Fir_msdPortCtrl.h"


static MSD_BOOL s_isEnablePortSegmentation[MAX_SOHO_DEVICES] = { MSD_FALSE }; //是否开启port_segmentation功能。

static PortSegmentationConfig s_portSegmentation[MAX_SOHO_DEVICES] = { 0 };

/**
 * @brief devicePortSegmentationModuleInitialSegmentationInfo
 * 初始化操作,后续会根据配置文件进行修改,目前就设置为默认值
 * @param devNum
 * @return
 */
MSD_STATUS devicePortSegmentationModuleInitialSegmentationInfo(IN MSD_U8 devNum)
{
    s_portSegmentation[devNum].isSymmetrically = MSD_FALSE;
    s_isEnablePortSegmentation[devNum] = MSD_TRUE;
    MSD_STATUS ret = devicePortSegmentationModuleResetSegmentation(devNum);
    s_isEnablePortSegmentation[devNum] = MSD_FALSE;
    return ret;
}


MSD_STATUS devicePortSegmentationModuleSetClearSegmentationToConfig(IN MSD_U8 devNum)
{
    MSD_STATUS  ret = MSD_OK;
    CHECK_DEV_NUM_IS_CORRECT;
    if(!s_isEnablePortSegmentation[devNum]){
        return MSD_FEATRUE_NOT_ALLOW;
    }
    MSD_U8 portCount = dev->numOfPorts;
    for (int i = 0; i < portCount - 1; ++i) {
        for (int j = 0; j < portCount - 1; ++j) {
            s_portSegmentation[devNum].memPortSegmentationSet[i][j] = MSD_FALSE;
        }
    }
    s_portSegmentation[devNum].isSymmetrically = MSD_FALSE;
    return ret;
}

MSD_STATUS devicePortSegmentationModuleSetAllSegmentationToConfig(IN MSD_U8 devNum, IN MSD_BOOL isForwardBack)
{
    MSD_STATUS  ret = MSD_OK;
    CHECK_DEV_NUM_IS_CORRECT;
    if(!s_isEnablePortSegmentation[devNum]){
        return MSD_FEATRUE_NOT_ALLOW;
    }
    if (isForwardBack != MSD_TRUE && isForwardBack != MSD_FALSE) {
        isForwardBack = MSD_FALSE;
    }
    MSD_U8 portCount = dev->numOfPorts;
    for (int i = 0; i < portCount - 1; ++i) {
        for (int j = 0; j < portCount - 1; ++j) {
            if (!isForwardBack && i == j) {////帧不允许转发给自身
                s_portSegmentation[devNum].memPortSegmentationSet[i][j] = MSD_FALSE;
            }
            else {
                s_portSegmentation[devNum].memPortSegmentationSet[i][j] = MSD_TRUE;
            }

        }
    }
    return ret;
}

MSD_STATUS devicePortSegmentationModuleSetSegmentationToConfig(IN MSD_U8 devNum,IN PortSegmentationConfig* segmentationConfig)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if(segmentationConfig == NULL){
        MSD_DBG_ERROR(("device_port_segmentation_module_set_segmentation failed,the segmentation_config is NULL!\n"));
        return MSD_BAD_PARAM;
    }
    if(!s_isEnablePortSegmentation[devNum]){
        return MSD_FEATRUE_NOT_ALLOW;
    }
    MSD_U8 portCount = dev->numOfPorts;
    int memPortSegmentationSetSize = segmentationConfig->memPortSegmentationSetSize;
    if (memPortSegmentationSetSize < portCount - 1) { //如果有端口没有设置具体的值时
        for(int i = memPortSegmentationSetSize; i < portCount - 1; ++i){//设置默认值
            for(int j = memPortSegmentationSetSize; j < portCount - 1; ++j){
                if(i != j){//i和j不相同时，代表出入口不相同，代表port i可以出口帧到port j
                    s_portSegmentation[devNum].memPortSegmentationSet[i][j] = MSD_TRUE;
                }else{ //端口不转发到自身端口
                    s_portSegmentation[devNum].memPortSegmentationSet[i][j] = MSD_FALSE;
                }
            }
        }
    }
    for (int i = 0; i < memPortSegmentationSetSize; ++i) {
        for (int j = 0; j < memPortSegmentationSetSize; ++j) {
            s_portSegmentation[devNum].memPortSegmentationSet[i][j] = segmentationConfig->memPortSegmentationSet[i][j];
        }
    }
    s_portSegmentation[devNum].isSymmetrically = segmentationConfig[devNum].isSymmetrically;
    s_portSegmentation[devNum].memPortSegmentationSetSize = portCount - 1;//设置为端口 - 1(port 0的出口信息不保存)
    return MSD_OK;
}

MSD_STATUS devicePortSegmentationModuleGetSegmentationFromConfig(IN MSD_U8 devNum, OUT PortSegmentationConfig* segmentationConfig)
{
    MSD_STATUS  ret = MSD_OK;
    CHECK_DEV_NUM_IS_CORRECT;
    if(segmentationConfig == NULL){
        MSD_DBG_ERROR(("device_port_segmentation_module_get_segmentation failed,segmentation_config is NULL!\n"));
        return MSD_BAD_PARAM;
    }
    if(!s_isEnablePortSegmentation[devNum]){
        return MSD_FEATRUE_NOT_ALLOW;
    }
    msdMemSet(segmentationConfig, 0, sizeof(PortSegmentationConfig));
    segmentationConfig->isSymmetrically = s_portSegmentation[devNum].isSymmetrically;
    int size = s_portSegmentation[devNum].memPortSegmentationSetSize;
    segmentationConfig->memPortSegmentationSetSize = size;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            segmentationConfig->memPortSegmentationSet[i][j] = s_portSegmentation[devNum].memPortSegmentationSet[i][j];
        }
    }
    return ret;
}

MSD_STATUS devicePortSegmentationModuleRefreshSegmentation(IN MSD_U8 devNum, OUT PortSegmentationConfig* segmentationConfig)
{
    MSD_STATUS  ret = MSD_OK;
    CHECK_DEV_NUM_IS_CORRECT;
    if (segmentationConfig == NULL) {
        MSD_DBG_ERROR(("device_port_segmentation_module_get_port_segmentation failed,the mem_port_segmentation_set param is NULL !\n"));
        return MSD_BAD_PARAM;
    }
    if(!s_isEnablePortSegmentation[devNum]){
        return MSD_FEATRUE_NOT_ALLOW;
    }
    msdMemSet(segmentationConfig,0,sizeof(PortSegmentationConfig));
    MSD_U8 portCount = dev->numOfPorts;//
    s_portSegmentation[devNum].memPortSegmentationSetSize = portCount - 1;
    segmentationConfig->memPortSegmentationSetSize = portCount - 1;
    segmentationConfig->isSymmetrically = s_portSegmentation[devNum].isSymmetrically;//从设置中获取结果
    //从Port 1开始获取
    MSD_LPORT memPorts[MSD_MAX_SWITCH_PORTS];//存放交换机端口(除Port 0外的所有端口)的VLAN Table值。
    MSD_U8 memPortsLen = 0;
    for (int i = 1; i < portCount; ++i) { //用以表示端口号，从Port 1 开始，i = 0代表 Port 1.....i = mem_ports_len - 1 代表最大的Port端口号
        msdMemSet(memPorts, 0, portCount * sizeof(MSD_LPORT));
        ret = msdPortBasedVlanMapGet(devNum, i, memPorts, &memPortsLen);//从Port 1开始获取，
        if (ret != MSD_OK) return ret;
        for (int j = 0; j < memPortsLen; ++j) {
            if (memPorts[j] != 0) { //Port 0的VLAN TABLE 不存放到结果数组中。设置为MSD_TRUE,代表相应的VLANTABLE被设置
                segmentationConfig->memPortSegmentationSet[i-1][memPorts[j] - 1] = MSD_TRUE;
                s_portSegmentation[devNum].memPortSegmentationSet[i - 1][memPorts[j] - 1] = MSD_TRUE;
            }
        }
    }
    return ret;
}


MSD_STATUS devicePortSegmentationModuleSaveSegmentation(IN MSD_U8 devNum)
{
    MSD_STATUS ret = MSD_OK;
    CHECK_DEV_NUM_IS_CORRECT;
    if(!s_isEnablePortSegmentation[devNum]){
        return MSD_FEATRUE_NOT_ALLOW;
    }
    MSD_U8 portCount = dev->numOfPorts;//
    PortSegmentationConfig *portSegmentation = &s_portSegmentation[devNum];//
    PortSegmentationConfig tempSegmentation;
    msdMemSet(&tempSegmentation,0,sizeof(portSegmentation));
    int memPortSegmentationSetSize = portCount - 1;//只需要交换机端口数 - 1个端口需要设置。
    if(s_portSegmentation[devNum].isSymmetrically){//对称的情况，将port_segmentation_set设置为对称的,但是在前端页面并不显示，仅仅是保存到交换机中
        for(int i = 0; i < memPortSegmentationSetSize; ++i){
            for(int j = 0; j < memPortSegmentationSetSize; ++j){
                if(portSegmentation->memPortSegmentationSet[i][j]){//port i到port j被设置
                    tempSegmentation.memPortSegmentationSet[i][j] = MSD_TRUE;
                }else{//port i到port j没有被设置
                    if(portSegmentation->memPortSegmentationSet[j][i]){//port j到port i被设置
                        tempSegmentation.memPortSegmentationSet[i][j] = MSD_TRUE;
                    }else{
                        tempSegmentation.memPortSegmentationSet[i][j] = MSD_FALSE;
                    }
                }
            }
        }
    }else{ //非对称的情况，不修改
        for(int i = 0; i < memPortSegmentationSetSize; ++i){
            for(int j = 0; j < memPortSegmentationSetSize; ++j){
                tempSegmentation.memPortSegmentationSet[i][j] = portSegmentation->memPortSegmentationSet[i][j];
            }
        }
    }
    MSD_LPORT memPorts[MSD_MAX_SWITCH_PORTS] = {0};//存放获取到的 Port 0 到Port 最大值的VLAN Table值。
    MSD_U8 memPortsLen = 0;
    for (int i = 0; i < memPortSegmentationSetSize; ++i) {//i = 0代表入口1.i = 1代表入口2......
        MSD_U8 memPortSize = 0;
        msdMemSet(memPorts, 0, MSD_MAX_SWITCH_PORTS * sizeof( MSD_LPORT));//
        //获取Port i + 1的出口Port 0的设置情况(即外部端口是否可以给Port 0发送帧)
        MSD_BOOL isPort0Set = MSD_FALSE;
        MSD_LPORT portNum = (MSD_LPORT)i+1;
        ret = msdPortBasedVlanMapGet(devNum, portNum, memPorts, &memPortsLen);//从Port 1开始获取
        if (ret != MSD_OK) return ret;
        if(memPorts[0] == 0){//port i is can send packet to port 0
            isPort0Set = MSD_TRUE;
        }
        msdMemSet(memPorts,0,MSD_MAX_SWITCH_PORTS * sizeof( MSD_LPORT));//
        if (isPort0Set) {//其余端口端口可以发送帧给Port 0
            memPorts[0] = 0;//设置为0
            memPortSize++;
        }

        //获取Port i + 1的其他出口端口的VLAN TABLE
        for (int j = 0; j < memPortSegmentationSetSize; ++j) {
            if (tempSegmentation.memPortSegmentationSet[i][j]) {
                memPorts[memPortSize] = j + 1; //j = 0时 ，即某行的第1列为1代表Port 1需要设置VLAN Table
                memPortSize++;
            }
        }
        ret = msdPortBasedVlanMapSet(devNum, portNum, memPorts, memPortSize);//从Port 1开始设置
        if (ret != MSD_OK) break;
    }
    return ret;
}

MSD_STATUS devicePortSegmentationModuleResetSegmentation(MSD_U8 devNum)
{
    MSD_STATUS ret = MSD_OK;
    CHECK_DEV_NUM_IS_CORRECT;
    if(!s_isEnablePortSegmentation[devNum]){
        return MSD_FEATRUE_NOT_ALLOW;
    }
    int portCount = dev->numOfPorts;
    MSD_LPORT memPorts[MSD_MAX_SWITCH_PORTS] = {0};//存放获取到的 Port 0 到Port 最大值的VLAN Table值。

    //设置所有端口的帧可以出口除自身以外的其他端口
    for(int i = 1 ; i < portCount;++i){
        msdMemSet(memPorts,0,MSD_MAX_SWITCH_PORTS * sizeof(MSD_LPORT));
        int count = 0;
        for(int j = 0; j < portCount; ++j){
            if(i != j){
                memPorts[count++] = j;
            }
        }
        ret = msdPortBasedVlanMapSet(devNum,i,memPorts, portCount - 1);
        if(ret != MSD_OK)
            break;
    }
    //save port1 to port max segmentation
    for(int i = 0 ; i < portCount - 1;++i){
        for(int j = 0; j < portCount - 1; ++j){
            if(i != j){
                s_portSegmentation[devNum].memPortSegmentationSet[i][j] = MSD_TRUE;
            }else{
                s_portSegmentation[devNum].memPortSegmentationSet[i][j] = MSD_FALSE;
            }
        }
    }
    s_portSegmentation[devNum].isSymmetrically = MSD_FALSE;
    s_portSegmentation[devNum].memPortSegmentationSetSize = portCount - 1;
    return ret;
}

MSD_STATUS devicePortSegmentationModuleSetEnableSegmentation(MSD_U8 devNum, MSD_BOOL isEnable)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if(isEnable != MSD_FALSE && isEnable != MSD_TRUE)//默认设置为true
        isEnable = MSD_TRUE;
    s_isEnablePortSegmentation[devNum] = isEnable;
    return MSD_OK;
}

MSD_STATUS devicePortSegmentationModuleGetEnableSegmentation(MSD_U8 devNum, MSD_BOOL *isEnable)
{
	CHECK_DEV_NUM_IS_CORRECT;
    *isEnable = s_isEnablePortSegmentation[devNum];
    return MSD_OK;
}

MSD_STATUS devicePortSegmentationModuleSaveOnePortSegmentation(MSD_U8 devNum, int portNum, MSD_BOOL *memPortSegmentationSet, int memPortSegmentationSize)
{
	CHECK_DEV_NUM_IS_CORRECT;
    int portCount = dev->numOfPorts;
    if(portNum <= 0 || portNum > portCount - 1){
        MSD_DBG_ERROR(("device_port_segmentation_module_save_one_port_segmentation failed,please check the the port_num param is correct(0~%d)!\n",portCount-1));
        return MSD_BAD_PARAM;
    }
    if(!s_isEnablePortSegmentation[devNum]){
        return MSD_FEATRUE_NOT_ALLOW;
    }
    if(memPortSegmentationSet == NULL){
        memPortSegmentationSize = 0;
    }
    MSD_LPORT tempSegmentation[MSD_MAX_SWITCH_PORTS] = {0};
    int count = 0;
    for(int i = 0; i < memPortSegmentationSize;++i){
        if(memPortSegmentationSet[i]){//如果被设置，就将端口号加入到到端口列表中
            tempSegmentation[count++] = i;
        }
    }
    for(int i = memPortSegmentationSize; i < portCount; ++i){ //没有设置的情况，则默认端口可以出口除自身外的所有端口.
        if(i != portNum){//除自身外的其他端口
            tempSegmentation[count++] = i;
        }
    }
    MSD_STATUS ret = msdPortBasedVlanMapSet(devNum, portNum, tempSegmentation, count);
    return ret;

}
