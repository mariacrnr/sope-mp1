#include "../include/xmod_aux.h"

double timeElapsed(void) {
    struct timeval time, begin;

    begin.tv_sec = 0;
    begin.tv_usec = 0;

    gettimeofday(&time, NULL);
    
    char* start_str = getenv("BEGIN_TIME");
    sscanf(start_str, "%ld %ld", &begin.tv_sec, &begin.tv_usec);
   

    double time_spent = (time.tv_sec + time.tv_usec / 1e6 - begin.tv_sec - begin.tv_usec / 1e6)*1e3;
    
	return time_spent;
}

void octalToVerb (mode_t perm, char * newPerm) {
    char res[10];
    
    res[0] = (perm & S_IRUSR) ? 'r' : '-';
    res[1] = (perm & S_IWUSR) ? 'w' : '-'; 
    res[2] = (perm & S_IXUSR) ? 'x' : '-'; 
    res[3] = (perm & S_IRGRP) ? 'r' : '-'; 
    res[4] = (perm & S_IWGRP) ? 'w' : '-'; 
    res[5] = (perm & S_IXGRP) ? 'x' : '-'; 
    res[6] = (perm & S_IROTH) ? 'r' : '-';  
    res[7] = (perm & S_IWOTH) ? 'w' : '-'; 
    res[8] = (perm & S_IXOTH) ? 'x' : '-'; 

    res[9] = '\0';
    snprintf(newPerm, sizeof(char)*10, "%s", res);
}

int toOctalMode(mode_t oldPerm, char mode[], mode_t *newPerm){
    mode_t newMask = 0x0;
    int rFlag = 0, wFlag = 0, xFlag = 0;

    for (size_t perm = 2; perm < strlen(mode); perm++) {
        switch (mode[perm]) {
            case 'r':
                if (!rFlag) {
                    newMask |= R_BIT;
                    rFlag = 1;
                    break;
                } else {
                    perror("Invalid arguments");
                    return 1;
                }
            case 'w':
                if (!wFlag) {
                    newMask |= W_BIT;
                    rFlag = 1;
                    wFlag = 1;
                    break;
                } else {
                    perror("Invalid arguments");
                    return 1;
                }
            case 'x':
                if (!xFlag) {
                    newMask |= X_BIT;
                    rFlag = 1;
                    wFlag = 1;
                    xFlag = 1;
                    break;
                } else {
                    perror("Invalid arguments");
                    return 1;
                }
            default:
                perror("Invalid Permissions");
                return 1;
        }
    }
    
    mode_t copyMask = newMask;

    switch (mode[0]) {
        case 'u':
            newMask = newMask << 6;
            break;
        case 'g':
            newMask = newMask << 3;
            break;
        case 'o':
            break;
        case 'a':
            newMask = (copyMask << 6) | (copyMask << 3) | copyMask;
            break;
        default:
            perror("Invalid User");
            return 1;
    }


    switch (mode[1]) {
        case '+':
            newMask |= oldPerm;
            break;
        case '-': 
            newMask = ((~newMask) & oldPerm);
            break;
        case '=':
            switch (mode[0]) {
                case 'u':
                    newMask = ((oldPerm & USER_MASK) | newMask);
                    break;
                case 'g':
                    newMask = ((oldPerm & GROUP_MASK) | newMask);
                    break;
                case 'o':
                    newMask = ((oldPerm & OTHERS_MASK) | newMask);
                    break;
                default:
                    break;
            }
            break;
        default:
            perror("Invalid Operator");
            return 1;
    }

    *newPerm = newMask;

    return 0;
}

