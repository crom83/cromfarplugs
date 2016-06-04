void WINAPI SaveToLog(const char* FncName, char *buf, long bufsize, BOOL flush, BOOL AddFile, BOOL AddLine)
{   
#ifdef _DEBUG 
    static crntflush = 0;

    _write(logfile, FncName, lstrlen(FncName));
    _write(logfile, ": ", 2);

    _write(logfile, buf, bufsize);    

    char c[50];
    if (AddFile) {
        sprintf(c, "; File: %s ", __FILE__);
        _write(logfile, c, lstrlen(c));
        _write(logfile, " ", 1);
    }

    if (AddLine) {
        sprintf(c, ", line: %d ", __LINE__);
        _write(logfile, c, lstrlen(c));        
    }

    _write(logfile, "\n", lstrlen("\n"));

    if (flush){
        if(crntflush++>flushdelay) _commit(logfile);        
    }
#endif
}