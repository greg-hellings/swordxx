/******************************************************************************
 *
 *  latin1utf16.cpp -    SWFilter descendant Latin1UTF16 to convert a Latin-1
 *            character to UTF-16
 *
 * $Id$
 *
 * Copyright 2001-2013 CrossWire Bible Society (http://www.crosswire.org)
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

#include <stdlib.h>
#include <stdio.h>
#include <latin1utf16.h>
#include <string>

namespace swordxx {

Latin1UTF16::Latin1UTF16() {
}


char Latin1UTF16::processText(std::string &text, const SWKey *key, const SWModule *module) {
    const unsigned char *from;
     if ((unsigned long)key < 2)    // hack, we're en(1)/de(0)ciphering
        return (char)-1;


    std::string orig = text;
    from = (const unsigned char *)orig.c_str();

    for (text = ""; *from; from++) {
        text.resize(text.size() + 2u, '\0');
       switch (*from) {
    case 0x80: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x20AC;
        break;
    case 0x82: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x201A;
        break;
    case 0x83: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x0192;
        break;
    case 0x84: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x201E;
        break;
    case 0x85: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x2026;
        break;
    case 0x86: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x2020;
        break;
    case 0x87: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x2021;
        break;
    case 0x88: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x02C6;
        break;
    case 0x89: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x2030;
        break;
    case 0x8A: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x0160;
        break;
    case 0x8B: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x2039;
        break;
    case 0x8C: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x0152;
        break;
    case 0x8E: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x017D;
        break;
    case 0x91: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x2018;
        break;
    case 0x92: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x2019;
        break;
    case 0x93: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x201C;
        break;
    case 0x94: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x201D;
        break;
    case 0x95: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x2022;
        break;
    case 0x96: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x2013;
        break;
    case 0x97: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x2014;
        break;
    case 0x98: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x02DC;
        break;
    case 0x99: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x2122;
        break;
    case 0x9A: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x0161;
        break;
    case 0x9B: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x203A;
        break;
    case 0x9C: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x0153;
        break;
    case 0x9E: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x017E;
        break;
    case 0x9F: // '�'
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) 0x0178;
        break;
       default:
        *((unsigned short *) &text[text.size() - 2u]) = (unsigned short) *from;
       }
    }
    return 0;
}

} /* namespace swordxx */
