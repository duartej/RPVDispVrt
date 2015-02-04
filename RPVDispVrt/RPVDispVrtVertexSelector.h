#ifndef RPVDispVrtVertexSelector_H
#define RPVDispVrtVertexSelector_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"
#include "EventPrimitives/EventPrimitives.h"

namespace Trk {
  class Track;

}

class RPVDispVrtVertexSelector : virtual public AthAlgTool, 
                                 virtual public IIncidentListener
{
 public:
  
  /** constructor */
  RPVDispVrtVertexSelector(const std::string& type, const std::string& name, const IInterface* parent ) ;
  /** destructor  */
  virtual ~RPVDispVrtVertexSelector(); 

  virtual StatusCode initialize();
  
  virtual StatusCode finalize();
  
  /** function to be executed at BeginEvent incident */
  virtual void handle(const Incident& inc);

  virtual bool goodTwoTrackVertex(const std::vector<const xAOD::TrackParticle*>& trackVec, 
				  const AmgVector(3)& vertexPosition,
				  const TLorentzVector& vertexMomentum,
				  const double chi2);

  virtual bool hitBasedFakeVertex(const std::vector<const xAOD::TrackParticle*>& trackVec, 
				  const AmgVector(3)& vertex);
  virtual void retrievePrimaryVertex();
  
  virtual bool closeToPrimaryVertex(const AmgVector(3)& vertexPosition,
				    const TLorentzVector& vertexMomentum );

  virtual xAOD::VertexContainer* selectForMaterialMap(xAOD::VertexContainer* inputColl);

  virtual xAOD::VertexContainer* selectGT2Trk(xAOD::VertexContainer* inputColl);

  /** AlgTool InterfaceID */
  static const InterfaceID& interfaceID() ;  

 private:
  double m_vtxChi2Cut;
  double m_primVtxCut;
  AmgVector(3) m_pVtx;
  bool m_havePrimaryVertex;
  std::string m_primVtxName;


};//end of class definitions

inline const InterfaceID& RPVDispVrtVertexSelector::interfaceID() {
  static const InterfaceID RPVDispVrtVertexSelectorIID("RPVDispVrtVertexSelector",1,0);
  return RPVDispVrtVertexSelectorIID;
}

#endif
