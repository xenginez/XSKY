#pragma once

#include "task.h"

namespace xsky
{
	class https_task : public task
	{
	public:
		void execute() override;
	};
}