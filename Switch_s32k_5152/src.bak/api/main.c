/******************************************************************************
 *                                                                            *
 *   FreeRTOS sample application for RTD on S32 platforms                     *
 *                                                                            *
 *   Copyright 2023 NXP                                                       *
 *                                                                            *
 ******************************************************************************/

/* Including necessary configuration files. */
#include "Clock_Ip.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "Siul2_Port_Ip.h"
#include "Siul2_Dio_Ip.h"
#include "umsdUtil.h"

#define main_TASK_PRIORITY                ( tskIDLE_PRIORITY + 2 )

SemaphoreHandle_t sem_handle;
volatile BaseType_t testResult = 0x33U;
TaskHandle_t sendTaskHandle,recvTaskHandle;

/**
 * 始终运行优先级最高且可运行的任务的一个后果是 从未进入阻塞或挂起状态的高优先级任务将永久性剥夺
 * 所有更低优先级任务的任何执行时间。这就是通常最好创建事件驱动型任务的原因之一 。
 * 例如，如果一个高优先级任务正在等待一个事件，那么它就不应 处于该事件的循环（轮询）中，因为如果处于轮询中，它会一直运行，
 * 永远不进入“阻塞”或 “挂起”状态。相反，该任务应进入“阻塞”状态等待这一事件。
 * 该事件可以 通过某个FreeRTOS任务间通信和同步原语发送至任务。
 * 收到事件后，优先级更高的任务会自动解除“阻塞”状态。
 * 这样低优先级 任务会运行，而高优先级任务会处于“阻塞”状态。
 */
/**
 * 如果 configUSE_PREEMPTION 为 0，则表示抢占关闭,这样只有当运行状态的任务进入“阻塞”或“挂起”状态
 * 或运行状态任务调用taskYIELD()或中断服务程序 (ISR) 手动请求上下文切换时，才会发生上下文切换。
 * configUSE_PREEMPTION 为 1，则表示开启任务抢占机制
 *
 * 如果 configUSE_TIME_SLICING 为 0，则表示时间切片已关闭， 因此调度器不会在每个 tick 中断上在同等优先级的任务之间切换。
 * 如果 configUSE_TIME_SLICING 为 1，则会在时间切片到之后，如果有其他相同优先级的任务，则会执行其他任务。
 */

/**
* @brief        SendTask is used to give the semaphore
* @details      SendTask give the semaphore every 1 second
*/
void SendTask( void *pvParameters )
{
    (void)pvParameters;
    BaseType_t operation_status;
    for( ;; )
    {
        operation_status = xSemaphoreGive(sem_handle);//释放信号量(发送事件，通知ReceiveTask可以继续执行)
        configASSERT(operation_status == pdPASS);//不允许ReceiveTask中断
        /* Not very exciting - just delay... */
        vTaskDelay(pdMS_TO_TICKS(1000));//等待1s，在等待的过程中，虽然ReceiveTask可以抢占该任务(即使该任务在执行程序的过程中，该任务也可能被中断（由于优先级较低）)，但是由于在这里阻塞，不会调用xSemaphoreGive，导致ReceiveTask也会阻塞在xSemaphoreTake的代码中。
    }
}

/**
* @brief        ReceiveTask get the semaphore and toggle pins
* @details      ReceiveTask try to get the semaphore with portMAX_DELAY timeout,
*               After receiving the semaphore successfully, the led will be toggle
*
*/
void ReceiveTask( void *pvParameters )
{
    (void)pvParameters;
    BaseType_t operation_status;
    BaseType_t count = 0;
    for( ;; )
    {
    	//如果不使用信号量，该任务的优先级较高，一旦执行该任务，SendTask将会得不到执行
        operation_status = xSemaphoreTake(sem_handle, portMAX_DELAY);// 尝试获取互斥信号量
        configASSERT(operation_status == pdPASS);//禁用中断（任何中断程序都不会生效），然后进行死循环。这样就出错时就能知道出错代码的原因
        Siul2_Dio_Ip_TogglePins(LED3_PORT, (1 << LED3_PIN));
        if(count++ > 10)
        {
            testResult = 0x5AU;
        }
    }
}

/**
* @brief        Main function of the example
* @details      Initializes the used drivers and uses 1 binary Semaphore and
*               2 tasks to toggle a LED.
*/

int main(void)
{
    /* Initialize Clock */

    Clock_Ip_StatusType Status_Init_Clock = CLOCK_IP_ERROR;
    Status_Init_Clock = Clock_Ip_Init(Clock_Ip_aClockConfig);

    if (Status_Init_Clock != CLOCK_IP_SUCCESS)
    {
        while(1); /* Error during initialization. */
    }

    /* Initialize all pins using the Port driver */
    Siul2_Port_Ip_PortStatusType Status_Init_Port = SIUL2_PORT_ERROR;
    Status_Init_Port = Siul2_Port_Ip_Init(NUM_OF_CONFIGURED_PINS_PortContainer_0_BOARD_InitPeripherals, g_pin_mux_InitConfigArr_PortContainer_0_BOARD_InitPeripherals);

    if(Status_Init_Port != SIUL2_PORT_SUCCESS)
    {
        while(1); /* Error during initialization. */
    }

    vSemaphoreCreateBinary(sem_handle);//初始化值为"满"，可用状态

    xTaskCreate( SendTask   , ( const char * const ) "SendTask", configMINIMAL_STACK_SIZE, (void*)0, main_TASK_PRIORITY, &sendTaskHandle);
    xTaskCreate( ReceiveTask, ( const char * const ) "RecTask" , configMINIMAL_STACK_SIZE, (void*)0, main_TASK_PRIORITY + 1,  &recvTaskHandle );

    vTaskStartScheduler();//执行任务循环

    for( ;; );

    return 0;
}

/** @} */
