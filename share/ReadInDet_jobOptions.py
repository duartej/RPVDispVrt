#--------------------------------------------------------------
# control input/output
#--------------------------------------------------------------
# --- specify input type
readESD = False
readAOD = True
# --- controls what is written out. ESD includes AOD, so it's normally enough
doWriteESD = False
doWriteAOD = False

#--------------------------------------------------------------
# control algorithms to be rerun
#--------------------------------------------------------------
# --- run InDetRecStatistics (only possible if readESD = True)
doInDetRecStatistics = True and readESD
# --- refit the tracks in ESD (only possible if readESD = True)
doRefitTracks = False and readESD
# --- redo primary vertexing
reDoPrimaryVertexing = False
# --- redo particle creation (recommended after revertexing on ESD, otherwise trackparticles are inconsistent)
reDoParticleCreation = False and readESD and reDoPrimaryVertexing
# --- redo conversion finding
reDoConversions = True
# --- run on the 13.0.30.3 reference ESD or AOD
read13Reference = False

#--------------------------------------------------------------
# Control - standard options (as in jobOptions.py)
#--------------------------------------------------------------
# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = INFO
# --- produce an atlantis data file
doJiveXML       = False
# --- run the Virtual Point 1 event visualisation
doVP1           = False
# --- do auditors ?
doAuditors      = False

import os
if os.environ['CMTCONFIG'].endswith('-dbg'):
	# --- do EDM monitor (debug mode only)
	doEdmMonitor    = True
	# --- write out a short message upon entering or leaving each algorithm
	doNameAuditor   = True
