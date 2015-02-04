/// RPV displaced vertices

#ifndef RPVDISPVTX_COMPATIBILIZER_H
#define RPVDISPVTX_COMPATIBILIZER_H

/** Base class */
#include "AthenaBaseComps/AthAlgorithm.h"

/** Gaudi */
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"

#include "RPVDispVrt/RPVVrt.h"
#include "RPVDispVrt/RPVVrtContainer.h"

/** forward declarations */

class DVCompatibilizer : public AthAlgorithm
{
public:

  //! Constructor.
  DVCompatibilizer(const std::string &name, ISvcLocator *pSvcLocator);

  //! Initialize
  virtual StatusCode initialize();

  //! Execute
  virtual StatusCode execute();

  //! Finalize
  virtual StatusCode finalize();

  //! Destructur
  ~DVCompatibilizer()
    { }

private:

  std::string m_inputVertices;
  std::string m_outputVertices;
  bool m_doSecondPassVertex;
};

#endif
