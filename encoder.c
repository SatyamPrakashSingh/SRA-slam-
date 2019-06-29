#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "SRA18.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"

#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/api.h"

#define ENCODER_PHASE_A_1     16
#define ENCODER_PHASE_B_1     17
#define ENCODER_PHASE_A_0     25
#define ENCODER_PHASE_B_0     26


#define ESP_INTR_FLAG_DEFAULT 0


static volatile bool dir_0 = 0;
static volatile int16_t count_0 = 0;
static volatile bool dir_1 = 0;
static volatile int16_t count_1 = 0;



static xQueueHandle gpio_evt_queue_0 = NULL;
static xQueueHandle gpio_evt_queue_1 = NULL;

static gpio_config_t io_conf;


static void config_isr(int arg)
{
    io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1 << arg);
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);
}

static void config_isr_neg(int arg)
{
    io_conf.intr_type = GPIO_PIN_INTR_NEGEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1 << arg);
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);
}


static void config_input(int arg)
{
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.pin_bit_mask = (1ULL << arg);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 0;
    io_conf.pull_down_en = 0;
    gpio_config(&io_conf);
}

static void config_output(int arg)
{
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.pin_bit_mask = (1ULL << arg);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = 0;
    io_conf.pull_down_en = 0;
    gpio_config(&io_conf);    
}

static volatile unsigned int isrcalls=0;
static volatile unsigned int isrcalls2=0;
static void IRAM_ATTR gpio_isr_handler_0(void* arg)
{
    isrcalls++;
    if(gpio_get_level(ENCODER_PHASE_B_0) == 0)
    {
        count_0++;
    }

    else if(gpio_get_level(ENCODER_PHASE_B_0) == 1)
    {
        count_0--;
    }
}

static void IRAM_ATTR gpio_isr_handler_1(void* arg)
{
    isrcalls2++;

    if(gpio_get_level(ENCODER_PHASE_B_1) == 0)
    {
        count_1++;
    }

    else if(gpio_get_level(ENCODER_PHASE_B_1) == 1)
    {
        count_1--;
    }
}




static void run_rhino(void* arg)
{   

    for(;;) 
    {
        
        // printf("The Encoder ticks for motor 0: %d\n", count_0);
        // printf("Isr Calls1 : %d   , Ticks1: %d ",isrcalls,count_0);
        // printf("Isr Calls2 : %d   , Ticks2: %d \n",isrcalls2,count_1);
       printf(" ,  %d , %d , \n",count_0,count_1 );

    }
}
void drive_task(void *arg)
{

    //enable_buttons();   //Enable the buttons connected to GPIO 2 and GPIO 5
    //mcpwm_initialize(); //Initialize PWM to control speed of motors

    while(1)
    {

        /*
            Make the Bot go forward, at a PWM of 80 if button 1 is pressed
        */
        int tickgoal=300;
        if(count_0<tickgoal)
        {
            
            printf("%s\n","BOT FORWARD");

            bot_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, 80, 80);   
        }

        /*
            Make the Bot go backward, at a PWM of 80 if button 1 is pressed
        */
        else if(count_0>tickgoal)
        {
            
            printf("%s\n","BOT BACKWARD");  

            bot_backward(MCPWM_UNIT_0, MCPWM_TIMER_0, 80, 80);  //Make the Bot go backward, at a PWM of 80 if button 2 is pressed
        }

        /*
            If no buttons are pressed, stop the bot
        */

        else 
        {
            bot_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
            printf("%s\n","BOT STOPPED");
        }
        if(count_1<tickgoal)
        {
            
            printf("%s\n","BOT FORWARD");

            bot_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, 80, 80);   
        }

        /*
            Make the Bot go backward, at a PWM of 80 if button 1 is pressed
        */
        else if(count_1>tickgoal)
        {
            
            printf("%s\n","BOT BACKWARD");  

            bot_backward(MCPWM_UNIT_0, MCPWM_TIMER_0, 80, 80);  //Make the Bot go backward, at a PWM of 80 if button 2 is pressed
        }

        /*
            If no buttons are pressed, stop the bot
        */

        else 
        {
            bot_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
            printf("%s\n","BOT STOPPED");
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void app_main()
{

    // initialise_wifi();

    config_isr(ENCODER_PHASE_A_0);
    config_isr(ENCODER_PHASE_A_1);
    
  
    config_input(ENCODER_PHASE_B_0);
    config_input(ENCODER_PHASE_B_1);


   // xTaskCreate(&drive_task,"drive_task",4096,NULL,1,NULL);
    xTaskCreate(run_rhino, "rhino robot", 2048, NULL, 10, NULL);

    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(ENCODER_PHASE_A_0, gpio_isr_handler_0, (void*) ENCODER_PHASE_B_0);
    gpio_isr_handler_add(ENCODER_PHASE_A_1, gpio_isr_handler_1, (void*) ENCODER_PHASE_B_1);

}

