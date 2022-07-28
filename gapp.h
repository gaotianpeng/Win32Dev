#ifndef __G_APP_H
#define __G_APP_H
#include "application.h"

class GApp: public App {
public:
	~GApp();

	BOOL InitInstance() override;
	void Uninitialize();
protected:

private:
};

#endif // __G_APP_H
