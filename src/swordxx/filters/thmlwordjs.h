/***************************************************************************
 *
 *  thmlwordjs.h -    Implementation of ThMLWordJS
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

#ifndef THMLWORDSJS_H
#define THMLWORDSJS_H

#include "../swoptfilter.h"


namespace swordxx {

class SWMgr;
  /** This Filter shows/hides strong's numbers in a ThML text
  */
class SWDLLEXPORT ThMLWordJS : public SWOptionFilter {
     SWModule *defaultGreekLex;
     SWModule *defaultHebLex;
     SWModule *defaultGreekParse;
     SWModule *defaultHebParse;
     SWMgr *mgr;

public:
    ThMLWordJS();
    ~ThMLWordJS() override;

    char processText(std::string & text,
                         SWKey const * key = nullptr,
                         SWModule const * module = nullptr) override;

    void setDefaultModules(SWModule * defaultGreekLex = nullptr,
                           SWModule * defaultHebLex = nullptr,
                           SWModule * defaultGreekParse = nullptr,
                           SWModule * defaultHebParse = nullptr)
    {
        this->defaultGreekLex   = defaultGreekLex;
        this->defaultHebLex     = defaultHebLex;
        this->defaultGreekParse = defaultGreekParse;
        this->defaultHebParse   = defaultHebParse;
    }
    void setMgr(SWMgr *mgr) { this->mgr = mgr; }
};

} /* namespace swordxx */
#endif
