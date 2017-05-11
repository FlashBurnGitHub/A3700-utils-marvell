/******************************************************************************
 *
 *  (C)Copyright 2005 - 2013 Marvell. All Rights Reserved.
 *  
 *  THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF MARVELL.
 *  The copyright notice above does not evidence any actual or intended 
 *  publication of such source code.
 *  This Module contains Proprietary Information of Marvell and should be
 *  treated as Confidential.
 *  The information in this file is provided for the exclusive use of the 
 *  licensees of Marvell.
 *  Such users have the right to use, modify, and incorporate this code into 
 *  products for purposes authorized by the license agreement provided they 
 *  include this notice and the associated copyright notice with any such
 *  product. 
 *  The information in this file is provided "AS IS" without warranty.
 *
 ******************************************************************************/
 /********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

	* Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.

	* Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.

	* Neither the name of Marvell nor the names of its contributors may be
	  used to endorse or promote products derived from this software without
	  specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include <algorithm>

#include "DDROperations.h"
#include "TimDescriptorParser.h"

CDDROperation::CDDROperation() :
		CTimLib() {
	m_OperationId = DDR_NOP;
	m_sOpIdText = "NOP";
	m_Value = 0;
}

CDDROperation::CDDROperation(DDR_OPERATION_SPEC_T OpId,
		const string & sOpIdText) :
		m_OperationId(OpId), m_sOpIdText(sOpIdText), CTimLib() {
	m_Value = 0;
}

CDDROperation::~CDDROperation(void) {

}

CDDROperation::CDDROperation(const CDDROperation& rhs) :
		CTimLib(rhs) {
	// copy constructor
	m_OperationId = rhs.m_OperationId;
	m_sOpIdText = rhs.m_sOpIdText;
	m_Value = rhs.m_Value;

}

CDDROperation& CDDROperation::operator=(const CDDROperation& rhs) {
	// assignment operator
	if (&rhs != this) {
		CTimLib::operator=(rhs);

		m_OperationId = rhs.m_OperationId;
		m_sOpIdText = rhs.m_sOpIdText;
		m_Value = rhs.m_Value;
	}
	return *this;
}

bool CDDROperation::SetOperationID(DDR_OPERATION_SPEC_T eOpId) {
	t_DDROperationListIter Iter =
			CDDROperations::s_DefinedDDROperations.begin();
	while (Iter != CDDROperations::s_DefinedDDROperations.end()) {
		if ((*Iter)->m_OperationId == eOpId) {
			*this = *(*Iter);
			break;
		}
		Iter++;
	}
	return true;
}

bool CDDROperation::SetOperationID(string& sOpIdText) {
	t_DDROperationListIter Iter =
			CDDROperations::s_DefinedDDROperations.begin();
	while (Iter != CDDROperations::s_DefinedDDROperations.end()) {
		if ((*Iter)->m_sOpIdText == sOpIdText) {
			*this = *(*Iter);
			break;
		}
		Iter++;
	}
	return true;
}

bool CDDROperation::ToBinary(ofstream& ofs) {
	return ofs.good();
}

t_DDROperationList CDDROperations::s_DefinedDDROperations;
static CDDROperation NOOP_OP(DDR_NOP, "NOP");
static CDDROperation DDR_INIT_ENABLE_OP(DDR_INIT_ENABLE, "DDR_INIT_ENABLE");
static CDDROperation DDR_MEMTEST_ENABLE_OP(DDR_MEMTEST_ENABLE,
		"DDR_MEMTEST_ENABLE");
static CDDROperation DDR_MEMTEST_START_ADDR_OP(DDR_MEMTEST_START_ADDR,
		"DDR_MEMTEST_START_ADDR");
static CDDROperation DDR_MEMTEST_SIZE_OP(DDR_MEMTEST_SIZE, "DDR_MEMTEST_SIZE");
static CDDROperation DDR_INIT_LOOP_COUNT_OP(DDR_INIT_LOOP_COUNT,
		"DDR_INIT_LOOP_COUNT");
static CDDROperation DDR_IGNORE_INST_TO_OP(DDR_IGNORE_INST_TO,
		"DDR_IGNORE_INST_TO");

int CDDROperations::s_DDROpCount = 0;

const string CDDROperations::Begin("Operations");
const string CDDROperations::End("End Operations");
const string CDDROperations::BlfBegin("DDROperations");
const string CDDROperations::BlfEnd("End_DDROperations");

CDDROperations::CDDROperations() :
		CErdBase(DDR_OPERATIONS_ERD, OPERATIONS_MAX) {
	s_DDROpCount++;

	if (s_DDROpCount == 1) {
		Reset();
		s_DefinedDDROperations.push_back(&NOOP_OP);
		s_DefinedDDROperations.push_back(&DDR_INIT_ENABLE_OP);
		s_DefinedDDROperations.push_back(&DDR_MEMTEST_ENABLE_OP);
		s_DefinedDDROperations.push_back(&DDR_MEMTEST_START_ADDR_OP);
		s_DefinedDDROperations.push_back(&DDR_MEMTEST_SIZE_OP);
		s_DefinedDDROperations.push_back(&DDR_INIT_LOOP_COUNT_OP);
		s_DefinedDDROperations.push_back(&DDR_IGNORE_INST_TO_OP);
	}
}

CDDROperations::~CDDROperations(void) {
	Reset();
	if (--s_DDROpCount == 0) {
		if (s_DefinedDDROperations.size() > 0)
			s_DefinedDDROperations.clear();
	}
}

// copy constructor
CDDROperations::CDDROperations(const CDDROperations& rhs) :
		CErdBase(rhs) {
	s_DDROpCount++;

	// need to do a deep copy of lists to avoid dangling references
	CDDROperations& nc_rhs = const_cast<CDDROperations&>(rhs);
	if (nc_rhs.m_DdrOperationsList.size() > 0) {
		for_each(nc_rhs.m_DdrOperationsList.begin(),
				nc_rhs.m_DdrOperationsList.end(), [this](CDDROperation*& pRhsOp)
				{
					CDDROperation* pOp = new CDDROperation(*pRhsOp);
					m_DdrOperationsList.push_back(pOp);
				});
	}
}

// assignment operator
CDDROperations& CDDROperations::operator=(const CDDROperations& rhs) {
	// assignment operator
	if (&rhs != this) {
		CErdBase::operator=(rhs);
		Reset();

		// need to do a deep copy of lists to avoid dangling references
		CDDROperations& nc_rhs = const_cast<CDDROperations&>(rhs);

		if (nc_rhs.m_DdrOperationsList.size() > 0) {
			for_each(nc_rhs.m_DdrOperationsList.begin(),
					nc_rhs.m_DdrOperationsList.end(),
					[this](CDDROperation*& pRhsOp)
					{
						CDDROperation* pOp = new CDDROperation(*pRhsOp);
						m_DdrOperationsList.push_back(pOp);
					});
		}
	}
	return *this;
}

void CDDROperations::Reset() {
	if (m_DdrOperationsList.size() > 0) {
		for_each(m_DdrOperationsList.begin(), m_DdrOperationsList.end(),
				[](CDDROperation*& pOp) {delete pOp;});
		m_DdrOperationsList.clear();
	}
}

unsigned int CDDROperations::PackageSize() {
	unsigned int iSize = (unsigned int) m_DdrOperationsList.size() * 8;

	return iSize;
}

bool CDDROperations::Parse(CTimDescriptor& TimDescriptor,
		CTimDescriptorLine*& pLine, bool bIsBlf) {
	m_bChanged = false;

	if (pLine->m_FieldName != (bIsBlf ? BlfBegin : Begin)) {
		stringstream ss;
		ss << endl << "Error: Parsing of '" << (bIsBlf ? BlfBegin : Begin);
		TimDescriptor.ParsingError(ss.str(), true, pLine);
		return false;
	}

	while ((pLine = TimDescriptor.GetNextLineField(pLine))) {
		if (pLine->m_FieldName == (bIsBlf ? BlfEnd : End)) {
			break;
		}

		bool bFound = false;
		t_DDROperationListIter iter = s_DefinedDDROperations.begin();
		while (iter != s_DefinedDDROperations.end()) {
			if (TrimWS(pLine->m_FieldName) == (*iter)->m_sOpIdText) {
				CDDROperation* pOp = new CDDROperation(*(*iter));
				pOp->m_Value = Translate(pLine->m_FieldValue);
				m_DdrOperationsList.push_back(pOp);
				bFound = true;
				break;
			}
			iter++;
		}

		if (!bFound) {
			stringstream ss;
			ss << endl << "Error: Parsing of '" << (bIsBlf ? BlfBegin : Begin);
			TimDescriptor.ParsingError(ss.str(), true, pLine);
			break;
		}
	}

	// field not found
	return true;
}

bool CDDROperations::ToBinary(ofstream& ofs) {
	bool bRet = true;
	return (ofs.good() && bRet);
}

int CDDROperations::AddPkgStrings(CReservedPackageData* pRPD) {
	t_DDROperationListIter iter = m_DdrOperationsList.begin();
	while (iter != m_DdrOperationsList.end()) {
		pRPD->AddData(new string(HexFormattedAscii((*iter)->m_OperationId)),
				new string((*iter)->m_sOpIdText));
		pRPD->AddData(new string(HexFormattedAscii((*iter)->m_Value)),
				new string("Value"));
		iter++;
	}

	return PackageSize();
}