else:
	doEdmMonitor    = False
	doNameAuditor   = False
	
	# safety section ... redoing vertexing is a tricky business to stay consistent ...
	if not (readESD or readAOD):
		print "You have to turn on reading of ESD or AOD! That's the purpose of this jobO!"
		if readESD and readAOD:
			print "I can either read ESD or AOD but not both at the same time! Turn on or the other off!"
			if readESD and reDoPrimaryVertexing and not reDoParticleCreation:
				print "INFO! You are running on ESD, redoing the vertexing but not recreating the TrackParticles!"
				print "INFO! To avoid inconsistencies do not use the old track particles in conjunction with the new vertex!"
				if doWriteESD or doWriteAOD:
					print "INFO! To avoid inconsistencies the old track particle (truth) container will not be in the new ESD/AOD!"
					if readAOD and reDoPrimaryVertexing:
						print "INFO! You are running on AOD, and redoing the vertexing. At the moment new track particles cannot be made from old ones."
						print "INFO! To avoid inconsistencies do not use the old track particles in conjunction with the new vertex!"
						if doWriteAOD:
							print "INFO! To avoid inconsistencies the old track particle (truth) container will not be in the new AOD!"
							if doRefitTracks and (reDoPrimaryVertexing or reDoParticleCreation):
								print "INFO! You are refitting tracks and also revertex and/or recreate track particles"
								print "INFO! The input for that will be the refitted tracks!"
								
								#--------------------------------------------------------------
								# detector description version
								#--------------------------------------------------------------
								DetDescrVersion = "ATLAS-GEO-02-01-00"
								if read13Reference:
									DetDescrVersion = "ATLAS-CSC-01-02-00"
									
									#--------------------------------------------------------------
									# load Global Flags and set defaults (import the new jobProperty globalflags)
									#--------------------------------------------------------------
									
									from AthenaCommon.GlobalFlags import globalflags
									# --- default is atlas geometry
									globalflags.DetGeo = 'atlas'
									# --- set defaults
									globalflags.DataSource  = 'geant4'
									globalflags.InputFormat = 'pool'
									# --- set geometry version
									globalflags.DetDescrVersion = DetDescrVersion
									# --- printout
									globalflags.print_JobProperties()
									
									from AthenaCommon.BeamFlags import jobproperties
									# --- default is zero luminosity
									jobproperties.Beam.numberOfCollisions = 0.0
									
									#--------------------------------------------------------------
									# Set Detector setup
									#--------------------------------------------------------------
									
									from AthenaCommon.DetFlags import DetFlags
									# --- switch on InnerDetector
									DetFlags.ID_setOn()
									# --- and switch off all the rest
									DetFlags.Calo_setOff()
									DetFlags.Muon_setOff()
									# ---- switch parts of ID off/on as follows
									#DetFlags.pixel_setOff()
									#DetFlags.SCT_setOff()
									#DetFlags.TRT_setOff()
									# --- printout
									DetFlags.Print()
									
									#--------------------------------------------------------------
									# Load Reconstruction configuration for tools only
									#--------------------------------------------------------------
									#--------------------------------------------------------------
									# Load InDet configuration
									#--------------------------------------------------------------
									
									# --- setup InDetJobProperties
									from InDetRecExample.InDetJobProperties import InDetFlags
									InDetFlags.preProcessing      = False
									InDetFlags.doiPatRec          = False
									InDetFlags.doxKalman          = False
									InDetFlags.doNewTracking      = False
									InDetFlags.doLowPt            = False
									InDetFlags.doBackTracking     = False
									InDetFlags.doTRTStandalone    = False
									InDetFlags.doTrtSegments      = False
									InDetFlags.postProcessing     = reDoPrimaryVertexing or reDoParticleCreation or reDoConversions or doInDetRecStatistics
									InDetFlags.doTruth            = True
									InDetFlags.doSlimming         = False
									InDetFlags.loadTools          = True
									InDetFlags.doVertexFinding    = reDoPrimaryVertexing
									InDetFlags.doParticleCreation = reDoParticleCreation
									InDetFlags.doConversions      = reDoConversions
									InDetFlags.doSecVertexFinder  = reDoConversions
									InDetFlags.doV0Finder         = False
									InDetFlags.doTrkNtuple        = False
									InDetFlags.doPixelTrkNtuple   = False
									InDetFlags.doVtxNtuple        = False and readAOD # for now only possible on AOD
									InDetFlags.doConvVtxNtuple    = False
									InDetFlags.doRefit            = doRefitTracks
									InDetFlags.doPrintConfigurables = True
									
									# --- activate (memory/cpu) monitoring
                                                                        #InDetFlags.doPerfMon = True
									
									# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
									# This way RecExCommon just needs to import the properties without doing anything else!
									# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

									# next section assures that keys are ok and nothing inconsistent is written out ...
									print "InDetRec_jobOptions: InDetKeys not set - setting to defaults"
									from InDetRecExample.InDetKeys import InDetKeys
									if InDetFlags.doVertexFinding():
										print "reDoPrimaryVertexing enabled: adjusting some StoreGate keys:"
										
										InDetKeys.PrimaryVertices = "New"+InDetKeys.PrimaryVertices()
										print "InDetKeys.PrimaryVertices = "+InDetKeys.PrimaryVertices()
										
										if readAOD: # this one is mutally exclusive with useRefittedTrack.. and the doParticleCreation flag
											InDetKeys.Tracks = InDetKeys.TrackParticles()
											print "InDetKeys.Tracks = "+InDetKeys.Tracks()
											
											if InDetFlags.doParticleCreation():
												InDetKeys.TrackParticles      = "New"+InDetKeys.TrackParticles()
												InDetKeys.TrackParticlesTruth = "New"+InDetKeys.TrackParticlesTruth()
												print "InDetKeys.TrackParticles      = "+InDetKeys.TrackParticles()
												print "InDetKeys.TrackParticlesTruth = "+InDetKeys.TrackParticlesTruth()
											else:
												InDetKeys.TrackParticles      = "Dummy"
												InDetKeys.TrackParticlesTruth = "Dummy"
												
												if InDetFlags.doConversions():
													InDetKeys.Conversions = "New"+InDetKeys.Conversions()
													InDetKeys.SecVertices = "New"+InDetKeys.SecVertices()
													
													print "Printing InDetKeys. Be aware that some might be adjusted lateron!"
													InDetKeys.print_JobProperties()
													
										                        # --------------------------------------------------------------
													# enable statistics for reading ESD testing
										                        # --------------------------------------------------------------
										      
													InDetFlags.doStatistics   = doInDetRecStatistics
													InDetKeys.StatNtupleName = "InDetRecStatistics.root"
													TrackCollectionKeys        = [InDetKeys.Tracks()]
													TrackCollectionTruthKeys   = [InDetKeys.TracksTruth()]
													
										                        # --------------------------------------------------------------
													# load master joboptions file
													# --------------------------------------------------------------

													include("InDetRecExample/InDetRec_all.py")
