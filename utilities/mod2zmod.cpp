/******************************************************************************
 *
 *  mod2zmod.cpp -    Compression on variable granularity
 *
 * $Id$
 *
 * Copyright 2000-2014 CrossWire Bible Society (http://www.crosswire.org)
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

#ifdef _MSC_VER
    #pragma warning( disable: 4251 )
#endif

#include <cstdio>
#include <fcntl.h>
#include <fstream>
#ifndef __GNUC__
#include <io.h>
#endif
#include <iostream>
#include <string>
#include <swordxx/filters/cipherfil.h>
#include <swordxx/keys/versekey.h>
#include <swordxx/modules/comments/zcom.h>
#include <swordxx/modules/common/bz2comprs.h>
#include <swordxx/modules/common/lzsscomprs.h>
#include <swordxx/modules/common/xzcomprs.h>
#include <swordxx/modules/common/zipcomprs.h>
#include <swordxx/modules/lexdict/zld.h>
#include <swordxx/modules/texts/ztext.h>
#include <swordxx/swmgr.h>
#include <swordxx/swtext.h>
#ifdef __GNUC__
#include <unistd.h>
#endif


using namespace swordxx;

using std::cerr;
using std::string;
using std::cout;

void errorOutHelp(char *appName) {
    cerr << appName << " - a tool to create compressed Sword++ modules\n";
    cerr << "version 0.1\n\n";
    cerr << "usage: "<< appName << " <modname> <datapath> [blockType [compressType [compressLevel [cipherKey]]]]\n\n";
    cerr << "datapath: the directory in which to write the zModule\n";
    cerr << "blockType  : (default 4)\n\t2 - verses\n\t3 - chapters\n\t4 - books\n";
    cerr << "compressType: (default 1):\n\t1 - LZSS\n\t2 - Zip\n\t3 - bzip2\n\t4 - xz\n";
    cerr << "compressLevel: (default varies by compressType):\n\tA digit from 1-9. Greater values compress more, but require more\n\ttime/memory. Use 0 for the default compression level.\n";
    cerr << "\n\n";
    exit(-1);
}


int main(int argc, char **argv)
{
    int iType = 4;
    int compType = 1;
    string cipherKey = "";
    SWCompress * compressor = nullptr;
    SWModule * inModule     = nullptr;
    SWModule * outModule    = nullptr;
    int compLevel = 0;

    if ((argc < 3) || (argc > 7)) {
        errorOutHelp(argv[0]);
    }

    if (argc > 3) {
        iType = atoi(argv[3]);
        if (argc > 4) {
            compType = atoi(argv[4]);
            if (argc > 5) {
                compLevel = atoi(argv[5]);
                if (argc > 6) {
                    cipherKey = argv[6];
                }
            }
        }
    }

    if ((iType < 2) || (compType < 1) || (compType > 4) || compLevel < 0 || compLevel > 9 || (!strcmp(argv[1], "-h")) || (!strcmp(argv[1], "--help")) || (!strcmp(argv[1], "/?")) || (!strcmp(argv[1], "-?")) || (!strcmp(argv[1], "-help"))) {
        errorOutHelp(argv[0]);
    }

    SWMgr mgr;

    ModMap::iterator it = mgr.Modules.find(argv[1]);
    if (it == mgr.Modules.end()) {
        fprintf(stderr, "error: %s: couldn't find module: %s\n", argv[0], argv[1]);
        exit(-2);
    }

    inModule = it->second;

    // Try to initialize a default set of datafiles and indicies at our
    // datapath location passed to us from the user.

#define BIBLE 1
#define LEX 2
#define COM 3

    int modType = 0;
    if (!strcmp(inModule->getType(), "Biblical Texts")) modType = BIBLE;
    if (!strcmp(inModule->getType(), "Lexicons / Dictionaries")) modType = LEX;
    if (!strcmp(inModule->getType(), "Commentaries")) modType = COM;

    switch (compType) {    // these are deleted by zText
    case 1: compressor = new LZSSCompress(); break;
    case 2: compressor = new ZipCompress(); break;
    case 3: compressor = new Bzip2Compress(); break;
    case 4: compressor = new XzCompress(); break;
    }
    if (compressor && compLevel > 0) {
        compressor->setLevel(compLevel);
    }

    int result = 0;
    switch (modType) {
    case BIBLE:
    case COM: {
        SWKey *k = inModule->getKey();
        VerseKey *vk = dynamic_cast<VerseKey *>(k);
        result = zText::createModule(argv[2], iType, vk->getVersificationSystem());
        break;
    }
    case LEX:
        result = zLD::createModule(argv[2]);
        break;
    }

    if (result) {
        fprintf(stderr, "error: %s: couldn't create module at path: %s\n", argv[0], argv[2]);
        exit(-3);
    }

    switch (modType) {
    case BIBLE:
    case COM: {
        SWKey *k = inModule->getKey();
        VerseKey *vk = dynamic_cast<VerseKey *>(k);
        outModule = new zText(argv[2], nullptr, nullptr, iType, compressor,
            ENC_UNKNOWN, DIRECTION_LTR, FMT_UNKNOWN, nullptr,
            vk->getVersificationSystem());    // open our datapath with our RawText driver.
        ((VerseKey *)(SWKey *)(*inModule))->setIntros(true);
        break;
    }
    case LEX:
        outModule = new zLD(argv[2], nullptr, nullptr, iType, compressor);        // open our datapath with our RawText driver.
        break;
    }

    SWFilter * cipherFilter = nullptr;
    if (!cipherKey.empty()) {
        cipherFilter = new CipherFilter(cipherKey.c_str());
        outModule->addRawFilter(cipherFilter);
    }

    string lastBuffer = "Something that would never be first module entry";
    SWKey bufferKey;
    SWKey lastBufferKey;
    SWKey *outModuleKey = outModule->createKey();
    VerseKey *vkey = dynamic_cast<VerseKey *>(outModuleKey);
    outModuleKey->setPersist(true);
    if (vkey) {
        vkey->setIntros(true);
        vkey->setAutoNormalize(false);
    }
    outModule->setKey(*outModuleKey);

    inModule->setSkipConsecutiveLinks(false);
    (*inModule) = TOP;
    while (!inModule->popError()) {
        bufferKey = *(SWKey *)(*inModule);
        // pseudo-check for link.  Will get most common links.
        if ((lastBuffer == inModule->getRawEntry()) &&(lastBuffer.length() > 0)) {
            *outModuleKey = bufferKey;
            outModule->linkEntry(&lastBufferKey);    // link to last key
        cout << "Adding [" << bufferKey << "] link to: [" << lastBufferKey << "]\n";
        }
        else {
            lastBuffer = inModule->getRawEntry();
            lastBufferKey = inModule->getKeyText();
            if (lastBuffer.length() > 0) {
                cout << "Adding [" << bufferKey << "] new text.\n";
                *outModuleKey = bufferKey;
//                outModule->getRawEntry();    // snap
//                outModule->setKey(bufferKey);
                (*outModule) << lastBuffer.c_str();    // save new text;
            }
            else {
                    cout << "Skipping [" << bufferKey << "] no entry in Module.\n";
            }
        }
        (*inModule)++;
    }
    delete outModule;
    delete outModuleKey;
    if (cipherFilter)
        delete cipherFilter;
}

