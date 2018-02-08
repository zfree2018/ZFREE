"""
@file    color.py
@author  Daniel Krajzewicz
@date    2012-12-04
@version $Id: color.py 13811 2013-05-01 20:31:43Z behrisch $

Library for reading and encoding of colors.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2010-2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""

from xml.sax import handler, parse


class RGBAColor:
    def __init__(self, r, g, b, a=None):
        self.r = r
        self.g = g
        self.b = b
        self.a = a
    def toXML(self):
        if self.a!=None:
            return "%s,%s,%s,%s" % (self.r, self.g, self.b, self.a)
        else:
            return "%s,%s,%s" % (self.r, self.g, self.b)
    def decodeXML(c):
        return [float(x) for x in c.split(",")]
        
          
