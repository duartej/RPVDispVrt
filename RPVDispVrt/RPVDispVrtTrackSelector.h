#ifndef RPVDispVrtTrackSelector_H
#define RPVDispVrtTrackSelector_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODTracking/VertexContainer.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "xAODTracking/TrackParticle.h"
#include "TTree.h"

namespace Trk {
  class Track;
}

class RPVDispVrtTrackSelector : virtual public AthAlgTool
{
 public:
  
  /** constructor */
  RPVDispVrtTrackSelector(const std::string& type, const std::string& name, const IInterface* parent ) ;
  /** destructor  */
  virtual ~RPVDispVrtTrackSelector(); 

  virtual StatusCode initialize();

  virtual StatusCode finalize();
  
  ///  virtual bool decision(const Trk::Track& track,const xAOD::Vertex* vertex=0) const;

  virtual bool decision(const xAOD::TrackParticle& track,const xAOD::Vertex* vertex=0) const;

  virtual bool decision(const xAOD::TrackParticle& track,const xAOD::VertexContainer* vertexColl) const;

  virtual bool passesTRTPixCut(const xAOD::TrackParticle& track) const;


  /** AlgTool InterfaceID */
  static const InterfaceID& interfaceID() ;  



 private:

  /* Track to vertex extrapolator **/
  ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;

  ToolHandle < Trk::ITrkVKalVrtFitter >  m_fitSvc;


  bool m_TRTPixCut;
  bool m_TrkQualityCut;
  bool m_TRTCut;
  float m_TrkPtCut;
  float m_TrkChi2Cut;
  float m_DstFromBeamCut;
  int m_CutPixelHits;
  int m_CutSctHits;
  int m_CutSiHits;
  int m_CutBLayHits;
  int m_CutSharedHits;
  int m_CutSiliconHits;
  int m_CutPixelHoles;
  bool m_useTrkToVtxTool;
  float m_killTrackFracLowEta;
  float m_killTrackFracHighEta;

  std::vector<float>* m_d0_old;
  std::vector<float>* m_d0_new;
  std::vector<float>* m_d0_simple;

  //const VxContainer* m_PrimVertexContainer;

};//end of class definitions

inline const InterfaceID& RPVDispVrtTrackSelector::interfaceID() {
  static const InterfaceID RPVDispVrtTrackSelectorIID("RPVDispVrtTrackSelector",1,0);
  return RPVDispVrtTrackSelectorIID;
}

#endif
