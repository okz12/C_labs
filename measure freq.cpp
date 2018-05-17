#include "mbed.h"
#include "EAOLED.h"
#include "TextDisplay.h"
#define PI 3.14159265359
#define DIV 32 //Number of samples in one complete sine or triangular wave
//More samples means a more accurate waveform but lower maximum output frequency and vice versa

void OutputWaveform(int freq, int type);
void AnalogSquareGenerator();
void AnalogSineGenerator();
void AnalogTriagGenerator();
void Display(int freq_digit[],int select, int wavetype);
void incr_timer_count(void);

EAOLED oled(p5, p6, p7, p8, p25);//I/O declarations
DigitalOut led[]={(LED1),(LED2),(LED3),(LED4)};
PortIn port(Port0, 0x00040C30);//PortIn declaration, Port0 holds the input pins we want, 40C30 is the bit code for the pins we want (4,5,10,11,18)
PwmOut pwmpin(p21);//PWM pin outputs squarewave
AnalogOut analogpin(p18);//Analog Output pin for different waveforms

Timeout interval;
Ticker flipper;
float analogSineWave[DIV];//Will hold 1 complete sine wave
float analogTriagWave[DIV];//Will hold 1 complete triangular wave
volatile int timer_count = 0;//Global variable needed to signal timeout to the main function. Starts at 0, exits loop at 1.
volatile int c=0;

int main(){
    int i, select=4, freq_digit[5], analog_count=0, wavetype=0, freq_out=0;    
    float touched_freq[4], untouched_freq[4], current_freq[4];
    bool touch_state[4], last_touch_state3=false;
    touch_state[0]=touch_state[1]=touch_state[2]=touch_state[3]=false;//Set all states to untouched
    freq_digit[0]=freq_digit[1]=freq_digit[2]=freq_digit[3]=freq_digit[4]=0;//Set frequency digits to 0
    
    for(i=0; i<DIV; i++){//Generating one full sine and triangular wave samples
        analogSineWave[i]=(1.0 + sin((2*PI*float(i))/DIV))/2.0;//A sine wave with an offset of 0.5, range from 0 to 1
        analogTriagWave[i]=(DIV-abs(DIV-2*float(i)))/DIV;//A triangular wave of amplitude 1
    }

    untouched_freq[0]=485000;//Defining touched and untouched frequency thresholds
    touched_freq[0]=460000;
    untouched_freq[1]=540000;
    touched_freq[1]=510000;
    untouched_freq[2]=505000;
    touched_freq[2]=490000;
    untouched_freq[3]=430000;
    touched_freq[3]=410000;
    
    //----------------------------COUNTING CODE FROM PART 1----------------------------------//
    int loop_count[4];//Count for 4 pins
    float timeperiod=0.1;//Time Period where the function counts
    oled.cls();// OLED clear screen
    int last_port_state=0,port_state,port_change;
    while (analog_count<10) {        
        timer_count=0;//Resetting counts and timer to 0 at the beginning of loop
        loop_count[0]=loop_count[1]=loop_count[2]=loop_count[3]=0;
            
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
                led[i]=touch_state[i]=false;//Give touch indication on corresponding mbed LED
            else if (current_freq[i]<touched_freq[i])//If current frequency less than touched threshold, pad is touched
                led[i]=touch_state[i]=true;
        }//If current frequency is between the 2 thresholds, previous state is preserved
        
        if(touch_state[0]){//If pad 0 is pressed, decrement the digit selected
            freq_digit[select]--;
            if (freq_digit[select]==-1)//Cycle through digits (0-9), if -1 is reached reset to 9
                freq_digit[select]=9;
        }
        
        if(touch_state[1]){//If pad 3 is pressed, increment the digit selected
            freq_digit[select]++;
            if (freq_digit[select]==10)//Cycle through digits (0-9), if 10 is reached reset to 0
                freq_digit[select]=0;
        }
        
        if(touch_state[2]){//If pad 2 is pressed, move cursor left
            select--;
            if (select==-1)//There are 5 digits, reset to 5 when decremented beyond 0
                select=4;
        }
        
        if(!touch_state[3]&&last_touch_state3){//If pad 3 is pressed and then unpressed, change analog wavetype
            wavetype++;
            if(wavetype==3)//There are only 3 wavetypes, so reset to 0 when 3 is reached
                wavetype=0;
        }
        last_touch_state3=touch_state[3];//Preserve touch_state of pad 3, this is needed to tell if the pad is being constantly pressed
        
        if(touch_state[3])//Incrementing analog_count each cycle if pad 3 is constantly pressed
            analog_count++;//If the count reaches 10 (~1 second) analog wave is output
        else
            analog_count=0;//If unpressed, count is reset to 0
            
        freq_out=0;//Calculating output frequency from digits
        for(i=0;i<5;i++)
            freq_out+=freq_digit[i]*pow(10.0,i);//freq_out is set to 0, then each digit is added with associated power of 10
            
        if(freq_out>0){        
            pwmpin.period(1.0/freq_out);//PWM constantly outputs the frequency, period is defined here
            pwmpin=0.5;//PWM duty cycle
        }
        else
            pwmpin=0;
        Display(freq_digit, select, wavetype);//Display on OLED
    }
    OutputWaveform(freq_out,wavetype);//Output PWM(p21) and Analog(p18) waveforms
}

