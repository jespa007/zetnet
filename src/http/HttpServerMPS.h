#ifndef __HTTP_SERVER_MPS_H__
#define __HTTP_SERVER_MPS_H__

typedef struct HttpServerMPS HttpServerMPS;

struct HttpServerMPS{
	void *data;
};

HttpServerMPS *		HttpServerMPS_New(void);
void 				HttpServerMPS_SetTimeDelay(HttpServerMPS 	*_this, unsigned long time );
void 				HttpServerMPS_Start(HttpServerMPS	*_this, int port);
void 				HttpServerMPS_Stop(HttpServerMPS 	*_this);

bool				HttpServerMPS_CanWrite(HttpServerMPS	*_this);
void				HttpServerMPS_Write(HttpServerMPS	*_this, uint8_t *ptr, size_t len);

void 				HttpServerMPS_Delete(HttpServerMPS 	*_this);

void 				HttpServerMPS_Delete(HttpServerMPS *_this);

#endif
