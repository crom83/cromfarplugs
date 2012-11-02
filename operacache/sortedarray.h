/*
Module : SortedArray.h
Purpose: Interface for an MFC template class which provides sorting and ordered insertion
         derived from the MFC class CArray or ATL class CSimpleArray
Created: PJN / 25-12-1999
History: PJN / 12-01-2000 Fixed a stack overflow in CSortedArray::Sort
         PJN / 21-02-2000 Fixed a number of problems in CSortedArray::Find
         PJN / 22-02-2000 Fixed a problem in CSortedArray::Find when there are no items in the array
         PJN / 29-02-2000 Fixed a problem in CSortedArray::Sort when there are no items in the array
         PJN / 27-08-2000 1. Fixed another stack overflow problem in CSortedArray::Sort.
                          2. Fixed a problem in CSortedArray::Sort where the comparison function
                          was returning negative values, 0 and positive values instead of -1, 0 & 1.
                          Thanks to Ted Crow for finding both of these problems.
         PJN / 01-10-2001 Fixed another bug in Sort!. Thanks to Jim Johnson for spotting this.
         PJN / 29-05-2002 1. Fixed a problem in CSortedArray::OrderedInsert. Thanks to John Young
                          for spotting and fixing this problem.
                          2. Updated copyright and usage instructions
         PJN / 06-12-2002 1. Rewrote the Sort method following reports of further problems by 
                          Serhiy Pavlov and Brian Rhodes
         PJN / 11-12-2002 1. Optimized code by replacing all calls to CArray<>::ElementAt with CArray<>::GetData
         PJN / 24-01-2003 1. Made CSortedArray::Find method const. Thanks to Serhiy Pavlov for reporting this.
         PJN / 18-08-2003 1. Made the class optionally independent of MFC. If the class detects than MFC is 
                          not being included, then the code will use CSimpleArray instead of CArray. This is 
                          a class provided in ATL as a substitute for CArray. Please note that the function
                          "OrderedInsert" is not available when using CSimpleArray as the parent class as 
                          CSimpleArray does not implement an "InsertAt" method.
         PJN / 03-11-2003 1. Now includes a new class "CSimpleArrayEx" which provides InsertAt support
                          for ATL's CSimpleArray class. This is now used by CSortedArray, rather than directly
                          using CSimpleArray
         PJN / 16-10-2004 1. Class now compiles cleanly on VC 7 if "Detect 64 bit portability issues" is enabled
                          as well as "Force conformance in for loops" is enabled.
         PJN / 22-12-2004 1. ASSERT / ATLASSERT and INT_PTR / int typedefs are now all done in one place. Thanks
                          to Serhiy Pavlov for suggesting this.
                          2. All functions are now declared in the class declaration
                          3. Reworked the classes to break the actual comparison code into a new traits class. 
                          You now have the choice of using a traits class which specifies the comparison function
                          via a function (CSortedArrayCompareFunction) or via a functor (CSortedArrayCompareFunctor).
                          Backward compatibility is kept by defined a class called CSortedArray which uses
                          a traits class which uses a function. If you want to use the new faster functor version
                          of the class then simply replace all instances of CSortedArray with CSortedArrayEx. 
                          Thanks to Serhiy Pavlov for this really nice addition.
                          4. Made CSortedArray::Find method non const again to allow use of GetData function. 
                          5. Updated the sample app to perform some speed tests on ATL Vs MFC and function 
                          pointer Vs Functor implementations.
         PJN / 11-10-2005 1. Updated the Find function to allow <0, 0 and >0 values to be allowed for the return
                          value from the comparison function / functor. This allows CString::Compare to be easily
                          used for comparison. Thanks to Serhiy Pavlov for reporting this.
                          2. Removed unused constructor from CSimpleArrayEx class.
                          3. Updated copyright details.
         PJN / 07-07-2006 1. Updated copyright details.
                          2. Minor update to the sample app to allow it to clean compile on VC 2005.
                          3. Updated the documentation to use the same style as the web site.
         PJN / 29-07-2006 1. Provided a new UniqueSort method which in addition to performing the standard sorting
                          of the array also removes any duplicates found. Thanks to John Cullen for suggesting this 
                          new feature.

Copyright (c) 1999 - 2006 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


////////////////////////////////// Macros / Includes ////////////////

#ifndef __SORTEDARRAY_H__
#define __SORTEDARRAY_H__

#ifdef _AFX
#ifndef __AFXTEMPL_H__
#include <afxtempl.h> 
#pragma message("To avoid this message, please put afxtempl.h in your PCH (normally stdafx.h)")
#endif
#else
#include "SimpleArrayEx.h"
#endif

#if defined(_AFX) && (_MFC_VER >= 0x700)
  typedef INT_PTR CSA_INT;
#else
  typedef int CSA_INT;
#endif

#ifndef ASSERT
#define ASSERT ATLASSERT
#endif


/////////////////////////// Classes /////////////////////////////////

//Class which provides a compare function via a class declaration. Used
//when deriving from CSortedArrayBase. It is a so called traits class
template<class TYPE, class ARG_TYPE>
class CSortedArrayCompareFunction
{
public:
//Typedefs
  typedef int COMPARE_FUNCTION(ARG_TYPE element1, ARG_TYPE element2);
  typedef COMPARE_FUNCTION* LPCOMPARE_FUNCTION;

//Constructors / Destructors
  CSortedArrayCompareFunction() : m_pCompareFunction(NULL)
  {
  }

//Methods
  void SetCompareFunction(LPCOMPARE_FUNCTION pCompareFunction)
  {
    ASSERT(pCompareFunction);
    m_pCompareFunction = pCompareFunction;
  }

  LPCOMPARE_FUNCTION GetCompareFunction() const 
  { 
    return m_pCompareFunction; 
  }

#ifdef _DEBUG
  BOOL IsCompareFunctionValid() const 
  { 
    return (m_pCompareFunction != NULL); 
  }
#endif

protected:
//Member variables
  LPCOMPARE_FUNCTION m_pCompareFunction;
};

//Class which provides a compare function via a functor. Used
//when deriving from CSortedArrayBase. It is a so called traits class
template<class COMPARE_TYPE>
class CSortedArrayCompareFunctor
{
public:
//methods
  void SetCompareFunction(const COMPARE_TYPE& pCompareFunctor)
  {
    m_pCompareFunction = pCompareFunctor;
  }

  COMPARE_TYPE GetCompareFunction() const 
  { 
    return m_pCompareFunction; 
  }

#ifdef _DEBUG
  BOOL IsCompareFunctionValid() const 
  { 
    return TRUE; 
  }
#endif

protected:
//Member variables
  COMPARE_TYPE m_pCompareFunction;
};

//Base class which implements sorting for its parent class CArray / CSimpleArray
template<class TYPE, class ARG_TYPE, class COMPARE_TYPE>
#ifdef _AFX
class CSortedArrayBase : public CArray<TYPE, ARG_TYPE>, public COMPARE_TYPE
#else
class CSortedArrayBase : public CSimpleArrayEx<TYPE, ARG_TYPE>, public COMPARE_TYPE
#endif
{
public:
//Methods
  CSA_INT OrderedInsert(ARG_TYPE newElement, CSA_INT nCount = 1)
  {
	  ASSERT(IsCompareFunctionValid()); 	//Did you forget to call SetCompareFunction prior to calling this function?

	  CSA_INT lo = 0;
	  CSA_INT hi = GetSize() - 1;

    TYPE* pData = GetData();
	  
	  //Find the insert location (mid) for the new element.
	  CSA_INT mid = hi / 2;

	  while (hi >= lo)
	  {
      ASSERT(pData); //Note we do not do the ASSERT outside of the loop because
                     //the value "pData" cannot be NULL in a fresh new array. But
                     //in this case we will not end up executing this code. Also
                     //since it is an ASSERT it will be completely optimized away
                     //in release mode builds so it has no impact on performance

		  int res = m_pCompareFunction(newElement, pData[mid]);

		  if (!res)
			  break;

		  if (res < 0)
			  hi = mid - 1;	//Insert in the lower half...
		  else 
        lo = mid + 1; //Insert in the upper half...
		  mid = ((hi - lo) / 2) + lo;
	  }

  #ifdef _AFX
	  InsertAt(mid, newElement, nCount);
  #else
    if (!InsertAt(mid, newElement, nCount))
      return -1;
  #endif
	  return mid;
  }
	bool IsEmpty()
	{
		return GetSize() == 0;
	}
	void SwapItems(int i, int j)
	{
		TYPE * pData = GetData();
		if (!pData || 
			i >= GetSize() || j >= GetSize() || 
			i < 0 || j < 0)
			return;

		TYPE x  = pData[i];
		pData[i] = pData[j];
		pData[j] = x;
	}
	bool RandomShuffle()
	{
		int size = GetSize();
		if (size <= 1) 
			return false;

		const int _RANDOM_BITS = 15;	// minimum random bits from rand()
		const int _RANDOM_MAX = (1U << _RANDOM_BITS) - 1;

		int _Next = 0;
		for (int _Index = 2; ++_Next != GetSize(); ++_Index)
		{	// assume unsigned long big enough for _Diff count
			unsigned long _Rm = _RANDOM_MAX;
			unsigned long _Rn = ::rand() & _RANDOM_MAX;
			for (; _Rm < static_cast<unsigned long>(_Index) && _Rm != ~0UL;
				_Rm = _Rm << _RANDOM_BITS | _RANDOM_MAX)
				_Rn = _Rn << _RANDOM_BITS
				| (::rand() & _RANDOM_MAX);	// build random value

			SwapItems(_Next, _Rn % _Index);
		}
		return true;
	}
	void Reverse()
	{
		if (IsEmpty())
			return;

		for (int i = 0; i < GetSize()/2; i++)
			SwapItems(i, GetSize()-i-1);
	}
  void Sort(CSA_INT nLowIndex = 0, CSA_INT nHighIndex = -1)
  {
    CSA_INT nSize = GetSize();

    //Sort all the data?
	  if (nHighIndex == -1)
      nHighIndex = nSize - 1;     

    //quick exit                  
    if ((nLowIndex == nHighIndex) || (nSize == 0))
      return;               

    //Validate the required values for this function
    ASSERT(nHighIndex <= (nSize - 1));
    ASSERT(IsCompareFunctionValid());

    //Do the actual quicksort  
    if (nLowIndex < nHighIndex)
    {    
      CSA_INT i = nLowIndex;
      CSA_INT j = nHighIndex;
			CSA_INT m = 1;
      TYPE* pData = GetData();

      ASSERT(pData);

			for (; i < j; (m > 0)?j--:i++)
			{
				if (m_pCompareFunction(pData[i], pData[j]) > 0)
				{
					TYPE x  = pData[i];
					pData[i] = pData[j];
					pData[j] = x;
					m *= -1;
				}
			}
			if (i != 0)
				Sort(nLowIndex, i - 1);
			Sort(i + 1, nHighIndex);
    }
  }
  
  void UniqueSort(CSA_INT nLowIndex = 0, CSA_INT nHighIndex = -1)
  {
    CSA_INT nSize = GetSize();
  
    //Sort all the data?
	  if (nHighIndex == -1)
      nHighIndex = nSize - 1;     
 
    //quick exit                  
    if ((nLowIndex == nHighIndex) || (nSize == 0))
      return;               

    //First sort the requested elements, using the "Sort" method
    Sort(nLowIndex, nHighIndex);
    
    //Now remove any duplicates
    TYPE* pData = GetData();
    ASSERT(pData);
    for (CSA_INT i=nHighIndex; i>nLowIndex; i--)
    {
      if (m_pCompareFunction(pData[i], pData[i-1]) == 0)
        RemoveAt(i);
    }
  }

  CSA_INT ArrangedFind(ARG_TYPE element, CSA_INT nLowIndex = 0, CSA_INT nHighIndex = -1)
  {
    ASSERT(IsCompareFunctionValid()); //Did you forget to call SetCompareFunction prior to calling this function

    CSA_INT nSize = GetSize();

    //If there are no items in the array, then return immediately
    if (nSize == 0)
      return -1;

    CSA_INT left = nLowIndex;
    CSA_INT right = nHighIndex;
    TYPE* pData = GetData();

    if (right == -1)
      right = nSize - 1;

    ASSERT(left <= right);
    ASSERT(pData);
  
    if (left == right) //Straight comparison fewer than 2 elements to search
      return (m_pCompareFunction(pData[left], element) == 0) ? left : -1;

    //do a binary chop to find the location where the element should be inserted
    CSA_INT nFoundIndex = -1;
    while ((nFoundIndex == -1) && (left != right))
    {
      CSA_INT nCompareIndex;

      if (right == (left + 2))
        nCompareIndex = left+1;
      else
        nCompareIndex = ((right - left) >> 1) + left;

      int nCompare = m_pCompareFunction(pData[nCompareIndex], element);

      if (nCompare < 0)
      {
        if ((right - left) == 1)
        {
          if (m_pCompareFunction(pData[right], element) == 0)
            nFoundIndex = right;
          else if (m_pCompareFunction(pData[left], element) == 0)
            nFoundIndex = left;
          else
            left = right;
        }
        else
          left = nCompareIndex;
      }
      else if (nCompare > 0)
      {
        if ((right - left) == 1)
        {
          if (m_pCompareFunction(pData[right], element) == 0)
            nFoundIndex = right;
          else if (m_pCompareFunction(pData[left], element) == 0)
            nFoundIndex = left;
          else
            right = left;
        }
        else
          right = nCompareIndex;
      }
      else
      {
        nFoundIndex = nCompareIndex;
      }
    }
  
    return nFoundIndex;
  }
};

//A derivation of CSortedArrayBase which provides the compare function via a pointer embedded in 
template<class TYPE, class ARG_TYPE>
class CSortedArray : public CSortedArrayBase<TYPE, ARG_TYPE, CSortedArrayCompareFunction<TYPE, ARG_TYPE> >
{
};

//A derivation of CSortedArrayBase which provides the compare function via a functor
template<class TYPE, class ARG_TYPE, class COMPARE_TYPE>
class CSortedArrayEx : public CSortedArrayBase<TYPE, ARG_TYPE, CSortedArrayCompareFunctor<COMPARE_TYPE> >
{
};

#endif //__SORTEDARRAY_H__
