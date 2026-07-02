#ifndef __ZETNET_H__
#define __ZETNET_H__

//----------------------------
// START ZETNET PROTOTYPES

#define ZETNET_VERSION_MAJOR	1
#define ZETNET_VERSION_MINOR	5
#define ZETNET_VERSION_PATCH	0


#include "base/@ZN_Base.h"
#include "net/@ZN_Net.h"
#include "http/@ZN_Http.h"

ZN_DLL_EXPORT bool ZN_Init(void);
ZN_DLL_EXPORT void ZN_DeInit(void);



#endif
