#include "misc/cc_structures.h"

BOOL fnt_is_valid(unsigned char* data, long size, long fsize)
{
    t_fnt_header fnt;    

    memcpy(&fnt, data, sizeof(t_fnt_header));		
		
	if (sizeof(t_fnt_header) > fsize || 
	    fnt.size != fsize ||
		fnt.id2 != 0x000e ||
		fnt.id3 != 0x0014 ||
		fnt.zero)
		return FALSE;

    return (fnt.id1 == 0x0500 ? 0 : 1) == 0 ?		
	        fnt.id1 == 0x0500 && fnt.id4 == 0x1012 : 
		    fnt.id1 == 0x0002 && !fnt.id4;
}
