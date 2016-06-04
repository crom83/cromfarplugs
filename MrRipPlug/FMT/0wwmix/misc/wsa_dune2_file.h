#include "misc/cc_structures.h"

BOOL wsa_d2_is_valid(unsigned char* data, long size, long fsize)
{   
	t_wsa_dune2_header header;    

	memcpy(&header, data, sizeof(t_wsa_dune2_header));
    int cb_index = 4 * (header.c_frames + 2);
    
	if (sizeof(t_wsa_dune2_header) + 4 > fsize || header.c_frames < 1 || header.c_frames > 1000 || sizeof(t_wsa_dune2_header) + cb_index > (unsigned long)fsize)
	    return FALSE;		
	
	return TRUE;
}
