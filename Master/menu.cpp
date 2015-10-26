#include "menu.h"
#include "K2400.h"
#include "SPA4156B.h"
#include "SPA4145B.h"
#include "Switchbox.h"


#include <iostream>
#include <windows.h>
#include "ni488.h"
#include <cstdlib>
#include "engine.h"

menu::menu()
{
	//ctor
}

menu::~menu()
{
	//dtor
}

void menu::menu_start(K2400 keithley, SPA4156B SPA, Switchbox switchbox){

	// get the port specs for all the devices on chip
	switchbox.getAllPorts(); // reads in all the pad specifications, reformats them for switchbox, and creates output file streams
	
	//get the email address for the program user
	//get the chip ID
	std::cout << "What is the email address to contact? \n";
	std::cin >> emailAddress;
	std::cout << "What is the ID for this chip?\n";
	std::cin >> chip_ID;

	int choice = 0;
	while (1){
		std::cout << "\n ===========-------  MENU  -------============= \n\n";
		std::cout << "   (1)  SHORT ALL devices (S and D pads) to Shorting Cap (04)\n";
		std::cout << "   (2)  OPEN ALL possible routes (0101:1028) \n\n" ;

		std::cout << "   (3)  Perform NANOWIRE SAMPLING YIELD measurements on a group of devices\n";
		std::cout << "   (4)  Perform GATE LEAKAGE SAMPLING YIELD measurements on a group of devices\n";
		std::cout << "   (5)  Perform GATELINE SAMPLING YIELD for the first listed device \n";
		std::cout << "   (6)  Execute IV SWEEP OF THE GATELINE for the first listed device \n\n";
		
		std::cout << "   (7)  Execute EM ON A GROUP of devices\n";
		std::cout << "   (8)  Execute EM AND SELF-BREAK MONITORING on a group of devices\n";
		std::cout << "   (9)  Execute SELF-BREAK MONITORING of a group of devices\n\n";

		std::cout << "   (10)  Execute SWEEP BREAKING on a group of devices \n";
		std::cout << "   (11)  Execute EM AND SWEEP BREAKING AND GATED IV SWEEPS on a group of devices \n\n";

		std::cout << "   (12)  Take IV SWEEPS oF a group of devices\n";
		std::cout << "   (13)  Take IG LEAKAGE SWEEPS oF a group of devices\n";
		std::cout << "   (14)  Take GATED IV SWEEPS WITH LEAKAGE MEASUREMENT of a group of devices \n";
		std::cout << "   (15)  Take GATED IV SWEEPS WITHOUT LEAKAGE MEASUREMENT of a group of devices \n\n";

		std::cout << "   (16)  Connect a device for IETS \n";
		std::cout << "   (17)  Connect a device and hold gate voltages for gated IETS\n\n";

		std::cout << "   (18)  Execute EM AND SELF-BREAK MONITORING and switch devices after full breaking \n\n";

		std::cout << "   (19)  Execute KEITHLEY SWEEP EM breaking of a group of devices \n";
		std::cout << "   (20)  Execute KEITHLEY CONSTANT-V DWELL EM breaking of a group of devices \n\n";

		std::cout << "   (21)  Execute EM ON PROBE STATION on a single device\n\n";

		std::cout << "   (22)  Execute EM AND KEITHLEY SWEEP breaking on a group of devices\n\n";

		std::cout << "   (23)  EXIT \n\n";

		std::cout << " ============================ Please enter a command:  ";
		std::cin >> choice;

		int ymflag = 0; // is this a yield monitoring measurement? affects how switchbox.getPorts creates output files streams
		FILE* outputs[36]; // this array will hold all the file pointers to the output files being written

		switch (choice){
		case 1:
					//SHORT ALL output ports(01 to 16) to Shorting Cap(04)
					switchbox.closeAll();
					break;
		case 2:
					//OPEN ALL channels from Shorting Cap (04) 
					switchbox.openAll();
					break;
		case 3:{
				   // Perform NANOWIRE YIELD measurements on a group of devices
				   ymflag = 1; // this is the only menu option where we are doing yield monitoring so the default upon initializations is 0, but here we change it to 1
				   switchbox.getPorts(ymflag, outputs);
				   SPA.setParams(3);
				   // perform yield samples and then close the output file
				   executeYield(SPA, switchbox, outputs);
				   closeFiles(ymflag, switchbox.ndev, outputs, switchbox);
				   break; }
		case 4:{
				   // Perform GATE LEAKAGE YIELD measurements on a group of devices
				   ymflag = 1; // this is the only menu option where we are doing yield monitoring so the default upon initializations is 0, but here we change it to 1
				   switchbox.getPorts(ymflag, outputs);  //now ymflag=1 will tell switchbox to just open one output file
				   SPA.setParams(3);
				   // perform gate leakage samples and then close output file
				   executeLeakageYield(SPA, switchbox, outputs);
				   closeFiles(ymflag, switchbox.ndev, outputs, switchbox);
				   break; }
		case 5:{
				    // Perform GATELINE INTEGRITY CHECK on all gatelines \n\n";
					ymflag = 1; // this is the only menu option where we are doing yield monitoring so the default upon initializations is 0, but here we change it to 1
					switchbox.getPorts(ymflag, outputs);  //now ymflag=1 will tell switchbox to just open one output file
					SPA.setParams(3);
					// perform gateline samples and then close output file
					executeGatelineCheck(SPA, switchbox, outputs);
					closeFiles(ymflag, switchbox.ndev, outputs, switchbox);
					break; }
		case 6:{
				   // Take SWEEP OF GATELINE of first device\n";
				   int outputfileflag = 1; //we just want one file, not one for each device
				   switchbox.getPorts(outputfileflag, outputs);
				   SPA.setParams(2);
				   executeGatelineSweep(SPA, switchbox, outputs);
				   closeFiles(outputfileflag, switchbox.ndev, outputs, switchbox);
				   break;}
		case 7:{	
				   // Execute EM ON A GROUP of devices\n";
				   switchbox.getPorts(ymflag, outputs); // reads in all the pad specifications and mark which devices are to be tested
				   keithley.setParams(); // sets the EM parameters (some user-input).  
				   executeEM(keithley, SPA, switchbox, outputs); // performs sequential EM runs on the devices listed in the input file, writing EM IV's to appropriate outputs.
				   closeFiles(ymflag, switchbox.ndev, outputs, switchbox); // close all the output files
				   break;}
		case 8:{
				   //Execute EM AND SELF-BREAK MONITORING on a group of devices\n";
				   switchbox.getPorts(ymflag, outputs);
				   keithley.setParams();
				   SPA.setParams(1); // sets the sampling parameters for monitoring the devices after EM (some user-input)
				   executeEM(keithley, SPA, switchbox, outputs); // performs sequential EM runs on the devices listed in the input file, writing EM IV's to appropriate outputs
				   executeMonitor(SPA, switchbox, outputs); //executes self-breaking monitoring until the user pressed 'Alt' key
				   closeFiles(ymflag, switchbox.ndev, outputs, switchbox);
				   break; }
		case 9:{
				   // Execute SELF-BREAK MONITORING of a group of devices\n\n";
				   switchbox.getPorts(ymflag, outputs);
				   SPA.setParams(1); // sets the sampling parameters for monitoring the devices (some user-input)
				   executeMonitor(SPA, switchbox, outputs); //executes self-breaking monitoring until the user pressed 'Alt' key
				   closeFiles(ymflag, switchbox.ndev, outputs, switchbox);
				   break; }
		case 10:{
					// Execute SWEEP BREAKING on a group of devices \n";
					switchbox.getPorts(ymflag, outputs);
					executeSweepBreak(SPA, switchbox, outputs);
					closeFiles(ymflag, switchbox.ndev, outputs, switchbox);
					break; }
		case 11:{
					//Execute EM AND SWEEP BREAKING AND GATED IV SWEEPS on a group of devices \n\n";
					executeEMandSweepBreakandGatedIV(SPA, switchbox, keithley);
					break; }
		case 12:{
					// Take IV SWEEPS on a group of devices\n";
				   switchbox.getPorts(ymflag, outputs);
				   SPA.setParams(2);
				   executeSweep(SPA, switchbox, outputs);
				   closeFiles(ymflag, switchbox.ndev, outputs, switchbox);
				   break; }
		case 13:{
					// Take IG LEAKAGE SWEEPS on a group of devices\n";
				   switchbox.getPorts(ymflag, outputs);
				   SPA.setParams(2);
				   executeLeakageSweep(SPA, switchbox, outputs);
				   closeFiles(ymflag, switchbox.ndev, outputs, switchbox);
				   break; }
		case 14:{
					// Take GATED IV SWEEPS WITH LEAKAGE MEASUREMENT of a group of devices \n";
					switchbox.getPorts(ymflag, outputs);
					SPA.setParams(4);
					executeGatedSweeps(SPA, switchbox, outputs);
					closeFiles(ymflag, switchbox.ndev, outputs, switchbox);
					break; }
		case 15:{
					// Take GATED IV SWEEPS WITHOUT LEAKAGE MEASUREMENT of a group of devices \n\n";
					switchbox.getPorts(ymflag, outputs);
					SPA.setParams(5);
					executeGatedSweepsNoIG(SPA, switchbox, outputs);
					closeFiles(ymflag, switchbox.ndev, outputs, switchbox);
					break; }
		case 16:{
					// Connect a device for IETS \n";
					ymflag = 2;
					switchbox.getPorts(ymflag, outputs);
					bool gateFlag = false;
					executeIETS(keithley, switchbox, outputs, gateFlag);
					closeFiles(ymflag, switchbox.ndev, outputs, switchbox);
					break; }
		case 17:{
					// Connect a device and hold gate voltages for gated IETS\n\n";
					ymflag = 2;
					switchbox.getPorts(ymflag, outputs);
					switchbox.openAll();   //don't want to risk breaking down other devices
					bool gateFlag = true;
					executeIETS(keithley, switchbox, outputs, gateFlag);
					switchbox.closeAll();
					closeFiles(ymflag, switchbox.ndev, outputs, switchbox);
					break; }
		case 18:{
					//Execute EM AND SELF-BREAK MONITORING and switch devices after full breaking. \n\n";
					switchbox.getPorts(ymflag, outputs);
					keithley.setParams();
					SPA.setParams(1); // sets the sampling parameters for monitoring the devices after EM (some user-input)
					executeEMandMonitor(keithley, SPA, switchbox, outputs); // performs sequential EM and then self-break monitoring on each device in turn - it switches to the next device once the DUT has self-broken to a very high resistance 
					closeFiles(ymflag, switchbox.ndev, outputs, switchbox);
					//send email to let us know that all devices have been used and a new chip is needed!
					SendMail(emailAddress);
					break; }
		case 19:{
					// Sweep Keithley for final breaking\n";
					switchbox.getPorts(ymflag, outputs);
					float volt_ramp_temp, volt_start_temp, volt_stop_temp, target_resistance_temp;
					int useEMVFlag;
					keithley.setParamsSweep(&volt_ramp_temp, &volt_start_temp, &volt_stop_temp, &target_resistance_temp); // sets the EM parameters (some user-input).  
					keithley.initializeSweep(volt_ramp_temp, volt_start_temp, volt_stop_temp, target_resistance_temp);
					executeKeithleySweep(keithley, SPA, switchbox, outputs);
					closeFiles(ymflag, switchbox.ndev, outputs, switchbox);
					break;}
		case 20:{
					// Dwell with Keithley for final breaking\n";
					switchbox.getPorts(ymflag, outputs);
					keithley.setParamsDwell(); // sets the EM parameters (some user-input).  
					executeKeithleyDwell(keithley, SPA, switchbox, outputs);
					closeFiles(ymflag, switchbox.ndev, outputs, switchbox);
					break;}
		case 21:{
					// Execute EM ON PROBE STATION on a single device\n";
					keithley.setParams();
					keithley.emSingleProbeStation();
					break; }
		case 22:{
					// Execute EM AND KEITHLEY SWEEP breaking on a group of devices
					executeEMandKeithleySweep(SPA, switchbox, keithley);
					break; }
		case 23:
			goto EXIT;
			break;

		default:
			break;
		}
	}
EXIT:;
}

