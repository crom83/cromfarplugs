BOOL vqa_is_valid(unsigned char* data, long size, long fsize)
{
    return strncmp((char*)data+8, "WVQAVQHD", 4) == 0?TRUE:FALSE;		
}