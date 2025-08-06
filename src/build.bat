@echo off
gcc main.c gsf/gsf.c gsf/gsf_dec.c gsf/gsf_enc.c gsf/gsf_compress.c gsf/gsf_info.c gsf/gsf_indx.c gsf/gsf_geo.c -o check_gsf.exe -lws2_32