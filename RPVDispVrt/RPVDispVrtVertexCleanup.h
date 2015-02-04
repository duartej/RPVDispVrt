#ifndef RPVDispVrtVertexCleanup_H
#define RPVDispVrtVertexCleanup_H

#include "xAODTracking/VertexContainer.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"

/// Forward declarations
namespace Trk {
  class Track;
}
class RPVDispVrtVertexer;

///This class' definition

class RPVDispVrtVertexCleanup : virtual public AthAlgTool
{
 public:
  
  /** constructor */
  RPVDispVrtVertexCleanup(const std::string& type, const std::string& name, const IInterface* parent ) ;
  /** destructor  */
  virtual ~RPVDispVrtVertexCleanup(); 

  virtual StatusCode initialize();
  
  virtual StatusCode finalize();
  
  virtual StatusCode cleanupVertices(xAOD::VertexContainer* vertices, 
				     const xAOD::TrackParticleContainer* trkColl);


  /** AlgTool InterfaceID */
  static const InterfaceID& interfaceID() ;  

 private:
  
  virtual StatusCode disassembleVertices(xAOD::VertexContainer* vertices, 
					 const xAOD::TrackParticleContainer* trkColl);

  virtual StatusCode classifyTracks(xAOD::VertexContainer* inputVertices, 
				    std::vector<std::vector<long int> >& trkInVrt);

  virtual double maxOfShared(xAOD::VertexContainer* vertices, 
			     std::vector<std::vector<long int > >& trkInVrt,
			     long int & SelectedTrack,
			     long int & SelectedVertex);

  virtual void removeTrackFromVertex(xAOD::VertexContainer* vertices, 
				     long int & SelectedTrack,
				     long int & SelectedVertex);

  virtual double minVrtVrtSig( xAOD::VertexContainer* vertices, int & V1, int & V2);

  virtual double minVrtVrtDist( xAOD::VertexContainer* vertices, int & V1, int & V2);

  virtual double minVrtVrtSigNext( xAOD::VertexContainer* vertices, int & V1, int & V2);

  virtual double VrtVrtDist( const xAOD::Vertex* vrt1, const xAOD::Vertex* vrt2);

  virtual StatusCode mergeAndRefitVertices( xAOD::VertexContainer* vertices, 
					    int & foundV1, int & foundV2,
					    const xAOD::TrackParticleContainer* trkColl);   

  virtual void removeEmptyVertices(xAOD::VertexContainer* vertices);

  virtual void mergeFinalVertices(xAOD::VertexContainer* vertices,
				  const xAOD::TrackParticleContainer* trkColl);

  virtual void setMinVrtDists(xAOD::VertexContainer* vertices);

  bool m_disassembleVertices;
  double m_disassembleChi2Cut;
  int m_TrackDetachCut;
  double m_VertexMergeCut;
  double m_VertexMergeDistCut;
  double m_VertexMergeFinalDistCut;
  double m_VertexMergeFinalSigCut;
  bool m_mergeFinalVerticesDistance;
  bool m_mergeFinalVerticesSignif;
  
  ToolHandle<RPVDispVrtVertexer> m_vtxTool;

};//end of class definitions

inline const InterfaceID& RPVDispVrtVertexCleanup::interfaceID() {
  static const InterfaceID RPVDispVrtVertexCleanupIID("RPVDispVrtVertexCleanup",1,0);
  return RPVDispVrtVertexCleanupIID;
}

#endif
