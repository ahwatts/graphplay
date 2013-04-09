#ifndef _DAE_FILE_H_
#define _DAE_FILE_H_

class DaeFile
{
public:
    DaeFile(void);
    ~DaeFile(void);

    static DaeFile* loadDaeFile(const char* filename);
};

#endif
