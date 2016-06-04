BOOL ogg_is_valid(unsigned char* data, long size, long fsize)
{
    return strncmp((char*)data+0x1D, "vorbis", 6) == 0?TRUE:FALSE;		
}