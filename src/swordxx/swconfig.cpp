/******************************************************************************
 *
 *  swconfig.cpp -    used for saving and retrieval of configuration
 *            information
 *
 * $Id$
 *
 * Copyright 1998-2013 CrossWire Bible Society (http://www.crosswire.org)
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

#include "swconfig.h"

#include <cstring>
#include <fcntl.h>
#include "filemgr.h"
#include "utilstr.h"


namespace swordxx {


SWConfig::SWConfig() {
}


SWConfig::SWConfig(const char * ifilename) {
    filename = ifilename;
    Load();
}


SWConfig::~SWConfig() {
}


void SWConfig::Load() {

    if (!filename.size()) return;    // assert we have a filename

    FileDesc *cfile;
    char *buf, *data;
    ConfigEntMap cursect;
    std::string sectname;
    bool first = true;

    Sections.erase(Sections.begin(), Sections.end());

    cfile = FileMgr::getSystemFileMgr()->open(filename.c_str(), FileMgr::RDONLY);
    if (cfile->getFd() > 0) {
        std::string line(FileMgr::getLine(cfile));

        // clean UTF encoding tags at start of file
        while (!line.empty() &&
                ((((unsigned char)line[0]) == 0xEF) ||
                 (((unsigned char)line[0]) == 0xBB) ||
                 (((unsigned char)line[0]) == 0xBF))) {
            line.erase(0u, 1u);
        }

        while (!line.empty()) {
            // ignore commented lines
            if (!line.empty() && (*line.rbegin()) != '#') {
                buf = new char [ line.length() + 1 ];
                strcpy(buf, line.c_str());
                if (*strstrip(buf) == '[') {
                    if (!first)
                        Sections.insert(SectionMap::value_type(sectname, cursect));
                    else first = false;

                    cursect.erase(cursect.begin(), cursect.end());

                    strtok(buf, "]");
                    sectname = buf+1;
                }
                else {
                    strtok(buf, "=");
                    if ((*buf) && (*buf != '=')) {
                        if ((data = strtok(nullptr, "")))
                            cursect.insert(ConfigEntMap::value_type(buf, strstrip(data)));
                        else cursect.insert(ConfigEntMap::value_type(buf, ""));
                    }
                }
                delete [] buf;
            }
            line = FileMgr::getLine(cfile);
        }
        if (!first)
            Sections.insert(SectionMap::value_type(sectname, cursect));

        FileMgr::getSystemFileMgr()->close(cfile);
    }
}


void SWConfig::Save() {

    if (!filename.size()) return;    // assert we have a filename

    FileDesc *cfile;
    std::string buf;
    SectionMap::iterator sit;
    ConfigEntMap::iterator entry;
    std::string sectname;

    cfile = FileMgr::getSystemFileMgr()->open(filename.c_str(), FileMgr::RDWR|FileMgr::CREAT|FileMgr::TRUNC);
    if (cfile->getFd() > 0) {

        for (sit = Sections.begin(); sit != Sections.end(); sit++) {
            buf =  "\n[";
            buf += (*sit).first.c_str();
            buf += "]\n";
            cfile->write(buf.c_str(), buf.length());
            for (entry = (*sit).second.begin(); entry != (*sit).second.end(); entry++) {
                buf = (*entry).first.c_str();
                buf += "=";
                buf += (*entry).second.c_str();
                buf += "\n";
                cfile->write(buf.c_str(), buf.length());
            }
        }
        buf = "\n";
        cfile->write(buf.c_str(), buf.length());
        FileMgr::getSystemFileMgr()->close(cfile);
    }
}


void SWConfig::augment(SWConfig &addFrom) {

    SectionMap::iterator section;
    ConfigEntMap::iterator entry, start, end;

    for (section = addFrom.Sections.begin(); section != addFrom.Sections.end(); section++) {
        for (entry = (*section).second.begin(); entry != (*section).second.end(); entry++) {
            start = Sections[section->first].lower_bound(entry->first);
            end   = Sections[section->first].upper_bound(entry->first);
            if (start != end) {
                if (((++start) != end)
                        || ((++(addFrom.Sections[section->first].lower_bound(entry->first))) != addFrom.Sections[section->first].upper_bound(entry->first))) {
                    for (--start; start != end; start++) {
                        if (!strcmp(start->second.c_str(), entry->second.c_str()))
                            break;
                    }
                    if (start == end)
                        Sections[(*section).first].insert(ConfigEntMap::value_type((*entry).first, (*entry).second));
                }
                else    Sections[section->first][entry->first.c_str()] = entry->second.c_str();
            }
            else    Sections[section->first][entry->first.c_str()] = entry->second.c_str();
        }
    }
}


ConfigEntMap & SWConfig::operator [] (const char *section) {
    return Sections[section];
}


} /* namespace swordxx */

