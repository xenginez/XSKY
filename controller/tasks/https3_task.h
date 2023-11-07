#pragma once

#include "task.h"

namespace xsky
{
	class https3_task : public task
	{
	public:
		void execute() override;
	};
}