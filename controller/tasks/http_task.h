#pragma once

#include "task.h"

namespace xsky
{
	class http_task : public task
	{
	public:
		void execute() override;
	};
}