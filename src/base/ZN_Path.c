#include "zetnet.h"

#ifndef MAX_FILENAME_LENGTH
#define MAX_FILENAME_LENGTH 100
#endif

#ifndef MAX_PATH_LENGTH
#define MAX_PATH_LENGTH 2096
#endif

#ifndef MAX_ABSOLUTE_FILENAME_LENGTH
#define MAX_ABSOLUTE_FILENAME_LENGTH (MAX_FILENAME_LENGTH+MAX_PATH_LENGTH)
#endif

bool ZN_Path_GetDirectoryName(char *dst, size_t dst_size, const char *absolute_filename) {
    const char *last_slash = NULL;
    const char *p = NULL;
    size_t len = 0;

    if (!dst || dst_size == 0 || !absolute_filename) {
        return false;
    }

    dst[0] = '\0';

    if (absolute_filename[0] == '\0') {
        return false;
    }

    //  Ignore trailing slash/backslash.
	//
    //    Example:
    //        "/home/user/" -> directory should become "/home/user",
    //        not "/home/user/".
    p = absolute_filename + strlen(absolute_filename);

    while (p > absolute_filename && (p[-1] == '/' || p[-1] == '\\')) {
        p--;
    }

    if (p == absolute_filename) {
        //    Path was only "/" or "\\".
        if (dst_size < 2) {
            return false;
        }

        dst[0] = absolute_filename[0];
        dst[1] = '\0';
        return true;
    }

    //  Find last slash before p.
    {
        const char *q = p;

        while (q > absolute_filename) {
            q--;

            if (*q == '/' || *q == '\\') {
                last_slash = q;
                break;
            }
        }
    }

    if (!last_slash) {
        //    No directory part.
        return true;
    }


    //    For "/file.txt", directory is "/".
    //    For "C:\\file.txt", directory is "C:\\".
    if (last_slash == absolute_filename) {
        len = 1;
    } else if ((last_slash == absolute_filename + 2) &&
               absolute_filename[1] == ':' &&
               (absolute_filename[2] == '\\' || absolute_filename[2] == '/')) {
        len = 3;
    } else {
        len = (size_t)(last_slash - absolute_filename);
    }

    if (len + 1 > dst_size) {
        return false;
    }

    memcpy(dst, absolute_filename, len);
    dst[len] = '\0';

    return true;
}

bool ZN_Path_GetFileName(char *dst, size_t dst_size, const char *absolute_filename) {
    const char *start = NULL;
    const char *end = NULL;
    const char *p = NULL;
    size_t len = 0;

    if (!dst || dst_size == 0 || !absolute_filename) {
        return false;
    }

    dst[0] = '\0';

    if (absolute_filename[0] == '\0') {
        return false;
    }

    end = absolute_filename + strlen(absolute_filename);


    //    Ignore trailing slash/backslash.
    //
    //    Example:
    //        "/home/user/" -> filename is "".
    //
    while (end > absolute_filename && (end[-1] == '/' || end[-1] == '\\')) {
        end--;
    }

    if (end == absolute_filename) {
        return true;
    }

    start = absolute_filename;

    p = end;
    while (p > absolute_filename) {
        p--;

        if (*p == '/' || *p == '\\') {
            start = p + 1;
            break;
        }
    }

    len = (size_t)(end - start);

    if (len + 1 > dst_size) {
        return false;
    }

    memcpy(dst, start, len);
    dst[len] = '\0';

    return true;
}





