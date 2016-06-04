BOOL pcx_is_valid(unsigned char* data, long size, long fsize)
{
    unsigned char t0[] = {0x0A, 0x00, 0x01}; //PCX 0 signature
    unsigned char t2[] = {0x0A, 0x02, 0x01}; //PCX 2 signature
    unsigned char t3[] = {0x0A, 0x03, 0x01}; //PCX 3 signature
    unsigned char t4[] = {0x0A, 0x04, 0x01}; //PCX 4 signature
    unsigned char t5[] = {0x0A, 0x05, 0x01}; //PCX 5 signature

    if (strncmp((char*)data, (char*)t0, 3)==0||
        strncmp((char*)data, (char*)t2, 3)==0||
        strncmp((char*)data, (char*)t3, 3)==0||
        strncmp((char*)data, (char*)t4, 3)==0||
        strncmp((char*)data, (char*)t5, 3)==0) return TRUE; else return FALSE;
}