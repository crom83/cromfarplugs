BOOL WAV_is_valid(unsigned char* data, long size, long fsize)
{
    return strncmp((char*)data+8, "WAVEfmt ", 8) == 0?TRUE:FALSE;		
}