void menu::executeEM(K2400 keithley, SPA4156B SPA, Switchbox switchbox, FILE* outputs[36]){

	float temperature;
	std::cout << "Please check TempB on Lakeshore and input in Kelvin \n";
	std::cin >> temperature;

	// get a filename and open a file that will be used to write the summary info to
	std::string file_summary;
	std::cout << "What is the filename stem to use for the EM Outcome Summary? \n";
	std::cin >> file_summary;
	char filebuffer[1024] = "";
	sprintf(filebuffer, "%s_%s_EMSummary.txt", file_summary.c_str(),chip_ID.c_str());
	FILE *summary = fopen(filebuffer, "w+");
	if (summary == NULL) { // check that fopen worked correctly
		std::cout << "fopen has failed for the output file " << filebuffer << "\n";
	}


	std::cout << "=======================================================================\n";
	std::cout << "Press the 'F12' key to interrupt active EM on a device...\n";
	std::cout << "=======================================================================\n";
	Sleep(500);

	typedef std::chrono::high_resolution_clock Clock;
	Clock::time_point t0;
	Clock::time_point t1;

	typedef std::chrono::minutes minutes;
	minutes Elapsed;

	float exitV;


	for (int devnum = 0; devnum < switchbox.ndev; devnum++){
		if (switchbox.portSpecs[4][devnum] == "y"){
			switchbox.closeChan(devnum, "keithley"); // all channels except DUT remain shorted to bias port, DUT is connected to relevant inputs
			t0 = Clock::now();
			switchbox.exitSummary[0][devnum] = keithley.emSingle(devnum, outputs); // writes IV data to output file. keithley will set his output to 0V after finishing EM run
			t1 = Clock::now();
			Elapsed = std::chrono::duration_cast<minutes>(t1 - t0);
			std::cout << "Device ID# " << switchbox.portSpecs[3][devnum].c_str() << " has completed active EM! \n";
			std::cout << "Elapsed time of electromigration was " << Elapsed.count() << " minutes. \n";
			std::cout << "Exit voltage of the electromigration was " << switchbox.exitSummary[0][devnum] << " V.\n";
			switchbox.openChan(devnum, "keithley",1); // get back to the state of all output ports shorted to keithley ground
			switchbox.exitSummary[1][devnum] = executeGapYield(devnum, SPA, switchbox); // will print out the actual resistance sampled at 100mV 
			switchbox.exitSummary[2][devnum] = Elapsed.count();
			std::cout << "---------------------------------------------------\n";
		}
	}

	fprintf(summary, "Chip ID, Device Ports, Run Type, Temperature (K), EM exit V (V), R @ 100mV after EM (ohms), Elapsed Time of EM (min))\n"); // header
	char buffer[1024];
	for (int devnum = 0; devnum < switchbox.ndev; devnum++){
		if (switchbox.portSpecs[4][devnum] == "y"){
			sprintf(buffer, "%s, %s, EM, %.3f, %.3f, %.3f, %.1f\n", chip_ID.c_str(), switchbox.portSpecs[3][devnum].c_str(), temperature, switchbox.exitSummary[0][devnum], switchbox.exitSummary[1][devnum], switchbox.exitSummary[2][devnum]);
			fprintf(summary, buffer);
		}
	}
	fclose(summary);

	SendMail(emailAddress);

}

