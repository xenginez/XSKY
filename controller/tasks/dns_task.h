#pragma once

#include "task.h"

namespace xsky
{
	class dns_task : public task
	{
	public:
		void execute() override;
	};
}