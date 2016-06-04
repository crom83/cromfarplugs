BOOL RIFF_is_valid(unsigned char* data, long size, long fsize)
{
    return strncmp((char*)data, "RIFF", 4) == 0?TRUE:FALSE;		
}