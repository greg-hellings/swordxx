/***************************************************************************
 *
 *  osishtmlhref.cpp -    OSIS to HTML with hrefs filter
 *
 * $Id$
 *
 * Copyright 2003-2014 CrossWire Bible Society (http://www.crosswire.org)
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

#include "osishtmlhref.h"

#include <cctype>
#include <cstdlib>
#include <stack>
#include "../keys/versekey.h"
#include "../stringmgr.h"
#include "../swmodule.h"
#include "../url.h"
#include "../utilstr.h"
#include "../utilxml.h"


namespace swordxx {

namespace {
    typedef std::stack<std::string> TagStack;
// though this might be slightly slower, possibly causing an extra bool check, this is a renderFilter
// so speed isn't the absolute highest priority, and this is a very minor possible hit
static inline void outText(const char *t, std::string &o, BasicFilterUserData *u) { if (!u->suspendTextPassThru) o += t; else u->lastSuspendSegment += t; }
static inline void outText(char t, std::string &o, BasicFilterUserData *u) { if (!u->suspendTextPassThru) o += t; else u->lastSuspendSegment += t; }

void processLemma(bool suspendTextPassThru, XMLTag &tag, std::string &buf) {
    std::string attrib;
    const char *val;
    if (!(attrib = tag.getAttribute("lemma")).empty()) {
        int count = tag.getAttributePartCount("lemma", ' ');
        int i = (count > 1) ? 0 : -1;        // -1 for whole value cuz it's faster, but does the same thing as 0
        do {
            attrib = tag.getAttribute("lemma", i, ' ');
            if (i < 0) i = 0;    // to handle our -1 condition
            val = strchr(attrib.c_str(), ':');
            val = (val) ? (val + 1) : attrib.c_str();
            std::string gh;
            if(*val == 'G')
                gh = "Greek";
            if(*val == 'H')
                gh = "Hebrew";
            const char *val2 = val;
            if ((strchr("GH", *val)) && (isdigit(val[1])))
                val2++;
            //if ((!strcmp(val2, "3588")) && (lastText.length() < 1))
            //    show = false;
            //else {
                if (!suspendTextPassThru) {
                    buf += formatted("<small><em class=\"strongs\">&lt;<a href=\"passagestudy.jsp?action=showStrongs&type=%s&value=%s\" class=\"strongs\">%s</a>&gt;</em></small>",
                            (gh.length()) ? gh.c_str() : "",
                            URL::encode(val2).c_str(),
                            val2);
                }
            //}

        } while (++i < count);
    }
}

void processMorph(bool suspendTextPassThru, XMLTag &tag, std::string &buf) {
    std::string attrib;
    const char *val;
    if (!(attrib = tag.getAttribute("morph")).empty()) { // && (show)) {
        std::string savelemma = tag.getAttribute("savlm");
        //if ((strstr(savelemma.c_str(), "3588")) && (lastText.length() < 1))
        //    show = false;
        //if (show) {
            int count = tag.getAttributePartCount("morph", ' ');
            int i = (count > 1) ? 0 : -1;        // -1 for whole value cuz it's faster, but does the same thing as 0
            do {
                attrib = tag.getAttribute("morph", i, ' ');
                if (i < 0) i = 0;    // to handle our -1 condition
                val = strchr(attrib.c_str(), ':');
                val = (val) ? (val + 1) : attrib.c_str();
                const char *val2 = val;
                if ((*val == 'T') && (strchr("GH", val[1])) && (isdigit(val[2])))
                    val2+=2;
                if (!suspendTextPassThru) {
                    buf += formatted("<small><em class=\"morph\">(<a href=\"passagestudy.jsp?action=showMorph&type=%s&value=%s\" class=\"morph\">%s</a>)</em></small>",
                            URL::encode(tag.getAttribute("morph").c_str()).c_str(),
                            URL::encode(val).c_str(),
                            val2);
                }
            } while (++i < count);
        //}
    }
}
}    // end anonymous namespace

// TODO: this bridge pattern is to preserve binary compat on 1.6.x
class OSISHTMLHREF::TagStacks {
public:
    TagStack quoteStack;
    TagStack hiStack;
};

OSISHTMLHREF::MyUserData::MyUserData(const SWModule *module, const SWKey *key) : BasicFilterUserData(module, key) {
    inXRefNote    = false;
    suspendLevel = 0;
    tagStacks = new TagStacks();
    wordsOfChristStart = "<font color=\"red\"> ";
    wordsOfChristEnd   = "</font> ";
    if (module) {
        osisQToTick = ((!module->getConfigEntry("OSISqToTick")) || (strcmp(module->getConfigEntry("OSISqToTick"), "false")));
        version = module->getName();
        BiblicalText = (!strcmp(module->getType(), "Biblical Texts"));
    }
    else {
        osisQToTick = true;    // default
        version = "";
    }
}

OSISHTMLHREF::MyUserData::~MyUserData() {
    delete tagStacks;
}

OSISHTMLHREF::OSISHTMLHREF() {
    setTokenStart("<");
    setTokenEnd(">");

    setEscapeStart("&");
    setEscapeEnd(";");

    setEscapeStringCaseSensitive(true);
    setPassThruNumericEscapeString(true);

    addAllowedEscapeString("quot");
    addAllowedEscapeString("apos");
    addAllowedEscapeString("amp");
    addAllowedEscapeString("lt");
    addAllowedEscapeString("gt");

    setTokenCaseSensitive(true);

    //    addTokenSubstitute("lg",  "<br />");
    //    addTokenSubstitute("/lg", "<br />");

    morphFirst = false;
    renderNoteNumbers = false;
}


bool OSISHTMLHREF::handleToken(std::string &buf, const char *token, BasicFilterUserData *userData) {
    MyUserData *u = (MyUserData *)userData;
    std::string scratch;
    bool sub = (u->suspendTextPassThru) ? substituteToken(scratch, token) : substituteToken(buf, token);
    if (!sub) {
  // manually process if it wasn't a simple substitution
        XMLTag tag(token);

        // <w> tag
        if (!strcmp(tag.getName(), "w")) {

            // start <w> tag
            if ((!tag.isEmpty()) && (!tag.isEndTag())) {
                u->w = token;
            }

            // end or empty <w> tag
            else {
                bool endTag = tag.isEndTag();
                std::string lastText;
                //bool show = true;    // to handle unplaced article in kjv2003-- temporary till combined

                if (endTag) {
                    tag = u->w.c_str();
                    lastText = u->lastTextNode.c_str();
                }
                else lastText = "stuff";

                std::string attrib;
                const char *val;
                if (!(attrib = tag.getAttribute("xlit")).empty()) {
                    val = strchr(attrib.c_str(), ':');
                    val = (val) ? (val + 1) : attrib.c_str();
                    outText(" ", buf, u);
                    outText(val, buf, u);
                }
                if (!(attrib = tag.getAttribute("gloss")).empty()) {
                    // I'm sure this is not the cleanest way to do it, but it gets the job done
                    // for rendering ruby chars properly ^_^
                    buf.resize(buf.size() - lastText.size());

                    outText("<ruby><rb>", buf, u);
                    outText(lastText.c_str(), buf, u);
                    outText("</rb><rp>(</rp><rt>", buf, u);
                    val = strchr(attrib.c_str(), ':');
                    val = (val) ? (val + 1) : attrib.c_str();
                    outText(val, buf, u);
                    outText("</rt><rp>)</rp></ruby>", buf, u);
                }
                if (!morphFirst) {
                    processLemma(u->suspendTextPassThru, tag, buf);
                    processMorph(u->suspendTextPassThru, tag, buf);
                }
                else {
                    processMorph(u->suspendTextPassThru, tag, buf);
                    processLemma(u->suspendTextPassThru, tag, buf);
                }
                if (!(attrib = tag.getAttribute("POS")).empty()) {
                    val = strchr(attrib.c_str(), ':');
                    val = (val) ? (val + 1) : attrib.c_str();
                    outText(" ", buf, u);
                    outText(val, buf, u);
                }

                /*if (endTag)
                    buf += "}";*/
            }
        }

        // <note> tag
        else if (!strcmp(tag.getName(), "note")) {
            if (!tag.isEndTag()) {
                std::string type = tag.getAttribute("type");
                bool strongsMarkup = (type == "x-strongsMarkup" || type == "strongsMarkup");    // the latter is deprecated
                if (strongsMarkup) {
                    tag.setEmpty(false);    // handle bug in KJV2003 module where some note open tags were <note ... />
                }

                if (!tag.isEmpty()) {

                    if (!strongsMarkup) {    // leave strong's markup notes out, in the future we'll probably have different option filters to turn different note types on or off
                        std::string footnoteNumber = tag.getAttribute("swordFootnote");
                        std::string noteName = tag.getAttribute("n");
                        char ch = ((!tag.getAttribute("type").empty() && ((!strcmp(tag.getAttribute("type").c_str(), "crossReference")) || (!strcmp(tag.getAttribute("type").c_str(), "x-cross-ref")))) ? 'x':'n');

                        u->inXRefNote = true; // Why this change? Ben Morgan: Any note can have references in, so we need to set this to true for all notes
//                        u->inXRefNote = (ch == 'x');

                        // see if we have a VerseKey * or descendant
                        VerseKey const * const vkey =
                                dynamic_cast<VerseKey const *>(u->key);
                        buf += formatted("<a href=\"passagestudy.jsp?action=showNote&type=%c&value=%s&module=%s&passage=%s\"><small><sup class=\"%c\">*%c%s</sup></small></a>",
                                ch,
                            URL::encode(footnoteNumber.c_str()).c_str(),
                            URL::encode(u->version.c_str()).c_str(),
                            URL::encode(vkey ? vkey->getText() : u->key->getText()).c_str(),
                            ch,
                            ch,
                            (renderNoteNumbers ? noteName.c_str() : ""));
                    }
                }
                u->suspendTextPassThru = (++u->suspendLevel);
            }
            if (tag.isEndTag()) {
                u->suspendTextPassThru = (--u->suspendLevel);
                u->inXRefNote = false;
                u->lastSuspendSegment = ""; // fix/work-around for nasb divineName in note bug
            }
        }

        // <p> paragraph and <lg> linegroup tags
        else if (!strcmp(tag.getName(), "p") || !strcmp(tag.getName(), "lg")) {
            if ((!tag.isEndTag()) && (!tag.isEmpty())) {    // non-empty start tag
                outText("<!P><br />", buf, u);
            }
            else if (tag.isEndTag()) {    // end tag
                outText("<!/P><br />", buf, u);
                userData->supressAdjacentWhitespace = true;
            }
            else {                    // empty paragraph break marker
                outText("<!P><br />", buf, u);
                userData->supressAdjacentWhitespace = true;
            }
        }

        // Milestoned paragraphs, created by osis2mod
        // <div type="paragraph" sID.../>
        // <div type="paragraph" eID.../>
        else if (tag.isEmpty() && !strcmp(tag.getName(), "div") && !tag.getAttribute("type").empty() && (!strcmp(tag.getAttribute("type").c_str(), "x-p") || !strcmp(tag.getAttribute("type").c_str(), "paragraph"))) {
            // <div type="paragraph"  sID... />
            if (!tag.getAttribute("sID").empty()) {    // non-empty start tag
                outText("<!P><br />", buf, u);
            }
            // <div type="paragraph"  eID... />
            else if (!tag.getAttribute("eID").empty()) {
                outText("<!/P><br />", buf, u);
                userData->supressAdjacentWhitespace = true;
            }
        }

        // <reference> tag
        else if (!strcmp(tag.getName(), "reference")) {
            if (!u->inXRefNote) {    // only show these if we're not in an xref note
                if (!tag.isEndTag()) {
                    std::string work;
                    std::string ref;
                    bool is_scripRef = false;

                    auto target(tag.getAttribute("osisRef"));
                    const char* the_ref = std::strchr(target.c_str(), ':');

                    if(!the_ref) {
                        // No work
                        ref = target;
                        is_scripRef = true;
                    }
                    else {
                        // Compensate for starting :
                        ref = the_ref + 1;

                        int size = target.size() - ref.size() - 1;
                        work.resize(size, '\0');
                        std::strncpy(&work[0u], target.c_str(), size);

                        // For Bible:Gen.3.15 or Bible.vulgate:Gen.3.15
                        if(!std::strncmp(work.c_str(), "Bible", 5))
                            is_scripRef = true;
                    }

                    if(is_scripRef)
                    {
                        buf += formatted("<a href=\"passagestudy.jsp?action=showRef&type=scripRef&value=%s&module=\">",
                            URL::encode(ref.c_str()).c_str()
//                            (work.size()) ? URL::encode(work.c_str()).c_str() : "")
                            );
                    }
                    else
                    {
                        // Dictionary link, or something
                        buf += formatted("<a href=\"sword://%s/%s\">",
                            URL::encode(work.c_str()).c_str(),
                            URL::encode(ref.c_str()).c_str()
                            );
                    }
                }
                else {
                    outText("</a>", buf, u);
                }
            }
        }

        // <l> poetry, etc
        else if (!strcmp(tag.getName(), "l")) {
            // end line marker
            if (!tag.getAttribute("eID").empty()) {
                outText("<br />", buf, u);
            }
            // <l/> without eID or sID
            // Note: this is improper osis. This should be <lb/>
            else if (tag.isEmpty() && tag.getAttribute("sID").empty()) {
                outText("<br />", buf, u);
            }
            // end of the line
            else if (tag.isEndTag()) {
                outText("<br />", buf, u);
            }
        }

        // <lb.../>
        else if (!strcmp(tag.getName(), "lb") && (tag.getAttribute("type").empty() || strcmp(tag.getAttribute("type").c_str(), "x-optional"))) {
            outText("<br />", buf, u);
            userData->supressAdjacentWhitespace = true;
        }
        // <milestone type="line"/>
        // <milestone type="x-p"/>
        // <milestone type="cQuote" marker="x"/>
        else if ((!strcmp(tag.getName(), "milestone")) && (!tag.getAttribute("type").empty())) {
            if (!strcmp(tag.getAttribute("type").c_str(), "line")) {
                outText("<br />", buf, u);
                if (!tag.getAttribute("subType").empty() && !strcmp(tag.getAttribute("subType").c_str(), "x-PM")) {
                    outText("<br />", buf, u);
                }
                userData->supressAdjacentWhitespace = true;
            }
            else if (!strcmp(tag.getAttribute("type").c_str(),"x-p"))  {
                if (!tag.getAttribute("marker").empty())
                    outText(tag.getAttribute("marker").c_str(), buf, u);
                else outText("<!p>", buf, u);
            }
            else if (!strcmp(tag.getAttribute("type").c_str(), "cQuote")) {
                auto mark(tag.getAttribute("marker"));
                bool hasMark = !mark.empty();
                auto tmp(tag.getAttribute("level"));
                int level = (!tmp.empty()) ? atoi(tmp.c_str()) : 1;

                // first check to see if we've been given an explicit mark
                if (hasMark)
                    outText(mark.c_str(), buf, u);
                // finally, alternate " and ', if config says we should supply a mark
                else if (u->osisQToTick)
                    outText((level % 2) ? '\"' : '\'', buf, u);
            }
        }

        // <title>
        else if (!strcmp(tag.getName(), "title")) {
            if ((!tag.isEndTag()) && (!tag.isEmpty())) {
                outText("<b>", buf, u);
            }
            else if (tag.isEndTag()) {
                outText("</b><br />", buf, u);
            }
        }

        // <list>
        else if (!strcmp(tag.getName(), "list")) {
            if((!tag.isEndTag()) && (!tag.isEmpty())) {
                outText("<ul>", buf, u);
            }
            else if (tag.isEndTag()) {
                outText("</ul>", buf, u);
            }
        }

        // <item>
        else if (!strcmp(tag.getName(), "item")) {
            if((!tag.isEndTag()) && (!tag.isEmpty())) {
                outText("<li>", buf, u);
            }
            else if (tag.isEndTag()) {
                outText("</li>", buf, u);
            }
        }
        // <catchWord> & <rdg> tags (italicize)
        else if (!strcmp(tag.getName(), "rdg") || !strcmp(tag.getName(), "catchWord")) {
            if ((!tag.isEndTag()) && (!tag.isEmpty())) {
                outText("<i>", buf, u);
            }
            else if (tag.isEndTag()) {
                outText("</i>", buf, u);
            }
        }

        // divineName
        else if (!strcmp(tag.getName(), "divineName")) {
            if ((!tag.isEndTag()) && (!tag.isEmpty())) {
                u->suspendTextPassThru = (++u->suspendLevel);
            }
            else if (tag.isEndTag()) {
                std::string lastText = u->lastSuspendSegment.c_str();
                u->suspendTextPassThru = (--u->suspendLevel);
                if (lastText.size()) {
                    toupperstr(lastText);
                    scratch = formatted("%c<font size=\"-1\">%s</font>", lastText[0], lastText.c_str()+1);

                    const unsigned char *tmpBuf = (const unsigned char *)lastText.c_str();
                    getUniCharFromUTF8(&tmpBuf);
                    int char_length = (tmpBuf - (const unsigned char *)lastText.c_str());
                    scratch = formatted("%.*s<font size=\"-1\">%s</font>",
                        char_length,
                        lastText.c_str(),
                        lastText.c_str() + char_length
                    );

                    outText(scratch.c_str(), buf, u);
                }
            }
        }

        // <hi> text highlighting
        else if (!strcmp(tag.getName(), "hi")) {
            std::string type = tag.getAttribute("type");
            // handle tei rend attribute
            if (!type.length()) type = tag.getAttribute("rend");
            if ((!tag.isEndTag()) && (!tag.isEmpty())) {
                if (type == "bold" || type == "b" || type == "x-b") {
                    outText("<b>", buf, u);
                }
                else if (type == "ol" || type == "overline" || type == "x-overline") {
                    outText("<span style=\"text-decoration:overline\">", buf, u);
                }
                else if (type == "super") {
                    outText("<sup>", buf, u);
                }
                else if (type == "sub") {
                    outText("<sub>", buf, u);
                }
                else {    // all other types
                    outText("<i>", buf, u);
                }
                u->tagStacks->hiStack.push(tag.toString());
            }
            else if (tag.isEndTag()) {
                std::string type = "";
                if (!u->tagStacks->hiStack.empty()) {
                    XMLTag tag(u->tagStacks->hiStack.top().c_str());
                    u->tagStacks->hiStack.pop();
                    type = tag.getAttribute("type");
                    if (!type.length()) type = tag.getAttribute("rend");
                }
                if (type == "bold" || type == "b" || type == "x-b") {
                    outText("</b>", buf, u);
                }
                else if (type == "ol") {
                    outText("</span>", buf, u);
                }
                else if (type == "super") {
                    outText("</sup>", buf, u);
                }
                else if (type == "sub") {
                    outText("</sub>", buf, u);
                }
                else {
                    outText("</i>", buf, u);
                }
            }
        }

        // <q> quote
        // Rules for a quote element:
        // If the tag is empty with an sID or an eID then use whatever it specifies for quoting.
        //    Note: empty elements without sID or eID are ignored.
        // If the tag is <q> then use it's specifications and push it onto a stack for </q>
        // If the tag is </q> then use the pushed <q> for specification
        // If there is a marker attribute, possibly empty, this overrides osisQToTick.
        // If osisQToTick, then output the marker, using level to determine the type of mark.
        else if (!strcmp(tag.getName(), "q")) {
            std::string type      = tag.getAttribute("type");
            std::string who       = tag.getAttribute("who");
            auto tmp(tag.getAttribute("level"));
            int level = (!tmp.empty()) ? atoi(tmp.c_str()) : 1;
            auto mark(tag.getAttribute("marker"));
            bool hasMark = !mark.empty();

            // open <q> or <q sID... />
            if ((!tag.isEmpty() && !tag.isEndTag()) || (tag.isEmpty() && !tag.getAttribute("sID").empty())) {
                // if <q> then remember it for the </q>
                if (!tag.isEmpty()) {
                    u->tagStacks->quoteStack.push(tag.toString());
                }

                // Do this first so quote marks are included as WoC
                if (who == "Jesus")
                    outText(u->wordsOfChristStart.c_str(), buf, u);

                // first check to see if we've been given an explicit mark
                if (hasMark)
                    outText(mark.c_str(), buf, u);
                //alternate " and '
                else if (u->osisQToTick)
                    outText((level % 2) ? '\"' : '\'', buf, u);
            }
            // close </q> or <q eID... />
            else if ((tag.isEndTag()) || (tag.isEmpty() && !tag.getAttribute("eID").empty())) {
                // if it is </q> then pop the stack for the attributes
                if (tag.isEndTag() && !u->tagStacks->quoteStack.empty()) {
                    XMLTag qTag(u->tagStacks->quoteStack.top().c_str());
                    u->tagStacks->quoteStack.pop();

                    type    = qTag.getAttribute("type");
                    who     = qTag.getAttribute("who");
                    tmp     = qTag.getAttribute("level");
                    level   = (!tmp.empty()) ? atoi(tmp.c_str()) : 1;
                    mark    = qTag.getAttribute("marker");
                    hasMark = !mark.empty();
                }

                // first check to see if we've been given an explicit mark
                if (hasMark)
                    outText(mark.c_str(), buf, u);
                // finally, alternate " and ', if config says we should supply a mark
                else if (u->osisQToTick)
                    outText((level % 2) ? '\"' : '\'', buf, u);

                // Do this last so quote marks are included as WoC
                if (who == "Jesus")
                    outText(u->wordsOfChristEnd.c_str(), buf, u);
            }
        }

        // <transChange>
        else if (!strcmp(tag.getName(), "transChange")) {
            if ((!tag.isEndTag()) && (!tag.isEmpty())) {
                std::string type = tag.getAttribute("type");
                u->lastTransChange = type;

                // just do all transChange tags this way for now
                if ((type == "added") || (type == "supplied"))
                    outText("<i>", buf, u);
                else if (type == "tenseChange")
                    outText( "*", buf, u);
            }
            else if (tag.isEndTag()) {
                std::string type = u->lastTransChange;
                if ((type == "added") || (type == "supplied"))
                    outText("</i>", buf, u);
            }
            else {    // empty transChange marker?
            }
        }

        // image
        else if (!strcmp(tag.getName(), "figure")) {
            auto const src(tag.getAttribute("src"));
            if (src.empty())        // assert we have a src attribute
                return false;

            std::string filepath;
            if (userData->module) {
                filepath = userData->module->getConfigEntry("AbsoluteDataPath");
                if ((filepath.size()) && (filepath[filepath.size()-1] != '/') && (src[0] != '/'))
                    filepath += '/';
            }
            filepath += src;

            // images become clickable, if the UI supports showImage.
            outText("<a href=\"passagestudy.jsp?action=showImage&value=", buf, u);
            outText(URL::encode(filepath.c_str()).c_str(), buf, u);
            outText("&module=", buf, u);
            outText(URL::encode(u->version.c_str()).c_str(), buf, u);
            outText("\">", buf, u);

            outText("<img src=\"file:", buf, u);
            outText(filepath.c_str(), buf, u);
            outText("\" border=\"0\" />", buf, u);

            outText("</a>", buf, u);
        }

        else {
              return false;  // we still didn't handle token
        }
    }
    return true;
}


} /* namespace swordxx */
