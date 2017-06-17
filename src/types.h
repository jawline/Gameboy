#ifndef _HELP_TYPES_DEF_H
#define _HELP_TYPES_DEF_H

typedef struct {
	union {
		struct { uint8_t low; uint8_t high; };
		uint16_t full;		
	};
} uint16_builder;

#endif