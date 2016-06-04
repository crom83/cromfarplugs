#include "misc/cc_structures.h"

BOOL shp_d2_is_valid(unsigned char* data, long size, long fsize)
{
	t_shp_dune2_header header;
	
    memcpy(&header, data, sizeof(t_shp_dune2_header));

    char cb_offset = (short)(data+sizeof(t_shp_dune2_header)+2) ? 2 : 4;

	if (sizeof(t_shp_dune2_header) + 4 > fsize || header.c_images < 1 || header.c_images > 1000|| sizeof(t_shp_dune2_header) + cb_offset * header.c_images> (unsigned long)fsize)
			return FALSE;	
    return TRUE;
}