void menu::executeKeithleySweep(K2400 keithley, SPA4156B SPA, Switchbox switchbox, FILE* outputs[36]){

	float temperature;
	std::cout << "Please check TempB on Lakeshore and input in Kelvin \n";
	std::cin >> temperature;

	// get a filename and open a file that will be used to write the summary info to
	std::string file_summary;
	std::cout << "What is the filename stem to use for the KeithleySweep Outcome Summary? \n";
	std::cin >> file_summary;
	char filebuffer[1024] = "";
	sprintf(filebuffer, "%s_%s_KeithleySweepSummary.txt", file_summary.c_str(), chip_ID.c_str());
	FILE *summary = fopen(filebuffer, "w+");
	if (summary == NULL) { // check that fopen worked correctly
		std::cout << "fopen has failed for the output file " << filebuffer << "\n";
	}


	std::cout << "=======================================================================\n";
	std::cout << "Press the 'F12' key to interrupt active EM on a device...\n";
	std::cout << "=======================================================================\n";
	Sleep(500);

	typedef std::chrono::high_resolution_clock Clock;
	Clock::time_point t0;
	Clock::time_point t1;

	typedef std::chrono::minutes minutes;
	minutes Elapsed;

	float exitV;


	for (int devnum = 0; devnum < switchbox.ndev; devnum++){
		if (switchbox.portSpecs[4][devnum] == "y"){
			switchbox.closeChan(devnum, "keithley"); // all channels except DUT remain shorted to bias port, DUT is connected to relevant inputs
			t0 = Clock::now();
			switchbox.exitSummary[0][devnum] = keithley.sweepSingle(devnum, outputs); // writes IV data to output file. keithley will set his output to 0V after finishing EM run
			t1 = Clock::now();
			Elapsed = std::chrono::duration_cast<minutes>(t1 - t0);
			std::cout << "Device ID# " << switchbox.portSpecs[3][devnum].c_str() << " has completed active EM! \n";
			std::cout << "Elapsed time of keithley sweep was " << Elapsed.count() << " minutes. \n";
			std::cout << "Exit voltage of the keithley sweep was " << switchbox.exitSummary[0][devnum] << " V.\n";
			switchbox.openChan(devnum, "keithley", 1); // get back to the state of all output ports shorted to keithley ground
			switchbox.exitSummary[1][devnum] = executeGapYield(devnum, SPA, switchbox); // will print out the actual resistance sampled at 100mV 
			switchbox.exitSummary[2][devnum] = Elapsed.count();
			std::cout << "---------------------------------------------------\n";
		}
	}

	fprintf(summary, "Chip ID, Device Ports, Run Type, Temperature (K), Keithley Sweep exit V (V), R @ 100mV after Keithley Sweep (ohms), Elapsed Time of Keithley Sweep (min))\n"); // header
	char buffer[1024];
	for (int devnum = 0; devnum < switchbox.ndev; devnum++){
		if (switchbox.portSpecs[4][devnum] == "y"){
			sprintf(buffer, "%s, %s, EM, %.3f, %.3f, %.3f, %.1f\n", chip_ID.c_str(), switchbox.portSpecs[3][devnum].c_str(), temperature, switchbox.exitSummary[0][devnum], switchbox.exitSummary[1][devnum], switchbox.exitSummary[2][devnum]);
			fprintf(summary, buffer);
		}
	}
	fclose(summary);

	SendMail(emailAddress);

}

void menu::executeKeithleyDwell(K2400 keithley, SPA4156B SPA, Switchbox switchbox, FILE* outputs[36]){

	int useEMVFlag = 0;
	float dwellV = 0;
	std::cout << "Use EM exit voltage as the dwell voltage? ('0'for NO, '1' for YES) \n";
	std::cin >> useEMVFlag;
	if (!useEMVFlag){ // if the user wants to use exit Vs from prior EM run then set those parameters here
		std::cout << "What is the dwell voltage? (in Volts)\n";
		std::cin >> dwellV;
	}

	float temperature;
	std::cout << "Please check TempB on Lakeshore and input in Kelvin \n";
	std::cin >> temperature;

	// get a filename and open a file that will be used to write the summary info to
	std::string file_summary;
	std::cout << "What is the filename stem to use for the KeithleyDwell Outcome Summary? \n";
	std::cin >> file_summary;
	char filebuffer[1024] = "";
	sprintf(filebuffer, "%s_%s_KeithleyDwellSummary.txt", file_summary.c_str(), chip_ID.c_str());
	FILE *summary = fopen(filebuffer, "w+");
	if (summary == NULL) { // check that fopen worked correctly
		std::cout << "fopen has failed for the output file " << filebuffer << "\n";
	}

	std::cout << "=======================================================================\n";
	std::cout << "Press the 'F12' key to interrupt active EM on a device...\n";
	std::cout << "=======================================================================\n";
	Sleep(500);

	typedef std::chrono::high_resolution_clock Clock;
	Clock::time_point t0;
	Clock::time_point t1;

	typedef std::chrono::minutes minutes;
	minutes Elapsed;

	float exitV;


	for (int devnum = 0; devnum < switchbox.ndev; devnum++){
		if (switchbox.portSpecs[4][devnum] == "y"){
			switchbox.closeChan(devnum, "keithley"); // all channels except DUT remain shorted to bias port, DUT is connected to relevant inputs
			if (useEMVFlag){
			dwellV = switchbox.exitSummary[1][devnum];
			}
			t0 = Clock::now();
			switchbox.exitSummary[0][devnum] = keithley.dwellSingle(devnum, outputs, dwellV); // writes IV data to output file. keithley will set his output to 0V after finishing EM run
			t1 = Clock::now();
			Elapsed = std::chrono::duration_cast<minutes>(t1 - t0);
			std::cout << "Device ID# " << switchbox.portSpecs[3][devnum].c_str() << " has completed active EM! \n";
			std::cout << "Elapsed time of keithley sweep was " << Elapsed.count() << " minutes. \n";
			std::cout << "Exit voltage of the keithley sweep was " << switchbox.exitSummary[0][devnum] << " V.\n";
			switchbox.openChan(devnum, "keithley", 1); // get back to the state of all output ports shorted to keithley ground
			switchbox.exitSummary[1][devnum] = executeGapYield(devnum, SPA, switchbox); // will print out the actual resistance sampled at 100mV 
			switchbox.exitSummary[2][devnum] = Elapsed.count();
			std::cout << "---------------------------------------------------\n";
		}
	}

	fprintf(summary, "Chip ID, Device Ports, Run Type, Temperature (K), Keithley Sweep exit V (V), R @ 100mV after Keithley Sweep (ohms), Elapsed Time of Keithley Sweep (min))\n"); // header
	char buffer[1024];
	for (int devnum = 0; devnum < switchbox.ndev; devnum++){
		if (switchbox.portSpecs[4][devnum] == "y"){
			sprintf(buffer, "%s, %s, EM, %.3f, %.3f, %.3f, %.1f\n", chip_ID.c_str(), switchbox.portSpecs[3][devnum].c_str(), temperature, switchbox.exitSummary[0][devnum], switchbox.exitSummary[1][devnum], switchbox.exitSummary[2][devnum]);
			fprintf(summary, buffer);
		}
	}
	fclose(summary);

	SendMail(emailAddress);

}

