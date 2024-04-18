#include "AD9910V2.h"

struct ad9910_reg AD9910;

/**
 * @brief AD9910 reg parameter init 
 * 
 */
uint8_t cfr1[] = {0x00, 0x40, 0x00, 0x00};						  //0x00 cfr1 control from left to right ,from high bit to low bit
uint8_t cfr2[] = {0x01, 0x00, 0x00, 0x00};						  //0x01 cfr2 control
uint8_t cfr3[] = {0x05, 0x3D, 0x41, 0x32};						  //0x02 cfr3 control

uint8_t aux_dac[] = {0x00, 0x00, 0x00, 0x00};					  //0x03 aux_dac control
uint8_t io_update_rate[] = {0xff, 0xff, 0xff, 0xff};			  //0x04 io_update_rate control
uint8_t ftw[] = {0x00, 0x00, 0x00, 0x00};						  //0x07 ftw control
uint8_t powt[] = {0x00, 0x00};  						//0x08 pow control
uint8_t asf[] = {0x00, 0x00, 0x00, 0x00};						  //0x09 asf control

uint8_t multichip_sync[] = {0x00, 0x00, 0x00, 0x00};			  //0x0a multichip_sync control
uint64_t digital_ramp_limit[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};		  //0x0b digital_ramp_limit control
uint64_t digital_ramp_step[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};		  //0x0c digital_ramp_step control
uint64_t digital_ramp_rate[] = {0x00, 0x00, 0x00, 0x00};		  						  //0x0d digital_ramp_rate control

// this profile must be set as uint64_t instand of uint8_t or the data will be wrong
// the data is from left to right ,from low bit to high bit
//uint64_t profile0[] = {0x3f, 0xff, 0x00, 0x00, 0x25, 0x09, 0x7b, 0x42};
uint64_t profile0[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00};
uint64_t profile1[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00}; //0x0f profile1 control
uint64_t profile2[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00}; //0x10 profile2 control
uint64_t profile3[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00}; //0x11 profile3 control
uint64_t profile4[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00}; //0x12 profile4 control
uint64_t profile5[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00}; //0x13 profile5 control
uint64_t profile6[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00}; //0x14 profile6 control
uint64_t profile7[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00}; //0x15 profile7 control


/**
 * @brief the AD9910_Init() function initial with IO port and parameters
 * 
 */
void AD9910_Init()
{
	AD9910.CFR1 = (cfr1[0]<<24) + (cfr1[1]<<16) + (cfr1[2]<<8) + cfr1[3];
	AD9910.CFR2 = (cfr2[0]<<24) + (cfr2[1]<<16) + (cfr2[2]<<8) + cfr2[3];
	AD9910.CFR3 = (cfr3[0]<<24) + (cfr3[1]<<16) + (cfr3[2]<<8) + cfr3[3];
	//AD9910.Aux_DAC_Control = 0x00007F7F;
	//AD9910.IO_UPDATE = 0x00000002;
	//AD9910.FTW = 0x0;
	//AD9910.POW = 0x0;
	//AD9910.ASF = 0x0;
	//AD9910.Multichip_Sync = 0x0;
	AD9910.Digital_Ramp_Limit = 0x0;
	AD9910.Digital_Ramp_Step_Size = 0x0;
	AD9910.Digital_Ramp_Rate = 0x0;

	AD9910.Profile_0 = (profile0[0]<<56) + (profile0[1]<<48) + (profile0[2]<<40) + (profile0[3]<<32) + (profile0[4]<<24) + (profile0[5]<<16) + (profile0[6]<<8) + profile0[7];
}

/********************************single tone mode*****************************************/
/**
 * @brief AD9910 single tone mode Amplitude convert
 * 
 * @param Amp 
 */
