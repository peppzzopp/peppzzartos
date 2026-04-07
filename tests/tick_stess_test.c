#include "kernel.h"
#include "systick.h"

void task(void){
    while(1){
        kernel_task_delay(1000);
    }
}

int main(){
    kernel_init();
    systick_enable();

    for(uint32_t i=0; i<31; i++){
        kernel_task_add(task);
    }
    kernel_start();
    while(1){
    }
    return 0;
}
