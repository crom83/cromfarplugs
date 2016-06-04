#include "misc/cc_structures.h"

BOOL cps_is_valid(unsigned char* data, long size, long fsize)
{
    t_cps_header cps;

    memcpy(&cps, data, sizeof(t_cps_header));	
	if (sizeof(t_cps_header) > fsize ||
			cps.image_size != 320 * 200 ||				
			cps.zero)
			return FALSE;
	switch (cps.unknown)
    {
        /*
	case 3:
	return header.size == size;	*/
	case 4:
	    return 2 + cps.size == fsize;
	default:
	    return FALSE;
	}    
}