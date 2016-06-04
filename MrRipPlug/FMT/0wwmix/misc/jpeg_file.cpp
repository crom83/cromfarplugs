BOOL jpeg_is_valid(unsigned char* data, long size, long fsize)
{
    if ((data[0]==0xFF)&&
        (data[1]==0xD8)&&
        (data[2]==0xFF)&&
        (data[3]==0xE0)&&
        (data[4]==0x10)&&
        (data[5]==0x4A)&&
        (data[6]==0x46)&&
        (data[7]==0x49)&&
        (data[8]==0x46)) return TRUE;
    else return FALSE;    
}