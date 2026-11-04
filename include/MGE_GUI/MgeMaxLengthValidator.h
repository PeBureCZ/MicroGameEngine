#pragma once
#include <memory>
#include <string_view>

#include "MgeTextValidator.h"

class MgeMaxLengthValidator;

namespace mge
{
	using MaxLengthValidator = std::shared_ptr<MgeMaxLengthValidator>;
	MaxLengthValidator createMaxLengthValidator(size_t maxLength);
}

/**@brief Accepts typed characters while the edited line has at most maxLength characters.*/
class MgeMaxLengthValidator : public MgeTextValidator
{
public:
	explicit MgeMaxLengthValidator(size_t maxLength) noexcept;

	[[nodiscard]] size_t getMaxLength() const noexcept;
	void setMaxLength(size_t newMaxLength) noexcept;

protected:
	[[nodiscard]] bool isTextAllowed(std::wstring_view candidateText) const noexcept override;

private:
	size_t m_maxLength = 0;
};
