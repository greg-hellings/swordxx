/******************************************************************************
 *
 *  rawgenbook.h -    code for class 'RawGenBook'- a module that reads raw
 *            text files:
 *            ot and nt using indexs ??.bks ??.cps ??.vss
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

#ifndef RAWGENBOOK_H
#define RAWGENBOOK_H

#include "../../swgenbook.h"

#include "../../defs.h"


namespace swordxx {

class FileDesc;

class SWDLLEXPORT RawGenBook : public SWGenBook {
    char *path;
    FileDesc *bdtfd;
    bool verseKey;

public:


    RawGenBook(char const * ipath,
               char const * iname = nullptr,
               char const * idesc = nullptr,
               SWTextEncoding encoding = ENC_UNKNOWN,
               SWTextDirection dir = DIRECTION_LTR,
               SWTextMarkup markup = FMT_UNKNOWN,
               char const * ilang = nullptr,
               char const * keyType = "TreeKey");
    ~RawGenBook() override;

    std::string & getRawEntryBuf() const override;
    bool isWritable() const override;
    static char createModule(const char *ipath);
    void setEntry(char const * inbuf, long len = -1) override;
    void linkEntry(SWKey const * linkKey) override;
    void deleteEntry() override;
    SWKey * createKey() const override;

    bool hasEntry(SWKey const * k) const override;

    SWMODULE_OPERATORS

};

} /* namespace swordxx */
#endif
