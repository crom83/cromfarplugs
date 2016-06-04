BOOL BINK_is_valid(unsigned char* data, long size, long fsize)
{
    return strncmp((char*)data, "BIKi", 4) == 0?TRUE:FALSE;               
}