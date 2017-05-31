#ifndef __READ_WRITE_CONF_H__
#define __READ_WRITE_CONF_H__



#define CFG_FILE "./cmmb_dtmb_mux.cfg"



int read_profile_string(const char *section, const char *key, char *value, int size, const char *default_value, const char *file);
int read_profile_int(const char *section, const char *key, int default_value, const char *file);
int write_profile_string(const char *section, const char *key, const char *value, const char *file);
int GetPrivateProfileInt(const char *section, const char *key, int default_value, const char* file);
int GetPrivateProfileString(const char *section, const char *key, const char *default_value, char *value, int size, const char* file);



#endif
