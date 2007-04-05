/*
 * Copyright 2005-2007 Andrew De Ponte
 * 
 * This file is part of lib_zdtm_sync.
 * 
 * lib_zdtm_sync is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or any
 * later version.
 * 
 * lib_zdtm_sync is distributed in the hopes that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with lib_zdtm_sync; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

/**
 * @file zdtm_export.h
 * @brief This is a specifications file for win32 export macros.
 *
 * The zdtm_export.h file is basically a file which defines macros which
 * are the appropriate values for either exporting / importing / neither
 * depending on which platform the build is for. This has to exist to be
 * able to properly create win32 DLLs.
 */

#ifndef ZDTM_EXPORT_H
#define ZDTM_EXPORT_H

#ifdef DLL_EXPORT
    #define ZDTM_EXPORT __declspec(dllexport)
#else
    #ifdef LIBZDTM_DLL_IMPORT
        #define ZDTM_EXPORT __declspec(dllimport)
    #endif
#endif
#ifndef ZDTM_EXPORT
    #define ZDTM_EXPORT 
#endif

#endif
