//
// SqModule: API used to communicate with and register squirrel modules
//

//
// Copyright (c) 2009 Brandon Jones
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//  claim that you wrote the original software. If you use this software
//  in a product, an acknowledgment in the product documentation would be
//  appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not be
//  misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source
//  distribution.
//

#if !defined(_SQ_MODULE_H_)
#define _SQ_MODULE_H_

#include "squirrel.h"

#ifdef __cplusplus
extern "C" {
#endif

    /* HSQAPI */
    /*
        Allows modules to interface with squirrel's C api without linking to the squirrel library
        If new functions are added to the Squirrel API, they should be added here too
    */
    typedef struct {
#define SQUIRREL_API_FUNC(fret, fname, fargs) fret (*sq_##fname) fargs;
#include "sqapi.h"
#undef SQUIRREL_API_FUNC
    } sq_api;
    typedef sq_api* HSQAPI;

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*_SQ_MODULE_H_*/
