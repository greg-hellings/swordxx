/******************************************************************************
 *
 *  teilatex.h -    Implementation of TEILaTeX
 *
 * $Id$
 *
 * Copyright 2012-2014 CrossWire Bible Society (http://www.crosswire.org)
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

#ifndef TEILATEX_H
#define TEILATEX_H

#include "../swbasicfilter.h"


namespace swordxx {

/** this filter converts TEI text to LaTeX text
 */
class SWDLLEXPORT TEILaTeX : public SWBasicFilter {
private:
    bool renderNoteNumbers;

protected:
    class MyUserData : public BasicFilterUserData {
    public:
        bool BiblicalText;
        std::string lastHi;
        bool firstCell; // for tables, indicates whether a cell is the first one in a row
        int consecutiveNewlines;

        std::string version;
        MyUserData(const SWModule *module, const SWKey *key);
    };

    BasicFilterUserData * createUserData(SWModule const * module,
                                         SWKey const * key) override
    { return new MyUserData(module, key); }

    bool handleToken(std::string & buf,
                     char const * token,
                     BasicFilterUserData * userData) override;

public:
    TEILaTeX();
    void setRenderNoteNumbers(bool val = true) { renderNoteNumbers = val; }
};

} /* namespace swordxx */
#endif
