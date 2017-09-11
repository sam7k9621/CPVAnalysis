/***************************************************************************************************
*
*  Filename    : checkEvtTool.h
*  Description : All-in-one class for parsing Goldon JSON File and checking against event
*
***************************************************************************************************/
#ifndef __CHECKEVTTOOL_H__
#define __CHECKEVTTOOL_H__

#include "fstream"
#include <iostream>
#include <map>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>


class checkEvtTool {
    public:

        checkEvtTool( bool debug = false );
        checkEvtTool( std::string myJson, bool debug = false );
        ~checkEvtTool(){}

        void addJson( std::string myJson );
        void makeJsonMap( bool savejson = false, std::string saveAs = "myJSON.txt" );
        void saveJson( std::string saveAs = "myJSON.txt" );
        void listMyJsons();
        void listGoodLumis();

        bool isGoodEvt( int runNumber, int LumiSec );

    private:

        bool hasJson;
        bool hasJsonMap;
        bool deBug;
        std::vector<std::string> myJsons;
        std::multimap<int, std::pair<int, int> > JsonMap;// <runNumber, pair<startLumiSec, endLumiSec> >, multimap for same runNumber

        void checkChars( char* nameInput, std::string& nameOutput, int& status );
};
#endif
