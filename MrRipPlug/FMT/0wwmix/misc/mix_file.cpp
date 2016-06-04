#include "misc/cc_structures.h"

BOOL mix_is_valid(unsigned char* data, long size, long fsize)
{
    t_mix_header mix;
    BOOL m_has_checksum;
    BOOL m_is_encrypted;
        
    memcpy(&mix, data, sizeof(t_mix_header));

    if (sizeof(t_mix_header) > fsize) return FALSE;
    
    if (mix.e.c_files && sizeof(t_mix_header) + mix.e.c_files * sizeof(t_mix_index_entry) + mix.e.size == (unsigned long)fsize)
                return TRUE;

    if (mix.flags & ~(mix_encrypted | mix_checksum))
                return false;
        m_has_checksum = mix.flags & mix_checksum;
        m_is_encrypted = mix.flags & mix_encrypted;
        Cblowfish bf;
        if (m_is_encrypted)
        {
                BYTE key[56];
                get_blowfish_key(data + 4, key);
                bf.set_key(key, 56);
        }
        int error = 0;
        {
                if (m_is_encrypted)
                {                        
                    bf.decipher(data + 84, &mix.e, 8);

                    if (!mix.e.c_files || 4 + (m_is_encrypted ? 80 : 0) + sizeof(t_mix_header) + 2 + (mix.e.c_files * sizeof(t_mix_index_entry) + 5 & ~7) + mix.e.size + (m_has_checksum ? 20 : 0) != (unsigned long)fsize)
                                error = true;
                }
                else
                {
                    memcpy(&mix, data+4, sizeof(t_mix_header));                    
                    if (!mix.e.c_files || 4 + sizeof(t_mix_header) + mix.e.c_files * sizeof(t_mix_index_entry) + mix.e.size + (m_has_checksum ? 20 : 0) != (unsigned long)fsize)
                                error = true;
                }
        }
        return !error;
}
