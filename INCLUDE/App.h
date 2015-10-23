#ifndef __APP_H
#define __APP_H

#include "_pch.h"

class App : public wxApp
{
	FILE*  mLogFile;
	wxLog*  mLogger;
public:
	virtual bool OnInit();
	virtual int OnExit();

	virtual ~App();
};

#endif // __APP_H
