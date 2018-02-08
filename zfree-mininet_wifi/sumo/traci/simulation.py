# -*- coding: utf-8 -*-
"""
@file    simulation.py
@author  Michael Behrisch
@date    2011-03-15
@version $Id: simulation.py 13811 2013-05-01 20:31:43Z behrisch $

Python implementation of the TraCI interface.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2008-2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import struct
import trace
import constants as tc

_RETURN_VALUE_FUNC = {tc.VAR_TIME_STEP:                         trace.Storage.readInt,
                      tc.VAR_LOADED_VEHICLES_NUMBER:            trace.Storage.readInt,
                      tc.VAR_LOADED_VEHICLES_IDS:               trace.Storage.readStringList,
                      tc.VAR_DEPARTED_VEHICLES_NUMBER:          trace.Storage.readInt,
                      tc.VAR_DEPARTED_VEHICLES_IDS:             trace.Storage.readStringList,
                      tc.VAR_ARRIVED_VEHICLES_NUMBER:           trace.Storage.readInt,
                      tc.VAR_ARRIVED_VEHICLES_IDS:              trace.Storage.readStringList,
                      tc.VAR_MIN_EXPECTED_VEHICLES:             trace.Storage.readInt,
                      tc.VAR_TELEPORT_STARTING_VEHICLES_NUMBER: trace.Storage.readInt,
                      tc.VAR_TELEPORT_STARTING_VEHICLES_IDS:    trace.Storage.readStringList,
                      tc.VAR_TELEPORT_ENDING_VEHICLES_NUMBER:   trace.Storage.readInt,
                      tc.VAR_TELEPORT_ENDING_VEHICLES_IDS:      trace.Storage.readStringList,
                      tc.VAR_DELTA_T:                           trace.Storage.readInt,
                      tc.VAR_NET_BOUNDING_BOX:                  lambda result: (result.read("!dd"), result.read("!dd"))}
subscriptionResults = trace.SubscriptionResults(_RETURN_VALUE_FUNC)

def _getUniversal(varID):
    result = trace._sendReadOneStringCmd(tc.CMD_GET_SIM_VARIABLE, varID, "")
    return _RETURN_VALUE_FUNC[varID](result)

def getCurrentTime():
    """getCurrentTime() -> integer
    
    .
    """
    return _getUniversal(tc.VAR_TIME_STEP)

def getLoadedNumber():
    """getLoadedNumber() -> integer
    
    .
    """
    return _getUniversal(tc.VAR_LOADED_VEHICLES_NUMBER)

def getLoadedIDList():
    """getLoadedIDList() -> list(string)
    
    .
    """
    return _getUniversal(tc.VAR_LOADED_VEHICLES_IDS)

def getDepartedNumber():
    """getDepartedNumber() -> integer
    returns the number vehicles which departed in the last time step.
    """
    return _getUniversal(tc.VAR_DEPARTED_VEHICLES_NUMBER)

def getDepartedIDList():
    """getDepartedIDList() -> list(string)
    returns the list of ids of all vehicles which departed in the last time step.
    """
    return _getUniversal(tc.VAR_DEPARTED_VEHICLES_IDS)

def getArrivedNumber():
    """getArrivedNumber() -> integer
    returns the number vehicles which arrived in the last time step.
    """
    return _getUniversal(tc.VAR_ARRIVED_VEHICLES_NUMBER)

def getArrivedIDList():
    """getArrivedIDList() -> list(string)
    returns the list of ids of all vehicles which arrived in the last time step.
    """
    return _getUniversal(tc.VAR_ARRIVED_VEHICLES_IDS)

def getMinExpectedNumber():
    """getMinExpectedNumber() -> integer
    
    .
    """
    return _getUniversal(tc.VAR_MIN_EXPECTED_VEHICLES)

def getStartingTeleportNumber():
    """getStartingTeleportNumber() -> integer
    
    .
    """
    return _getUniversal(tc.VAR_TELEPORT_STARTING_VEHICLES_NUMBER)

def getStartingTeleportIDList():
    """getStartingTeleportIDList() -> list(string)
    
    .
    """
    return _getUniversal(tc.VAR_TELEPORT_STARTING_VEHICLES_IDS)

def getEndingTeleportNumber():
    """getEndingTeleportNumber() -> integer
    
    .
    """
    return _getUniversal(tc.VAR_TELEPORT_ENDING_VEHICLES_NUMBER)

def getEndingTeleportIDList():
    """getEndingTeleportIDList() -> list(string)
    
    .
    """
    return _getUniversal(tc.VAR_TELEPORT_ENDING_VEHICLES_IDS)

def getDeltaT():
    """getDeltaT() -> integer
    
    .
    """
    return _getUniversal(tc.VAR_DELTA_T)

def getNetBoundary():
    """getNetBoundary() -> ((double, double), (double, double))
    
    .
    """
    return _getUniversal(tc.VAR_NET_BOUNDING_BOX)

def convert2D(edgeID, pos, laneIndex=0, toGeo=False):
    posType = tc.POSITION_2D
    if toGeo:
        posType = tc.POSITION_LAT_LON
    trace._beginMessage(tc.CMD_GET_SIM_VARIABLE, tc.POSITION_CONVERSION, "", 1+4 + 1+4+len(edgeID)+8+1 + 1+1)
    trace._message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, 2)
    trace._message.string += struct.pack("!Bi", tc.POSITION_ROADMAP, len(edgeID)) + edgeID
    trace._message.string += struct.pack("!dBBB", pos, laneIndex, tc.TYPE_UBYTE, posType)
    return trace._checkResult(tc.CMD_GET_SIM_VARIABLE, tc.POSITION_CONVERSION, "").read("!dd")

def convertRoad(x, y, isGeo=False):
    posType = tc.POSITION_2D
    if isGeo:
        posType = tc.POSITION_LAT_LON
    trace._beginMessage(tc.CMD_GET_SIM_VARIABLE, tc.POSITION_CONVERSION, "", 1+4 + 1+8+8 + 1+1)
    trace._message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, 2)
    trace._message.string += struct.pack("!Bdd", posType, x, y)
    trace._message.string += struct.pack("!BB", tc.TYPE_UBYTE, tc.POSITION_ROADMAP)
    result = trace._checkResult(tc.CMD_GET_SIM_VARIABLE, tc.POSITION_CONVERSION, "")
    return result.readString(), result.readDouble(), result.read("!B")[0]

def convertGeo(x, y, fromGeo=False):
    fromType = tc.POSITION_2D
    toType = tc.POSITION_LAT_LON
    if fromGeo:
        fromType = tc.POSITION_LAT_LON
        toType = tc.POSITION_2D
    trace._beginMessage(tc.CMD_GET_SIM_VARIABLE, tc.POSITION_CONVERSION, "", 1+4 + 1+8+8 + 1+1)
    trace._message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, 2)
    trace._message.string += struct.pack("!Bdd", fromType, x, y)
    trace._message.string += struct.pack("!BB", tc.TYPE_UBYTE, toType)
    return trace._checkResult(tc.CMD_GET_SIM_VARIABLE, tc.POSITION_CONVERSION, "").read("!dd")

def getDistance2D(x1, y1, x2, y2, isGeo=False, isDriving=False):
    """getDistance2D(double, double, double, double, boolean, boolean) -> double
    
    .
    """
    posType = tc.POSITION_2D
    if isGeo:
        posType = tc.POSITION_LAT_LON
    distType = tc.REQUEST_AIRDIST
    if isDriving:
        distType = tc.REQUEST_DRIVINGDIST
    trace._beginMessage(tc.CMD_GET_SIM_VARIABLE, tc.DISTANCE_REQUEST, "", 1+4 + 1+8+8 + 1+8+8 + 1)
    trace._message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, 3)
    trace._message.string += struct.pack("!Bdd", posType, x1, y1)
    trace._message.string += struct.pack("!BddB", posType, x2, y2, distType)
    return trace._checkResult(tc.CMD_GET_SIM_VARIABLE, tc.DISTANCE_REQUEST, "").readDouble()

def getDistanceRoad(edgeID1, pos1, edgeID2, pos2, isDriving=False):
    """getDistanceRoad(string, double, string, double, boolean) -> double
    
    .
    """
    distType = tc.REQUEST_AIRDIST
    if isDriving:
        distType = tc.REQUEST_DRIVINGDIST
    trace._beginMessage(tc.CMD_GET_SIM_VARIABLE, tc.DISTANCE_REQUEST, "", 1+4 + 1+4+len(edgeID1)+8+1 + 1+4+len(edgeID2)+8+1 + 1)
    trace._message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, 3)
    trace._message.string += struct.pack("!Bi", tc.POSITION_ROADMAP, len(edgeID1)) + edgeID1
    trace._message.string += struct.pack("!dBBi", pos1, 0, tc.POSITION_ROADMAP, len(edgeID2)) + edgeID2
    trace._message.string += struct.pack("!dBB", pos2, 0, distType)
    return trace._checkResult(tc.CMD_GET_SIM_VARIABLE, tc.DISTANCE_REQUEST, "").readDouble()


def subscribe(varIDs=(tc.VAR_DEPARTED_VEHICLES_IDS,), begin=0, end=2**31-1):
    """subscribe(list(integer), double, double) -> None
    
    Subscribe to one or more simulation values for the given interval.
    A call to this method clears all previous subscription results.
    """
    subscriptionResults.reset()
    trace._subscribe(tc.CMD_SUBSCRIBE_SIM_VARIABLE, begin, end, "x", varIDs)

def getSubscriptionResults():
    """getSubscriptionResults() -> dict(integer: <value_type>)
    
    Returns the subscription results for the last time step.
    It is not possible to retrieve older subscription results than the ones
    from the last time step.
    """
    return subscriptionResults.get("x")
