#include "misc/cc_structures.h"

BOOL hva_is_valid(unsigned char* data, long size, long fsize)
{
    t_hva_header hva;
    memcpy(&hva, data, sizeof(t_hva_header));	
	return !(sizeof(t_hva_header) > fsize ||
	    sizeof(t_hva_header) + (48 * hva.c_frames + 16) * hva.c_sections != (unsigned long)fsize);
}

	