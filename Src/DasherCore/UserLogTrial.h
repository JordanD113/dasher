
// Represent all the data about one trial by a user (ie one
// input sequence using speech or normal Dasher).
//
// Copyright 2005 by Keith Vertanen

#ifndef __USER_LOG_TRIAL_H__
#define __USER_LOG_TRIAL_H__

#include "FileLogger.h"
#include <fstream>
#include <string>
#include <vector>
#include "SimpleTimer.h"
#include "TimeSpan.h"
#include "UserLocation.h"
#include "Alphabet/Alphabet.h"
#include "DasherTypes.h"
#include "UserLogParam.h"
#include <algorithm>

#ifdef USER_LOG_TOOL
    #include "XMLUtil.h"

    // Types used to return two dimensional grid of double values
    typedef double**                                        DENSITY_GRID;
    typedef vector<DENSITY_GRID>                            VECTOR_DENSITY_GRIDS;
    typedef vector<DENSITY_GRID>::iterator                  VECTOR_DENSITY_GRIDS_ITER;
    typedef vector<VECTOR_DENSITY_GRIDS>                    VECTOR_VECTOR_DENSITY_GRIDS;
    typedef vector<VECTOR_DENSITY_GRIDS>::iterator          VECTOR_VECTOR_DENSITY_GRIDS_ITER;
    typedef vector<VECTOR_VECTOR_DENSITY_GRIDS>             VECTOR_VECTOR_VECTOR_DENSITY_GRIDS;
    typedef vector<VECTOR_VECTOR_DENSITY_GRIDS>::iterator   VECTOR_VECTOR_VECTOR_DENSITY_GRIDS_ITER;

#endif

extern CFileLogger* gLogger;

class CUserLogTrial;

typedef vector<CUserLogTrial>               VECTOR_USER_LOG_TRIAL;
typedef vector<CUserLogTrial>::iterator     VECTOR_USER_LOG_TRIAL_ITER;
typedef vector<CUserLogTrial*>              VECTOR_USER_LOG_TRIAL_PTR;
typedef vector<CUserLogTrial*>::iterator    VECTOR_USER_LOG_TRIAL_PTR_ITER;

// Used to indicate what type of event caused symbols to
// be added or deleted by Dasher.  This could for example
// be used to log which button was pressed in button
// Dasher.
enum eUserLogEventType
{
    userLogEventMouse       = 0     // Normal mouse navigation
};

// Keeps track of a single instance of AddSymbols() or 
// DeleteSymbols() being called.  
struct NavLocation
{
    string                                  strHistory;     // Display symbol history after the adds or deletes are carried out
    CTimeSpan*                              span;           // Track the time between this update and the one that comes next
    eUserLogEventType                       event;          // What triggered the adding or deleting of symbols
    int                                     numDeleted;     // How many symbols deleted (0 if it is an AddSymbols() call)
    Dasher::VECTOR_SYMBOL_PROB_DISPLAY*     pVectorAdded;   // Info on all added symbols   
    double                                  avgBits;        // Average bits required to write this history (assuming no errors)
};

typedef vector<NavLocation*>    VECTOR_NAV_LOCATION_PTR;

struct WindowSize
{
    int         top;
    int         left;
    int         bottom;
    int         right;
};

// Stores the time span and all the locations and mouse locations
// that occur during a start/stop cycle of navigation.
struct NavCycle
{
    CTimeSpan*                  pSpan;                    // Tracks time span of this navgiation cycle
    VECTOR_NAV_LOCATION_PTR     vectorNavLocations;       // Locations when text was added or deleted
    VECTOR_USER_LOCATION_PTR    vectorMouseLocations;     // Stores mouse locations and time stamps    
};

typedef vector<NavCycle*>               VECTOR_NAV_CYCLE_PTR;
typedef vector<NavCycle*>::iterator     VECTOR_NAV_CYCLE_PTR_ITER;

using namespace std;

class CUserLogTrial
{
public:
    CUserLogTrial(string strCurrentTrialFilename);
    ~CUserLogTrial();

    bool                        HasWritingOccured();
    void                        StartWriting();
    void                        StopWriting();
    void                        AddSymbols(Dasher::VECTOR_SYMBOL_PROB* vectorNewSymbolProbs, eUserLogEventType event, Dasher::CAlphabet* pCurrentAlphabet);
    void                        DeleteSymbols(int numToDelete, eUserLogEventType);  
    string                      GetXML(const string& prefix = "");
    void                        Done();
    void                        AddWindowSize(int top, int left, int bottom, int right);
    void                        AddCanvasSize(int top, int left, int bottom, int right);
    void                        AddMouseLocation(int x, int y, float nats);
    void                        AddMouseLocationNormalized(int x, int y, bool bStoreIntegerRep, float nats);
    bool                        IsWriting();
    void                        AddParam(const string& strName, const string& strValue, int optionMask = 0);
    static string               GetParamXML(CUserLogParam* param, const string& strPrefix = "");

#ifdef USER_LOG_TOOL
    CUserLogTrial(const string& strXML);
    static VECTOR_USER_LOG_PARAM_PTR    ParseParamsXML(const string& strXML);
    static WindowSize                   ParseWindowXML(const string& strXML);
    VECTOR_STRING                       GetTabMouseXY(bool bReturnNormalized);
    VECTOR_DENSITY_GRIDS                GetMouseDensity(int gridSize);
    static DENSITY_GRID                 MergeGrids(int gridSize, DENSITY_GRID pGridA, DENSITY_GRID pGridB);
#endif

protected:
    CTimeSpan*                          m_pSpan;
    bool                                m_bWritingStart;
    string                              m_strCurrentTrial;          // Stores information passed to us from the UserTrial app
    WindowSize                          m_windowCoordinates;        // Records the window coordinates at the start of navigation
    WindowSize                          m_canvasCoordinates;        // The size of our canvas during navigation
    Dasher::VECTOR_SYMBOL_PROB_DISPLAY  m_vectorHistory;            // Tracks all the symbols, probs, display text entererd during this trial
    VECTOR_USER_LOG_PARAM_PTR           m_vectorParams;             // Stores general parameters we want stored in each trial tag in the XML
    VECTOR_NAV_CYCLE_PTR                m_vectorNavCycles;
    string                              m_strCurrentTrialFilename;  // Where to look for info on the current subject's trial

    // Used whenever we need a temporary char* buffer
    static const int                    TEMP_BUFFER_SIZE = 4096;
    char                                m_strTempBuffer[TEMP_BUFFER_SIZE];  
    
    void                        GetUserTrialInfo();
    string                      GetHistoryDisplay();
    double                      GetHistoryAvgBits();
    void                        StopPreviousTimer();
    void                        InitMemberVars();

    NavCycle*                   GetCurrentNavCycle();
    NavCycle*                   AddNavCycle();
    NavLocation*                GetCurrentNavLocation();

    // Various helpers for outputting the XML, this allows subclasses to
    // add there own GetXML() method but reuse code for shared parts.
    string                      GetLocationXML(NavLocation* location, const string& prefix);
    string                      GetSummaryXML(const string& prefix);
    string                      GetStatsXML(const string& prefix, const string& strText, CTimeSpan* pSpan, double avgBits);
    string                      GetWindowCanvasXML(const string& prefix);
    string                      GetParamsXML(const string& prefix);
    string                      GetNavCyclesXML(const string& prefix);

};

#endif