void menu::executeEMandMonitor(K2400 keithley, SPA4156B SPA, Switchbox switchbox, FILE* outputs[36]){
	Sleep(500);
	for (int devnum = 0; devnum < switchbox.ndev; devnum++){
		if (switchbox.portSpecs[4][devnum] == "y"){
			switchbox.closeChan(devnum, "keithley"); // all channels except DUT remain shorted to bias port, DUT is connected to relevant inputs
			std::cout << "=======================================================================\n";
			std::cout << "Press the 'F12' key to interrupt active EM on a device...\n";
			std::cout << "=======================================================================\n";
			keithley.emSingle(devnum, outputs); // writes IV data to output file. keithley will set his output to 0V after finishing EM run
			std::cout << "device #" << devnum << " has completed active EM! \n";
			switchbox.openChan(devnum, "keithley",1); // get back to the state of all output ports shorted to keithley ground
			switchbox.openChan(devnum, "ShortCap",0); // all devices except the group of DUTs remain shorted to shorting cap
			std::cout << "=======================================================================\n";
			std::cout << "Self-break monitoring will continue until the 'F12' key is pressed...\n";
			std::cout << "=======================================================================\n";
			SPA.constconfigSample();
			switchbox.closeChanMonitor(devnum, "SPA");
			int deadFlag = 0;
			int cycle = 0;
			while (!GetAsyncKeyState(VK_F12) && deadFlag==0){
				deadFlag=SPA.constsampleSingle(devnum, outputs, cycle);
				cycle++;
			}
			switchbox.openChanMonitor(devnum, "SPA"); // assumes all channels in the group of interest should remain open away from shorting cap, this just opens the channel from the measurement instrument
		}
	}

	switchbox.closeAll(); // this will first open all channels

}

void menu::executeMonitor(SPA4156B SPA, Switchbox switchbox, FILE* outputs[36]){
	std::cout << "=======================================================================\n";
	std::cout << "Self-break monitoring will continue until the 'F12' key is pressed...\n";
	std::cout << "=======================================================================\n";
	int cycle = 0;

	// check if its just a single device we're monitoring
	int count = 0;
	int singlestore = 0;
	for (int counter = 0; counter < switchbox.ndev; counter++){
		if (switchbox.portSpecs[4][counter] == "y"){
			count = count + 1;
			switchbox.openChan(counter, "ShortCap",0); // all devices except the group of DUTs remain shorted to shorting cap
			singlestore = counter; // singlestore will be the devnum for the single DUT in the event that there is only one
		}
	}

	if (count == 1){ // if going to be monitoring only one device
		SPA.constconfigSample();
		switchbox.closeChanMonitor(singlestore, "SPA");
	}

	
	while (!GetAsyncKeyState(VK_F12)){

		// if its just one device rather than a group, use the constant bias sampling approach
		if (count == 1){
			SPA.constsampleSingle(singlestore, outputs, cycle); // 
		}

		// if its a group of device we have to just have a single sampling approach and switch between devices every time point
		else{
			for (int devnum = 0; devnum < switchbox.ndev; devnum++){
				if (switchbox.portSpecs[4][devnum] == "y"){
					switchbox.closeChanMonitor(devnum, "SPA"); // assumes all channels in the group of interest have been opened away from shorting cap, this just closes the channel to measurement instrument
					int deadFlag = 0;
					deadFlag = SPA.sampleSingle(devnum, outputs, cycle); 
					if (deadFlag == 1){
						switchbox.portSpecs[4][devnum] = "n";
					}
					switchbox.openChanMonitor(devnum, "SPA"); // assumes all channels in the group of interest should remain open away from shorting cap, this just opens the channel from the measurement instrument
				}
			}
		}
		cycle++;
	}

	switchbox.closeAll(); // this will first open all channels
}

void menu::executeYield(SPA4156B SPA, Switchbox switchbox, FILE* outputs[36]){
	// get some comments demarcating this particular round of sampling and write it to the YM output file
	std::string comments;
	std::cout << "What are user comments for this yield check?\n";
	std::getline(std::cin, comments);
	fprintf(outputs[0], "UserComments---%s \n\n", comments.c_str());
	for (int devnum = 0; devnum < switchbox.ndev; devnum++){
		if (switchbox.portSpecs[4][devnum] == "y"){
			// write device identifier to the output text file, to be followed by the measurement recording
			FILE* sb_output = outputs[0]; // set the output file stream to yield monitor file
			fprintf(sb_output, "Device ID# %s R @ 1 mV = ", switchbox.portSpecs[3][devnum].c_str());
			fflush(sb_output);
			std::cout << "Device ID# " << switchbox.portSpecs[3][devnum].c_str() << " R @ 1mV = ";
			switchbox.closeChan(devnum, "SPA"); // all channels except DUT remain shorted to bias port, DUT is connected to relevant inputs
			Sleep(100);
			SPA.yieldSingle(devnum, outputs); // similar to sampleSingle but always writes to output[0]
			Sleep(100);
			switchbox.openChan(devnum, "SPA",1); // get back to the state of all output ports shorted to shorting cap ground
			Sleep(100);
		}
	}
}

float menu::executeGapYield(int devnum, SPA4156B SPA, Switchbox switchbox){
	//this is a function that samples a device at 100mV after some initial electromigration
	switchbox.closeChan(devnum, "SPA"); // all channels except DUT remain shorted to bias port, DUT is connected to relevant inputs
	Sleep(50);
	float resistance = SPA.yieldGapSingle(); //Samples at 100mV, returns resistance
	Sleep(50);
	switchbox.openChan(devnum, "SPA",1); // get back to the state of all output ports shorted to shorting cap ground
	std::cout << "Actual Resistance of Device " << switchbox.portSpecs[3][devnum].c_str() << " at 100 mV is " << resistance << " Ohms.\n";
	return resistance;
}

void menu::executeGatelineCheck(SPA4156B SPA, Switchbox switchbox, FILE* outputs[36]){
	// get some comments demarcating this particular round of sampling and write it to the YM output file
	std::string comments;
	std::cout << "What are user comments for this gateline check? (no spaces) \n";
	std::cin >> comments;
	fprintf(outputs[0], "UserComments---%s \n", comments.c_str());
	switchbox.openAll();
	std::cout << "...Assuming all devices are on a single gateline...\n";
	int devnum = 0;
	fprintf(outputs[0], "Gateline @ 1mV = ");
	fflush(outputs[0]);
	std::cout << " Gateline @ 1mV = ";
	switchbox.closeChan(devnum, "Gateline"); // all channels except DUT remain shorted to bias port, DUT is connected to relevant inputs
	Sleep(250);
	SPA.yieldSingle(devnum, outputs); // similar to sampleSingle but always writes to output[0]
	Sleep(250);
	switchbox.openChan(devnum, "Gateline",0); // do NOT go back to all output ports shorted to keithley ground
	Sleep(250);
	switchbox.closeAll();
}

void menu::executeGatelineSweep(SPA4156B SPA, Switchbox switchbox, FILE* outputs[36]){
	switchbox.openAll();
	std::cout << "...Assuming all devices are on a single gateline...\n";
	int devnum = 0;
	int gateFlag = 0; //
	switchbox.closeChan(devnum, "Gateline"); // all channels except DUT remain shorted to bias port, DUT is connected to relevant inputs
	Sleep(250);
	//SPA.yieldSingle(devnum, outputs); // similar to sampleSingle but always writes to output[0]
	SPA.sweepSingle(devnum, outputs, gateFlag); // writes IV data to output file. 
	Sleep(250);
	switchbox.openChan(devnum, "Gateline", 0); // do NOT go back to all output ports shorted to keithley ground
	Sleep(250);
	switchbox.closeAll();
}

