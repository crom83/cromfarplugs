BOOL vxl_is_valid(unsigned char* data, long size, long fsize)
{
    return strncmp((char*)data, "Voxel Animation", 15) == 0?TRUE:FALSE;		
}