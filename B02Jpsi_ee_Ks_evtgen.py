#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Descriptor: [B0 -> (KS0 -> pi+ pi-) (J/psi(1S) -> e+ e-)]

#############################################################
# This steering file generates, simulates, and reconstructs
# a sample of n events of signal MC.
#
# Usage: basf2 example.py
#
# Input: None
# Output: output.root
#
# Example steering file - 2011 Belle II Collaboration
#############################################################
#The directories might need to be modified
from basf2 import *
from simulation import add_simulation
from reconstruction import add_reconstruction, add_mdst_output
from HLTTrigger import add_HLT_Y4S
from ROOT import Belle2
import glob

# background (collision) files
bg = glob.glob('/group/belle2/users/harat/basf2/cvmfsv000700/BG/set1/[A-Z]*.root')  # if you run at KEKCC
#bg = glob.glob('./BG/[A-Z]*.root')

# create path
main = create_path()

reset_database()
use_local_database(Belle2.FileSystem.findFile("data/framework/database.txt"),
                   "", True, LogLevel.ERROR)

# specify number of events to be generated
eventinfosetter = register_module('EventInfoSetter')
eventinfosetter.param('evtNumList', [90000])  # we want to process 1000 events
#eventinfosetter.param('runList', [1])  # from run number 1
#eventinfosetter.param('expList', [1])  # and experiment number 1
main.add_module(eventinfosetter)

# generate BBbar events
evtgeninput = register_module('EvtGenInput')
evtgeninput.param('userDECFile', './B0_jpsieeKs.dec')
main.add_module(evtgeninput)

# detecor simulation
# add_simulation(main)
add_simulation(main, bkgfiles=bg)
# or add_simulation(main, components) to simulate a selection of detectors

# HLT L3 simulation
main.add_module('Level3')

# reconstruction
add_reconstruction(main)
# or add_reconstruction(main, components) to run the reconstruction of a selection of detectors

# HLT physics trigger
add_HLT_Y4S(main)

# output

output = register_module('RootOutput')
#output.param('updateFileCatalog', False)
output.param('outputFileName', 'output.root')
branches = [
    'Tracks',
    'V0s',
    'TrackFitResults',
    'PIDLikelihoods',
    'TracksToPIDLikelihoods',
    'ECLClusters',
    'ECLClustersToTracks',
    'KLMClusters',
    'KLMClustersToTracks',
    'TRGSummary',
    'MCParticles',
    'TracksToMCParticles',
    'ECLClustersToMCParticles',
    'KLMClustersToMCParticles'
]
persistentBranches = [
    'FileMetaData',
    'BackgroundInfos'
    ]
output.param('branchNames', branches)
output.param('branchNamesPersistent', persistentBranches)
main.add_module(output)

# display a progress bar while running
# main.add_module('ProgressBar')

# process events and print call statistics
set_log_level(LogLevel.ERROR)
process(main)
print(statistics)
