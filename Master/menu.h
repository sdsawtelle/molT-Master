#ifndef MENU_H
#define MENU_H

#include "K2400.h"
#include "SPA4156B.h"
#include "SPA4145B.h"
#include "Switchbox.h"
#include <iostream>
#include <chrono>
#include "engine.h"

class menu
{
public:
	menu();
	virtual ~menu();
	void menu_start(K2400 keithley, SPA4156B SPA, Switchbox switchbox);
	std::string chip_ID;
	std::string emailAddress;
	std::string ft_ID;
	void SendMail(std::string emailAddress);

	void test();

protected:
private:
	void executeEM(int emType, K2400 keithley, SPA4156B SPA, Switchbox switchbox, FILE* outputs[36], std::string outputs_fnames[36]);
	void executeEMandIV(int emType, SPA4156B SPA, Switchbox switchbox, K2400 keithley);

	void executeYield(SPA4156B SPA, Switchbox switchbox, FILE* outputs[36]);
	float executeGapYield(int devnum, SPA4156B SPA, Switchbox switchbox);

	void executeGatelineSweep(SPA4156B SPA, Switchbox switchbox, FILE* outputs[36]);
	void executeLeakageYield(SPA4156B SPA, Switchbox switchbox, FILE* outputs[36]);
	void executeLeakageSweep(SPA4156B SPA, Switchbox switchbox, FILE* outputs[36]);

	void executeIV(SPA4156B SPA, Switchbox switchbox, FILE* outputs[36]);
	void executeGatedIV(int leakageFlag, SPA4156B SPA, Switchbox switchbox, FILE* outputs[36]);

	void keithleyDEP(K2400 keithley, Switchbox switchbox, FILE* outputs[36]);
	void executeIETS(K2400 keithley, Switchbox switchbox, FILE* outputs[36], bool gateFlag);
	void holdGateVoltage(K2400 keithley, Switchbox switchbox, FILE* outputs[36], int devnum);

	void executeEMandMonitor(int emType, K2400 keithley, SPA4156B SPA, Switchbox switchbox, FILE* outputs[36]);
	void executeMonitor(SPA4156B SPA, Switchbox switchbox, FILE* outputs[36]);
	void executeGatedMonitor(SPA4156B SPA, K2400 keithley, Switchbox switchbox, FILE* outputs[36]);
	void closeFiles(int ymflag, int ndev, FILE* outputs[36], Switchbox switchbox);
};

#endif // MENU_H

