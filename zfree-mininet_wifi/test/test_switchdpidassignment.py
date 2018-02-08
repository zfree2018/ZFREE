#!/usr/bin/env python

"""Package: mininet
   Regression tests for ap dpid assignment."""

import unittest
import sys

from mininet.net import Mininet
from mininet.node import Station, Controller
from mininet.node import (UserAP, OVSAP, IVSSwitch)
from mininet.topo import Topo
from mininet.log import setLogLevel
from mininet.util import quietRun
from mininet.clean import cleanup


class TestSwitchDpidAssignmentOVS(unittest.TestCase):
    "Verify Switch dpid assignment."

    accessPointClass = OVSAP  # overridden in subclasses

    def tearDown(self):
        "Clean up if necessary"
        # satisfy pylint
        assert self
        if sys.exc_info != (None, None, None):
            cleanup()

    def testDefaultDpid(self):
        """Verify that the default dpid is assigned using a valid provided
        canonical apname if no dpid is passed in ap creation."""
        ap = Mininet(topo=Topo(),
                          accessPoint=self.accessPointClass,
                          station=Station, controller=Controller, isWiFi=True).addAccessPoint('ap1')
        self.assertEqual(ap.defaultDpid(), ap.dpid)

    def dpidFrom(self, num):
        "Compute default dpid from number"
        fmt = ('%0' + str(self.accessPointClass.dpidLen) + 'x')
        return fmt % num

    def testActualDpidAssignment(self):
        """Verify that AP dpid is the actual dpid assigned if dpid is
        passed in ap creation."""
        dpid = self.dpidFrom(0xABCD)
        ap = Mininet(topo=Topo(), accessPoint=self.accessPointClass,
                          station=Station, controller=Controller, isWiFi=True).addAccessPoint(
                            'ap1', dpid=dpid)
        self.assertEqual(ap.dpid, dpid)

    #def testDefaultDpidAssignmentFailure(self):
    #    """Verify that Default dpid assignment raises an Exception if the
    #    name of the ap does not contain a digit. Also verify the
    #    exception message."""
    #    with self.assertRaises(Exception) as raises_cm:
    #        Mininet(topo=Topo(), accessPoint=self.accessPointClass,
    #                 station=Station, controller=Controller, isWiFi=True).addAccessPoint('A')
    #    self.assertEqual(raises_cm.exception.message, 'Unable to derive '
    #                     'default datapath ID - please either specify a dpid '
    #                     'or use a canonical ap name such as ap23.')

    def testDefaultDpidLen(self):
        """Verify that Default dpid length is 16 characters consisting of
        16 - len(hex of first string of contiguous digits passed in ap
        name) 0's followed by hex of first string of contiguous digits passed
        in ap name."""
        ap = Mininet(topo=Topo(), accessPoint=self.accessPointClass,
                          station=Station, controller=Controller, isWiFi=True).addAccessPoint('ap123')

        self.assertEqual(ap.dpid, self.dpidFrom(123))

class OVSUser(OVSAP):
    "OVS User AP convenience class"
    def __init__(self, *args, **kwargs):
        kwargs.update(datapath='user')
        OVSAP.__init__(self, *args, **kwargs)

class testSwitchOVSUser(TestSwitchDpidAssignmentOVS):
    "Test dpid assignnment of OVS User AP."
    accessPointClass = OVSUser

@unittest.skipUnless(quietRun('which ivs-ctl'),
                      'IVS ap is not installed')
class testSwitchIVS(TestSwitchDpidAssignmentOVS):
    "Test dpid assignment of IVS ap."
    accessPointClass = IVSSwitch

@unittest.skipUnless(quietRun('which ofprotocol'),
                      'Reference user ap is not installed')
class testSwitchUserspace(TestSwitchDpidAssignmentOVS):
    "Test dpid assignment of Userspace ap."
    accessPointClass = UserAP

if __name__ == '__main__':
    setLogLevel('warning')
    unittest.main()
