# -*- coding: utf-8 -*-
"""
@file    lane.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@date    2011-03-17
@version $Id: lane.py 13857 2013-05-02 19:55:59Z behrisch $

Python implementation of the TraCI interface.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import struct
import trace
import constants as tc

def _readLinks(result):
    result.read("!Bi") # Type Compound, Length
    nbLinks = result.readInt()
    links = []
    for i in range(nbLinks):
        result.read("!B")                           # Type String
        approachedLane = result.readString()
        result.read("!B")                           # Type String
        approachedInternal = result.readString()
        result.read("!B")                           # Type Byte
        hasPrio = bool(result.read("!B"))
        result.read("!B")                           # Type Byte
        isOpen = bool(result.read("!B"))
        result.read("!B")                           # Type Byte
        hasFoe = bool(result.read("!B"))
        result.read("!B")                           # Type String
        state = result.readString() #not implemented
        result.read("!B")                           # Type String
        direction = result.readString() #not implemented
        result.read("!B")                           # Type Float
        length = result.readDouble()
        links.append((approachedLane, hasPrio, isOpen, hasFoe))
    return links


_RETURN_VALUE_FUNC = {tc.ID_LIST:                   trace.Storage.readStringList,
                      tc.VAR_LENGTH:                trace.Storage.readDouble,
                      tc.VAR_MAXSPEED:              trace.Storage.readDouble,
                      tc.VAR_WIDTH:                 trace.Storage.readDouble,
                      tc.LANE_ALLOWED:              trace.Storage.readStringList,
                      tc.LANE_DISALLOWED:           trace.Storage.readStringList,
                      tc.LANE_LINK_NUMBER:          lambda result: result.read("!B")[0],
                      tc.LANE_LINKS:                _readLinks,
                      tc.VAR_SHAPE:                 trace.Storage.readShape,
                      tc.LANE_EDGE_ID:              trace.Storage.readString,
                      tc.VAR_CO2EMISSION:           trace.Storage.readDouble,
                      tc.VAR_COEMISSION:            trace.Storage.readDouble,
                      tc.VAR_HCEMISSION:            trace.Storage.readDouble,
                      tc.VAR_PMXEMISSION:           trace.Storage.readDouble,
                      tc.VAR_NOXEMISSION:           trace.Storage.readDouble,
                      tc.VAR_FUELCONSUMPTION:       trace.Storage.readDouble,
                      tc.VAR_NOISEEMISSION:         trace.Storage.readDouble,
                      tc.LAST_STEP_MEAN_SPEED:      trace.Storage.readDouble,
                      tc.LAST_STEP_OCCUPANCY:       trace.Storage.readDouble,
                      tc.LAST_STEP_LENGTH:          trace.Storage.readDouble,
                      tc.VAR_CURRENT_TRAVELTIME:    trace.Storage.readDouble,
                      tc.LAST_STEP_VEHICLE_NUMBER:  trace.Storage.readInt,
                      tc.LAST_STEP_VEHICLE_HALTING_NUMBER: trace.Storage.readInt,
                      tc.LAST_STEP_VEHICLE_ID_LIST: trace.Storage.readStringList}
subscriptionResults = trace.SubscriptionResults(_RETURN_VALUE_FUNC)

def _getUniversal(varID, laneID):
    result = trace._sendReadOneStringCmd(tc.CMD_GET_LANE_VARIABLE, varID, laneID)
    return _RETURN_VALUE_FUNC[varID](result)

def getIDList():
    """getIDList() -> list(string)
    
    Returns a list of all lanes in the network.
    """
    return _getUniversal(tc.ID_LIST, "")

def getLength(laneID):
    """getLength(string) -> double
    
    Returns the length in m.
    """
    return _getUniversal(tc.VAR_LENGTH, laneID)

def getMaxSpeed(laneID):
    """getMaxSpeed(string) -> double
    
    Returns the maximum allowed speed on the lane in m/s.
    """
    return _getUniversal(tc.VAR_MAXSPEED, laneID)

def getWidth(laneID):
    """getWidth(string) -> double
    
    Returns the width of the lane in m.
    """
    return _getUniversal(tc.VAR_WIDTH, laneID)

def getAllowed(laneID):
    """getAllowed(string) -> list(string)
    
    Returns a list of allowed vehicle classes. An empty list means all vehicles are allowed.
    """
    return _getUniversal(tc.LANE_ALLOWED, laneID)

def getDisallowed(laneID):
    """getDisallowed(string) -> list(string)
    
    Returns a list of disallowed vehicle classes.
    """
    return _getUniversal(tc.LANE_DISALLOWED, laneID)

def getLinkNumber(laneID):
    """getLinkNumber(string) -> integer
    
    Returns the number of connections to successive lanes.
    """
    return _getUniversal(tc.LANE_LINK_NUMBER, laneID)

def getLinks(laneID):
    """getLinks(string) -> list((string, bool, bool, bool))
    
    A list containing ids of successor lanes together with priority, open and foe.
    """
    return _getUniversal(tc.LANE_LINKS, laneID)

def getShape(laneID):
    """getShape(string) -> list((double, double))
    
    List of 2D positions (cartesian) describing the geometry.
    """
    return _getUniversal(tc.VAR_SHAPE, laneID)

def getEdgeID(laneID):
    """getEdgeID(string) -> string
    
    Returns the id of the edge the lane belongs to.
    """
    return _getUniversal(tc.LANE_EDGE_ID, laneID)

def getCO2Emission(laneID):
    """getCO2Emission(string) -> double
    
    Returns the CO2 emission in mg for the last time step on the given lane.
    """
    return _getUniversal(tc.VAR_CO2EMISSION, laneID)

def getCOEmission(laneID):
    """getCOEmission(string) -> double
    
    Returns the CO emission in mg for the last time step on the given lane.
    """
    return _getUniversal(tc.VAR_COEMISSION, laneID)

def getHCEmission(laneID):
    """getHCEmission(string) -> double
    
    Returns the HC emission in mg for the last time step on the given lane.
    """
    return _getUniversal(tc.VAR_HCEMISSION, laneID)

def getPMxEmission(laneID):
    """getPMxEmission(string) -> double
    
    Returns the particular matter emission in mg for the last time step on the given lane.
    """
    return _getUniversal(tc.VAR_PMXEMISSION, laneID)

def getNOxEmission(laneID):
    """getNOxEmission(string) -> double
    
    Returns the NOx emission in mg for the last time step on the given lane.
    """
    return _getUniversal(tc.VAR_NOXEMISSION, laneID)

def getFuelConsumption(laneID):
    """getFuelConsumption(string) -> double
    
    Returns the fuel consumption in ml for the last time step on the given lane.
    """
    return _getUniversal(tc.VAR_FUELCONSUMPTION, laneID)

def getNoiseEmission(laneID):
    """getNoiseEmission(string) -> double
    
    Returns the noise emission in db for the last time step on the given lane.
    """
    return _getUniversal(tc.VAR_NOISEEMISSION, laneID)

def getLastStepMeanSpeed(laneID):
    """getLastStepMeanSpeed(string) -> double
    
    Returns the average speed in m/s for the last time step on the given lane.
    """
    return _getUniversal(tc.LAST_STEP_MEAN_SPEED, laneID)

def getLastStepOccupancy(laneID):
    """getLastStepOccupancy(string) -> double
    
    Returns the occupancy in % for the last time step on the given lane.
    """
    return _getUniversal(tc.LAST_STEP_OCCUPANCY, laneID)

def getLastStepLength(laneID):
    """getLastStepLength(string) -> double
    
    Returns the total vehicle length in m for the last time step on the given lane.
    """
    return _getUniversal(tc.LAST_STEP_LENGTH, laneID)

def getTraveltime(laneID):
    """getTraveltime(string) -> double
    
    Returns the estimated travel time in s for the last time step on the given lane.
    """
    return _getUniversal(tc.VAR_CURRENT_TRAVELTIME, laneID)

def getLastStepVehicleNumber(laneID):
    """getLastStepVehicleNumber(string) -> integer
    
    Returns the total number of vehicles for the last time step on the given lane.
    """
    return _getUniversal(tc.LAST_STEP_VEHICLE_NUMBER, laneID)

def getLastStepHaltingNumber(laneID):
    """getLastStepHaltingNumber(string) -> integer
    
    Returns the total number of halting vehicles for the last time step on the given lane.
    A speed of less than 0.1 m/s is considered a halt.
    """
    return _getUniversal(tc.LAST_STEP_VEHICLE_HALTING_NUMBER, laneID)

def getLastStepVehicleIDs(laneID):
    """getLastStepVehicleIDs(string) -> list(string)
    
    Returns the ids of the vehicles for the last time step on the given lane.
    """
    return _getUniversal(tc.LAST_STEP_VEHICLE_ID_LIST, laneID)


def subscribe(laneID, varIDs=(tc.LAST_STEP_VEHICLE_NUMBER,), begin=0, end=2**31-1):
    """subscribe(string, list(integer), double, double) -> None
    
    Subscribe to one or more lane values for the given interval.
    A call to this method clears all previous subscription results.
    """
    subscriptionResults.reset()
    trace._subscribe(tc.CMD_SUBSCRIBE_LANE_VARIABLE, begin, end, laneID, varIDs)

def getSubscriptionResults(laneID=None):
    """getSubscriptionResults(string) -> dict(integer: <value_type>)
    
    Returns the subscription results for the last time step and the given lane.
    If no lane id is given, all subscription results are returned in a dict.
    If the lane id is unknown or the subscription did for any reason return no data,
    'None' is returned.
    It is not possible to retrieve older subscription results than the ones
    from the last time step.
    """
    return subscriptionResults.get(laneID)

def subscribeContext(laneID, domain, dist, varIDs=(tc.LAST_STEP_VEHICLE_NUMBER,), begin=0, end=2**31-1):
    subscriptionResults.reset()
    trace._subscribeContext(tc.CMD_SUBSCRIBE_LANE_CONTEXT, begin, end, laneID, domain, dist, varIDs)

def getContextSubscriptionResults(laneID=None):
    return subscriptionResults.getContext(laneID)


def setAllowed(laneID, allowedClasses):
    trace._beginMessage(tc.CMD_SET_LANE_VARIABLE, tc.LANE_ALLOWED, laneID, 1+4+sum(map(len, allowedClasses))+4*len(allowedClasses))
    trace._message.string += struct.pack("!Bi", tc.TYPE_STRINGLIST, len(allowedClasses))
    for c in allowedClasses:
        trace._message.string += struct.pack("!i", len(c)) + c
    trace._sendExact()

def setDisallowed(laneID, disallowedClasses):
    trace._beginMessage(tc.CMD_SET_LANE_VARIABLE, tc.LANE_DISALLOWED, laneID, 1+4+sum(map(len, disallowedClasses))+4*len(disallowedClasses))
    trace._message.string += struct.pack("!Bi", tc.TYPE_STRINGLIST, len(disallowedClasses))
    for c in disallowedClasses:
        trace._message.string += struct.pack("!i", len(c)) + c
    trace._sendExact()

def setMaxSpeed(laneID, speed):
    trace._sendDoubleCmd(tc.CMD_SET_LANE_VARIABLE, tc.VAR_MAXSPEED, laneID, speed)

def setLength(laneID, length):
    trace._sendDoubleCmd(tc.CMD_SET_LANE_VARIABLE, tc.VAR_LENGTH, laneID, length)
