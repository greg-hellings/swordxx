/******************************************************************************
 *
 *  zipcomprs.h -    definition of Class ZipCompress used for data
 *            compression
 *
 * $Id$
 *
 * Copyright 2000-2013 CrossWire Bible Society (http://www.crosswire.org)
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

#ifndef ZIPCOMPRS_H
#define ZIPCOMPRS_H

#include "swcomprs.h"

#include "../../defs.h"


namespace swordxx {

class SWDLLEXPORT ZipCompress : public SWCompress {

protected:
public:
    ZipCompress();
    ~ZipCompress() override;

    void Encode() override;
    void Decode() override;
};

} /* namespace swordxx */
#endif
