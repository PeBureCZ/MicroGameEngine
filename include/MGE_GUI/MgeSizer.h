#pragma once

#include "MgeWidget.h"

enum class SizerType : bool
{
    VERTICAL = 0,
    HORIZONTAL = 1,
};

class MgeSizer : public MgeWidget
{
public:
	MgeSizer(SizerType type);

	void layout() noexcept override;

private:
	SizerType m_orientation = SizerType::VERTICAL;

	void resizeChildren() noexcept;
};


