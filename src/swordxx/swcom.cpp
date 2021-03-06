/******************************************************************************
 *
 *  swcom.cpp -    code for base class 'SWCom'- The basis for all commentary
 *        modules
 *
 * $Id$
 *
 * Copyright 1997-2013 CrossWire Bible Society (http://www.crosswire.org)
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

#include "swcom.h"

#include "keys/versekey.h"
#include "localemgr.h"
#include "utilstr.h"


namespace swordxx {

namespace {

SWKey * staticCreateKey(char const * const versification) {
    VerseKey * const vk = new VerseKey();
    vk->setVersificationSystem(versification);
    return vk;
}

} // anonymous namespace


/******************************************************************************
 * SWCom Constructor - Initializes data for instance of SWCom
 *
 * ENT:    imodname - Internal name for module
 *    imoddesc - Name to display to user for module
 */

SWCom::SWCom(const char *imodname, const char *imoddesc, SWTextEncoding enc, SWTextDirection dir, SWTextMarkup mark, const char *ilang, const char *versification): SWModule(staticCreateKey(versification), imodname, imoddesc, "Commentaries", enc, dir, mark, ilang) {
    this->versification = nullptr;
    stdstr(&(this->versification), versification);
    tmpVK1 = (VerseKey *)createKey();
    tmpVK2 = (VerseKey *)createKey();
        tmpSecond = false;
}


/******************************************************************************
 * SWCom Destructor - Cleans up instance of SWCom
 */

SWCom::~SWCom() {
    delete tmpVK1;
    delete tmpVK2;
    delete [] versification;
}


SWKey * SWCom::createKey() const { return staticCreateKey(versification); }


long SWCom::getIndex() const {
    VerseKey *key = &getVerseKey();
    entryIndex = key->getIndex();
    return entryIndex;
}

void SWCom::setIndex(long iindex) {
    VerseKey *key = &getVerseKey();
    key->setTestament(1);
    key->setIndex(iindex);

    if (key != this->key) {
        this->key->copyFrom(*key);
    }
}


VerseKey &SWCom::getVerseKey(const SWKey *keyToConvert) const {
    SWKey const * tmp = keyToConvert ? keyToConvert : this->key;
    /// \bug remove const_cast:
    SWKey * thisKey = const_cast<SWKey *>(tmp);

    if (VerseKey * const key = dynamic_cast<VerseKey *>(thisKey))
        return *key;
    if (ListKey * const lkTest = dynamic_cast<ListKey *>(thisKey))
        if (VerseKey * const key =
                dynamic_cast<VerseKey *>(lkTest->getElement()))
            return *key;

    VerseKey * retKey = (tmpSecond) ? tmpVK1 : tmpVK2;
    tmpSecond = !tmpSecond;
    retKey->setLocale(LocaleMgr::getSystemLocaleMgr()->getDefaultLocaleName());
    (*retKey) = *thisKey;
    return (*retKey);
}


} /* namespace swordxx */
