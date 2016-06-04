BOOL png_is_valid(unsigned char* data, long size, long fsize)
{
    unsigned char t[] = {0x89, 0x50, 0x4E, 0x47}; //PNG start signature
    return strncmp((char*)data, (char*)t, 4) == 0?TRUE:FALSE;		
}