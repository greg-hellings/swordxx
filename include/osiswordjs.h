/***************************************************************************
 *
 *  osiswordjs.h -    Implementation of OSISWordJS
 *
 * $Id$
 *
 * Copyright 2005-2013 CrossWire Bible Society (http://www.crosswire.org)
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

#ifndef OSISWORDSJS_H
#define OSISWORDSJS_H

#include <swoptfilter.h>

namespace swordxx {

class SWMgr;
  /** This Filter shows/hides strong's numbers in a OSIS text
  */
class SWDLLEXPORT OSISWordJS : public SWOptionFilter {
     SWModule *defaultGreekLex;
     SWModule *defaultHebLex;
     SWModule *defaultGreekParse;
     SWModule *defaultHebParse;
     SWMgr *mgr;

public:
    OSISWordJS();
    virtual ~OSISWordJS();
    virtual char processText(std::string &text, const SWKey *key = 0, const SWModule *module = 0);
    void setDefaultModules(SWModule *defaultGreekLex = 0, SWModule *defaultHebLex = 0, SWModule *defaultGreekParse = 0, SWModule *defaultHebParse = 0) {
        this->defaultGreekLex   = defaultGreekLex;
        this->defaultHebLex     = defaultHebLex;
        this->defaultGreekParse = defaultGreekParse;
        this->defaultHebParse   = defaultHebParse;
    }
    void setMgr(SWMgr *mgr) { this->mgr = mgr; }
};

} /* namespace swordxx */
#endif
