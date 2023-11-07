#pragma once

#include "task.h"

namespace xsky
{
	class udp_task : public task
	{
	public:
		void execute() override;
	};
}