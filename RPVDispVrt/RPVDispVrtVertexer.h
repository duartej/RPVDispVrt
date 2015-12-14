
#ifndef _RPVDispVrt_RPVDispVrt_H
#define _RPVDispVrt_RPVDispVrt_H
#include <vector>
#include <map>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "EventKernel/IParticle.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ITHistSvc.h"

#include "McParticleEvent/TruthParticleContainer.h"
#include "HepPDT/ParticleDataTable.hh"
#include "HepPDT/TableBuilder.hh"
#include "HepPDT/ParticleData.hh"

#include "xAODTracking/VertexContainer.h"

#include "RPVDispVrt/RPVDispVrtVertexSelector.h"

//#include "EventKernel/IParticle.h"

//#include "ParticleEvent/ParticleBaseContainer.h"

//#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODTracking/TrackParticleContainer.h"

//#include "Particle/TrackParticle.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"

#include <string>

#include "HepPDT/ParticleDataTable.hh"

#include "GeneratorObjects/McEventCollection.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
//-----
#include "TrkEventUtils/InverseTruthMap.h"
#include "TrkEventUtils/TruthCollectionInverter.h"
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "EventPrimitives/EventPrimitives.h"
#include "JetEvent/JetCollection.h"

using namespace std;

namespace Trk {
  class ITruthToTrack;
  class IExtrapolator;
  
}

class RPVDispVrtVertexer : virtual public IAlgTool, virtual public AthAlgTool {
  public:
    /** Standard Athena-Algorithm Constructor */
    RPVDispVrtVertexer(const std::string& type, 
		       const std::string& name, 
		       const IInterface* parent ) ;
    /** Default Destructor */
    virtual ~RPVDispVrtVertexer();
    
    virtual StatusCode initialize();
    
    virtual StatusCode findVertices(xAOD::VertexContainer* vertices,
				    const xAOD::TrackParticleContainer* trkColl);

    ///    xAOD::Vertex* refitVertex(xAOD::Vertex* vertex,const xAOD::TrackParticleContainer* trkColl);
    //void refitVertex(xAOD::Vertex* vertex,const xAOD::TrackParticleContainer* trkColl);
    StatusCode refitVertex(xAOD::Vertex* vertex,const xAOD::TrackParticleContainer* trkColl);
    virtual StatusCode finalize();
    
    xAOD::Vertex* makeVertex(std::vector<const xAOD::TrackParticle*>  ListBaseTracks,
			     std::vector<long int> SelTrk,
			     const xAOD::TrackParticleContainer* trkColl);

    /** AlgTool InterfaceID */
    static const InterfaceID& interfaceID() ;

        
    
    void addVertexInfo(xAOD::Vertex*, 
		       const xAOD::TrackParticleContainer* trkColl,
		       vector<const xAOD::TrackParticle*>  &ListBaseTracks,
		       AmgVector(3) vertex,
		       TLorentzVector  vertexMom,
		       long int Charge,
		       std::vector<double> vertexCov,
		       std::vector<double> Chi2PerTrk,
		       std::vector< vector<double> > TrkAtVrt,
		       //		       double Chi2,
		       //		       double Ndf,
		       vector<long int> SelTrk);

    //    double calcMass(std::vector<Trk::VxTrackAtVertex* >* vtxTracks, double trkMass) ;
    double calcMass(std::vector<Trk::VxTrackAtVertex >* vtxTracks, double trkMass) ;

 private:
   

    ToolHandle <Trk::ITruthToTrack> m_truthToTrack; // tool to create trkParam from genPart
    ToolHandle <RPVDispVrtVertexSelector> m_vtxSel; // tool to create trkParam from genPart
    Trk::IExtrapolator* m_extrapolator; //coordinate transformations and perigee maker

    ToolHandle < Trk::ITrackSummaryTool >  m_sumSvc;
    ToolHandle < Trk::ITrkVKalVrtFitter >  m_fitSvc;    

    int                      m_SummaryToolExist;

    std::vector<int> makeTwoTrackVertices(const xAOD::TrackParticleContainer* trackColl);

    StatusCode makeNTrackVertices(xAOD::VertexContainer* vertices,
				  const xAOD::TrackParticleContainer* trackColl,
				  std::vector<int>& usedTrackIndices);

    //    void FillMatrix3x3(AmgSymMatrix(3)& matrix, std::vector<double>& inputVec, int iTrk=-1);
    AmgSymMatrix(3) FillMatrix3x3(std::vector<double>& inputVec, int iTrk=-1);
    AmgSymMatrix(5) FillMatrix5x5(std::vector<double>& inputVec, int iTrk=-1);
    void FillUnitMatrix3x3(AmgSymMatrix(3)& matrix);
    void FillUnitMatrix5x5(AmgSymMatrix(5)& matrix);


  };

inline const InterfaceID& RPVDispVrtVertexer::interfaceID() {
  static const InterfaceID RPVDispVrtVertexerIID("RPVDispVrtVertexer",1,0);
  return RPVDispVrtVertexerIID;
}


#endif
