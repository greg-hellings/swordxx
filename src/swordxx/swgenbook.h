/******************************************************************************
 *
 *  swgenbook.h -    Implementation of SWGenBook
 *
 * $Id$
 *
 * Copyright 2002-2013 CrossWire Bible Society (http://www.crosswire.org)
 *    CrossWire Bible Society
 *    P. O. Box 2528
 *    Tempe, AZ  85280-2528
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */

#ifndef SWGENBOOK_H
#define SWGENBOOK_H

#include "swmodule.h"

#include "defs.h"
#include "keys/treekey.h"


namespace swordxx {

/** The basis for all Book modules.
*/
class SWDLLEXPORT SWGenBook : public SWModule {

protected:
    mutable TreeKey *tmpTreeKey;
    TreeKey & getTreeKey(SWKey * k = nullptr) const;

public:
    /** Initializes data for instance of SWGenBook
    */
    SWGenBook(SWKey * key_,
              char const * imodname = nullptr,
              char const * imoddesc = nullptr,
              SWTextEncoding encoding = ENC_UNKNOWN,
              SWTextDirection dir = DIRECTION_LTR,
              SWTextMarkup markup = FMT_UNKNOWN,
              char const * ilang = nullptr);

    virtual ~SWGenBook();


    // OPERATORS -----------------------------------------------------------------

    SWMODULE_OPERATORS

};

} /* namespace swordxx */
#endif
