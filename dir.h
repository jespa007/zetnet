//    Copyright (C) 2007 Jordi Espada Brau (jespa003@hotmail.com)
//    Free Software Foundation, Inc.  Contributed by Gustawo Patow Ariel
//
//    This file is part of iFileSystem Tool.
//
//    iFileSystem Tool is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    iFileSystem Tool is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with iFileSystem Tool; ; see the file COPYING. if not, write to 
//    the Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
//    Boston, MA  02111-1307  USA

#ifndef __DIR_H__
#define __DIR_H__

#include <stdio.h>

#ifdef __linux__
extern "C"  {
#include "findfirst.h"
}
#else
#ifdef WIN32
#include <io.h>
#include <direct.h>
#endif
#endif




class tDir;

typedef void (tDir::*tDirVoidVoidFunction)(void);
typedef bool (tDir::*tDirBoolVoidFunction)(void);

class tDir{
private:
//    void *struct_block_dir;
    struct _finddata_t c_file;
   intptr_t hFile;

    char name[200];
    bool end;
public:
 /*   tDirBoolVoidFunction read_first;
    tDirVoidVoidFunction read_next;
    tDirBoolVoidFunction done;*/



    bool read_first(const char *dir)
    {

        char ini_dir[200];
        sprintf(ini_dir,"%s/*.*",dir);
        
        if((hFile = _findfirst(ini_dir, &c_file)) == -1L)
        {
            printf("No subdirectories in current directory!\n");
            return false;
        }

        end = false;
        strcpy(name,c_file.name);

        return true;
    }

    void read_next()
    {
        end = (_findnext(hFile, &c_file) != 0);

        if(!end) strcpy(name,c_file.name);

    }

    bool done()
    {
    	return end;
    }

    void close()
    {
        _findclose(hFile);



    }


private:

    enum{
        UNIX_PLATAFORM = 0,
        WIN32_PLATAFORM,

    };

    //------------------------------------------------------------------
public:

    tDir()
    {
        end = true;
    }

    // generic functions
    bool IsDir() // if not is archive!
    {   
        if(!end)
            return ((c_file.attrib & _A_SUBDIR ) != 0);
        
        return false;
    }
    //------------------------------------------------------------------
    // generic functions
/*    bool done() // if not is archive!
    {   
        return false;
    }
    //------------------------------------------------------------------*/
    const char *GetName(void)
    {
        return (const char *)name;
    }
    //------------------------------------------------------------------

};

#endif