void AD9910_AMP_Convert(uint32_t Amp)
{
	uint64_t Temp;
	Temp = (uint64_t)Amp * 28.4829; //calulate ASF : 25.20615385=(2^14)/650
	if (Temp > 0x3fff)
		Temp = 0x3fff;
	Temp &= 0x3fff;
	profile0[1] = (uint8_t)Temp;
	profile0[0] = (uint8_t)(Temp >> 8);

	AD9910.Profile_0 = (profile0[0]<<56) + (profile0[1]<<48) + (profile0[2]<<40) + (profile0[3]<<32) + (profile0[4]<<24) + (profile0[5]<<16) + (profile0[6]<<8) + profile0[7];
}
/**
 * @brief AD9910 single tone mode Freq convert
 * @param Freq 
 */
void AD9910_Freq_Convert(uint64_t Freq)
{
	uint64_t Temp;
	Temp = (uint64_t)Freq * 4.294967296; //calulate FTW : 4.294967296=(2^32)/1000000000
	profile0[7] = (uint8_t)Temp;
	profile0[6] = (uint8_t)(Temp >> 8);
	profile0[5] = (uint8_t)(Temp >> 16);
	profile0[4] = (uint8_t)(Temp >> 24);

	AD9910.Profile_0 = (profile0[0]<<56) + (profile0[1]<<48) + (profile0[2]<<40) + (profile0[3]<<32) + (profile0[4]<<24) + (profile0[5]<<16) + (profile0[6]<<8) + profile0[7];
}

/**
 * @brief AD9910 single tone mode Phase convert
 * 
 * @param Phi 
 */
void AD9910_Phi_Convert(uint64_t Phi)
{
	uint64_t Temp;
	Temp = (uint64_t)Phi *65536/360;; //calulate POW : 182.04444444=(2^16)/360
	profile0[3] = (uint8_t)Temp;
	profile0[2] = (uint8_t)(Temp >> 8);

	AD9910.Profile_0 = (profile0[0]<<56) + (profile0[1]<<48) + (profile0[2]<<40) + (profile0[3]<<32) + (profile0[4]<<24) + (profile0[5]<<16) + (profile0[6]<<8) + profile0[7];
}