void menu::executeLeakageYield(SPA4156B SPA, Switchbox switchbox, FILE* outputs[36]){
	// get some comments demarcating this particular round of sampling and write it to the YM output file
	std::string comments;
	std::cout << "What are user comments for this leakage yield test? (no spaces) \n";
	std::cin >> comments;
	fprintf(outputs[0], "UserComments---%s \n\n", comments.c_str());

	switchbox.openAll(); //in case some other grounded device has a leakage path
	for (int devnum = 0; devnum < switchbox.ndev; devnum++){
		if (switchbox.portSpecs[4][devnum] == "y" && switchbox.portSpecs[5][devnum] != "n"){
			// write device identifier to the output text file, to be followed by the measurement recording
			FILE* sb_output = outputs[0]; // set the output file stream to yield monitor file
			fprintf(sb_output, "Device ID# %s Ig = ", switchbox.portSpecs[3][devnum].c_str());
			fflush(sb_output);
			std::cout << "Device ID# " << switchbox.portSpecs[3][devnum].c_str() << " Ig = ";
			switchbox.closeChan(devnum, "Leakage"); // all channels except DUT remain shorted to bias port, DUT (one of the S/D and one of the gate pads) is connected to relevant inputs
			SPA.yieldSingle(devnum, outputs); // similar to sampleSingle but always writes to output[0]
			switchbox.openChan(devnum, "Leakage",0); // do NOT go back to the state of all output ports shorted to keithley ground
		}
	}
	switchbox.closeAll(); //get back to being all shorted to ground 
}

void menu::executeSweep(SPA4156B SPA, Switchbox switchbox, FILE* outputs[36]){
	for (int devnum = 0; devnum < switchbox.ndev; devnum++){
		if (switchbox.portSpecs[4][devnum] == "y"){
			switchbox.closeChan(devnum, "SPA"); // all channels except DUT remain shorted to bias port, DUT is connected to relevant inputs
			int gateFlag = 0; // to call SPA.sweepSingle with the correct GPIB read commands
			SPA.sweepSingle(devnum, outputs, gateFlag); // writes IV data to output file. 
			switchbox.openChan(devnum, "SPA",1); // get back to the state of all output ports shorted to switchbox ground
			Sleep(1000);
			// SPA.ShowGraph("Null");
		}
	}
}

void menu::executeGatedSweeps(SPA4156B SPA, Switchbox switchbox, FILE* outputs[36]){
	switchbox.openAll(); //in case some other grounded device has a leakage path
	for (int devnum = 0; devnum < switchbox.ndev; devnum++ && switchbox.portSpecs[5][devnum] != "n"){
		if (switchbox.portSpecs[4][devnum] == "y"){
			switchbox.closeChan(devnum, "SPA"); // all channels except DUT remain shorted to bias port, DUT is connected to relevant inputs
			switchbox.closeChan(devnum, "SPAgate"); // close both gate pads down to SMU3 of SPA
			int gateFlag = 1; // to call SPA.sweepSingle with the correct GPIB read commands
			SPA.sweepSingle(devnum, outputs, gateFlag); // writes IV data to output file. 
			switchbox.openChan(devnum, "SPA",0); // get back to the state of all output ports shorted to switchbox ground
			switchbox.openChan(devnum, "SPAgate",0); // close both gate pads down to SMU3 of SPA
			Sleep(1000);
		}
	}
	switchbox.closeAll(); //get back to being all shorted to ground 
}

void menu::executeGatedSweepsNoIG(SPA4156B SPA, Switchbox switchbox, FILE* outputs[36]){
	switchbox.openAll(); //in case some other grounded device has a leakage path
	for (int devnum = 0; devnum < switchbox.ndev; devnum++ && switchbox.portSpecs[5][devnum] != "n"){
		if (switchbox.portSpecs[4][devnum] == "y"){
			switchbox.closeChan(devnum, "SPA"); // all channels except DUT remain shorted to bias port, DUT is connected to relevant inputs
			switchbox.closeChan(devnum, "SPAgate"); // close both gate pads down to SMU3 of SPA
			int gateFlag = 2; // to call SPA.sweepSingle with the correct GPIB read commands
			SPA.sweepSingle(devnum, outputs, gateFlag); // writes IV data to output file. 
			switchbox.openChan(devnum, "SPA",0); // get back to the state of all output ports shorted to switchbox ground
			switchbox.openChan(devnum, "SPAgate",0); // close both gate pads down to SMU3 of SPA
			Sleep(1000);
		}
	}
	switchbox.closeAll(); //get back to being all shorted to ground 
}

void menu::executeLeakageSweep(SPA4156B SPA, Switchbox switchbox, FILE* outputs[36]){

	switchbox.openAll(); //in case some other grounded device has a leakage path
	for (int devnum = 0; devnum < switchbox.ndev; devnum++){
		if (switchbox.portSpecs[4][devnum] == "y" && switchbox.portSpecs[5][devnum]!="n"){ // make sure its marked _and_ a gated device
			switchbox.closeChan(devnum, "Leakage"); // all channels except DUT and its common drain group remain shorted to bias port, DUT S (first pad) is connected to SMU1 and gatepad1 is connected to SMU2 on SPA (input ports 01,02)
			int gateFlag = 0; // to call SPA.sweepSingle with the correct GPIB read commands
			SPA.sweepSingle(devnum, outputs, gateFlag); // writes IV data to output file.
			switchbox.openChan(devnum, "Leakage",0); // do NOT go back to all devices grounded to shorting cap
			Sleep(200);
		}
	}
	switchbox.closeAll(); //get back to being all shorted to ground 
}

