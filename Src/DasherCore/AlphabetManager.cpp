#include "AlphabetManager.h"
#include "DasherModel.h"
#include "DasherNode.h"

#include <vector>
#include <iostream>

using namespace Dasher;

CAlphabetManager::CAlphabetManager( CDasherModel *pModel, CLanguageModel *pLanguageModel ) 
  : m_pModel(pModel), m_pLanguageModel(pLanguageModel) {
}

CDasherNode *CAlphabetManager::GetRoot(CDasherNode *pParent, int iLower, int iUpper) {
  CDasherNode *pNewNode;

  // FIXME - Make this a CDasherComponent

  pNewNode = new CDasherNode(*m_pModel, pParent, 0, 0, Opts::Nodes1, iLower, iUpper, m_pLanguageModel, false, 7);
  
  pNewNode->SetContext(m_pLanguageModel->CreateEmptyContext()); // FIXME - handle context properly

  pNewNode->m_pNodeManager = this;

  return pNewNode;
}

void CAlphabetManager::PopulateChildren( CDasherNode *pNode ) {

  // Actually create the children here
  
  // FIXME: this has to change for history stuff and Japanese dasher
    std::vector < symbol > newchars; // place to put this list of characters
    std::vector < unsigned int >cum; // for the probability list

    m_pModel->GetProbs(pNode->Context(), newchars, cum, m_pModel->GetLongParameter(LP_NORMALIZATION));
    int iChildCount = newchars.size();

    DASHER_TRACEOUTPUT("ChildCount %d\n", iChildCount);
    // work out cumulative probs in place
    for(int i = 1; i < iChildCount; i++)
      cum[i] += cum[i - 1];

    // create the children
    ColorSchemes NormalScheme, SpecialScheme;
    if((pNode->ColorScheme() == Nodes1) || (pNode->ColorScheme() == Special1)) {
      NormalScheme = Nodes2;
      SpecialScheme = Special2;
    }
    else {
      NormalScheme = Nodes1;
      SpecialScheme = Special1;
    }

    ColorSchemes ChildScheme;

    int iLbnd = 0;
    for(int j = 0; j < iChildCount; j++) {
      if(newchars[j] == m_pModel->GetSpaceSymbol())
        ChildScheme = SpecialScheme;
      else
        ChildScheme = NormalScheme;
      CDasherNode *pNewNode;

      if( newchars[j] == m_pModel->GetControlSymbol() )
	pNewNode = m_pModel->GetRoot(1, pNode, iLbnd, cum[j]);
      else {
	pNewNode = new CDasherNode(*m_pModel, pNode, newchars[j], j, ChildScheme, iLbnd, cum[j], m_pLanguageModel, false, m_pModel->GetColour(j));
	pNewNode->m_pNodeManager = this;
      }

      pNode->Children()[j] = pNewNode;
      iLbnd = cum[j];
    }

}

void CAlphabetManager::ClearNode( CDasherNode *pNode ) {
  // Should this be responsible for actually doing the deletion
}

void CAlphabetManager::Output( CDasherNode *pNode ) {
 symbol t = pNode->Symbol();
 if(t) { // Ignore symbol 0 (root node)
   Dasher::CEditEvent oEvent(1, m_pModel->GetAlphabet().GetText(t));
   m_pModel->InsertEvent(&oEvent);
 }
}

void CAlphabetManager::Undo( CDasherNode *pNode ) {
  symbol t = pNode->Symbol();
  if(t) { // Ignore symbol 0 (root node)
    Dasher::CEditEvent oEvent(2, m_pModel->GetAlphabet().GetText(t));
    m_pModel->InsertEvent(&oEvent);
  }
}