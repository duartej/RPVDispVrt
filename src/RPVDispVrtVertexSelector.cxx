#include "RPVDispVrt/RPVDispVrtVertexSelector.h"
#include "TrkTrackSummary/TrackSummary.h"

#include <iostream>

//constructor
RPVDispVrtVertexSelector::RPVDispVrtVertexSelector
( const std::string& type, const std::string& name,const IInterface* parent )
  :  AthAlgTool(type,name,parent),
     m_pVtx(0,0,0),
     m_havePrimaryVertex(false)
{
  declareInterface< RPVDispVrtVertexSelector  >( this );
  declareProperty("VtxChi2Cut",m_vtxChi2Cut=5.0);
  declareProperty("PrimaryVtxCut",m_primVtxCut=-20.);
  declareProperty("PrimaryVertexName",m_primVtxName="PrimaryVertices");
}

//destructor  
RPVDispVrtVertexSelector::~RPVDispVrtVertexSelector() {
}

StatusCode 
RPVDispVrtVertexSelector::initialize() {
  IIncidentSvc* incsvc;
  StatusCode sc = service("IncidentSvc", incsvc);
  int priority = 100;
  if( sc.isSuccess() ) {
    incsvc->addListener( this, "BeginEvent", priority);
  }
  return StatusCode::SUCCESS;
}

StatusCode 
RPVDispVrtVertexSelector::finalize() {
  return StatusCode::SUCCESS;
}

void
RPVDispVrtVertexSelector::handle(const Incident& inc) {
  if (inc.type() == "BeginEvent") {
    m_havePrimaryVertex = false;
  }
  return;
}


bool RPVDispVrtVertexSelector::goodTwoTrackVertex(const std::vector<const xAOD::TrackParticle*>& trackVec, 
						  const AmgVector(3)& vertexPosition,
						  const TLorentzVector& vertexMomentum,
						  const double chi2) {

  if (chi2 > m_vtxChi2Cut) {
    return false;
  }
  if (hitBasedFakeVertex(trackVec,vertexPosition)) return false;
  if (closeToPrimaryVertex(vertexPosition,vertexMomentum)) return false;
  return true;

}

//from VrtSecInclusive 
bool RPVDispVrtVertexSelector::hitBasedFakeVertex(const std::vector<const xAOD::TrackParticle*>& trackVec, 
						  const AmgVector(3)& vertex) {

  return false;
  

  if (trackVec.size() !=2) return true;
  const Trk::TrackSummary* trkiSum = trackVec[0]->track()->trackSummary();
  const Trk::TrackSummary* trkjSum = trackVec[1]->track()->trackSummary();

  int trkiBLHit = trkiSum->get(Trk::numberOfBLayerHits);
  int trkjBLHit = trkjSum->get(Trk::numberOfBLayerHits);
  double rad = sqrt(vertex.x()*vertex.x() + vertex.y()*vertex.y()); 
 
  bool Pix1iHit = trkiSum->isHit(Trk::pixelBarrel1); 
  bool Pix1jHit = trkjSum->isHit(Trk::pixelBarrel1); 
  //
  bool Pix2iHit = trkiSum->isHit(Trk::pixelBarrel2); 
  bool Pix2jHit = trkjSum->isHit(Trk::pixelBarrel2); 
  bool Pix2Hit = Pix2iHit && Pix2jHit; // both tracks have hits in Pixel2
  //
  // look at all SCT hits and not just barrel SCT hits
	       //
  bool SctiHit = trkiSum->get(Trk::numberOfSCTHits) >0;
  bool SctjHit = trkjSum->get(Trk::numberOfSCTHits) >0;
  bool SctHit = SctiHit && SctjHit; // both tracks have some SCT

  //
  

	       
	       // rough guesses for active layers:
	       // Pix0 (BLayer): 47.7-54.4, Pix1: 85.5-92.2, Pix2: 119.3-126.1:
	       // Sct0: 290-315, Sct1: 360-390, Sct2: 430-460, Sct3:500-530
	       //
 	       // if vertex has R< BLayer radius, tracks in it need to have hits in BLayer
	       // if vertex is outside BLayer, tracks can't have hits in BLayer
               // if vertex is within BLayer, require some Pixel hits on both trks
  if(rad<=47.7 && (trkiBLHit<1 || trkjBLHit<1)) return true;
  if(rad>54.4 && (trkiBLHit>0 || trkjBLHit>0)) return true;
  bool somePixi = trkiBLHit>0 || Pix1iHit || Pix2iHit ; //trki has some Pixel Hit
  bool somePixj = trkjBLHit>0 || Pix1jHit || Pix2jHit; // trkj has some Pixel Hit
  bool somePix = somePixi && somePixj; // both trks have some Pixel hits
  if((rad>47.7 && rad <=54.4) && !somePix) return true;
  //
  // if rad is between Pix0 and Pix1, 
  // both tracks should have hits in Pix1 or Pix2 but none in Pix0
  somePixi = Pix1iHit || Pix2iHit ; // trki has hit in Pix1 or Pix2
  somePixj = Pix1jHit || Pix2jHit ;// trkj
  somePix = somePixi && somePixj;  // both trks have hits in either Pix1 and/or Pix2

  if((rad > 54.4 && rad <=92.2) && !somePix) return true; 
  //
  // if rad is greater Pix1, neither track should have any hit in Blayer or Pix1
  // check on Blayer is redundant at this point
  somePixi = trkiBLHit>0 || Pix1iHit;
  somePixj = trkjBLHit>0 || Pix1jHit;
  if(rad > 92.2 && (somePixi || somePixj)) return true;
  // if rad is between Pix1 and Pix2, both tracks should have hits in Pix2 & SCT
  // if rad in Pix2 (119.3-126.1) just require SCT hit - don't check on Pix2
  
  if((rad> 92.2 && rad <=119.3) && !Pix2Hit) return true;
  if((rad> 92.2 && rad <=126.1) && !SctHit) return true;
  
  // if rad > Pix2, neither track can have any Pix hits,should have some SCT hits
  // check on Blayer is redundant
  somePixi= (trkiBLHit>0 || Pix1iHit || Pix2iHit);
  somePixj = (trkjBLHit>0 || Pix1jHit || Pix2jHit);
  if(rad > 126.1 && (somePixi || somePixj) ) return true;
  if(rad > 126.1 && !SctHit) return true;
  return false;
  
}