void menu::executeSweepBreak(SPA4156B SPA, Switchbox switchbox, FILE* outputs[36]){

	//let user set the SPA sweep breaking parameters
	std::cout << "\n\n>>>> Please set sweep breaking parameters...\n\n";
	float maxStart, minStart, targetR, stepBias, stepV;
	int useEMVFlag;
	SPA.setSwBrParamsLater(&useEMVFlag, &minStart, &maxStart, &targetR, &stepBias, &stepV);
	float temperature;
	std::cout << "Please check TempB on Lakeshore and input in Kelvin \n";
	std::cin >> temperature;

	// get a filename and open a file that will be used to write the summary info to
	std::string file_summary;
	std::cout << "What is the filename stem to use for the SwBr Outcome Summary? \n";
	std::cin >> file_summary;
	char filebuffer[1024] = "";
	sprintf(filebuffer, "%s_%s_SwBrSummary.txt", file_summary.c_str(),chip_ID.c_str());
	FILE *summary = fopen(filebuffer, "w+");
	if (summary == NULL) { // check that fopen worked correctly
		std::cout << "fopen has failed for the output file \n" << filebuffer << "\n";
	}

	typedef std::chrono::high_resolution_clock Clock;
	typedef std::chrono::minutes minutes;

	for (int devnum = 0; devnum < switchbox.ndev; devnum++){
		if (switchbox.portSpecs[4][devnum] == "y"){
			if (useEMVFlag){ // if the user wants to use exit Vs from prior EM run then set those parameters here
				minStart = switchbox.exitSummary[1][devnum]-0.07;
				maxStart = switchbox.exitSummary[1][devnum]-0.02;
			}
			switchbox.closeChan(devnum, "SPA"); // all channels except DUT remain shorted to bias port, DUT is connected to relevant inputs
			Clock::time_point t0 = Clock::now();
			switchbox.exitSummary[3][devnum] = SPA.sweepBreak(devnum, outputs, maxStart, minStart, targetR, stepBias, stepV, temperature); // writes IV data to output file. keithley will set his output to 0V after finishing EM run
			Clock::time_point t1 = Clock::now();
			switchbox.openChan(devnum, "SPA", 1); // get back to the state of all output ports shorted to keithley ground
			minutes Elapsed = std::chrono::duration_cast<minutes>(t1 - t0);
			switchbox.exitSummary[5][devnum] = Elapsed.count(); //save the total time to EM in the summary array
			std::cout << "---------------------------------------------------\n";
			std::cout << "Device ID# " << switchbox.portSpecs[3][devnum].c_str() << " has completed active Sweep Breaking! \n";
			std::cout << "Elapsed time of sweep breaking was " << Elapsed.count() << " minutes. \n";
			std::cout << "Exit voltage of the sweep breaking was " << switchbox.exitSummary[3][devnum] << " V.\n";
			switchbox.exitSummary[4][devnum] = executeGapYield(devnum, SPA, switchbox);
			std::cout << "---------------------------------------------------\n";
			
		}
	}

	SPA.SendMail(emailAddress);

	fprintf(summary, "Chip ID, Device Ports, Run Type, Temperature (K), SwBr exit V (V), R @ 100mV after SwBr (ohms), Elapsed Time of SwBr (min))\n"); // header
	char buffer[1024];
	for (int devnum = 0; devnum < switchbox.ndev; devnum++){

		if (switchbox.portSpecs[4][devnum] == "y"){
			sprintf(buffer, "%s, %s, SwBr, %.3f, %.3f, %.3f, %.1f\n", chip_ID.c_str(), switchbox.portSpecs[3][devnum].c_str(), temperature, switchbox.exitSummary[3][devnum], switchbox.exitSummary[5][devnum], switchbox.exitSummary[4][devnum]);
			fprintf(summary, buffer);
		}
	}
	fclose(summary);

}

void menu::executeEMandSweepBreakandGatedIV(SPA4156B SPA, Switchbox switchbox, K2400 keithley){
	
	// let user set EM parameters for keithley
	std::cout << "\n\n>>>> Please set EM parameters...\n\n";
	int ymflag = 0; // is this a yield monitoring measurement? affects how switchbox.getPorts creates output files streams
	FILE* outputsEM[36]; // this array will hold all the file pointers to the output files being written for EM
	switchbox.getPorts(ymflag, outputsEM);
	keithley.setParams(); // sets the EM parameters (some user-input)

	//let user set the SPA sweep breaking parameters
	std::cout << "\n\n>>>> Please set sweep breaking parameters...\n\n";
	FILE* outputsSWBR[36]; // this array will hold all the file pointers to the output files being written for SWeep breaking
	switchbox.getPorts(ymflag, outputsSWBR);
	float maxStart, minStart, targetR, stepBias, stepV;
	int useEMVFlag;
	SPA.setSwBrParamsLater(&useEMVFlag, &minStart, &maxStart, &targetR, &stepBias, &stepV);

	//let user set the SPA Gated IV sweep parameters
	std::cout << "\n\n>>>> Please set Gated IV Sweep parameters...\n";
	FILE* outputsGIV[36]; // this array will hold all the file pointers to the output files being written for gated IV sweeps
	switchbox.getPorts(ymflag, outputsGIV);
	float Vstart, Vstop, Vstep, Vstart_gate, Vstep_gate;
	int Nsteps_gate;
	SPA.setGatedIVParamsLater(&Vstart, &Vstop, &Vstep, &Vstart_gate, &Vstep_gate, &Nsteps_gate); //asks user for parameters and saves them for later

	// get the current temperature - hopefully doesn't change much over the course of the measurement
	float temperature;
	std::cout << "Please check TempB on Lakeshore and input in Kelvin \n";
	std::cin >> temperature;


	// get a filename and open a file that will be used to write the summary info to
	std::string file_summary;
	std::cout << "What is the filename stem to use for the EM and SwBr Outcome Summary? \n";
	std::cin >> file_summary;
	char filebuffer[1024] = "";
	sprintf(filebuffer, "%s_%s_EMAndSwBrSummary.txt", file_summary.c_str(), chip_ID.c_str());
	FILE *summary = fopen(filebuffer, "w+");
	if (summary == NULL) { // check that fopen worked correctly
		std::cout << "fopen has failed for the output file " << filebuffer << "\n";
	}


	//set up time keeping apparatus
	typedef std::chrono::high_resolution_clock Clock;
	typedef std::chrono::minutes minutes;
	Clock::time_point t0;
	Clock::time_point t1;
	minutes Elapsed;



	for (int devnum = 0; devnum < switchbox.ndev; devnum++){
		if (switchbox.portSpecs[4][devnum] == "y"){
			//
			// Perform EM on the device...
			//
			switchbox.closeChan(devnum, "keithley"); // all channels except DUT remain shorted to bias port, DUT is connected to relevant inputs
			t0 = Clock::now();
			switchbox.exitSummary[0][devnum] = keithley.emSingle(devnum, outputsEM); // keithley.emSingle IV data to output file. keithley will set his output to 0V after finishing EM run and return the exit voltage
			t1 = Clock::now();
			switchbox.openChan(devnum, "keithley",1); // get back to the state of all output ports shorted to keithley ground
			Elapsed = std::chrono::duration_cast<minutes>(t1 - t0);
			switchbox.exitSummary[2][devnum] = Elapsed.count(); //save the total time to EM in the summary array
			std::cout << "---------------------------------------------------\n";
			std::cout << "Device ID# " << switchbox.portSpecs[3][devnum].c_str() << " has completed active EM! \n";
			std::cout << "Elapsed time of electromigration was " << Elapsed.count() << " minutes. \n";
			std::cout << "Exit voltage of the electromigration was " << switchbox.exitSummary[0][devnum] << " V.\n";
			switchbox.exitSummary[1][devnum] = executeGapYield(devnum, SPA, switchbox); // will print out the actual resistance sampled at 100mV 
			std::cout << "---------------------------------------------------\n";
			//
			//Perform Sweep Breaking on the device...
			//
			if (useEMVFlag){ // if the user wants to use exit Vs from prior EM run then set those parameters here
				minStart = switchbox.exitSummary[0][devnum]-0.02;
				maxStart = switchbox.exitSummary[0][devnum]-0.015;
			}
			switchbox.closeChan(devnum, "SPA"); // all channels except DUT remain shorted to bias port, DUT is connected to relevant inputs
			t0 = Clock::now();
			switchbox.exitSummary[3][devnum] = SPA.sweepBreak(devnum, outputsSWBR, maxStart, minStart, targetR, stepBias, stepV, temperature); // writes IV data to output file, returns max Bias of last sweep
			t1 = Clock::now();
			switchbox.openChan(devnum, "SPA",1); // get back to the state of all output ports shorted to keithley ground
			Elapsed = std::chrono::duration_cast<minutes>(t1 - t0);
			switchbox.exitSummary[5][devnum] = Elapsed.count(); //save the total time to EM in the summary array
			std::cout << "---------------------------------------------------\n";
			std::cout << "Device ID# " << switchbox.portSpecs[3][devnum].c_str() << " has completed Sweep Breaking! \n";
			std::cout << "Elapsed time of sweep breaking was " << Elapsed.count() << " minutes. \n";
			std::cout << "Exit voltage of the sweep breaking was " << switchbox.exitSummary[3][devnum] << " V.\n";
			switchbox.exitSummary[4][devnum] = executeGapYield(devnum, SPA, switchbox); // will print out the actual resistance sampled at 100mV 
			std::cout << "---------------------------------------------------\n";
		}
	}

	// close EM and sweep breaking record files
	closeFiles(ymflag, switchbox.ndev, outputsEM, switchbox);
	closeFiles(ymflag, switchbox.ndev, outputsSWBR, switchbox);
	// print out summary of EM and sweep breaking to Summary.txt
	fprintf(summary, "Chip ID, Device Ports, Run Type, Temperature (K), exit V (V), R @ 100mV after Run (ohms), Elapsed Time of Run (min))\n"); // header
	char buffer[1024];
	for (int devnum = 0; devnum < switchbox.ndev; devnum++){
		if (switchbox.portSpecs[4][devnum] == "y"){
			// print out the EM summary
			sprintf(buffer, "%s, %s, EM, %.3f, %.3f, %.3f, %.1f\n", chip_ID.c_str(), switchbox.portSpecs[3][devnum].c_str(), temperature, switchbox.exitSummary[0][devnum], switchbox.exitSummary[1][devnum], switchbox.exitSummary[2][devnum]);
			fprintf(summary, buffer);
			// print out the SwBr summary
			sprintf(buffer, "%s, %s, SwBr, %.3f, %.3f, %.3f, %.1f\n", chip_ID.c_str(), switchbox.portSpecs[3][devnum].c_str(), temperature, switchbox.exitSummary[3][devnum], switchbox.exitSummary[4][devnum], switchbox.exitSummary[5][devnum]);
			fprintf(summary, buffer);
		}
	}
	fclose(summary);

	//
	//Perform Gated IV's on the Device...
	//
	//SPA.configGatedSweeps(Vstart, Vstop, Vstep, Vstart_gate, Vstep_gate, Nsteps_gate);
	//executeGatedSweeps(SPA, switchbox, outputsGIV);
	closeFiles(ymflag, switchbox.ndev, outputsGIV, switchbox);
	SendMail(emailAddress);	//send mail so person knows everything is done

}

