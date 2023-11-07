#pragma once

#include "task.h"

namespace xsky
{
	class tcp_task : public task
	{
	public:
		void execute() override;
	};
}