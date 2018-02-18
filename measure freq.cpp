#include "mbed.h"
#include "EAOLED.h"
#include "TextDisplay.h"

EAOLED oled(p5, p6, p7, p8, p25);//I/O declarations
DigitalOut led[]={(LED1),(LED2),(LED3),(LED4)};
PortIn port(Port0, 0x00040C30);//PortIn declaration, Port0 holds the input pins we want, 40C30 is the bit code for the pins we want (4,5,10,11,18)

Timeout interval;
volatile int timer_count = 0;//Global variable needed to signal timeout to the main function. Starts at 0, exits loop at 1.
void incr_timer_count(void);//Function increments timer_count

int main(){
    float touched_freq[4], untouched_freq[4], current_freq[4];
    bool touch_state[4];
    touch_state[0] = touch_state[1] = touch_state[2] = touch_state[3] = false;//Set all states to untouched
    
    untouched_freq[0]=485000;//Defining touched and untouched frequency thresholds
    touched_freq[0]=460000;
    untouched_freq[1]=540000;
    touched_freq[1]=510000;
    untouched_freq[2]=485000;
    touched_freq[2]=450000;
    untouched_freq[3]=430000;
    touched_freq[3]=410000;
    
    //----------------------------COUNTING CODE FROM PART 1----------------------------------//
    int loop_count[4];//Count for 4 pins
    float timeperiod=0.1;//Time Period where the function counts
    oled.cls();// OLED clear screen
    int last_port_state=0,port_state,port_change,i;
    while (1) {        
        timer_count=0;//Resetting counts and timer to 0 at the beginning of loop
        loop_count[0] = loop_count[1] = loop_count[2] = loop_count[3] = 0;
            
        interval.attach(&incr_timer_count, timeperiod);//Raising an interrupt after timeperiod and going to incr_timer_count function
        while (timer_count!=1){//Counts pulses until value is 0
            port_state = port.read();//Read the value from the port
            port_change = last_port_state^port_state;//XOR with last state to only leave the changed bits as 1
            if(port_change&(1<<4))//Bitshift by 4 to represent 4th pin, if 1 (changed) increment
                loop_count[3]++;//P6
            if(port_change&(1<<11))
                loop_count[1]++;//P9
            if(port_change&(1<<10))
                loop_count[2]++;//P10
            if(port_change&(1<<18))
                loop_count[0]++;//P12
            last_port_state = port_state;//Save current state for next comparison
        }
    //--------------------------END COUNTING CODE FROM PART 1--------------------------------//    
    
        for(i=0;i<4;i++){//Calculate current frequency and determine if pad is touched
            current_freq[i]=float(loop_count[i])/(2*timeperiod);
            if (current_freq[i]>untouched_freq[i])//If current frequency greater than untouched threshold, pad is untouched
                led[i]=touch_state[i]=false;//Give touch indication on LED
            else if (current_freq[i]<touched_freq[i])//If current frequency less than touched threshold, pad is touched
                led[i]=touch_state[i]=true;
        }//If current frequency is between the 2 thresholds, previous state is preserved
        
        for(i=0;i<4;i++){//Display
            oled.locate(0,2*i);//Locate position 0,2*i (x,y) on OLED
            oled.printf("%d: %.0f", i, current_freq[i]);//Print frequency
            if(touch_state[i])
                oled.printf(" T");//T and U give indication whether pad is touched or not
            else
                oled.printf(" U");
        }
    }
}

void incr_timer_count(void) { //Increments timercount to 1, stopping the counting loop
    timer_count = 1;
}