void menu::executeEMandKeithleySweep(SPA4156B SPA, Switchbox switchbox, K2400 keithley){

	//// let user set EM parameters for keithley
	//std::cout << "\n\n>>>> Please set EM parameters...\n\n";
	//int ymflag = 0; // is this a yield monitoring measurement? affects how switchbox.getPorts creates output files streams
	//FILE* outputsEM[36]; // this array will hold all the file pointers to the output files being written for EM
	//switchbox.getPorts(ymflag, outputsEM);
	//keithley.setParams(); // sets the EM parameters (some user-input)


	////let user set the KEITHLEy sweep breaking parameters
	//std::cout << "\n\n>>>> Please set Keithley Sweep breaking parameters...\n\n";
	//FILE* outputsKS[36]; // this array will hold all the file pointers to the output files being written for SWeep breaking
	//switchbox.getPorts(ymflag, outputsKS);
	//float volt_ramp_temp, volt_start_temp, volt_stop_temp, target_resistance_temp;
	//int useEMVFlag;
	//keithley.setParamsSweep(&volt_ramp_temp, &volt_start_temp, &volt_stop_temp, &target_resistance_temp);

	//////let user set the SPA Gated IV sweep parameters
	////std::cout << "\n\n>>>> Please set Gated IV Sweep parameters...\n";
	////FILE* outputsGIV[36]; // this array will hold all the file pointers to the output files being written for gated IV sweeps
	////switchbox.getPorts(ymflag, outputsGIV);
	////float Vstart, Vstop, Vstep, Vstart_gate, Vstep_gate;
	////int Nsteps_gate;
	////SPA.setGatedIVParamsLater(&Vstart, &Vstop, &Vstep, &Vstart_gate, &Vstep_gate, &Nsteps_gate); //asks user for parameters and saves them for later

	//// get the current temperature - hopefully doesn't change much over the course of the measurement
	//float temperature;
	//std::cout << "Please check TempB on Lakeshore and input in Kelvin \n";
	//std::cin >> temperature;


	//// get a filename and open a file that will be used to write the summary info to
	//std::string file_summary;
	//std::cout << "What is the filename stem to use for the EM and SwBr Outcome Summary? \n";
	//std::cin >> file_summary;
	//char filebuffer[1024] = "";
	//sprintf(filebuffer, "%s_%s_EMAndSwBrSummary.txt", file_summary.c_str(), chip_ID.c_str());
	//FILE *summary = fopen(filebuffer, "w+");
	//if (summary == NULL) { // check that fopen worked correctly
	//	std::cout << "fopen has failed for the output file " << filebuffer << "\n";
	//}


	////set up time keeping apparatus
	//typedef std::chrono::high_resolution_clock Clock;
	//typedef std::chrono::minutes minutes;
	//Clock::time_point t0;
	//Clock::time_point t1;
	//minutes Elapsed;



	//for (int devnum = 0; devnum < switchbox.ndev; devnum++){
	//	if (switchbox.portSpecs[4][devnum] == "y"){
	//		//
	//		// Perform EM on the device...
	//		//
	//		switchbox.closeChan(devnum, "keithley"); // all channels except DUT remain shorted to bias port, DUT is connected to relevant inputs
	//		t0 = Clock::now();
	//		switchbox.exitSummary[0][devnum] = keithley.emSingle(devnum, outputsEM); // keithley.emSingle IV data to output file. keithley will set his output to 0V after finishing EM run and return the exit voltage
	//		t1 = Clock::now();
	//		switchbox.openChan(devnum, "keithley", 1); // get back to the state of all output ports shorted to keithley ground
	//		Elapsed = std::chrono::duration_cast<minutes>(t1 - t0);
	//		switchbox.exitSummary[2][devnum] = Elapsed.count(); //save the total time to EM in the summary array
	//		std::cout << "---------------------------------------------------\n";
	//		std::cout << "Device ID# " << switchbox.portSpecs[3][devnum].c_str() << " has completed active EM! \n";
	//		std::cout << "Elapsed time of electromigration was " << Elapsed.count() << " minutes. \n";
	//		std::cout << "Exit voltage of the electromigration was " << switchbox.exitSummary[0][devnum] << " V.\n";
	//		switchbox.exitSummary[1][devnum] = executeGapYield(devnum, SPA, switchbox); // will print out the actual resistance sampled at 100mV 
	//		std::cout << "---------------------------------------------------\n";
	//		//
	//		//Perform Sweep Breaking on the device...
	//		//
	//		if (useEMVFlag){ // if the user wants to use exit Vs from prior EM run then set those parameters here
	//			minStart = switchbox.exitSummary[0][devnum] - 0.02;
	//			maxStart = switchbox.exitSummary[0][devnum] - 0.015;
	//		}
	//		switchbox.closeChan(devnum, "SPA"); // all channels except DUT remain shorted to bias port, DUT is connected to relevant inputs
	//		t0 = Clock::now();
	//		switchbox.exitSummary[3][devnum] = SPA.sweepBreak(devnum, outputsSWBR, maxStart, minStart, targetR, stepBias, stepV, temperature); // writes IV data to output file, returns max Bias of last sweep
	//		t1 = Clock::now();
	//		switchbox.openChan(devnum, "SPA", 1); // get back to the state of all output ports shorted to keithley ground
	//		Elapsed = std::chrono::duration_cast<minutes>(t1 - t0);
	//		switchbox.exitSummary[5][devnum] = Elapsed.count(); //save the total time to EM in the summary array
	//		std::cout << "---------------------------------------------------\n";
	//		std::cout << "Device ID# " << switchbox.portSpecs[3][devnum].c_str() << " has completed Sweep Breaking! \n";
	//		std::cout << "Elapsed time of sweep breaking was " << Elapsed.count() << " minutes. \n";
	//		std::cout << "Exit voltage of the sweep breaking was " << switchbox.exitSummary[3][devnum] << " V.\n";
	//		switchbox.exitSummary[4][devnum] = executeGapYield(devnum, SPA, switchbox); // will print out the actual resistance sampled at 100mV 
	//		std::cout << "---------------------------------------------------\n";
	//	}
	//}

	//// close EM and sweep breaking record files
	//closeFiles(ymflag, switchbox.ndev, outputsEM, switchbox);
	//closeFiles(ymflag, switchbox.ndev, outputsSWBR, switchbox);
	//// print out summary of EM and sweep breaking to Summary.txt
	//fprintf(summary, "Chip ID, Device Ports, Run Type, Temperature (K), exit V (V), R @ 100mV after Run (ohms), Elapsed Time of Run (min))\n"); // header
	//char buffer[1024];
	//for (int devnum = 0; devnum < switchbox.ndev; devnum++){
	//	if (switchbox.portSpecs[4][devnum] == "y"){
	//		// print out the EM summary
	//		sprintf(buffer, "%s, %s, EM, %.3f, %.3f, %.3f, %.1f\n", chip_ID.c_str(), switchbox.portSpecs[3][devnum].c_str(), temperature, switchbox.exitSummary[0][devnum], switchbox.exitSummary[1][devnum], switchbox.exitSummary[2][devnum]);
	//		fprintf(summary, buffer);
	//		// print out the SwBr summary
	//		sprintf(buffer, "%s, %s, SwBr, %.3f, %.3f, %.3f, %.1f\n", chip_ID.c_str(), switchbox.portSpecs[3][devnum].c_str(), temperature, switchbox.exitSummary[3][devnum], switchbox.exitSummary[4][devnum], switchbox.exitSummary[5][devnum]);
	//		fprintf(summary, buffer);
	//	}
	//}
	//fclose(summary);

	////
	////Perform Gated IV's on the Device...
	////
	////SPA.configGatedSweeps(Vstart, Vstop, Vstep, Vstart_gate, Vstep_gate, Nsteps_gate);
	////executeGatedSweeps(SPA, switchbox, outputsGIV);
	//closeFiles(ymflag, switchbox.ndev, outputsGIV, switchbox);
	//SendMail(emailAddress);	//send mail so person knows everything is done

}

