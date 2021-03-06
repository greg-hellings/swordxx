/***************************************************************************
 *
 *  thmlhtml.cpp -    ThML to HTML filter
 *
 * $Id$
 *
 * Copyright 1999-2013 CrossWire Bible Society (http://www.crosswire.org)
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

#include "thmlhtml.h"

#include <cstdlib>
#include "../swmodule.h"
#include "../utilxml.h"


namespace swordxx {


ThMLHTML::ThMLHTML() {
    setTokenStart("<");
    setTokenEnd(">");

    setEscapeStart("&");
    setEscapeEnd(";");

    setEscapeStringCaseSensitive(true);
    setPassThruNumericEscapeString(true);

    addAllowedEscapeString("quot");
    addAllowedEscapeString("amp");
    addAllowedEscapeString("lt");
    addAllowedEscapeString("gt");

    addAllowedEscapeString("nbsp");
    addAllowedEscapeString("brvbar"); // "�"
    addAllowedEscapeString("sect");   // "�"
    addAllowedEscapeString("copy");   // "�"
    addAllowedEscapeString("laquo");  // "�"
    addAllowedEscapeString("reg");    // "�"
    addAllowedEscapeString("acute");  // "�"
    addAllowedEscapeString("para");   // "�"
    addAllowedEscapeString("raquo");  // "�"

    addAllowedEscapeString("Aacute"); // "�"
    addAllowedEscapeString("Agrave"); // "�"
    addAllowedEscapeString("Acirc");  // "�"
    addAllowedEscapeString("Auml");   // "�"
    addAllowedEscapeString("Atilde"); // "�"
    addAllowedEscapeString("Aring");  // "�"
    addAllowedEscapeString("aacute"); // "�"
    addAllowedEscapeString("agrave"); // "�"
    addAllowedEscapeString("acirc");  // "�"
    addAllowedEscapeString("auml");   // "�"
    addAllowedEscapeString("atilde"); // "�"
    addAllowedEscapeString("aring");  // "�"
    addAllowedEscapeString("Eacute"); // "�"
    addAllowedEscapeString("Egrave"); // "�"
    addAllowedEscapeString("Ecirc");  // "�"
    addAllowedEscapeString("Euml");   // "�"
    addAllowedEscapeString("eacute"); // "�"
    addAllowedEscapeString("egrave"); // "�"
    addAllowedEscapeString("ecirc");  // "�"
    addAllowedEscapeString("euml");   // "�"
    addAllowedEscapeString("Iacute"); // "�"
    addAllowedEscapeString("Igrave"); // "�"
    addAllowedEscapeString("Icirc");  // "�"
    addAllowedEscapeString("Iuml");   // "�"
    addAllowedEscapeString("iacute"); // "�"
    addAllowedEscapeString("igrave"); // "�"
    addAllowedEscapeString("icirc");  // "�"
    addAllowedEscapeString("iuml");   // "�"
    addAllowedEscapeString("Oacute"); // "�"
    addAllowedEscapeString("Ograve"); // "�"
    addAllowedEscapeString("Ocirc");  // "�"
    addAllowedEscapeString("Ouml");   // "�"
    addAllowedEscapeString("Otilde"); // "�"
    addAllowedEscapeString("oacute"); // "�"
    addAllowedEscapeString("ograve"); // "�"
    addAllowedEscapeString("ocirc");  // "�"
    addAllowedEscapeString("ouml");   // "�"
    addAllowedEscapeString("otilde"); // "�"
    addAllowedEscapeString("Uacute"); // "�"
    addAllowedEscapeString("Ugrave"); // "�"
    addAllowedEscapeString("Ucirc");  // "�"
    addAllowedEscapeString("Uuml");   // "�"
    addAllowedEscapeString("uacute"); // "�"
    addAllowedEscapeString("ugrave"); // "�"
    addAllowedEscapeString("ucirc");  // "�"
    addAllowedEscapeString("uuml");   // "�"
    addAllowedEscapeString("Yacute"); // "�"
    addAllowedEscapeString("yacute"); // "�"
    addAllowedEscapeString("yuml");   // "�"

    addAllowedEscapeString("deg");    // "�"
    addAllowedEscapeString("plusmn"); // "�"
    addAllowedEscapeString("sup2");   // "�"
    addAllowedEscapeString("sup3");   // "�"
    addAllowedEscapeString("sup1");   // "�"
    addAllowedEscapeString("nbsp");   // "�"
    addAllowedEscapeString("pound");  // "�"
    addAllowedEscapeString("cent");   // "�"
    addAllowedEscapeString("frac14"); // "�"
    addAllowedEscapeString("frac12"); // "�"
    addAllowedEscapeString("frac34"); // "�"
    addAllowedEscapeString("iquest"); // "�"
    addAllowedEscapeString("iexcl");  // "�"
    addAllowedEscapeString("ETH");    // "�"
    addAllowedEscapeString("eth");    // "�"
    addAllowedEscapeString("THORN");  // "�"
    addAllowedEscapeString("thorn");  // "�"
    addAllowedEscapeString("AElig");  // "�"
    addAllowedEscapeString("aelig");  // "�"
    addAllowedEscapeString("Oslash"); // "�"
    addAllowedEscapeString("curren"); // "�"
    addAllowedEscapeString("Ccedil"); // "�"
    addAllowedEscapeString("ccedil"); // "�"
    addAllowedEscapeString("szlig");  // "�"
    addAllowedEscapeString("Ntilde"); // "�"
    addAllowedEscapeString("ntilde"); // "�"
    addAllowedEscapeString("yen");    // "�"
    addAllowedEscapeString("not");    // "�"
    addAllowedEscapeString("ordf");   // "�"
    addAllowedEscapeString("uml");    // "�"
    addAllowedEscapeString("shy");    // "�"
    addAllowedEscapeString("macr");   // "�"

    addAllowedEscapeString("micro");  // "�"
    addAllowedEscapeString("middot"); // "�"
    addAllowedEscapeString("cedil");  // "�"
    addAllowedEscapeString("ordm");   // "�"
    addAllowedEscapeString("times");  // "�"
    addAllowedEscapeString("divide"); // "�"
    addAllowedEscapeString("oslash"); // "�"

    setTokenCaseSensitive(true);

    addTokenSubstitute("note", " <font color=\"#800000\"><small>(");
    addTokenSubstitute("/note", ")</small></font> ");
}


bool ThMLHTML::handleToken(std::string &buf, const char *token, BasicFilterUserData *userData) {
    if (!substituteToken(buf, token)) { // manually process if it wasn't a simple substitution
        MyUserData *u = (MyUserData *)userData;
        XMLTag tag(token);
        if (!strcmp(tag.getName(), "sync")) {
            if (!tag.getAttribute("type").empty() && !tag.getAttribute("value").empty() && !strcmp(tag.getAttribute("type").c_str(), "Strongs")) {
                auto value(tag.getAttribute("value"));
                if (!value.empty()) {
                    auto const firstChar(*(value.c_str()));
                    if (firstChar == 'H' || firstChar == 'G' || firstChar == 'A') {
                        value.erase(0u, 1u);
                        buf += "<small><em>";
                        buf += value;
                        buf += "</em></small>";
                    }
                    else if (firstChar == 'T') {
                        value.erase(0u, 2u);

                        buf += "<small><i>";
                        buf += value;
                        buf += "</i></small>";
                    }
                }
            }
            else if (!tag.getAttribute("type").empty() && !tag.getAttribute("value").empty() && !strcmp(tag.getAttribute("type").c_str(), "morph")) {
                buf += "<small><em>";
                buf += tag.getAttribute("value");
                buf += "</em></small>";
            }
            else if (!tag.getAttribute("type").empty() && !tag.getAttribute("value").empty() && !strcmp(tag.getAttribute("type").c_str(), "lemma")) {
                buf += "<small><em>(";
                buf += tag.getAttribute("value");
                buf += ")</em></small>";
            }
        }
        else if (!strcmp(tag.getName(), "div")) {
            if (tag.isEndTag() && (u->SecHead)) {
                buf += "</i></b><br />";
                u->SecHead = false;
            }
            else if (!tag.getAttribute("class").empty()) {
                if (!strcmp(tag.getAttribute("class").c_str(), "sechead")) {
                    u->SecHead = true;
                    buf += "<br /><b><i>";
                }
                else if (!strcmp(tag.getAttribute("class").c_str(), "title")) {
                    u->SecHead = true;
                    buf += "<br /><b><i>";
                }
            }
        }
        else if (!strcmp(tag.getName(), "img")) {
            const char *src = strstr(token, "src");
            if (!src)        // assert we have a src attribute
                return false;

            buf += '<';
            for (const char *c = token; *c; c++) {
                if (c == src) {
                    for (;((*c) && (*c != '"')); c++)
                        buf += *c;

                    if (!*c) { c--; continue; }

                    buf += '"';
                    if (*(c+1) == '/') {
                        buf += "file:";
                        buf += userData->module->getConfigEntry("AbsoluteDataPath");
                        if (buf[buf.length()-2] == '/')
                            c++;        // skip '/'
                    }
                    continue;
                }
                buf += *c;
            }
            buf += '>';
        }
        else if (!strcmp(tag.getName(), "scripRef")) { //do nothing with scrip refs, we leave them out

        }
        else {
            buf += '<';
            buf += token;
            buf += '>';

//            return false;  // we still didn't handle token
        }
    }
    return true;
}


} /* namespace swordxx */
