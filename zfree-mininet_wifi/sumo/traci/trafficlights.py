# -*- coding: utf-8 -*-
"""
@file    trafficlights.py
@author  Michael Behrisch
@date    2011-03-16
@version $Id: trafficlights.py 12906 2012-10-30 11:02:27Z behrisch $

Python implementation of the TraCI interface.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import struct
import trace
import constants as tc

class Phase:
    def __init__(self, duration, duration1, duration2, phaseDef):
        self._duration = duration
        self._duration1 = duration1
        self._duration2 = duration2
        self._phaseDef = phaseDef
        
    def __repr__(self):
        return ("Phase:\nduration: %s\nduration1: %s\nduration2: %s\nphaseDef: %s\n" %
                (self._duration, self._duration1, self._duration2, self._phaseDef))
        
class Logic:
    def __init__(self, subID, type, subParameter, currentPhaseIndex, phases):
        self._subID = subID
        self._type = type
        self._subParameter = subParameter
        self._currentPhaseIndex = currentPhaseIndex
        self._phases = phases
        
    def __repr__(self):
        result = ("Logic:\nsubID: %s\ntype: %s\nsubParameter: %s\ncurrentPhaseIndex: %s\n" %
                  (self._subID, self._type, self._subParameter, self._currentPhaseIndex))
        for p in self._phases:
            result += str(p)
        return result


def _readLogics(result):
    result.readLength()
    nbLogics = result.read("!i")[0]    # Number of logics
    logics = []
    for i in range(nbLogics):
        result.read("!B")                       # Type of SubID
        subID = result.readString()
        result.read("!B")                       # Type of Type
        type = result.read("!i")[0]             # Type
        result.read("!B")                       # Type of SubParameter
        subParameter = result.read("!i")[0]     # SubParameter
        result.read("!B")                       # Type of Current phase index
        currentPhaseIndex = result.read("!i")[0]    # Current phase index
        result.read("!B")                       # Type of Number of phases
        nbPhases = result.read("!i")[0]         # Number of phases
        phases = []
        for j in range(nbPhases):
            result.read("!B")                   # Type of Duration
            duration = result.read("!i")[0]     # Duration
            result.read("!B")                   # Type of Duration1
            duration1 = result.read("!i")[0]    # Duration1
            result.read("!B")                   # Type of Duration2
            duration2 = result.read("!i")[0]    # Duration2
            result.read("!B")                   # Type of Phase Definition
            phaseDef = result.readString()      # Phase Definition
            phase = Phase(duration, duration1, duration2, phaseDef)
            phases.append(phase)
        logic = Logic(subID, type, subParameter, currentPhaseIndex, phases)
        logics.append(logic)
    return logics

def _readLinks(result):
    result.readLength()
    nbSignals = result.read("!i")[0] # Length
    signals = []
    for i in range(nbSignals):
        result.read("!B")                           # Type of Number of Controlled Links
        nbControlledLinks = result.read("!i")[0]    # Number of Controlled Links
        controlledLinks = []
        for j in range(nbControlledLinks):
            result.read("!B")                       # Type of Link j
            link = result.readStringList()          # Link j
            controlledLinks.append(link)
        signals.append(controlledLinks)
    return signals


_RETURN_VALUE_FUNC = {tc.ID_LIST:                     trace.Storage.readStringList,
                      tc.TL_RED_YELLOW_GREEN_STATE:   trace.Storage.readString,
                      tc.TL_COMPLETE_DEFINITION_RYG:  _readLogics,
                      tc.TL_CONTROLLED_LANES:         trace.Storage.readStringList,
                      tc.TL_CONTROLLED_LINKS:         _readLinks,
                      tc.TL_CURRENT_PROGRAM:          trace.Storage.readString,
                      tc.TL_CURRENT_PHASE:            trace.Storage.readInt,
                      tc.TL_NEXT_SWITCH:              trace.Storage.readInt}
subscriptionResults = trace.SubscriptionResults(_RETURN_VALUE_FUNC)

def _getUniversal(varID, tlsID):
    result = trace._sendReadOneStringCmd(tc.CMD_GET_TL_VARIABLE, varID, tlsID)
    return _RETURN_VALUE_FUNC[varID](result)

def getIDList():
    """getIDList() -> list(string)
    
    Returns a list of all traffic lights in the network.
    """
    return _getUniversal(tc.ID_LIST, "")

def getRedYellowGreenState(tlsID):
    """getRedYellowGreenState(string) -> string
    
    .
    """
    return _getUniversal(tc.TL_RED_YELLOW_GREEN_STATE, tlsID)

def getCompleteRedYellowGreenDefinition(tlsID):
    """getCompleteRedYellowGreenDefinition(string) -> 
    
    .
    """
    return _getUniversal(tc.TL_COMPLETE_DEFINITION_RYG, tlsID)

def getControlledLanes(tlsID):
    """getControlledLanes(string) -> c
    
    .
    """
    return _getUniversal(tc.TL_CONTROLLED_LANES, tlsID)

def getControlledLinks(tlsID):
    """getControlledLinks(string) -> list(list(list(string)))
    
    .
    """
    return _getUniversal(tc.TL_CONTROLLED_LINKS, tlsID)

def getProgram(tlsID):
    """getProgram(string) -> string
    
    .
    """
    return _getUniversal(tc.TL_CURRENT_PROGRAM, tlsID)

def getPhase(tlsID):
    """getPhase(string) -> integer
    
    .
    """
    return _getUniversal(tc.TL_CURRENT_PHASE, tlsID)

def getNextSwitch(tlsID):
    """getNextSwitch(string) -> integer
    
    .
    """
    return _getUniversal(tc.TL_NEXT_SWITCH, tlsID)


def subscribe(tlsID, varIDs=(tc.TL_CURRENT_PHASE,), begin=0, end=2**31-1):
    """subscribe(string, list(integer), double, double) -> None
    
    Subscribe to one or more traffic light values for the given interval.
    A call to this method clears all previous subscription results.
    """
    subscriptionResults.reset()
    trace._subscribe(tc.CMD_SUBSCRIBE_TL_VARIABLE, begin, end, tlsID, varIDs)

def getSubscriptionResults(tlsID=None):
    """getSubscriptionResults(string) -> dict(integer: <value_type>)
    
    Returns the subscription results for the last time step and the given traffic light.
    If no traffic light id is given, all subscription results are returned in a dict.
    If the traffic light id is unknown or the subscription did for any reason return no data,
    'None' is returned.
    It is not possible to retrieve older subscription results than the ones
    from the last time step.
    """
    return subscriptionResults.get(tlsID)

def subscribeContext(tlsID, domain, dist, varIDs=(tc.TL_CURRENT_PHASE,), begin=0, end=2**31-1):
    subscriptionResults.reset()
    trace._subscribeContext(tc.CMD_SUBSCRIBE_TL_CONTEXT, begin, end, tlsID, domain, dist, varIDs)

def getContextSubscriptionResults(tlsID=None):
    return subscriptionResults.getContext(tlsID)


def setRedYellowGreenState(tlsID, state):
    trace._sendStringCmd(tc.CMD_SET_TL_VARIABLE, tc.TL_RED_YELLOW_GREEN_STATE, tlsID, state)

def setPhase(tlsID, index):
    trace._sendIntCmd(tc.CMD_SET_TL_VARIABLE, tc.TL_PHASE_INDEX, tlsID, index)

def setProgram(tlsID, programID):
    trace._sendStringCmd(tc.CMD_SET_TL_VARIABLE, tc.TL_PROGRAM, tlsID, programID)

def setPhaseDuration(tlsID, phaseDuration):
    trace._sendIntCmd(tc.CMD_SET_TL_VARIABLE, tc.TL_PHASE_DURATION, tlsID, int(1000*phaseDuration))

def setCompleteRedYellowGreenDefinition(tlsID, tls):
    length = 1+4 + 1+4+len(tls._subID) + 1+4 + 1+4 + 1+4 + 1+4 # tls parameter
    itemNo = 1+1+1+1+1
    for p in tls._phases:
        length += 1+4 + 1+4 + 1+4 + 1+4+len(p._phaseDef)
        itemNo += 4
    trace._beginMessage(tc.CMD_SET_TL_VARIABLE, tc.TL_COMPLETE_PROGRAM_RYG, tlsID, length)
    trace._message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, itemNo)
    trace._message.string += struct.pack("!Bi", tc.TYPE_STRING, len(tls._subID)) + tls._subID # programID
    trace._message.string += struct.pack("!Bi", tc.TYPE_INTEGER, 0) # type
    trace._message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, 0) # subitems
    trace._message.string += struct.pack("!Bi", tc.TYPE_INTEGER, tls._currentPhaseIndex) # index
    trace._message.string += struct.pack("!Bi", tc.TYPE_INTEGER, len(tls._phases)) # phaseNo
    for p in tls._phases:
        trace._message.string += struct.pack("!BiBiBi", tc.TYPE_INTEGER, p._duration, tc.TYPE_INTEGER, p._duration1, tc.TYPE_INTEGER, p._duration2)
        trace._message.string += struct.pack("!Bi", tc.TYPE_STRING, len(p._phaseDef)) + p._phaseDef
    trace._sendExact()