void menu::executeSweepBreakAndSelfBreak_PS(SPA4145B SPA_probeStation, FILE* output){
	float maxStart;
	float minStart;
	float targetR;
	float stepBias;
	float stepV;

	std::cout << "What is the start voltage for all sweeps i.e. min bias (in Volts)? \n";
	std::cin >> minStart;
	std::cout << "What is the starting max bias for sweeps (in Volts)? \n";
	std::cin >> maxStart;
	std::cout << "What is the target resistance (in Ohms)? \n";
	std::cin >> targetR;
	std::cout << "What is the step size for max bias on successive sweeps (in Volts)? \n";
	std::cin >> stepBias;
	std::cout << "What is the step size for voltage during sweeps (in Volts)? \n";
	std::cin >> stepV;
	float temperature;
	std::cout << "Please check TempB on Lakeshore and input in Kelvin \n";
	std::cin >> temperature;

	SPA_probeStation.sweepBreak(output, maxStart, minStart, targetR, stepBias, stepV); 
	
	std::cout << "=======================================================================\n";
	std::cout << "Self-break monitoring will continue until the 'F12' key is pressed...\n";
	std::cout << "=======================================================================\n";
	int cycle = 0;

	SPA_probeStation.constconfigSample();

	while (!GetAsyncKeyState(VK_F12)){
		SPA_probeStation.constsampleSingle(output, cycle); // 
		cycle++;
	}

}

void menu::executeIETS(K2400 keithley, Switchbox switchbox, FILE* outputs[36], bool gateFlag){
	for (int devnum = 0; devnum < switchbox.ndev; devnum++){
		if (switchbox.portSpecs[4][devnum] == "y"){
			switchbox.closeChan(devnum, "IETS"); // all channels except DUT remain shorted to bias port, DUT is connected to relevant inputs
			std::cout << "Device has been connected to IETS measurement on input ports 05 and 06.\n";
			if (gateFlag){
				holdGateVoltage(keithley, switchbox, outputs, devnum);
				switchbox.openChan(devnum, "HoldGate", 0);
			}
			std::cout << "=======================================================================\n";
			std::cout << "Press the 'F12' key to disconnect the device...\n";
			std::cout << "=======================================================================\n";
			while (!GetAsyncKeyState(VK_F12)){
			}
			switchbox.openChan(devnum, "IETS", 1);
		}
	}
}

void menu::holdGateVoltage(K2400 keithley, Switchbox switchbox, FILE* outputs[36], int devnum){
			switchbox.closeChan(devnum, "HoldGate"); //this will NOT first unground all the other devices sharing common drain see Switchbox::closeChan
			std::cout << "Device ID# " << switchbox.portSpecs[3][devnum].c_str() << " has had its gateline connected to Keithley.\n";
			int choice = 1;
			while (choice){
				keithley.holdGateVoltage();
				std::cout << "Hold another gate voltage? --- 1 for 'yes' , 0 for 'no'. \n";
				std::cin >> choice;
			}
}

void menu::closeFiles(int ymflag, int ndev, FILE* outputs[36], Switchbox switchbox){
	if (ymflag == 1){ fclose(outputs[0]);}
	else if (ymflag == 0){
		for (int devnum = 0; devnum < ndev; devnum++){
			if (switchbox.portSpecs[4][devnum] == "y"){
				fclose(outputs[devnum]);
			}
		}
	}
}

void menu::SendMail(std::string emailAddress){
	std::string program = "";
	program += "require \'rubygems\' \nrequire \'net/smtp\' \nrequire \'tlsmail\'\n";
	program += "time1 = Time.new \n";
	program += "Net::SMTP.enable_tls(OpenSSL::SSL::VERIFY_NONE)\n";
	program += "FROM_EMAIL = \"cryptobatman123@gmail.com\" \nPASSWORD = \"ProjBatman\" \n";
	program += "TO_EMAIL = \"";
	program += emailAddress;
	program += "\" \n";
	program += "msgstr = <<END_OF_MESSAGE\n";
	program += "From: Your Name <#{FROM_EMAIL}> \nTo: my phone <#{TO_EMAIL}> \nSubject: finished last device at <#{time1.inspect}> \n";
	program += "Date: Sat, 23 Jun 2001 16:26:43 +0900 \nMessage-Id: <unique.message.id.string@example.com> \n";
	program += "Come check CryoPC! \n";
	program += "END_OF_MESSAGE\n";
	program += "Net::SMTP.start(\'smtp.gmail.com\', 587, \'gmail.com\', \n";
	program += "FROM_EMAIL, PASSWORD, :plain) do |smtp| \n";
	program += "smtp.send_message msgstr, FROM_EMAIL, TO_EMAIL\n";
	program += "end";
	FILE* mailing = fopen("mail.rb", "w+");
	fprintf(mailing, program.c_str());
	fclose(mailing);
	system("Ruby  mail.rb");
}
