%include <stl.i>
%include <std_map.i>
%include <std_pair.i>
%include <std_multimap.i>

/*
Include SWModule and DirEntry here, so that it will be able to appear below
*/
%{
#include "swmodule.h"
#include "filemgr.h"
%}

/* 
These are the stl templates that are used throughout Sword. Some have more
than one name in Sword, depending on context. However, these are all wrapped with the one class.
For example, sword::AttributeValueMap is the same as sword::ConfigEntMap
Both are wrapped as the former, however
*/
%inline %{
typedef std::map< sword::SWBuf, sword::SWBuf > AttributeValueMap;
typedef std::map< sword::SWBuf, AttributeValueMap> AttributeListMap;
typedef std::map< sword::SWBuf, AttributeListMap> AttributetypeListMap; 
%}

/* Used by SWModule and SWConfig */
%template() std::pair <sword::SWBuf, sword::SWBuf>;
%template(AttributeValueMap) std::map < sword::SWBuf, sword::SWBuf >;

/* Used by SWModule */
%template() std::pair <sword::SWBuf, AttributeValueMap>;
%template(AttributeListMap) std::map < sword::SWBuf, AttributeValueMap>;
%template() std::pair < sword::SWBuf, AttributeListMap>;
%template(AttributeTypeListMap) std::map < sword::SWBuf, AttributeListMap>;

/* Used by SWConfig */
%template(SectionMapMultiMap) std::multimap < sword::SWBuf, AttributeValueMap >;

/* Used by SWMgr */
%template() std::pair<sword::SWBuf, sword::SWModule*>;
%template(ModuleMap) std::map<sword::SWBuf, sword::SWModule*>;

/* Used by SWMgr and LocaleMgr */
%template(StringVector) std::vector < sword::SWBuf >;

/* Used by InstallMgr */
#ifndef EXCLUDE_INSTALLMGR

%template() std::pair<sword::SWBuf, sword::InstallSource*>;
%template(InstallSourceMap) std::map<sword::SWBuf, sword::InstallSource*>;
%template() std::pair<sword::SWModule *, int>;
%template() std::map<sword::SWModule *, int>;
#endif

/* Used by DirEntry */
%template(DirEntryVector) std::vector < sword::DirEntry > ;
