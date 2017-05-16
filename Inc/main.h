#ifndef __MAIN_H__
#define __MAIN_H__
#ifdef __cplusplus
extern "C"{
#endif
	
	#define PRODUCT_TYPE_E01		0
	#define PRODUCT_TYPE_E01S		1
	#define PRODUCT_TYPE			PRODUCT_TYPE_E01S
	
	#if PRODUCT_TYPE == PRODUCT_TYPE_E01
		#define PRODUCT_NAME		"E01"
	#elif PRODUCT_TYPE == PRODUCT_TYPE_E01S
		#define PRODUCT_NAME		"E01S"
	#else
		#define PRODUCT_NAME		""
	#endif
	
	#define MODULE_NAME				" UR"
	#define HARDWARE_PLATFORM		"04"
	#define VISION					"V001"
	#define PUBLISH_DATE			"20170324"
	
	#define MODULE_INFO				PRODUCT_NAME MODULE_NAME HARDWARE_PLATFORM VISION PUBLISH_DATE
	
#ifdef __cplusplus
}
#endif
#endif
