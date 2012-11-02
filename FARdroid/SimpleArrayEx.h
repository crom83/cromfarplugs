/*
Module : SimpleArrayEx.h
Purpose: Interface for a template class which supports "InsertAt" for CSimpleArray
Created: PJN / 03-11-2003
History: None

Copyright (c) 2003 - 2006 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


////////////////////////////////// Macros ///////////////////////////

#ifndef __SIMPLEARRAYEX_H__
#define __SIMPLEARRAYEX_H__


/////////////////////////// Classes /////////////////////////////////

template<class TYPE, class ARG_TYPE>
class CSimpleArrayEx : public CSimpleArray<TYPE>
{
public:
//Methods
  BOOL InsertAt(int nIndex, ARG_TYPE newElement, int nCount = 1)
  {
    //Realloc the array if necessary
    if ((m_nSize + nCount) >= m_nAllocSize)
    {
 			int nNewAllocSize = (m_nAllocSize == 0) ? 1 : (m_nSize * 2);
      while (nNewAllocSize < (m_nSize + nCount))
        nNewAllocSize *= 2;

			TYPE* aT = (TYPE*)realloc(m_aT, nNewAllocSize * sizeof(TYPE));
			if (aT == NULL)
				return FALSE;
			m_nAllocSize = nNewAllocSize;
			m_aT = aT;
    }

    //Move the existing elements at the insertion position  
		if (m_nSize && (nIndex != (m_nSize - 1)))
		{
			int size = m_nSize-nIndex;
			TYPE * data = (TYPE*)calloc(size, sizeof(TYPE));
			memcpy(data, (void*)&m_aT[nIndex], size*sizeof(TYPE));
			memcpy((void*)&m_aT[nIndex + nCount], data, size*sizeof(TYPE));
			free(data);
			//memmove((void*)&m_aT[nIndex + nCount], (void*)&m_aT[nIndex], nCount * sizeof(TYPE));
		}

    //Finally store the new values into the array
		m_nSize += nCount;
    for (int i=0; i<nCount; i++)
		  SetAtIndex(nIndex+i, newElement);

		return TRUE;
  }

  BOOL InsertAt(int nStartIndex, CSimpleArrayEx<TYPE, ARG_TYPE>* pNewArray)
  {
	  if (pNewArray->GetSize() > 0)
	  {
		  if (!InsertAt(nStartIndex, pNewArray->operator[](0), pNewArray->GetSize()))
        return FALSE;

		  for (int i = 0; i < pNewArray->GetSize(); i++)
			  SetAtIndex(nStartIndex + i, pNewArray->operator[](i));
	  }

    return TRUE;
  }
	BOOL AddArray(CSimpleArrayEx<TYPE, ARG_TYPE>* pNewArray)
	{
		for (int i = 0; i < pNewArray->GetSize(); i++)
			Add((*pNewArray)[i]);

		return TRUE;
	}
};

#endif //__SIMPLEARRAYEX_H__
