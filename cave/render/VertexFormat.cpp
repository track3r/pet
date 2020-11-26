#include "pch.h"
#include "VertexFormat.h"


constexpr VertexFormat::VertexFormat(const VertexFormatCode code)
: m_code(code)
, m_offsets(makeOffsetsArray(code))
, m_size(calculateSize(code))
{

}

constexpr VertexFormat::VertexFormat(const std::initializer_list<VertexAttributeIndex>& input)
: m_code(makeCode(input))
, m_offsets(makeOffsetsArray(m_code))
, m_size(calculateSize(m_code))
{

}

constexpr VertexFormatCode VertexFormat::makeCode(const std::initializer_list<VertexAttributeIndex>& input)
{
	VertexFormatCode code = 0;
	for (VertexAttributeIndex i : input)
	{
		code |= (VertexFormatCode)1 << (VertexFormatCode)i;
	}

	return code;
}

constexpr VertexFormat::Offsets VertexFormat::makeOffsetsArray(const VertexFormatCode code)
{
	Offsets out = {};
	char offset = 0;

	int cur = 0;
	for (VertexFormatCode i = 0; i < (VertexFormatCode)VertexAttributeIndex::Last; i++)
	{
		if (code & (VertexFormatCode)1 << i)
		{
			out[i] = offset;
			offset += (char)c_attribs[i].size;
		}
	}

	return out;
}

/*constexpr bool VertexFormat::haveAttribute(const VertexAttributeIndex attr) const
{
	auto ret = m_code & ((VertexFormatCode)1 << (VertexFormatCode)attr);
	return ret != 0;
}*/

constexpr int VertexFormat::calculateSize(const VertexFormatCode code)
{
	int size = 0;
	for (VertexFormatCode i = 0; i < (int)VertexAttributeIndex::Last; i++)
		if (code & (VertexFormatCode)1 << i)
			size += c_attribs[i].size;

	return size * sizeof(float);
}

/*constexpr int VertexFormat::attributeOffset(const VertexAttributeIndex attr) const
{
	return m_offsets[(int)attr] * sizeof(float);
}*/