BOOL AVI_is_valid(unsigned char* data, long size, long fsize)
{
    return strncmp((char*)data+8, "AVI LIST", 8) == 0?TRUE:FALSE;		
}