#pragma once

#include <stdint.h>
#include <initializer_list>
#include <array>

enum class VertexAttributeIndex
{
	Pos = 0,
	Uv = 1,
	Normal = 2,
	Tangent = 3,
	Color = 4,
	DrawId = 5,
	Matrix = 6,
	Last
};

struct VertexAttribute
{
	VertexAttributeIndex index;
	int size;
	const char* name;
};

constexpr VertexAttribute c_attribs[] =
{
	{ VertexAttributeIndex::Pos, 3, "v_position" },
	{ VertexAttributeIndex::Uv, 2, "v_uv" },
	{ VertexAttributeIndex::Normal, 3, "v_normal" },
	{ VertexAttributeIndex::Tangent, 3, "v_tangent" },
	{ VertexAttributeIndex::Color, 4, "v_color" },
	{ VertexAttributeIndex::DrawId, 1, "v_drawId" },
	{ VertexAttributeIndex::Matrix, 12, "v_matrix" }
};

typedef int64_t VertexFormatCode;

class VertexFormat
{
public:
	typedef std::array<char, (int)VertexAttributeIndex::Last> Offsets;

	constexpr VertexFormat(const VertexFormatCode code);
	constexpr VertexFormat(const std::initializer_list<VertexAttributeIndex>& input);

	template<class T, VertexAttributeIndex a>
	constexpr T& value(void* base) const
	{
		static_assert(sizeof(T) == c_attribs[(int)a].size * sizeof(float), "Wrong data type");
		const char* ptr = (char*)base + attributeOffset(a);
		return *((T*)ptr);
	}

	constexpr uint32_t size() const { return m_size; }
	constexpr bool haveAttribute(const VertexAttributeIndex attr) const
	{
		return (m_code & ((VertexFormatCode)1 << (VertexFormatCode)attr)) != 0;
	}
	constexpr int attributeOffset(const VertexAttributeIndex attr) const
	{
		return m_offsets[(int)attr] * sizeof(float);
	}

private:
	static constexpr VertexFormatCode makeCode(const std::initializer_list<VertexAttributeIndex>& input);
	static constexpr Offsets makeOffsetsArray(const VertexFormatCode code);
	static constexpr int calculateSize(const VertexFormatCode code);

private:
	const VertexFormatCode m_code;
	const Offsets m_offsets;
	const int m_size;
};

const VertexFormat c_defaultVf =
{
	VertexAttributeIndex::Pos,
	VertexAttributeIndex::Uv,
	VertexAttributeIndex::Normal,
	//VertexAttributeIndex::Tangent
};

const VertexFormat c_debugVf =
{
	VertexAttributeIndex::Pos,
	VertexAttributeIndex::Color
};

const VertexFormat c_testVf =
{
	VertexAttributeIndex::Pos
};
