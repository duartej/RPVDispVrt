/// RPV displaced vertices

#ifndef RPVDISPVRT_RPVDISPVRTTESTALG_H
#define RPVDISPVRT_RPVDISPVRTTESTALG_H

/** Base class */
#include "AthenaBaseComps/AthAlgorithm.h"

/** Gaudi */
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

/** forward declarations */


class RPVDispVrtTestAlg : public AthAlgorithm
{
public:

  //! Constructor.
  RPVDispVrtTestAlg(const std::string &name, ISvcLocator *pSvcLocator);

  //! Initialize
  virtual StatusCode initialize();

  //! Execute
  virtual StatusCode execute();

  //! Finalize
  virtual StatusCode finalize();

  //! Destructur
  ~RPVDispVrtTestAlg() 
    { }

private:


};

#endif
