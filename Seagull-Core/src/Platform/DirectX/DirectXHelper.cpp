#include "sgpch.h"
#include "DirectXHelper.h"

#include <comdef.h>

namespace SG
{

	DxException::DxException( HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber) 
		:m_ErrorCode(hr), m_FunctionName(functionName), m_Filename(filename), m_LineNumber(lineNumber) 
	{}

	std::wstring DxException::ToString() const
	{
		// Get the string description of the error code.
		_com_error err(m_ErrorCode);
		std::wstring msg = err.ErrorMessage();

		return m_ErrorCode + L" failed in " + m_Filename + L"; line " + std::to_wstring(m_LineNumber) + L"; error: " + msg;
	}

}