void AD9910_RAMP_Chrip_generate(uint64_t start_freq, uint64_t stop_freq, uint64_t sweep_time,uint64_t sys_time)
{
	//sweep_time单位为ns，必须是4的整数倍
	//产生时间20us；产生频率步进次1us/4ns=250次；步进频率=(230M-170M)/250=240000
	uint64_t Step_count = 0;
	uint64_t Step_freq = 0;

	Step_count = sweep_time/sys_time; //5000 = 20us/4ns
    Step_freq = (stop_freq-start_freq)/Step_count;

	//RAMP enable, set freq
	cfr2[0] = 0x01; //set amp
	//cfr2[1] = 0x08;//非驻留高低位都禁用
	cfr2[1] = 0x48;//非驻留高低位都禁用 打开syns_clk
	//cfr2[1] = 0x0e;
	//cfr2[1] = (direction == FRE_LOW_HIGH) ? 0x0c: 0x0a;// CFR2[19] 1;CFR2[21:20] 00;CFR2 [18:17] 00; oe是双边啁啾 oc是低到高单边增加 0a是高到低
	cfr2[2] = 0x00;
	cfr2[3] = 0x00; // only allow SDI

    AD9910.CFR2 =  (cfr2[0]<<24) + (cfr2[1]<<16) + (cfr2[2]<<8) + cfr1[3];

	/*****/
	uint64_t Temp_1, Temp_2, Temp_freq_step_P, Temp_freq_step_N, Temp_time_step_P, Temp_time_step_N;
	Temp_1 = (uint64_t)start_freq*4.294967296; //(2^32)/1000000000 = 4.294967296
	if(stop_freq > 400000000)
		stop_freq = 400000000;
	Temp_2 = (uint64_t)stop_freq*4.294967296;
	if(Step_freq > 400000000)
		Step_freq = 400000000;
	Temp_freq_step_P = (uint64_t)Step_freq*4.294967296;
	Temp_freq_step_N = Temp_freq_step_P;

	Temp_time_step_P = (uint64_t)(sys_time*0.250); // 1Ghz/4 ns
	if(Temp_time_step_P > 0xffff)
		Temp_time_step_P = 0xffff;
	Temp_time_step_N = Temp_time_step_P;

	//
	digital_ramp_limit[7]=(uint8_t)Temp_1;
	digital_ramp_limit[6]=(uint8_t)(Temp_1>>8);
	digital_ramp_limit[5]=(uint8_t)(Temp_1>>16);
	digital_ramp_limit[4]=(uint8_t)(Temp_1>>24);
	digital_ramp_limit[3]=(uint8_t)Temp_2;
	digital_ramp_limit[2]=(uint8_t)(Temp_2>>8);
	digital_ramp_limit[1]=(uint8_t)(Temp_2>>16);
	digital_ramp_limit[0]=(uint8_t)(Temp_2>>24);
	//
	digital_ramp_step[7]=(uint8_t)Temp_freq_step_P;
	digital_ramp_step[6]=(uint8_t)(Temp_freq_step_P>>8);
	digital_ramp_step[5]=(uint8_t)(Temp_freq_step_P>>16);
	digital_ramp_step[4]=(uint8_t)(Temp_freq_step_P>>24);
	digital_ramp_step[3]=(uint8_t)Temp_freq_step_N;
	digital_ramp_step[2]=(uint8_t)(Temp_freq_step_N>>8);
	digital_ramp_step[1]=(uint8_t)(Temp_freq_step_N>>16);
	digital_ramp_step[0]=(uint8_t)(Temp_freq_step_N>>24);
	//
	digital_ramp_rate[3]=(uint8_t)Temp_time_step_P;
	digital_ramp_rate[2]=(uint8_t)(Temp_time_step_P>>8);
	digital_ramp_rate[1]=(uint8_t)Temp_time_step_N;
	digital_ramp_rate[0]=(uint8_t)(Temp_time_step_N>>8);

	AD9910.Digital_Ramp_Limit = (digital_ramp_limit[0]<<56) + (digital_ramp_limit[1]<<48) + (digital_ramp_limit[2]<<40)
			 + (digital_ramp_limit[3]<<32) + (digital_ramp_limit[4]<<24) + (digital_ramp_limit[5]<<16) + (digital_ramp_limit[6]<<8) + digital_ramp_limit[7];
	AD9910.Digital_Ramp_Step_Size = (digital_ramp_step[0]<<56) + (digital_ramp_step[1]<<48) + (digital_ramp_step[2]<<40)
					 + (digital_ramp_step[3]<<32) + (digital_ramp_step[4]<<24) + (digital_ramp_step[5]<<16) + (digital_ramp_step[6]<<8) + digital_ramp_step[7];
	AD9910.Digital_Ramp_Rate = (digital_ramp_rate[0]<<24) + (digital_ramp_rate[1]<<16) + (digital_ramp_rate[2]<<8) + digital_ramp_rate[3];


	/********/

	// OSK port enable
	cfr1[0] = 0x00; // RAM  disable
	cfr1[1] = 0xc0; // anti-sinx filter
	//cfr1[2] = 0x02;
	cfr1[2] = 0x62;
	//cfr1[2] = 0x42;
	cfr1[3] = 0x00; //
    AD9910.CFR1 =  (cfr1[0]<<24) + (cfr1[1]<<16) + (cfr1[2]<<8) + cfr1[3];

	//pow
	/*
	powt[0] = 0x00;
	powt[1] = 0x00;
	AD9910.POW = (powt[0]<<8) + powt[1];
	AD9910_Reg_Write(_POW, _POW_SIZE, (uint64_t)AD9910.POW);
	AD9910_IO_UPDATE();
	*/
    //AD9910_Phi_Convert(0);
	//ASF
	asf[0] = 0x00; //ASF ramp [15:8]
	asf[1] = 0x01; //ASF ramp [7:0]   set time 4ns
	asf[2] = 0xff; //ASF scale [13:6]  0xff
	asf[3] = 0xff; //ASF scale [5:0] 0xff  ASF_STEP[1:0] 11 8step
	AD9910.ASF = (asf[0]<<24) + (asf[1]<<16) + (asf[2]<<8) + asf[3];
}

