#include "MgeMaxLengthValidator.h"

MgeMaxLengthValidator::MgeMaxLengthValidator(size_t maxLength) noexcept
	: m_maxLength(maxLength)
{
}

size_t MgeMaxLengthValidator::getMaxLength() const noexcept
{
	return m_maxLength;
}

void MgeMaxLengthValidator::setMaxLength(size_t newMaxLength) noexcept
{
	m_maxLength = newMaxLength;
}

bool MgeMaxLengthValidator::isTextAllowed(std::wstring_view candidateText) const noexcept
{
	return candidateText.size() <= m_maxLength;
}

namespace mge
{
	MaxLengthValidator createMaxLengthValidator(size_t maxLength)
	{
		return std::make_shared<MgeMaxLengthValidator>(maxLength);
	}
}
