#ifndef __ZN_COMMON_H__
#define __ZN_COMMON_H__

#define ZN_THIS_DATA_NULL(_this) ((_this)==NULL || (_this)->data==NULL)


#define ZN_UNUSUED_PARAM(x) ((void)x)
#define ZN_UNUSED_2PARAMS(_param1, param2) ((void)(_param1),(void)(param2))

#endif
