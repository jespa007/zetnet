#ifndef __ZETNET_H__
#define __ZETNET_H__

//----------------------------
// START ZETNET PROTOTYPES

#define ZETNET_VERSION_MAJOR	1
#define ZETNET_VERSION_MINOR	5
#define ZETNET_VERSION_PATCH	0


#include "base/@base.h"
#include "net/@net.h"
#include "http/@http.h"

bool ZN_Init(void);
void ZN_DeInit(void);



#endif
