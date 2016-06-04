BOOL pal_is_valid(unsigned char* data, long size, long fsize)
{
#pragma pack(push, 1)
    struct {
        unsigned char r;
        unsigned char g;
        unsigned char b;
    } pal;
#pragma pack(pop)

    if (fsize != 768)
	    return FALSE;    
	for (int i = 0; i < 256; i++)
	{
        memcpy(&pal, data+i, 3);
	    if ((pal.r | pal.g | pal.b) & 0xc0)
		return FALSE;
	}
	return TRUE;
}