void Display(int freq_digit[],int select, int wavetype){
    oled.locate(0,0);
    oled.printf("F: %d%d%d%d%dHz",freq_digit[4],freq_digit[3],freq_digit[2],freq_digit[1],freq_digit[0]);//Print frequency on first line
    
    oled.locate(0,2);//Print digit selecting cursor on 2nd line
    for(int i=select;i<5;i++)
        oled.printf(" ");
    oled.printf("  ^    ");
    
    oled.locate(0,4);
    if(wavetype==0)//Print different wavetypes with selecting cursor on 3rd, 4th and 5th lines
        oled.printf(">Square");//Selecting cursor if current wavetype is selected
    else
        oled.printf(" Square");//If not, a blank space
        
    oled.locate(0,5);
    if(wavetype==1)
        oled.printf(">Sine");//Extra spaces to move onto next line
    else
        oled.printf(" Sine");
    
    oled.locate(0,6);
    if(wavetype==2)
        oled.printf(">Triangle");
    else
        oled.printf(" Triangle");
}

void OutputWaveform(int freq, int type){//Outputs Analog waveform on p18
    if (freq>0){//Only output if freq>0, values below this are invalid. Negative numbers cannot be input using our interface but 0 can.      
        if(type==1){
            flipper.detach();//Detach previous ticker, if any
            flipper.attach(&AnalogSineGenerator, 1.0/(freq*DIV));//AnalogSineGenerator outputs a wave of time period 1/freq
            // but needs to be called DIV times for each sample in the sample waveform. Hence T=1/freq*DIV
        }
        else if (type==2){
            flipper.detach();
            flipper.attach(&AnalogTriagGenerator, 1.0/(freq*DIV));//Similar to SineGen above but uses Triangular waveform sample array
        }
        else{
            flipper.detach();
            flipper.attach(&AnalogSquareGenerator, 0.5/freq);//Square wave generator needs to invert output every 0.5 seconds to generate a wave
            //of 1Hz frequency (0.5 seconds low and 0.5 seconds high). For higher frequency, more inversions are needed so T=0.5/F.
        }
    }
}

void AnalogSquareGenerator(){//Square generator needs to invert pin every half cycle to give square wave
    analogpin = !analogpin;
}
    
void AnalogSineGenerator(){//Sine generator needs output the next sample from the array containing a full sine wave
    analogpin = analogSineWave[c];//Output sample c
    c++;//Increment c for the next time this function is called
    if(c==DIV)//If c reaches the end of the array, set to 0 to cycle through sample wave again
        c=0;
}

void AnalogTriagGenerator(){//Similar to AnalogSineGenerator above but uses Triangular wave sample instead
    analogpin = analogTriagWave[c];
    c++;
    if(c==DIV)
        c=0;
}

void incr_timer_count(void) { //Increments timercount to 1, stopping the counting loop
    timer_count = 1;
}
