/// RPV displaced vertices

#ifndef RPVDISPVRT_RPVDISPVTX_H
#define RPVDISPVRT_RPVDISPVTX_H

/** Base class */
#include "AthenaBaseComps/AthAlgorithm.h"

/** Gaudi */
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODTracking/Vertex.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"

/** forward declarations */

class RPVDispVrtTrackSelector;
class RPVDispVrtVertexSelector;
class RPVDispVrtVertexer;
class RPVDispVrtVertexCleanup;

class RPVDispVrt : public AthAlgorithm
{
public:

  //! Constructor.
  RPVDispVrt(const std::string &name, ISvcLocator *pSvcLocator);

  //! Initialize
  virtual StatusCode initialize();

  //! Execute
  virtual StatusCode execute();

  //! Finalize
  virtual StatusCode finalize();

  //! Destructur
  ~RPVDispVrt() 
    { }

private:

  ToolHandle<RPVDispVrtTrackSelector> m_trkTool;
  ToolHandle<RPVDispVrtVertexer> m_vtxTool;
  ToolHandle<RPVDispVrtVertexCleanup> m_vtxClean;
  ToolHandle<RPVDispVrtVertexSelector> m_vtxSelector;

  std::string m_trackCollName;
  std::string m_primVtxName;
  std::string m_secVtxName;

  bool m_randomizeTracks;
  bool m_useAllPrimVtx;
  bool m_materialMapOnly;
  bool m_gt2trkOnly;


};

#endif
