#include "VertexFormat.h"


VertexFormat::VertexFormat(VertexFormatCode code) 
: m_code(code)
, m_offsets(makeOffsetsArray(code))
, m_size(calculateSize(code))
{

}

VertexFormat::VertexFormat(const std::initializer_list<VertexAttributeIndex>& input) 
: m_code(makeCode(input))
, m_offsets(makeOffsetsArray(m_code))
, m_size(calculateSize(m_code))
{

}

VertexFormatCode VertexFormat::makeCode(const std::initializer_list<VertexAttributeIndex>& input)
{
	VertexFormatCode code = 0;
	for (VertexAttributeIndex i : input)
	{
		code |= (VertexFormatCode)1 << (VertexFormatCode)i;
	}

	return code;
}

VertexFormat::Offsets VertexFormat::makeOffsetsArray(VertexFormatCode code)
{
	Offsets out;
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

size_t VertexFormat::size() const
{
	return m_size;
}

bool VertexFormat::haveAttribute(VertexAttributeIndex attr) const
{
	auto ret = m_code & ((VertexFormatCode)1 << (VertexFormatCode)attr);
	return ret != 0;
}

int VertexFormat::calculateSize(VertexFormatCode code)
{
	int size = 0;
	for (VertexFormatCode i = 0; i < (int)VertexAttributeIndex::Last; i++)
		if (code & (VertexFormatCode)1 << i)
			size += c_attribs[i].size;

	return size * sizeof(float);
}

int VertexFormat::attributeOffset(VertexAttributeIndex attr) const
{
	return m_offsets[(int)attr] * sizeof(float);
}