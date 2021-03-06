/******************************************************************************
 *
 *  imp2ld.cpp -    Utility to import LD modules in IMP format
 *
 * $Id$
 *
 * Copyright 2002-2014 CrossWire Bible Society (http://www.crosswire.org)
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
#include <fstream>
#include <iostream>
#include <string>
#include <swordxx/modules/lexdict/rawld.h>
#include <swordxx/modules/lexdict/rawld4.h>
#include <swordxx/modules/lexdict/zld.h>
#include <swordxx/modules/common/lzsscomprs.h>
#include <swordxx/modules/common/zipcomprs.h>
#include <swordxx/modules/common/bz2comprs.h>
#include <swordxx/modules/common/xzcomprs.h>
#include <vector>


using std::string;

using namespace swordxx;

void usage(char const * progName, char const * error = nullptr) {
    if (error) fprintf(stderr, "\n%s: %s\n", progName, error);
    fprintf(stderr, "\n=== imp2ld (Revision $Rev$) Sword++ lexicon importer.\n");
    fprintf(stderr, "\nusage: %s <imp_file> [options]\n", progName);
    fprintf(stderr, "  -a\t\t\t augment module if exists (default is to create new)\n");
    fprintf(stderr, "  -z <l|z|b|x>\t\t use compression (default: none)\n");
    fprintf(stderr, "\t\t\t\t l - LZSS; z - ZIP; b - bzip2; x - xz\n");
    fprintf(stderr, "  -o <output_path>\t\t where to write data files.\n");
    fprintf(stderr, "  -4\t\t\t use 4 byte size entries (default: 2).\n");
    fprintf(stderr, "  -b <entry_count>\t\t compression block size (default 30 entries)\n");
    fprintf(stderr, "  -s\t\t\t case sensitive keys (default is not case sensitive)\n");
    fprintf(stderr, "  -P\t\t\t disable key Strong's number padding (by default keys will be padded).");
    fprintf(stderr, "\n");
    fprintf(stderr, "'imp' format is a simple standard for importing data into Sword++ modules.\n"
        "Required is a plain text file containing $$$key lines followed by content.\n\n"
        "$$$Abraham\n"
        "Abraham was the father of Isaac...\n"
        "He was called by God to leave his country and journey to the land of Canaan...\n"
        "$$$Isaac\n"
        "Isaac was the son of Abraham and Sarah...\n\n");
    exit(-1);
}

int main(int argc, char **argv) {

    std::vector<string> linkbuffer;
    signed long i = 0;
    string keybuffer;
    string entbuffer;
    string linebuffer;
    char links = 0;
    string modname;
    std::string outPath          = "";
    bool append            = false;
    long blockCount = 30;
    bool caseSensitive = false;
    SWCompress * compressor = nullptr;
    std::string compType         = "";
    bool fourByteSize      = false;
    bool strongsPadding    = true;

    if (argc < 2) usage(*argv);

    const char *progName   = argv[0];
    const char *inFileName = argv[1];

    for (int i = 2; i < argc; i++) {
        if (!strcmp(argv[i], "-a")) {
            append = true;
        }
        else if (!strcmp(argv[i], "-z")) {
            if (fourByteSize) usage(*argv, "Cannot specify both -z and -4");
            compType = "ZIP";
            if (i+1 < argc && argv[i+1][0] != '-') {
                switch (argv[++i][0]) {
                case 'l': compType = "LZSS"; break;
                case 'z': compType = "ZIP"; break;
                case 'b': compType = "BZIP2"; break;
                case 'x': compType = "XZ"; break;
                }
            }
        }
        else if (!strcmp(argv[i], "-Z")) {
            if (compType.size()) usage(*argv, "Cannot specify both -z and -Z");
            if (fourByteSize) usage(*argv, "Cannot specify both -Z and -4");
            compType = "LZSS";
        }
        else if (!strcmp(argv[i], "-4")) {
            fourByteSize = true;
        }
        else if (!strcmp(argv[i], "-P")) {
            strongsPadding = false;
        }
        else if (!strcmp(argv[i], "-b")) {
            if (i+1 < argc) {
                blockCount = atoi(argv[++i]);
                if (blockCount > 0) continue;
            }
            usage(*argv, "-b requires in entry count integer > 0");
        }
        else if (!strcmp(argv[i], "-o")) {
            if (i+1 < argc) outPath = argv[++i];
            else usage(progName, "-o requires <output_path>");
        }
        else if (!strcmp(argv[i], "-s")) {
            caseSensitive = true;
        }
        else usage(progName, (((std::string)"Unknown argument: ")+ argv[i]).c_str());
    }



    if (outPath.size() < 1) {
        for (i = 0; (i < 16) && (inFileName[i]) && (inFileName[i] != '.'); i++) {
            outPath += inFileName[i];
        }
    }

    std::ifstream infile(inFileName);
    if (!infile.is_open()) {
        fprintf(stderr, "\nERROR: %s: could not open file for reading: %s\n\n", *argv, inFileName);
        exit(-2);
    }

    SWModule * mod = nullptr;
    SWKey *key, *linkKey;

    if (compType == "LZSS") {
        compressor = new LZSSCompress();
    }
    else if (compType == "ZIP") {
        compressor = new ZipCompress();
    }
    else if (compType == "BZIP2") {
        compressor = new Bzip2Compress();
    }
    else if (compType == "XZ") {
        compressor = new XzCompress();
    }


    // setup module
    if (!append) {
        if (compressor) {
            if (zLD::createModule(outPath.c_str())) {
                fprintf(stderr, "ERROR: %s: couldn't create module at path: %s \n", *argv, outPath.c_str());
                exit(-1);
            }
        }
        else {
            if (!fourByteSize)
                RawLD::createModule(outPath.c_str());
            else    RawLD4::createModule(outPath.c_str());
        }
    }

    if (compressor) {
        // Create a compressed text module allowing very large entries
        // Taking defaults except for first, fourth, fifth and last argument
        mod = new zLD(outPath.c_str(), nullptr, nullptr, blockCount, compressor, ENC_UNKNOWN, DIRECTION_LTR, FMT_UNKNOWN, nullptr, caseSensitive, strongsPadding);
    }
    else {
        mod = (!fourByteSize)
            ? (SWModule *)new RawLD (outPath.c_str(), nullptr, nullptr, ENC_UNKNOWN, DIRECTION_LTR, FMT_UNKNOWN, nullptr, caseSensitive, strongsPadding)
            : (SWModule *)new RawLD4(outPath.c_str(), nullptr, nullptr, ENC_UNKNOWN, DIRECTION_LTR, FMT_UNKNOWN, nullptr, caseSensitive, strongsPadding);
    }




    key = mod->createKey();
    linkKey = mod->createKey();
    key->setPersist(true);
    mod->setKey(key);

    while (!infile.eof()) {
        std::getline(infile, linebuffer);
        if (linebuffer.size() > 3 && linebuffer.substr(0,3) == "$$$") {
            if (keybuffer.size() && entbuffer.size()) {
                std::cout << keybuffer << std::endl;
                *key = keybuffer.c_str();

                mod->setEntry(entbuffer.c_str(), entbuffer.size());
                for (i = 0; i < links; i++) {
                    std::cout << "Linking: " << linkbuffer[i] << std::endl;
                    *linkKey = linkbuffer[i].c_str();
                    mod->linkEntry(linkKey);
                }
            }
            if (linebuffer.size() > 3)
                keybuffer = linebuffer.substr(3,linebuffer.size());

            entbuffer.resize(0);
            linkbuffer.clear();
            links = 0;
        }
        else if (linebuffer.size() > 3 && linebuffer.substr(0,3) == "%%%") {
            linkbuffer.push_back(linebuffer.substr(3,linebuffer.size()));
            links++;
        }
        else {
            entbuffer += linebuffer;
        }
    }

    //handle final entry
    if (keybuffer.size() && entbuffer.size()) {
        std::cout << keybuffer << std::endl;
        *key = keybuffer.c_str();

        mod->setEntry(entbuffer.c_str(), entbuffer.size());
        for (i = 0; i < links; i++) {
            std::cout << "Linking: " << linkbuffer[i] << std::endl;
            *linkKey = linkbuffer[i].c_str();
            mod->linkEntry(linkKey);
        }
    }

    infile.close();

    delete linkKey;
    delete mod;
    delete key;

    return 0;
}