bool
RPVDispVrtVertexSelector::closeToPrimaryVertex(const AmgVector(3)& vertexPosition, 
					       const TLorentzVector& vertexMomentum ) {
  if (! m_havePrimaryVertex) retrievePrimaryVertex();
  AmgVector(3) vDist=vertexPosition - m_pVtx;
  double vPos=(vDist.x()*vertexMomentum.Px()+vDist.y()*vertexMomentum.Py()+vDist.z()*vertexMomentum.Pz())/
    vertexMomentum.Rho();
  if (vPos < m_primVtxCut) {
    return true;
  }
  return false;
}

void
RPVDispVrtVertexSelector::retrievePrimaryVertex()
{
  const xAOD::VertexContainer* primVertices(0);
  StatusCode sc = evtStore()->retrieve(primVertices,m_primVtxName);
  if (sc.isFailure()) 
    msg(MSG::ERROR)<<"Failed to retrieve VxPrimaryCandidate collection "<<m_primVtxName<<endreq;
  else msg(MSG::DEBUG)<<"retrieved VxPrimaryCandidate size "<<primVertices->size()<<endreq;  

  if (primVertices->size() == 0) {
    msg(MSG::WARNING)<<"Primary vertex not found, will skip this event"<<endreq;
    return;
  }
  //  const Trk::RecVertex rVtx = (primVertices->at(0))->recVertex();
  m_pVtx = primVertices->at(0)->position();
  m_havePrimaryVertex = true;
  return;
}

xAOD::VertexContainer*
RPVDispVrtVertexSelector::selectForMaterialMap(xAOD::VertexContainer* vertices)
{

  xAOD::VertexContainer::iterator vrtIt = vertices->begin();

  /*
  while (vrtIt != vertices->end()) {
    if (((*vrtIt)->trackIndices()->size() != 3) || 
	((*vrtIt)->massPionHypo() > 5000.)) {
      vrtIt = vertices->erase(vrtIt);
    } else {
      ++vrtIt;
    }
  }
  */
  return vertices;
}

xAOD::VertexContainer*
RPVDispVrtVertexSelector::selectGT2Trk(xAOD::VertexContainer* vertices)
{

  xAOD::VertexContainer::iterator vrtIt = vertices->begin();
  /*
  while (vrtIt != vertices->end()) {
    if ((*vrtIt)->trackIndices()->size() < 3) {
      vrtIt = vertices->erase(vrtIt);
    } else {
      ++vrtIt;
    }
  }
  */
  return vertices;
}
