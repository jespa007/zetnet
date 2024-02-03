#ifndef __ZN_HTTP_SERVER_MPS_H__
#define __ZN_HTTP_SERVER_MPS_H__

typedef struct ZN_HttpServerMPS ZN_HttpServerMPS;

struct ZN_HttpServerMPS{
	void *data;
};

ZN_HttpServerMPS *		ZN_HttpServerMPS_New(void);
void 					ZN_HttpServerMPS_SetTimeDelay(ZN_HttpServerMPS 	*_this, unsigned long time );
void 					ZN_HttpServerMPS_Start(ZN_HttpServerMPS	*_this, int port);
void 					ZN_HttpServerMPS_Stop(ZN_HttpServerMPS 	*_this);

bool					ZN_HttpServerMPS_CanWrite(ZN_HttpServerMPS	*_this);
void					ZN_HttpServerMPS_Write(ZN_HttpServerMPS	*_this, uint8_t *ptr, size_t len);

void 					ZN_HttpServerMPS_Delete(ZN_HttpServerMPS 	*_this);

void 					ZN_HttpServerMPS_Delete(ZN_HttpServerMPS *_this);

#endif
