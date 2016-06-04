#ifndef CC_FILE_CPP
#define CC_FILE_CPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "stdafx.h"
#include "riff_file.cpp"
#include "avi_file.cpp"
#include "wav_file.cpp"
#include "bink_file.cpp"
#include "jpeg_file.cpp"
#include "aud_file.h"
#include "bin_file.h"
#include "cps_file.h"
#include "fnt_file.h"
#include "hva_file.h"
#include "mix_file.cpp"
/*#include "mp3_file.h"*/
#include "ogg_file.h"
#include "pal_file.h"
#include "pcx_file.h"
#include "png_file.h"
#include "shp_dune2_file.h"
/*#include "shp_file.h"
#include "shp_ts_file.h"
#include "st_file.h"
#include "string_conversion.h"
#include "text_file.h"
#include "tmp_file.h"
#include "tmp_ra_file.h"
#include "tmp_ts_file.h"
#include "voc_file.h"*/
#include "vqa_file.h"
#include "vqp_file.h"
#include "vxl_file.h"
#include "wsa_dune2_file.h"
/*#include "wsa_file.h"
#include "xif_file.h"*/

enum t_file_type
{
        ft_ai_ini_ts, ft_ai_ini_ra2, ft_art_ini_ts, ft_art_ini_ra2, ft_aud, ft_avi, ft_bin, ft_bink, ft_bmp, ft_clipboard, ft_cps, ft_csv, ft_dir, ft_drive, ft_fnt, ft_html, ft_hva, ft_ini,
        ft_jpeg, ft_map_dune2, ft_map_td, ft_map_ra, ft_map_ts, ft_map_ts_preview, ft_map_ra2, ft_mix, ft_mng, ft_mp3, ft_mrf, 
        ft_ogg, ft_pak, ft_pal, ft_pal_jasc, ft_pcx_single, ft_pcx, ft_png_single, ft_png, ft_pkt_ts, ft_riff, ft_rules_ini_ts, ft_rules_ini_ra2, ft_shp_dune2, ft_shp, ft_shp_ts, 
        ft_sound_ini_ts, ft_sound_ini_ra2, ft_st, ft_text, ft_theme_ini_ts, ft_theme_ini_ra2, ft_tmp, ft_tmp_ra, ft_tmp_ts, ft_voc, ft_vpl, ft_vqa, ft_vqp, ft_vxl, ft_wav, 
        ft_wav_ima_adpcm, ft_wav_pcm, ft_wsa_dune2, ft_wsa, ft_xcc_lmd, ft_xcc_unknown, ft_xif, ft_zip, ft_unknown, ft_count
};


t_file_type get_file_type(unsigned char* data, long size, char* ext, long fsize)
{
        t_file_type ft = ft_unknown;        

        if (aud_is_valid(data, size, fsize)){
                ft = ft_aud;
                strcpy(ext, "aud");}
        else if (bin_is_valid(data, size, fsize)){
                ft = ft_bin;
                strcpy(ext, "bin");}
        else if (BINK_is_valid(data, size, fsize)){
                ft = ft_bink;
                strcpy(ext, "bik");}
        else if (cps_is_valid(data, size, fsize)){
                ft = ft_cps;
                strcpy(ext, "cps");}
        else if (fnt_is_valid(data, size, fsize)){
                ft = ft_fnt;
                strcpy(ext, "fnt");}
        else if (hva_is_valid(data, size, fsize)){
                ft = ft_hva;
                strcpy(ext, "hva");}
        else /*if (mp3_f.load(data, size), mp3_f.is_valid()){
                ft = ft_mp3;
                strcpy(ext, "mp3")}
        else */if (jpeg_is_valid(data, size, fsize)){
                ft = ft_jpeg;
                strcpy(ext, "jpeg");}
        else if (ogg_is_valid(data, size, fsize)){
                ft = ft_ogg;
                strcpy(ext, "ogg");}
        else if (pal_is_valid(data, size, fsize)){
                ft = ft_pal;
                strcpy(ext, "pal");}
        else if (pcx_is_valid(data, size, fsize)){
                ft = ft_pcx;
                strcpy(ext, "pcx");}
        else if (png_is_valid(data, size, fsize)){
                ft = ft_png;
                strcpy(ext, "png");}
        else if (RIFF_is_valid(data, size, fsize)){
                if (AVI_is_valid(data, size, fsize)){
                    ft = ft_avi;
                    strcpy(ext, "avi");
                } else {
                    if (WAV_is_valid(data, size, fsize)){
                        ft = ft_wav;
                        strcpy(ext, "wav");
                    } else{
                        ft = ft_riff;
                        strcpy(ext, "riff");
                    }
                }
               }
        else if (shp_d2_is_valid(data, size, fsize)){
                ft = ft_shp_dune2;
                strcpy(ext, "shp");}
        else/* if (shp_f.load(data, size), shp_f.is_valid()){
                ft = ft_shp;
                strcpy(ext, "shp")}
        else if (shp_ts_f.load(data, size), shp_ts_f.is_valid()){
                ft = ft_shp_ts;
                strcpy(ext, "shp")}
        else if (st_f.load(data, size), st_f.is_valid()){
                ft = ft_st;
                strcpy(ext, "st")}
        else if (text_f.load(data, size), text_f.is_valid()){
                ft = ft_text;
                strcpy(ext, "text")}
        else if (tmp_f.load(data, size), tmp_f.is_valid()){
                ft = ft_tmp;
                strcpy(ext, "tmp")}
        else if (tmp_ra_f.load(data, size), tmp_ra_f.is_valid()){
                ft = ft_tmp_ra;
                strcpy(ext, "tmp")}
        else if (tmp_ts_f.load(data, size), tmp_ts_f.is_valid()){
                ft = ft_tmp_ts;
                strcpy(ext, "tmp")}
        else if (voc_f.load(data, size), voc_f.is_valid()){
                ft = ft_voc;
                strcpy(ext, "voc")}
        else */if (vqa_is_valid(data, size, fsize)){
                ft = ft_vqa;
                strcpy(ext, "vqa");}
        else if (vqp_is_valid(data, size, fsize)){
                ft = ft_vqp;
                strcpy(ext, "vqp");}
        else if (vxl_is_valid(data, size, fsize)){
                ft = ft_vxl;
                strcpy(ext, "vxl");}
        else if (wsa_d2_is_valid(data, size, fsize)){
                ft = ft_wsa_dune2;
                strcpy(ext, "wsa");}
        else /*if (wsa_f.load(data, size), wsa_f.is_valid()){
                ft = ft_wsa;
                strcpy(ext, "wsa")}
        else if (xif_f.load(data, size), xif_f.is_valid()){
                ft = ft_xif;
                strcpy(ext, "xif")}
        else */if (mix_is_valid(data, size, fsize)){
                ft = ft_mix;
                strcpy(ext, "mix");}        
        return ft;
}

#endif