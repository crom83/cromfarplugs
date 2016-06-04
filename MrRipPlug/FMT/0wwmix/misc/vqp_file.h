BOOL vqp_is_valid(unsigned char* data, long size, long fsize)
{
    long c_tables;
    memcpy(&c_tables, data, 4);    
	
	return (4 + 32896 * c_tables == fsize);
}



